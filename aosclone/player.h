#pragma once

#include <voxlap\voxlap.h>
#include <voxlap\extension\FloatVector3D.h>
#include <voxlap\extension\LongVector3D.h>
#include "general.h"
#include "packet.h"

#define TOOL_SPADE   0
#define TOOL_BLOCK   1
#define TOOL_GUN     2
#define TOOL_GRENADE 3
#define MAX_TOOL     TOOL_GRENADE

#define WEAPON_SEMI    0
#define WEAPON_SMG     1
#define WEAPON_SHOTGUN 2

#define TOOL_SPADE_PRIMARY_RATE    0.20
#define TOOL_SPADE_SECONDARY_RATE   1.0
#define TOOL_BLOCK_RATE             0.5
#define TOOL_BLOCK_PICK_RATE		0.2
#define TOOL_GRENADE_DELAY          0.1
#define TOOL_GRENADE_TIME           3.0

#define TOOL_BLOCK_STOCK   50
#define TOOL_GRENADE_STOCK  3

#define WEAPON_SEMI_DELAY    0.5
#define WEAPON_SMG_DELAY     0.1
#define WEAPON_SHOTGUN_DELAY 1.0

#define WEAPON_SEMI_RELOAD      2.5
#define WEAPON_SMG_RELOAD       2.5
#define WEAPON_SHOTGUN_RELOAD  0.25

#define WEAPON_SEMI_AMMO    10
#define WEAPON_SMG_AMMO     30
#define WEAPON_SHOTGUN_AMMO  6

#define WEAPON_SEMI_STOCK     50
#define WEAPON_SMG_STOCK     120
#define WEAPON_SHOTGUN_STOCK  48

#define WEAPON_SEMI_SPREAD    0.2
#define WEAPON_SMG_SPREAD     0.8
#define WEAPON_SHOTGUN_SPREAD   3

#define WEAPON_SEMI_RECOIL    2.5
#define WEAPON_SMG_RECOIL     0.8
#define WEAPON_SHOTGUN_RECOIL 8.0

#define WEAPON_SEMI_BULLETS     1
#define WEAPON_SMG_BULLETS      1
#define WEAPON_SHOTGUN_BULLETS  8

#define WEAPON_SEMI_BLOCK_DMG    4
#define WEAPON_SMG_BLOCK_DMG     2
#define WEAPON_SHOTGUN_BLOCK_DMG 1

#define TOOL_SPADE_BLOCK_DMG     4

#define RUBBERBAND_DISTANCE	3.0
#define MINE_RANGE			4.0

#define PLAYER_SWITCHTIME 0.5
#define PLAYER_RESTOCK_INTERVAL 

#define PLAYER_UNCONNECTED -2

#define ID_HIT_DEAD_PLAYER -2

void aos_player_initmodels();

struct PlayerType
{
	voxlap::extension::FloatVector3D position;
	voxlap::extension::FloatVector3D cam_position;
	voxlap::extension::FloatVector3D velocity;

	voxlap::extension::FloatVector3D orientation_for;
	voxlap::extension::FloatVector3D orientation_hei;
	voxlap::extension::FloatVector3D orientation_str;

	voxlap::extension::FloatVector3D orientation_dead;

	bool alive;

	bool local;
	aos_ubyte id;

	aos_byte team;
	aos_ubyte tool;
	aos_ubyte weapon;

	aos_ubyte health;

	aos_ubyte specid;
	aos_ubyte specmode;
	
	aos_ubyte respawntime;
	float lastrespawntick;

	aos_ubyte block_stock;
	long block_color;

	aos_ubyte weapon_ammo;
	aos_ubyte weapon_stock;
	
	aos_ubyte grenade_stock;
	bool grenade_active;

	bool primary;
	bool secondary;
	aos_ubyte lastinputstate;

