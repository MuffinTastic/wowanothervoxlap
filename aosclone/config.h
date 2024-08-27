#pragma once

namespace config {

	extern long xres;
	extern long yres;
	extern unsigned char vol;
	extern bool inverty;
	extern bool windowed;
	extern long net_tries;
	extern bool shadows;
	extern bool drunkcam;
	extern long fps_cap;
	extern float mouse_sens_x;
	extern float mouse_sens_y;
	extern char name[16];

	extern unsigned char move_forward;
	extern unsigned char move_backward;
	extern unsigned char move_left;
	extern unsigned char move_right;
	extern unsigned char jump;
	extern unsigned char crouch;
	extern unsigned char sneak;
	extern unsigned char sprint;
	extern unsigned char view_map;
	extern unsigned char reload;
	extern unsigned char cube_color_up;
	extern unsigned char cube_color_down;
	extern unsigned char cube_color_left;
	extern unsigned char cube_color_right;
	extern unsigned char cube_color_sample;
	extern unsigned char quit_game;
	extern unsigned char save_map;
	extern unsigned char volume_up;
	extern unsigned char volume_down;
	extern unsigned char view_score;
	extern unsigned char show_mouse;
	extern unsigned char change_team;
	extern unsigned char change_weapon;
	extern unsigned char last_weapon;
	extern unsigned char toggle_graph;

	extern unsigned char slot1;
	extern unsigned char slot2;
	extern unsigned char slot3;
	extern unsigned char slot4;
	extern unsigned char slot5;
	extern unsigned char slot6;
	extern unsigned char slot7;
	extern unsigned char slot8;
	extern unsigned char slot9;
	extern unsigned char slot10;

	void load();

}