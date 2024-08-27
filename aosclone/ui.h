#pragma once

#include "player.h"

extern bool aos_ui_quitmode, aos_ui_chatmode, aos_ui_mapmode, aos_ui_statmode;
extern long aos_ui_switchmode;

#define AOS_UI_SWITCH_NONE    0
#define AOS_UI_SWITCH_TEAM    1
#define AOS_UI_SWITCH_WEAPON  2

void aos_ui_init();

void aos_ui_keypress(long, aos_ubyte);
aos_ubyte aos_ui_get_slot(aos_ubyte);

void aos_ui_updateminimap();
void aos_ui_draw_minimap(long, long, long, long);

void aos_ui_updatestats();
void aos_ui_draw_stats();
void aos_ui_add_inbytes(unsigned short);
void aos_ui_add_outbytes(unsigned short);

void aos_ui_draw_quit();

void aos_ui_add_chat_message(long color, char *format, ...);
void aos_ui_add_killfeed_message(long color, char *format, ...);
void aos_ui_draw_chat();

void aos_ui_draw_hovernames();
void aos_ui_draw_scoreboard();

void aos_ui_drawbar(long, long, long, long, long, double);

void aos_ui_draw_mapload();
void aos_ui_draw_tc_bar(aos_byte, TC_Territory *);

void aos_ui_attempt_respawn_sound(aos_byte);

void aos_ui_draw_ammo();
void aos_ui_draw_health();
void aos_ui_draw_target();
void aos_ui_draw_switch();

void aos_ui_uninit();
