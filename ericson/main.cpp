#include "sysmain.h"
#include "voxlap5.h"

#include <stdio.h>
#include <math.h>

dpoint3d ipos, istr, ihei, ifor;

double odtotclk, dtotclk;
float fsynctics;
long totclk;

long initapp(long argc, char **argv)
{
	xres = 1280; yres = 720; colbits = 32; fullscreen = 0; alwaysactive = 1;
	prognam = "another voxlap program";

	readklock(&dtotclk);
	initvoxlap();
	loadnul(&ipos, &istr, &ihei, &ifor);
	loadvxl("hallway.vxl");

	ipos.z = -16.0;

	totclk = (long)(dtotclk*1000.0);
	fsynctics = 1.f;

	return 0;
}

void doframe()
{
	// Timing
	odtotclk = dtotclk; readklock(&dtotclk);
	totclk = (long)(dtotclk*1000.0); fsynctics = (float)(dtotclk - odtotclk);

	// Render
	long frameptr, pitch, xdim, ydim;
	startdirectdraw(&frameptr, &pitch, &xdim, &ydim);

	voxsetframebuffer(frameptr, pitch, xdim, ydim);
	setcamera(&ipos, &istr, &ihei, &ifor, xdim*.5, ydim*.5, xdim*.5);

	opticast();

	if ((long)(dtotclk) % 10 > 5) {
		for (int x = 0; x < 512; x++) {
			for (int y = 0; y < 512; y++) {
				int tz = getfloorz(x, y, 0);
				long tcol = getcube(x, y, tz);
				if (tcol <= 1)
					break;
				if ((x % 64 == 0 || y % 64 == 0) && (x > 0 && y > 0)) {
					drawpoint2d(xdim / 2 - 256 + x, ydim / 2 - 256 + y, 0xCCCCCC);
				} else {
					drawpoint2d(xdim / 2 - 256 + x, ydim / 2 - 256 + y, *(long*)tcol);
				}
			}
		}
	} else {
		for (int x = 0; x < 128; x++) {
			for (int y = 0; y < 128; y++) {
				int tx = (int)ipos.x + x - 64;
				int ty = (int)ipos.y + y - 64;

				int tz = getfloorz(tx, ty, 0);
				long tcol = getcube(tx, ty, tz);
				if (tcol <= 1)
					break;
				if ((tx % 64 == 0 || ty % 64 == 0) && (tx > 0 && ty > 0)) {
					drawpoint2d(xdim - 128 - 20 + x, 20 + y, 0xCCCCCC);
				} else {
					drawpoint2d(xdim - 128 - 20 + x, 20 + y, *(long*)tcol);
				}
			}
		}
	}

	stopdirectdraw();
	nextpage();

	//Update

	ipos.x = 256.0 + sin(dtotclk)*128.0;
	ipos.y = 256.0 + cos(dtotclk)*96.0;

	readkeyboard(); if (keystatus[1]) quitloop(); // quit if esc is pressed
}

void uninitapp()
{
	uninitvoxlap();
}