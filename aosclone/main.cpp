#include <string>

#include <SDL\SDL.h>

#include <voxlap\sysmain.h>
#include <voxlap\voxlap.h>
#include <voxlap\extension\DoubleVector3D.h>
#include <voxlap\extension\FloatVector3D.h>
#include <voxlap\extension\LongVector3D.h>

#include "constants.h"

#define MAIN_CPP
#include "config.h"
#include "general.h"
#include "vxl.h"
#include "world.h"
#include "player.h"
#include "ui.h"
#include "networking.h"

double odtotclk, dtotclk;
float fsynctics;
long totclk;

long sysmain::initapp(long argc, char **argv)
{
	if (argc < 2) {
		showError("Error: No server address was provided!");
		return -1;
	}

	config::load();

	sysmain::xres = config::xres;
	sysmain::yres = config::yres;
	sysmain::fullscreen = !config::windowed;

	sysmain::setalwaysactive(1);
	sysmain::prognam = GAME_TITLE;

	sysmain::mousmoth = 0;

	voxlap::initvoxlap();
	voxlap::point3d tmp_p, tmp_s, tmp_h, tmp_f;
	voxlap::loadnul(&tmp_p, &tmp_s, &tmp_h, &tmp_f);

	voxlap::vx5.maxscandist = AOS_FOG_DISTANCE;
	
	voxlap::vx5.fogcol = 0;
	voxlap::vx5.fallcheck = 1;

	//voxlap::setsideshades(0, 64, 32, 32, 8, 48);
	//voxlap::setsideshades(0, 112, 64, 64, 32, 96); // <3 0.76 lighting
	voxlap::setsideshades(0, 64, 32, 32, 16, 48);
	
	voxlap::vx5.colfunc = voxlap::jitcolfunc;
	voxlap::vx5.curcol = DEFAULT_COLOR;
	voxlap::vx5.kv6col = 0x808080;

	if (!aos_init_models())
		return -1;

	sysmain::readklock(&dtotclk);
	totclk = (long)(dtotclk*1000.0);
	fsynctics = 1.f;

	for (int i = 0; i < 34; i++) {
		aos_players[i].id = i;
		aos_players[i].init();
	}


	aos_ui_init();
	//aos_ui_updateminimap();

	if (!aos_network_init())
		return -1;

	long ret = 0;

	for (long i = 0; i < config::net_tries && ret == 0; i++) {
		ret = aos_network_connect(argv[1]);

		if (ret == 0 && config::net_tries > 1)
			showError("Server connection failed\nRetrying.. (%d/%d)", i + 1, config::net_tries);
	}

	if (ret != 1) {
		showError("Server connection failed");
		return -1;
	}

	return 0;
}

void aos_handleinput()
{
	PlayerType *cur_pl = aos_world_get_local_player();

	static long lbstatus = 0;
	float fmousx, fmousy;
	long key, bstatus;
	bool update_mb;

	sysmain::readmouse(&fmousx, &fmousy, &bstatus);
	//dbstatus = (lbstatus ^ bstatus) & bstatus; // detect rising edge change in bstatus
	update_mb = bstatus != lbstatus;
	lbstatus = bstatus;

	sysmain::readkeyboard();

	// Keyboard
	while (key = sysmain::keyread()) {
		if (key == 0) continue;
		aos_ubyte scancode = (key & 0xFF00) >> 8;
		aos_ui_keypress(key, scancode);
		cur_pl->local_handle_keypress(key, scancode);
	}
	
	cur_pl->local_handle_input(fmousx, fmousy, update_mb);
}

void aos_update()
{
	PlayerType *cur_pl = aos_world_get_local_player();

	aos_handleinput();

	if (!aos_network_service())
		sysmain::quitloop();

	cur_pl->local_update();

	if (aos_gamestate > GameState::MAP_LOADING) {
		if (config::shadows)
			aos_vxl_updatelighting(&cur_pl->position);
		aos_world_update_block_damage();
		aos_vxl_update_damaged_color();

		for (int i = 0; i < 32; i++) {
			PlayerType *player = &aos_players[i];
			
			player->update();
		}
	}
	aos_world_update_team_entities();

	voxlap::updatevxl();
	aos_world_update_falling();
	aos_world_update_bullets();
	aos_world_update_grenades();
	
	aos_ui_updatestats();
}

