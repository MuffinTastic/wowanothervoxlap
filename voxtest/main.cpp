#include <voxlap\voxlap5.h>
#include <voxlap\sysmain.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <string>

long cursoractive;
static long mousex, mousey;

typedef struct { long f, p, x, y; } tiletype;

tiletype target;

point3d ipos, istr, ihei, ifor;

double odtotclk, dtotclk;
float fsynctics;
long totclk;

point3d zAx;
vx5sprite desklamp;

void updateminimap();
long mmapsiz = 128;
long* mapbuf;
const char* const sectors[64] = {
	"A1", "B1", "C1", "D1", "E1", "F1", "G1", "H1",
	"A2", "B2", "C2", "D2", "E2", "F2", "G2", "H2",
	"A3", "B3", "C3", "D3", "E3", "F3", "G3", "H3",
	"A4", "B4", "C4", "D4", "E4", "F4", "G4", "H4",
	"A5", "B5", "C5", "D5", "E5", "F5", "G5", "H5",
"A6", "B6", "C6", "D6", "E6", "F6", "G6", "H6",
"A7", "B7", "C7", "D7", "E7", "F7", "G7", "H7",
"A8", "B8", "C8", "D8", "E8", "F8", "G8", "H8",
};

bool quitmode, mapmode, typemode;
std::string input;

const char* quitmsg = "QUIT? (Y / N)";

long sysmain::initapp(long argc, char **argv)
{
	xres = 1280;
	yres = 720;
	colbits = 32;
	fullscreen = 0;

	setalwaysactive(1);
	prognam = "voxtest";

	initvoxlap();
	loadnul(&ipos, &istr, &ihei, &ifor);

	lpoint3d p1 = { 0, 0, 0 };
	lpoint3d p2 = { 511, 511, 63 };
	setrect(&p1, &p2, -1);

	vx5.colfunc = jitcolfunc;
	vx5.curcol = 0x80800000;

	ipos = { 256.0f, 256.0f, 32.0f };
	ifor = { 1.0f, 0.0f, 0.0f };
	istr = { 0.0f, 1.0f, 0.0f };
	ihei = { 0.0f, 0.0f, 1.0f };

	//TODO: implement AoS-style lighting.
	//NOTE: Diagonal (y++ z++ x==) lighting, shadow increment maxes to 10 blocks
	//      Process is most likely going to be cpu clock expensive - limit to -64 - +64 XY range on player pos every 5s!
	///vx5.lightmode = 1;

	vx5.maxscandist = 128 * sqrt(2);
	vx5.fogcol = 0x80E8FF;

	setsideshades(0, 64, 32, 32, 8, 48);

	zAx.x = zAx.y = 0.0; zAx.z = 1.0;

	desklamp.voxnum = getkv6("kv6\\spade.kv6");
	desklamp.p.x = 255; desklamp.p.y = 200; desklamp.p.z = 32;
	desklamp.s.x = .4; desklamp.s.y = 0; desklamp.s.z = 0;
	desklamp.h.x = 0; desklamp.h.y = .4; desklamp.h.z = 0;
	desklamp.f.x = 0; desklamp.f.y = 0; desklamp.f.z = .4;
	desklamp.kfatim = 0; desklamp.okfatim = 0; desklamp.flags = 0;

	kpzload("png\\target.png", &target.f, &target.p, &target.x, &target.y);

	mapbuf = (long*)malloc(VSID * VSID * sizeof(long));
	updateminimap();

	mapmode = false;
	quitmode = false;

	typemode = false;
	input = "";

	return 0;
}

void updateminimap()
{
	for (long x = 0; x < VSID; x++) {
		for (long y = 0; y < VSID; y++) {
			if ((x % 64) == 0 || (y % 64) == 0) {
				mapbuf[(x + VSID * y)] = 0xFFCCCCCC;
			} else {
				long pclr = getcube(x, y, getfloorz(x, y, 0));
				if (pclr == 0 || pclr == 1) {
					mapbuf[(x + VSID * y)] = 0xFF000000;
					continue;
				}
				mapbuf[(x + VSID * y)] = (*(long*)pclr & 0x00FFFFFF) | 0xFF000000; // ignores lighting
			}
		}
	}
}

