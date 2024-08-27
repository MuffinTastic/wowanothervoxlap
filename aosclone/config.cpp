#include "config.h"

#include <Windows.h>
#include <Shlwapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

namespace config {

	long xres;
	long yres;
	unsigned char vol;
	bool inverty;
	bool windowed;
	long net_tries;
	bool shadows;
	bool drunkcam;
	long fps_cap;
	float mouse_sens_x;
	float mouse_sens_y;
	char name[16];

	unsigned char move_forward;
	unsigned char move_backward;
	unsigned char move_left;
	unsigned char move_right;
	unsigned char jump;
	unsigned char crouch;
	unsigned char sneak;
	unsigned char sprint;
	unsigned char view_map;
	unsigned char reload;
	unsigned char cube_color_up;
	unsigned char cube_color_down;
	unsigned char cube_color_left;
	unsigned char cube_color_right;
	unsigned char cube_color_sample;
	unsigned char quit_game;
	unsigned char save_map;
	unsigned char volume_up;
	unsigned char volume_down;
	unsigned char view_score;
	unsigned char show_mouse;
	unsigned char change_team;
	unsigned char change_weapon;
	unsigned char last_weapon;
	unsigned char toggle_graph;

	unsigned char slot1;
	unsigned char slot2;
	unsigned char slot3;
	unsigned char slot4;
	unsigned char slot5;
	unsigned char slot6;
	unsigned char slot7;
	unsigned char slot8;
	unsigned char slot9;
	unsigned char slot10;