	bool move_for;
	bool move_back;
	bool move_left;
	bool move_right;
	bool crouch;
	bool sprint;
	bool sneak;
	bool jump;
	bool airborne;
	bool wade;
	float lastclimb;

	// misc?
	char name[16];
	unsigned long score;

	float steprate;
	float laststep;
	float curstep;
	aos_ubyte stepstate;

	bool dead_rise;

	aos_ubyte last_tool;
	bool tool_switched;
	float lastdisable;
	float lastswitch;
	bool tool_active;

	bool block_drag;
	aos_ubyte block_drag_num;

	float lastpossent;
	float lastorisent;
	float lastrestock;

	//hitscan
	long *hitcol, hitsid;
	float hitdist, hitdistxy;
	voxlap::extension::LongVector3D hitp, startp, endp;

	aos_byte reload_remaining;
	float lastreload;

	float fmousx, fmousy;
	bool lclk, rclk;
	bool lclicked, rclicked;
	bool luclicked, ruclicked;

	float lastaction;
	float nextaction;
	float curaction;
	float nextaction_secondary;
	float curaction_secondary;

	voxlap::vx5sprite head;
	voxlap::vx5sprite torso;
	voxlap::vx5sprite arms;
	voxlap::vx5sprite left_leg;
	voxlap::vx5sprite right_leg;
	voxlap::vx5sprite dead;
	voxlap::vx5sprite item;
	voxlap::vx5sprite flag;

	void init();

	void local_handle_input(float, float, bool);
	void local_handle_keypress(long, aos_ubyte);
	void local_switch_to_valid_tool();
	void local_update();

	void update();

	void local_set_camera(long, long);
	void draw_local_models();
	void draw_local_switchmodels();

	float render_scale;
	void upd_draw_models();

	void fire_weapon();
	void reload_weapon();

	bool use_block();
	void use_spade(bool);
	void use_grenade(float);

	void update_m_inputs(bool);

	void check_local();
	bool ctf_has_enemy_flag();

	aos_ubyte get_weapon_default_stock();
	aos_ubyte get_weapon_default_ammo();
	float get_weapon_default_fire_delay();
	float get_weapon_default_reload_delay();
	float get_weapon_default_spread();
	float get_weapon_default_recoil();
	float get_weapon_default_bullets();
	float get_tool_primary_delay();
	float get_tool_secondary_delay();
	aos_ubyte get_tool_default_block_damage();
	bool local_get_tool_is_valid(aos_ubyte tool);
	bool get_tool_can_activate();
	bool get_weapon_hide_while_reloading();
	bool get_weapon_can_zoom();

	long hitscan_test(voxlap::point3d *, voxlap::point3d *, bool);
	long hitscan_players(long *, voxlap::point3d *, voxlap::point3d *, bool);

	void reposition(voxlap::point3d *);
	void reposition(float x, float y, float z);
	void reorient(voxlap::point3d *);
	void reorient(float x, float y, float z);
	void reorient_lat(float dx, float dy, float multx, float multy);
	void set_team(aos_ubyte);
	void set_tool(aos_ubyte);
	void set_weapon(aos_ubyte);
	void set_color(long, bool);

	void kill(aos_ubyte);

	void check_restock();
	void restock();
	
	void disconnect();

	bool try_uncrouch();

	void boxclipmove_player();
	long move_player();
	
	void boxclipmove_spectator();
	void move_spectator();

	void boxclipbounce_dead();
	void move_dead();
	
	void existingplayer(aos_byte team, aos_ubyte weapon, aos_ubyte tool, unsigned long kills, long block_color, char* name);
	bool createplayer(aos_byte team, aos_ubyte weapon, float x, float y, float z, char *name);

	void create_existingplayer_packet(AoSPacket_ExistingPlayer *);
	aos_ubyte get_inputdata();
	aos_ubyte get_weaponinput();

	void set_inputdata(aos_ubyte);
	void set_weaponinput(aos_ubyte);
};