void drawminimap()
{
	if (mapmode) {
		drawtile((long)mapbuf, VSID * sizeof(long), VSID, VSID, VSID << 15, VSID << 15,
			sysmain::xres << 15, sysmain::yres << 15, 65536, 65536, 0, -1);
	} else {
		long mmaphalf = (mmapsiz / 2);
		long x = min(max((long)ipos.x, mmaphalf), VSID - mmaphalf) - mmaphalf;
		long y = min(max((long)ipos.y, mmaphalf), VSID - mmaphalf) - mmaphalf;
		long cursec_x = (long)ipos.x / 64;
		long cursec_y = (long)ipos.y / 64;

		print6x8((sysmain::xres - mmaphalf - 20) - 3, 20 - 8, 0xFFFFFF, -1, sectors[cursec_x + 8 * cursec_y]);
		drawtile((long)&mapbuf[(x + VSID * y)], VSID * sizeof(long), 128, 128, 0, 0,
			(sysmain::xres - mmapsiz - 20) << 16, 20 << 16, 65536, 65536, 0, -1);
	}
}

double lastdeath = 0;
int deathpos = 0;

void updatedeath()
{
	if (lastdeath + 0.001 < dtotclk) {
		for (int i = 0; i < 512; i++) {
			if (deathpos < 512 * 512) {
				int offsoffs = 0;
				if ((deathpos / 512) % 2)
					offsoffs++;
				lpoint3d ofs = { deathpos % 512 + offsoffs, deathpos / 512, 0 };
				vspans span = { 63, 0, 0, 0 };
				if (deathpos % 2)
					setspans(&span, 1, &ofs, 0);
				lastdeath = dtotclk;
				deathpos++;
			}
		}
	}
}

