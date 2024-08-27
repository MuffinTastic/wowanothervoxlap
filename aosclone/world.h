#pragma once

#include "general.h"
#include "player.h"
#include "team.h"

#include <voxlap\extension\FloatVector3D.h>

extern TeamType aos_spectator_team;
extern TeamType aos_team1_team;
extern TeamType aos_team2_team;

extern PlayerType		aos_players[36];
extern aos_ubyte		aos_local_player_id;
PlayerType *			aos_world_get_local_player();

extern aos_ubyte	aos_tc_territory_count;
extern TC_Territory	aos_tc_territories[TC_MAX_TERRITORIES];

TeamType *aos_world_get_team(aos_byte);

void aos_world_update_falling();
void aos_world_draw_falling();

void aos_world_update_team_entities();

void aos_world_draw_team_entities();

TC_Territory *	aos_world_get_closest_territory(voxlap::extension::FloatVector3D *);
void			aos_world_moveobject(aos_ubyte, aos_ubyte, float, float, float);
void			aos_world_moveobject(aos_ubyte, aos_ubyte);

const char *aos_world_get_sector(float, float);

void aos_world_render_wirecube(long, long, long, long);

void aos_world_create_bullet(voxlap::point3d, voxlap::point3d, aos_ubyte);
void aos_world_update_bullets();
void aos_world_draw_bullets();

void aos_world_create_grenade(voxlap::point3d, voxlap::point3d, float);
void aos_world_update_grenades();
void aos_world_draw_grenades();

#define MAX_DAMAGED_BLOCKS 512

#define AOS_WORLD_DMG_GET_I(x, y, z) (x | y << 9 | z << 18)

inline void aos_world_damage_xyz(long *x, long *y, long *z, long i)
{
	*x = i & 511;
	*y = (i >> 9) & 511;
	*z = (i >> 18) & 511;
}

typedef struct blockdamage
{
	long i;
	float time;
	aos_ubyte damage;
	bool destroyed;
};

extern blockdamage aos_world_damage[MAX_DAMAGED_BLOCKS];
extern long aos_world_damagenum;

bool		 aos_world_block_damage(long, long, long, aos_ubyte, bool);
void		 aos_world_block_damage_clear(long, long, long);
void		 aos_world_update_block_damage();

//void aos_world_