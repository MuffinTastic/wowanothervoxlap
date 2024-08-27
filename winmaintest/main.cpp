#include "sysmain.h"
#include "voxlap5.h"

#include <stdio.h>
#include <math.h>

#include <string>

dpoint3d ipos, istr, ihei, ifor;

double odtotclk, dtotclk;
float fsynctics;
long totclk;

long initapp(long argc, char **argv)
{
	xres = 1280; yres = 720; colbits = 32; fullscreen = 0; alwaysactive = 1;
	prognam = "voxtest";

	initvoxlap();
	loadnul(&ipos, &istr, &ihei, &ifor);

	totclk = (long)(dtotclk*1000.0);
	fsynctics = 1.f;

	return 0;
}

void doframe()
{
	// Timing
	odtotclk = dtotclk; readklock(&dtotclk);
	totclk = (long)(dtotclk*1000.0); fsynctics = (float)(dtotclk - odtotclk);

	long frameptr, pitch, xdim, ydim;
	ddrawdebugmode = 0; ddrawuseemulation = 0;
	startdirectdraw(&frameptr, &pitch, &xdim, &ydim);

	voxsetframebuffer(frameptr, pitch, xdim, ydim);
	setcamera(&ipos, &istr, &ihei, &ifor, xdim*.5, ydim*.5, xdim*.5);

	opticast();

	stopdirectdraw();
	nextpage();

	readkeyboard(); if (keystatus[1]) quitloop();

	long key;
	while (key = keyread()) {
		if ((key & 0xFF) == 'T')
			quitloop();
	}
}

void uninitapp()
{
	uninitvoxlap();
}