void sysmain::doframe()
{
	// Timing
	odtotclk = dtotclk; sysmain::readklock(&dtotclk);
	totclk = (long)(dtotclk*1000.0); fsynctics = (float)(dtotclk - odtotclk);

	long frameptr, pitch, xdim, ydim;
	startdirectdraw(&frameptr, &pitch, &xdim, &ydim);

	voxsetframebuffer(frameptr, pitch, xdim, ydim);
	setcamera_float(&ipos, &istr, &ihei, &ifor, xdim*.5f, ydim*.5f, xdim*.5f);

	opticast();

	drawsprite(&desklamp);

	if (cursoractive)
		drawminimap();

	lpoint3d hit;
	long *ind;
	long dir;
	if (!cursoractive) {
		drawtile(target.f, target.p, target.x, target.y, target.x << 15, target.y << 15,
			xres << 15, yres << 15, 65536, 65536, 0, -1);

		hitscan_f(&ipos, &ifor, &hit, &ind, &dir);
	} else {
		drawtile(target.f, target.p, target.x, target.y, target.x << 15, target.y << 15,
			mousex << 16, mousey << 16, 65536, 65536, 0, -1);

		point3d hei = { ihei.x, ihei.y, ihei.z };
		point3d str = { istr.x, istr.y, istr.z };
		point3d screen_ifor;
		screen_ifor.x = ifor.x;
		screen_ifor.y = ifor.y;
		screen_ifor.z = ifor.z;
		double rot_amount = (45.0 / (xres*.5)) * (PI / 180.0) * 1.09;
		axisrotate(&screen_ifor, &hei, rot_amount * (mousex - (xres >> 1)));
		axisrotate(&screen_ifor, &str, -rot_amount * (mousey - (yres >> 1)));

		hitscan_f(&ipos, &screen_ifor, &hit, &ind, &dir);
	}

	if (!cursoractive)
		drawminimap();

	if (quitmode) {
		print6x8(xdim / 2 - strlen(quitmsg) * 3, 50, 0xFF0000, -1, quitmsg);
	}

	if (typemode) {
		print6x8(10, ydim - ydim / 3, 0xFFFFFF, -1, "Map: %s", input.c_str());
	}

	print6x8(0, 0, 0x00FF00, -1, "FPS: %.f", 1.0 / fsynctics);

	stopdirectdraw();
	nextpage();

	sysmain::readkeyboard();
	
	long key;
	while (key = keyread()) {
		if (key == 0) continue;
		char scancode = (key & 0xFF00) >> 8;
		if (typemode) {
			if (scancode == 0x01) {
				input = "";
				typemode = false;
			} else if (scancode == 0x1C) {
				if (input == "null") {
					point3d nullp3d;
					loadnul(&nullp3d, &nullp3d, &nullp3d, &nullp3d);
					ipos = { 256.0f, 256.0f, 32.0f };
					ifor = { 1.0f, 0.0f, 0.0f };
					istr = { 0.0f, 1.0f, 0.0f };
					ihei = { 0.0f, 0.0f, 1.0f };
				} else {
					if (loadvxl(("vxl/" + input).c_str())) {
						updateminimap();
						if (getcube((long)ipos.x, (long)ipos.y, (long)ipos.z)) { // camera in block
							ipos.x = 255.0;
							ipos.y = 255.0;
							ipos.z = -32.0;
						}
					}
				}
				updatevxl();
				input = "";
				typemode = false;
			} else if (scancode == 0x0E) {
				if (input.length() > 0)
					input.pop_back();
			} else {
				char inp = key & 0xFF;
				if (inp == 0) continue;
				input += inp;
			}
		} else {
			if (quitmode) {
				if (scancode == 0x15) { // Y
					quitloop();
				} else if (scancode == 0x31) { // N
					quitmode = false;
				}
			} else {
				if (scancode == 0x01) { // ESC
					quitmode = true;
				} else if (scancode == 0x14) { // T
					{
						typemode = true;
					}
				} else if (scancode == 0x32) { // M
					updateminimap();
					mapmode = !mapmode;
				}
			}
		}
		if (scancode == 0x3F) {
			cursoractive = !cursoractive;
			if (cursoractive) {
				mousex = xres / 2;
				mousey = yres / 2;
			}
		}
	}

	float fmousx, fmousy; long bstatus; static long pbstatus = 0;
	readmouse(&fmousx, &fmousy, &bstatus);
	
	mousex = min(max(mousex + (long)fmousx, 0), xres);
	mousey = min(max(mousey + (long)fmousy, 0), yres);

	vspans span;
	lpoint3d nothing = { 0, 0, 0 };

	if (ind != NULL) {
		span.x = hit.x;
		span.y = hit.y;
		span.z0 = 0;
		span.z1 = 63;
		//if (bstatus != pbstatus) {
			if (bstatus & 1) {
				vx5.curcol = 0x80800000;
				setspans(&span, 1, &nothing, 0);
			} else if (bstatus & 2) {
				vx5.curcol = 0x80704030;
				setspans(&span, 1, &nothing, -1);
			}
		//}
	}

	pbstatus = bstatus;

	if (!cursoractive) {
	
		orthorotate(istr.z*.1,
			fmousy*fsynctics*.016 * 4.0f,
			fmousx*fsynctics*.016 * 4.0f,
			&istr, &ihei, &ifor);
	} else {
		orthorotate(istr.z*.1, 0, 0, &istr, &ihei, &ifor);
	}

	if (!quitmode && !typemode) {
		float forward = (sysmain::keystatus[0x11]) - (sysmain::keystatus[0x1F]); // W, S
		float strafe = (sysmain::keystatus[0x20]) - (sysmain::keystatus[0x1E]); // D, A
		point3d ivel;
		ivel.x = ((forward * ifor.x) + (strafe * istr.x)) * (50.0f * fsynctics);
		ivel.y = ((forward * ifor.y) + (strafe * istr.y)) * (50.0f * fsynctics);
		ivel.z = ((forward * ifor.z) + (strafe * istr.z)) * (50.0f * fsynctics);
		clipmove_float(&ipos, &ivel, 1.5);
	}

	updatedeath();
	updatevxl();
}

void sysmain::uninitapp()
{
	free(mapbuf);
	uninitvoxlap();
}