	void load()
	{
		char tmp[1024];
		GetCurrentDirectory(1024, tmp);
		std::string workingdir = (std::string(tmp) + "\\");

		std::string config_file = (workingdir + "config.ini");

		xres		=     GetPrivateProfileInt		("client", "xres",				640,				config_file.c_str());
		yres		=     GetPrivateProfileInt		("client", "yres",				480,				config_file.c_str());
		vol			=     GetPrivateProfileInt		("client", "vol",				100,				config_file.c_str()) * 10;
		inverty		=     GetPrivateProfileInt		("client", "inverty",			0,					config_file.c_str());
		windowed	=     GetPrivateProfileInt		("client", "windowed",			1,					config_file.c_str());
		net_tries	= max(GetPrivateProfileInt		("client", "net_tries",			0,					config_file.c_str()), 1);
		shadows		=     GetPrivateProfileInt		("client", "shadows",			0,					config_file.c_str());
		drunkcam	=     GetPrivateProfileInt		("client", "drunkcam",			0,					config_file.c_str());
		fps_cap		=     GetPrivateProfileInt		("client", "fps_cap",			60,					config_file.c_str());
					      GetPrivateProfileString	("client", "mouse_sensitivity", "1.0", tmp, 128,	config_file.c_str());
		mouse_sens_x = atof(tmp);
					      GetPrivateProfileString   ("client", "mouse_sens_y",      "0.0", tmp, 128,    config_file.c_str());
		mouse_sens_y = atof(tmp); mouse_sens_y = (mouse_sens_y) ? mouse_sens_y : mouse_sens_x;
					      GetPrivateProfileString	("client", "name",				"Deuce", name, 16,  config_file.c_str());

		std::string controls_file = (workingdir + "controls.ini");

		GetPrivateProfileString("controls", "move_forward",			"0x11", tmp, 128, controls_file.c_str());
		move_forward = (unsigned char)strtoul(tmp, NULL, 16);
		GetPrivateProfileString("controls", "move_backward",		"0x1F", tmp, 128, controls_file.c_str());
		move_backward = (unsigned char)strtoul(tmp, NULL, 16);
		GetPrivateProfileString("controls", "move_left",			"0x1E", tmp, 128, controls_file.c_str());
		move_left = (unsigned char)strtoul(tmp, NULL, 16);
		GetPrivateProfileString("controls", "move_right",			"0x20", tmp, 128, controls_file.c_str());
		move_right = (unsigned char)strtoul(tmp, NULL, 16);
		GetPrivateProfileString("controls", "jump",					"0x39", tmp, 128, controls_file.c_str());
		jump = (unsigned char)strtoul(tmp, NULL, 16);
		GetPrivateProfileString("controls", "crouch",				"0x1D", tmp, 128, controls_file.c_str());
		crouch = (unsigned char)strtoul(tmp, NULL, 16);
		GetPrivateProfileString("controls", "sneak",				"0x2F", tmp, 128, controls_file.c_str());
		sneak = (unsigned char)strtoul(tmp, NULL, 16);
		GetPrivateProfileString("controls", "sprint",				"0x2A", tmp, 128, controls_file.c_str());
		sprint = (unsigned char)strtoul(tmp, NULL, 16);
		GetPrivateProfileString("controls", "view_map",				"0x32", tmp, 128, controls_file.c_str());
		view_map = (unsigned char)strtoul(tmp, NULL, 16);
		GetPrivateProfileString("controls", "reload",				"0x13", tmp, 128, controls_file.c_str());
		reload = (unsigned char)strtoul(tmp, NULL, 16);
		GetPrivateProfileString("controls", "cube_color_up",		"0xC8", tmp, 128, controls_file.c_str());
		cube_color_up = (unsigned char)strtoul(tmp, NULL, 16);
		GetPrivateProfileString("controls", "cube_color_down",		"0xD0", tmp, 128, controls_file.c_str());
		cube_color_down = (unsigned char)strtoul(tmp, NULL, 16);
		GetPrivateProfileString("controls", "cube_color_left",		"0xCB", tmp, 128, controls_file.c_str());
		cube_color_left = (unsigned char)strtoul(tmp, NULL, 16);
		GetPrivateProfileString("controls", "cube_color_right",		"0xCD", tmp, 128, controls_file.c_str());
		cube_color_right = (unsigned char)strtoul(tmp, NULL, 16);
		GetPrivateProfileString("controls", "cube_color_sample",	"0x12", tmp, 128, controls_file.c_str());
		cube_color_sample = (unsigned char)strtoul(tmp, NULL, 16);
		GetPrivateProfileString("controls", "quit_game",			"0x1",  tmp, 128, controls_file.c_str());
		quit_game = (unsigned char)strtoul(tmp, NULL, 16);
		GetPrivateProfileString("controls", "save_map",				"0x3B", tmp, 128, controls_file.c_str());
		save_map = (unsigned char)strtoul(tmp, NULL, 16);
		GetPrivateProfileString("controls", "volume_up",			"0x4E", tmp, 128, controls_file.c_str());
		volume_up = (unsigned char)strtoul(tmp, NULL, 16);
		GetPrivateProfileString("controls", "volume_down",			"0x4A", tmp, 128, controls_file.c_str());
		volume_down = (unsigned char)strtoul(tmp, NULL, 16);
		GetPrivateProfileString("controls", "view_score",			"0x0F", tmp, 128, controls_file.c_str());
		view_score = (unsigned char)strtoul(tmp, NULL, 16);
		GetPrivateProfileString("controls", "show_mouse",			"0x1F", tmp, 128, controls_file.c_str());
		show_mouse = (unsigned char)strtoul(tmp, NULL, 16);
		GetPrivateProfileString("controls", "change_team",			"0x33", tmp, 128, controls_file.c_str());
		change_team = (unsigned char)strtoul(tmp, NULL, 16);
		GetPrivateProfileString("controls", "change_weapon",		"0x34", tmp, 128, controls_file.c_str());
		change_weapon = (unsigned char)strtoul(tmp, NULL, 16);
		GetPrivateProfileString("controls", "last_weapon",			"0x10", tmp, 128, controls_file.c_str());
		last_weapon = (unsigned char)strtoul(tmp, NULL, 16);
		GetPrivateProfileString("controls", "toggle_graph",			"0x57", tmp, 128, controls_file.c_str());
		toggle_graph = (unsigned char)strtoul(tmp, NULL, 16);

		GetPrivateProfileString("controls", "slot_1", "0x02", tmp, 128, controls_file.c_str());
		slot1 = (unsigned char)strtoul(tmp, NULL, 16);
		GetPrivateProfileString("controls", "slot_2", "0x03", tmp, 128, controls_file.c_str());
		slot2 = (unsigned char)strtoul(tmp, NULL, 16);
		GetPrivateProfileString("controls", "slot_3", "0x04", tmp, 128, controls_file.c_str());
		slot3 = (unsigned char)strtoul(tmp, NULL, 16);
		GetPrivateProfileString("controls", "slot_4", "0x05", tmp, 128, controls_file.c_str());
		slot4 = (unsigned char)strtoul(tmp, NULL, 16);
		GetPrivateProfileString("controls", "slot_5", "0x06", tmp, 128, controls_file.c_str());
		slot5 = (unsigned char)strtoul(tmp, NULL, 16);
		GetPrivateProfileString("controls", "slot_6", "0x07", tmp, 128, controls_file.c_str());
		slot6 = (unsigned char)strtoul(tmp, NULL, 16);
		GetPrivateProfileString("controls", "slot_7", "0x08", tmp, 128, controls_file.c_str());
		slot7 = (unsigned char)strtoul(tmp, NULL, 16);
		GetPrivateProfileString("controls", "slot_8", "0x09", tmp, 128, controls_file.c_str());
		slot8 = (unsigned char)strtoul(tmp, NULL, 16);
		GetPrivateProfileString("controls", "slot_9", "0x0A", tmp, 128, controls_file.c_str());
		slot9 = (unsigned char)strtoul(tmp, NULL, 16);
		GetPrivateProfileString("controls", "slot_10", "0x0B", tmp, 128, controls_file.c_str());
		slot10 = (unsigned char)strtoul(tmp, NULL, 16);
	}

}