#include "config.h"

#include <Windows.h>
#include <Shlwapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

namespace config {

	long xres;
	long yres;
	long fscn;

	long mv_fw;
	long mv_bk;
	long mv_lf;
	long mv_rh;

	double sensi;

	void load(const char * configfile)
	{
		std::string file = "";
		char *tmp = (char *)malloc(1024);
		GetCurrentDirectory(1024, tmp);
		file += tmp; file += "\\"; file += configfile;

		xres = (long)GetPrivateProfileInt("config", "xres", 640, file.c_str());
		yres = (long)GetPrivateProfileInt("config", "yres", 480, file.c_str());
		fscn = (long)GetPrivateProfileInt("config", "fullscreen", 0, file.c_str());

		GetPrivateProfileString("config", "sensitivity", "1.0", tmp, 1024, file.c_str());
		sensi = atof(tmp);

		free(tmp);
	}

}