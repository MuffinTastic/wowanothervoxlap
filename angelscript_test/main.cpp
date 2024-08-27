#include <voxlap\sysmain.h>
#include <voxlap\voxlap5.h>
using namespace sysmain;

#include "scripting.h"

double odtotclk, dtotclk;
double fsynctics;

long sysmain::initapp(long argc, char **argv)
{
	initvoxlap();

	sysmain::prognam = "voxscript";

	if (!script_init())
		return -1;
	//script_uninit();
	//return -1;

	//setcamera_float(&ipos, &istr, &ihei, &ifor, sysmain::xres*.5, sysmain::yres*.5, sysmain::xres*.5);

	return 0;
}

void sysmain::doframe()
{
	// Timing
	odtotclk = dtotclk; sysmain::readklock(&dtotclk);
	fsynctics = dtotclk - odtotclk;

	script_loop(fsynctics);

	//drawsprite(&desklamp);
	//print6x8(2, 2, 0x8D50AC, 0, "%d fps", (int)(1.0f / fsynctics));
}

void sysmain::uninitapp()
{
	script_uninit();
	uninitvoxlap();
	kzuninit();
}