void aos_render()
{
	if (sysmain::fullscreen && !sysmain::canrender())
		return;

	PlayerType *cur_pl = aos_world_get_local_player();

	long frameptr, pitch, xdim, ydim;
	sysmain::startdirectdraw(&frameptr, &pitch, &xdim, &ydim);
	voxlap::voxsetframebuffer(frameptr, pitch, xdim, ydim);
	
	if (aos_gamestate > GameState::MAP_LOADING) {
		cur_pl->local_set_camera(xdim, ydim);
		voxlap::opticast();

		aos_world_draw_team_entities();
		aos_world_draw_falling();
		aos_world_draw_bullets();
		aos_world_draw_grenades();

		//voxlap::extension::FloatVector3D ori = cur_pl->orientation_for;
		//ori.y = -ori.y;
		//ori.z = -ori.z;
		//voxlap::point3d p = cur_pl->cam_position + ori * 16.0f; // voxlap::point3d{ 32.0f, 32.0f, 42.0f };
		//voxlap::drawsquarefill(p.x, p.y, p.z, -4.0f, 0xFFFF0000);

		for (int i = 0; i < 32; i++) {
			PlayerType *loop_pl = &aos_players[i];

			if (loop_pl->team > ID_TEAM_SPEC) {
				if (i == aos_local_player_id)
					continue;
				
				loop_pl->upd_draw_models();
			}
		}

		//cur_pl->draw_firstp_models();
		cur_pl->draw_local_models();

		if (cur_pl->alive && cur_pl->team != PLAYER_UNCONNECTED) {
			aos_ui_draw_target();
		}

		aos_ui_draw_health();
		aos_ui_draw_ammo();

		aos_ui_draw_hovernames();

		cur_pl->draw_local_switchmodels();

		aos_ui_draw_switch();

		aos_ui_draw_minimap(frameptr, pitch, xdim, ydim);
		aos_ui_draw_scoreboard();
		aos_ui_draw_stats();

	} else {
		sysmain::clearscreen(0x000000);
		aos_ui_draw_mapload();
	}

	aos_ui_draw_quit();
	aos_ui_draw_chat();

	voxlap::print6x8(0, sysmain::yres - 24, 0x00FF00, 0, "FPS: %.f", 1.0 / fsynctics);
	voxlap::print6x8(0, sysmain::yres - 16, 0xFFFF00, 0, "fs: %f", fsynctics);
	voxlap::print6x8(0, sysmain::yres - 8, 0xFF0000, 0, "dt %f", dtotclk);

	sysmain::stopdirectdraw();
	sysmain::nextpage();
}

void sysmain::doframe()
{
	long ptotclk = totclk;

	aos_update();
	aos_render();

	// Timing
	odtotclk = dtotclk; sysmain::readklock(&dtotclk);
	totclk = (long)(dtotclk*1000.0); fsynctics = (float)(dtotclk - odtotclk);

	if (aos_gamestate <= GameState::MAP_LOADING) {
		if (totclk - ptotclk < 1000 / 30) { // 30fps constant, not choppy but not too resource hoggy
			sysmain::sleep((1000 / 30) - (Uint32)(totclk - ptotclk));
		}
	} else if (config::fps_cap) {
		if (totclk - ptotclk < 1000 / config::fps_cap) {
			sysmain::sleep((1000 / config::fps_cap) - (Uint32)(totclk - ptotclk));
		}
	}
}

void sysmain::uninitapp()
{
	aos_network_disconnect();
	aos_network_uninit();
	aos_ui_uninit();
	aos_uninit_models();
	voxlap::uninitvoxlap();
}