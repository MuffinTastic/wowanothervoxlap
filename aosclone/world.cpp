#include "world.h"
#include "vxl.h"

#define NULL 0

TeamType aos_spectator_team;
TeamType aos_team1_team;
TeamType aos_team2_team;
PlayerType aos_players[36];
aos_ubyte aos_local_player_id;
aos_ubyte aos_tc_territory_count = 0;
TC_Territory aos_tc_territories[TC_MAX_TERRITORIES];

TeamType *aos_world_get_team(aos_byte team)
{
	TeamType *ret = NULL;
	switch (team) {
	case ID_TEAM_SPEC:
		ret = &aos_spectator_team;
		break;

	case ID_TEAM1:
		ret = &aos_team1_team;
		break;

	case ID_TEAM2:
		ret = &aos_team2_team;
		break;
	}

	return ret;
}

PlayerType *aos_world_get_local_player()
{
	return &aos_players[aos_local_player_id];
}

#include <voxlap\sysmain.h>
#include <voxlap\voxlap.h>
#include "general.h"
#include "config.h"

#include <math.h>

struct DebrisType : voxlap::vx5sprite
{
	voxlap::point3d v, r;
	long owner;
	double tim;
};

//Debris variables:
static char aos_unitfalldelay[255] =
{
	125, 51, 40, 34, 29, 27, 24, 23, 22, 20, 19, 19, 17, 17, 17, 16,
	15, 15, 14, 15, 13, 14, 13, 13, 13, 12, 12, 12, 12, 11, 11, 12,
	11, 10, 11, 11, 10, 10, 10, 10, 10, 10, 9, 10, 9, 9, 9, 10, 9, 8, 9, 9, 9, 8, 9, 8, 8, 8, 9, 8, 8, 8, 8, 8,
	7, 8, 8, 7, 8, 7, 8, 7, 8, 7, 7, 7, 7, 7, 8, 7, 7, 6, 7, 7, 7, 7, 6, 7, 7, 6, 7, 6, 7, 6, 7, 6,
	7, 6, 6, 7, 6, 6, 6, 6, 6, 6, 7, 6, 6, 6, 5, 6, 6, 6, 6, 6, 6, 5, 6, 6, 6, 5, 6, 5, 6, 6, 5, 6,
	5, 6, 5, 6, 5, 5, 6, 5, 6, 5, 5, 6, 5, 5, 5, 6, 5, 5, 5, 5, 5, 5, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5,
	5, 4, 5, 5, 5, 5, 5, 5, 5, 4, 5, 5, 5, 4, 5, 5, 5, 4, 5, 5, 4, 5, 4, 5, 5, 4, 5, 4, 5, 5, 4, 5,
	4, 5, 4, 5, 4, 4, 5, 4, 5, 4, 4, 5, 4, 5, 4, 4, 5, 4, 4, 5, 4, 4, 4, 5, 4, 4, 4, 5, 4, 4, 4, 4,
	5, 4, 4, 4, 4, 4, 4, 4, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
};

#define MAXDEBRIS 512
static DebrisType aos_debris[MAXDEBRIS];
static long aos_numdebris = 0;

extern double dtotclk;
extern float fsynctics;

void aos_world_update_falling()
{
	long i, j;
	do {
		j = 0;
		voxlap::startfalls();
		for (i = voxlap::vx5.flstnum - 1; i >= 0; i--) {
			if (voxlap::vx5.flstcnt[i].userval == -1) //New piece
			{
				voxlap::vx5.flstcnt[i].userval2 = dtotclk + aos_unitfalldelay[0];
				voxlap::vx5.flstcnt[i].userval = 1;

				//New piece!
				//fp = vx5.flstcnt[i].centroid;
				if ((aos_numdebris < MAXDEBRIS) && (voxlap::meltfall(&aos_debris[aos_numdebris], i, 1))) {
					long k = aos_numdebris++;
					aos_playsound(AOS_SOUND_DEBRIS_BREAK, config::vol, 1.0, &aos_debris[k].p, KSND_3D);

					aos_debris[k].tim = dtotclk; aos_debris[k].owner = 0;
					aos_debris[k].v.x = 0;
					aos_debris[k].v.y = 0;
					aos_debris[k].v.z = 0;

					//aos_debris[k].r.x = (lastbulpos.y - fp.y)*lastbulvel.z - (lastbulpos.z - fp.z)*lastbulvel.y;
					//aos_debris[k].r.y = (lastbulpos.z - fp.z)*lastbulvel.x - (lastbulpos.x - fp.x)*lastbulvel.z;
					//aos_debris[k].r.z = (lastbulpos.x - fp.x)*lastbulvel.y - (lastbulpos.y - fp.y)*lastbulvel.x;

					aos_debris[k].r.x = ((float)rand() / 16383.5f) - 1.f;
					aos_debris[k].r.y = ((float)rand() / 16383.5f) - 1.f;
					aos_debris[k].r.z = ((float)rand() / 16383.5f) - 1.f;
					continue;
				}
			}
			if (dtotclk >= voxlap::vx5.flstcnt[i].userval2) //Timeout: drop piece #i by 1 unit
			{
				//dofall(i);
				voxlap::vx5.flstcnt[i].userval2 += aos_unitfalldelay[voxlap::vx5.flstcnt[i].userval++];
				if (dtotclk >= voxlap::vx5.flstcnt[i].userval2) j = 1;
			}
		}
		voxlap::finishfalls();
	} while (j);

	voxlap::point3d fpos, fp;
	voxlap::lpoint3d lp;

	for (long i = aos_numdebris - 1; i >= 0; i--) {
		if ((dtotclk > aos_debris[i].tim + 1.25f) && (aos_debris[i].owner >= 5)) {
			aos_debris[i].s.x *= .90; aos_debris[i].s.y *= .90; aos_debris[i].s.z *= .90;
			aos_debris[i].h.x *= .90; aos_debris[i].h.y *= .90; aos_debris[i].h.z *= .90;
			aos_debris[i].f.x *= .90; aos_debris[i].f.y *= .90; aos_debris[i].f.z *= .90;
			if (aos_debris[i].s.x*aos_debris[i].s.x + aos_debris[i].s.y*aos_debris[i].s.y + aos_debris[i].s.z*aos_debris[i].s.z < .1*.1)
			{
				if (aos_debris[i].voxnum) {
					//explodesprite(&aos_debris[i], .125, 0, 3);

					free(aos_debris[i].voxnum);
				}
				aos_numdebris--;
				aos_debris[i] = aos_debris[aos_numdebris];
			}
			continue;
		}

		fpos = aos_debris[i].p;

		//Do velocity & gravity
		aos_debris[i].v.z += fsynctics * 64; 
		aos_debris[i].p.x += aos_debris[i].v.x * fsynctics;
		aos_debris[i].p.y += aos_debris[i].v.y * fsynctics;
		aos_debris[i].p.z += aos_debris[i].v.z * fsynctics;

		//Do rotation
		float f = min(dtotclk - aos_debris[i].tim, 250)*fsynctics*0.4f;
		voxlap::axisrotate(&aos_debris[i].s, &aos_debris[i].r, f);
		voxlap::axisrotate(&aos_debris[i].h, &aos_debris[i].r, f);
		voxlap::axisrotate(&aos_debris[i].f, &aos_debris[i].r, f);

		//Make it bounce
		//if (!voxlap::cansee(&fpos, &aos_debris[i].p, &lp))  //Wake up immediately if it hit a wall
		lp = { (long)aos_debris[i].p.x, (long)aos_debris[i].p.y, (long)aos_debris[i].p.z };
		if (voxlap::isvoxelsolid(lp.x, lp.y, lp.z))
		{
			aos_debris[i].p = fpos;
			voxlap::estnorm(lp.x, lp.y, lp.z, &fp);
			f = (aos_debris[i].v.x*fp.x + aos_debris[i].v.y*fp.y + aos_debris[i].v.z*fp.z)*2.0f;
			aos_debris[i].v.x = (aos_debris[i].v.x - fp.x*f)*.5f;
			aos_debris[i].v.y = (aos_debris[i].v.y - fp.y*f)*.5f;
			aos_debris[i].v.z = (aos_debris[i].v.z - fp.z*f)*.5f;
			//aos_debris[i].v.x = -fp.x * 10.0f;
			//aos_debris[i].v.y = -fp.y * 10.0f;
			//aos_debris[i].v.z = -fp.z * 10.0f;
			
			aos_debris[i].r.x = ((float)rand() / 16383.5f) - 1.f;
			aos_debris[i].r.y = ((float)rand() / 16383.5f) - 1.f;
			aos_debris[i].r.z = ((float)rand() / 16383.5f) - 1.f;

			aos_debris[i].owner++;

			#define DEBRIS_BOUNCE_SOUND_THRESHOLD 0.1f
			if (fabsf(aos_debris[i].v.x) > DEBRIS_BOUNCE_SOUND_THRESHOLD ||
				fabsf(aos_debris[i].v.y) > DEBRIS_BOUNCE_SOUND_THRESHOLD || 
				fabsf(aos_debris[i].v.z) > DEBRIS_BOUNCE_SOUND_THRESHOLD)
				aos_playsound(AOS_SOUND_DEBRIS_BOUNCE, config::vol, 1.0, &aos_debris[i].p, KSND_3D);
		}
	}
}

void aos_world_draw_falling()
{
	for (long i = aos_numdebris - 1; i >= 0; i--) {
		voxlap::drawsprite(&aos_debris[i]);
	}
}

void aos_world_update_team_entities()
{
	if (aos_gamemode == ID_GAMEMODE_TC) {
		for (int i = 0; i < aos_tc_territory_count; i++) {
			TC_Territory *territory = &aos_tc_territories[i];
			if (territory->rate != 0) {
				territory->progress = min(max(0.0, territory->progress + (double)territory->rate * TC_CAPTURE_RATE * (double)fsynctics), 1.0);
			}
		}
	}
}

#define AOS_BASE_SCALE (1.0f / 3.3f)
#define AOS_FLAG_SCALE (1.0f / 5.5f)

void aos_world_draw_team_entities()
{
	voxlap::vx5sprite sprite; sprite.flags = 0;

	voxlap::extension::FloatVector3D sstr = { 1.0, 0.0, 0.0 };
	voxlap::extension::FloatVector3D shei = { 0.0, 1.0, 0.0 };
	voxlap::extension::FloatVector3D sfor = { 0.0, 0.0, 1.0 };
	voxlap::extension::FloatVector3D tmp(0);
	sprite.s = sstr; sprite.h = shei; sprite.f = sfor;

	if (aos_gamemode == ID_GAMEMODE_CTF) {
		sprite.voxnum = aos_team1_team.ctf_base_voxnum;
		sprite.p = aos_team1_team.ctf_base_pos;
		sprite.s = sstr * AOS_BASE_SCALE;
		sprite.h = shei * AOS_BASE_SCALE;
		sprite.f = sfor * AOS_BASE_SCALE;
		voxlap::drawsprite(&sprite);

		sprite.voxnum = aos_team2_team.ctf_base_voxnum;
		sprite.p = aos_team2_team.ctf_base_pos;
		sprite.s = sstr * AOS_BASE_SCALE;
		sprite.h = shei * AOS_BASE_SCALE;
		sprite.f = sfor * AOS_BASE_SCALE;
		voxlap::drawsprite(&sprite);

		voxlap::axisrotate(&sstr, &sfor, dtotclk);
		voxlap::axisrotate(&shei, &sfor, dtotclk);
		tmp = { 0.5f, 0.5f, 0 };

		if (aos_team1_team.ctf_flag_dropped) {
			sprite.voxnum = aos_team1_team.ctf_flag_voxnum;
			if ((int)sprite.p.x == 256 && (int)sprite.p.y == 256) // center of map
				sprite.p = aos_team1_team.ctf_flag_pos;
			else
				sprite.p = aos_team1_team.ctf_flag_pos + tmp;
			sprite.s = sstr * AOS_FLAG_SCALE;
			sprite.h = shei * AOS_FLAG_SCALE;
			sprite.f = sfor * AOS_FLAG_SCALE;
			voxlap::drawsprite(&sprite);
		}

		if (aos_team2_team.ctf_flag_dropped && aos_team1_team.ctf_flag_pos != aos_team2_team.ctf_flag_pos) {
			sprite.voxnum = aos_team2_team.ctf_flag_voxnum;
			if ((int)sprite.p.x == 256 && (int)sprite.p.y == 256)
				sprite.p = aos_team2_team.ctf_flag_pos;
			else
				sprite.p = aos_team2_team.ctf_flag_pos + tmp;
			sprite.s = sstr * AOS_FLAG_SCALE;
			sprite.h = shei * AOS_FLAG_SCALE;
			sprite.f = sfor * AOS_FLAG_SCALE;
			voxlap::drawsprite(&sprite);
		}
	} else if (aos_gamemode == ID_GAMEMODE_TC) {
		sprite.s = sstr * AOS_BASE_SCALE;
		sprite.h = shei * AOS_BASE_SCALE;
		sprite.f = sfor * AOS_BASE_SCALE;
		for (int i = 0; i < aos_tc_territory_count; i++) {
			TC_Territory *territory = &aos_tc_territories[i];
			sprite.voxnum = territory->voxnum;
			sprite.p = territory->position;
			voxlap::drawsprite(&sprite);
		}
	}
}

TC_Territory *aos_world_get_closest_territory(voxlap::extension::FloatVector3D *position)
{
	for (int i = 0; i < aos_tc_territory_count; i++) {
		TC_Territory *territory = &aos_tc_territories[i];
		voxlap::extension::FloatVector3D dif = (territory->position - *position).absolute();
		if (dif.x < TC_CAPTURE_DISTANCE &&
			dif.y < TC_CAPTURE_DISTANCE &&
			dif.z < TC_CAPTURE_DISTANCE)
			return territory;
	}

	return NULL;
}

void aos_world_moveobject(aos_ubyte id, aos_ubyte team, float x, float y, float z)
{
	if (aos_gamemode == ID_GAMEMODE_CTF) {
		switch (id) {
		case ID_TEAM1_BASE:
			aos_team1_team.ctf_base_pos = { x, y, z };
			break;
		case ID_TEAM2_BASE:
			aos_team2_team.ctf_base_pos = { x, y, z };
			break;
		case ID_TEAM1_FLAG:
			aos_team1_team.ctf_flag_pos = { x, y, z };
			break;
		case ID_TEAM2_FLAG:
			aos_team2_team.ctf_flag_pos = { x, y, z };
			break;
		}
	} else if (aos_gamemode == ID_GAMEMODE_TC) {
		if (id >= aos_tc_territory_count) return;
		if (team > 2) return;
		aos_tc_territories[id].position = { x, y, z };
		aos_tc_territories[id].team = team;
		aos_tc_territories[id].voxnum = aos_get_model(AOS_MODEL_TEAM1_CAPTURE_POINT, team, false);
	}
}

void aos_world_moveobject(aos_ubyte id, aos_ubyte team)
{
	if (aos_gamemode == ID_GAMEMODE_TC) {
		if (id >= aos_tc_territory_count) return;
		if (team > 2) return;
		aos_tc_territories[id].team = team;
		aos_tc_territories[id].voxnum = aos_get_model(AOS_MODEL_TEAM1_CAPTURE_POINT, team, false);
	}
}

static const char *const sectors[64] = {
	"A1", "B1", "C1", "D1", "E1", "F1", "G1", "H1",

	"A2", "B2", "C2", "D2", "E2", "F2", "G2", "H2",

	"A3", "B3", "C3", "D3", "E3", "F3", "G3", "H3",

	"A4", "B4", "C4", "D4", "E4", "F4", "G4", "H4",

	"A5", "B5", "C5", "D5", "E5", "F5", "G5", "H5",

	"A6", "B6", "C6", "D6", "E6", "F6", "G6", "H6",

	"A7", "B7", "C7", "D7", "E7", "F7", "G7", "H7",

	"A8", "B8", "C8", "D8", "E8", "F8", "G8", "H8",
};
static const char *const sector_ohno = "oh no...";

const char *aos_world_get_sector(float x, float y)
{
	long cursec_x = (long)x / 64;
	long cursec_y = (long)y / 64;
	if (cursec_x < 0 || cursec_y < 0 || cursec_x >= 8 || cursec_y >= 8)
		return sector_ohno;
	return sectors[cursec_x + 8 * cursec_y];
}

void aos_world_render_wirecube(long x, long y, long z, long col)
{
	voxlap::drawline3d(x,			y,			z,			x + 1.0f,	y,			z,			col);
	voxlap::drawline3d(x,			y + 1.0f,	z,			x + 1.0f,	y + 1.0f,	z,			col);
	voxlap::drawline3d(x,			y + 1.0f,	z + 1.0f,	x + 1.0f,	y + 1.0f,	z + 1.0f,	col);
	voxlap::drawline3d(x,			y,			z + 1.0f,	x + 1.0f,	y,			z + 1.0f,	col);
	voxlap::drawline3d(x,			y,			z,			x,			y + 1.0f,	z,			col);
	voxlap::drawline3d(x + 1.0f,	y,			z,			x + 1.0f,	y + 1.0f,	z,			col);
	voxlap::drawline3d(x + 1.0f,	y,			z + 1.0f,	x + 1.0f,	y + 1.0f,	z + 1.0f,	col);
	voxlap::drawline3d(x,			y,			z + 1.0f,	x,			y + 1.0f,	z + 1.0f,	col);
	voxlap::drawline3d(x,			y,			z,			x,			y,			z + 1.0f,	col);
	voxlap::drawline3d(x + 1.0f,	y,			z,			x + 1.0f,	y,			z + 1.0f,	col);
	voxlap::drawline3d(x + 1.0f,	y + 1.0f,	z,			x + 1.0f,	y + 1.0f,	z + 1.0f,	col);
	voxlap::drawline3d(x,			y + 1.0f,	z,			x,			y + 1.0f,	z + 1.0f,	col);
}

typedef struct BulletType
{
	voxlap::kv6data *voxnum;
	voxlap::extension::FloatVector3D pos;
	voxlap::extension::FloatVector3D ori;
	float time;
};

#define BULLET_POINTER_BASED 0
#define BULLET_STATIC 0

#define MAXBULLETS 512
#define BULLET_SCALE (1.0f / 16.0f)
#define BULLET_VELOCITY 256.0f
#define BULLET_LIFESPAN 5.0f // 5s

#if (BULLET_POINTER_BASED == 1)

#if (BULLET_STATIC == 1)
static bullettype *aos_world_bullets[MAXBULLETS];
static long aos_world_numbullets = 0;
#else
bullettype *aos_world_bullets[MAXBULLETS];
long aos_world_numbullets = 0;
#endif

void aos_world_create_bullet(voxlap::point3d *pos, voxlap::point3d *ori, aos_ubyte weapon)
{
	if (aos_world_numbullets >= MAXBULLETS) return;
	PlayerType *cur_pl = aos_world_get_local_player();
	if (((cur_pl->cam_position - *pos) * voxlap::point3d{ 1.0f, 1.0f, 0.0f }).lengthsquared() > 128.0*128.0) return;

	bullettype *bullet = (bullettype *)malloc(sizeof(bullettype));
	if (bullet == NULL) return;

	switch (weapon) {
	case WEAPON_SEMI:
	default:
		bullet->voxnum = aos_get_model(AOS_MODEL_TRACER_SEMI, false, false);
		break;
	case WEAPON_SMG:
		bullet->voxnum = aos_get_model(AOS_MODEL_TRACER_SMG, false, false);
		break;
	case WEAPON_SHOTGUN:
		bullet->voxnum = aos_get_model(AOS_MODEL_TRACER_SHOTGUN, false, false);
		break;
	}

	bullet->ori = *ori;
	bullet->pos = bullet->ori * BULLET_VELOCITY * fsynctics + *pos;

	bullet->time = dtotclk;

	aos_world_bullets[aos_world_numbullets++] = bullet;
}

void aos_world_update_bullets()
{
	PlayerType *cur_pl = aos_world_get_local_player();
	bullettype *bullet;
	for (long i = aos_world_numbullets - 1; i >= 0; i--) {
		bullet = aos_world_bullets[i];

		voxlap::point3d oldpos = bullet->pos;
		bullet->pos += bullet->ori * BULLET_VELOCITY * fsynctics;

		voxlap::lpoint3d collid;

		bool delete_bullet = false;
		if (((cur_pl->cam_position - bullet->pos) * voxlap::point3d{ 1.0f, 1.0f, 0.0f }).lengthsquared() > 128.0*128.0 // beyond fog
			|| bullet->time + BULLET_LIFESPAN < dtotclk // too old
			|| !voxlap::cansee(&oldpos, &bullet->pos, &collid)) // colliding with block
		{
			aos_world_numbullets--;
			aos_world_bullets[i] = aos_world_bullets[aos_world_numbullets];
			//*bullet = { 0 };
			free(bullet); bullet = NULL;
			// create bullet splash effect!!!
		}
	}
}

void aos_world_draw_bullets()
{
	voxlap::extension::FloatVector3D str, hei;
	voxlap::vx5sprite sprite;
	sprite.flags = 0;
	for (long i = 0; i < aos_world_numbullets; i++) {
		bullettype *bullet = aos_world_bullets[i];

		sprite.voxnum = bullet->voxnum;
		set_orientation_vectors(&bullet->ori, &str, &hei);

		sprite.s = str * -BULLET_SCALE;
		sprite.h = bullet->ori * BULLET_SCALE;
		sprite.f = hei *  BULLET_SCALE;

		sprite.p = bullet->pos - hei * BULLET_SCALE / 2 + str * BULLET_SCALE / 2;

		voxlap::drawsprite(&sprite);
	}
}
#else

#if (BULLET_STATIC == 1)
static bullettype aos_world_bullets[MAXBULLETS];
static long aos_world_numbullets = 0;
#else
BulletType aos_world_bullets[MAXBULLETS];
long aos_world_numbullets = 0;
#endif

void aos_world_create_bullet(voxlap::point3d pos, voxlap::point3d ori, aos_ubyte weapon)
{
	if (aos_world_numbullets >= MAXBULLETS) return;
	PlayerType *cur_pl = aos_world_get_local_player();
	if (((cur_pl->cam_position - pos) * voxlap::point3d{ 1.0f, 1.0f, 0.0f }).lengthsquared() > 128.0*128.0) return;

	BulletType bullet = { 0 };

	switch (weapon) {
	case WEAPON_SEMI:
	default:
		bullet.voxnum = aos_get_model(AOS_MODEL_TRACER_SEMI, false, false);
		break;
	case WEAPON_SMG:
		bullet.voxnum = aos_get_model(AOS_MODEL_TRACER_SMG, false, false);
		break;
	case WEAPON_SHOTGUN:
		bullet.voxnum = aos_get_model(AOS_MODEL_TRACER_SHOTGUN, false, false);
		break;
	}

	bullet.ori = ori;
	bullet.pos = bullet.ori * BULLET_VELOCITY * fsynctics + pos;

	bullet.time = dtotclk;

	aos_world_bullets[aos_world_numbullets++] = bullet;
}

void aos_world_update_bullets()
{
	PlayerType *cur_pl = aos_world_get_local_player();
	BulletType *bullet;
	for (long i = aos_world_numbullets - 1; i >= 0; i--) {
		bullet = &aos_world_bullets[i];

		voxlap::point3d oldpos = bullet->pos;
		bullet->pos += bullet->ori * BULLET_VELOCITY * fsynctics;

		voxlap::lpoint3d lp;
		lp.x = (long)floor(bullet->pos.x);
		lp.y = (long)floor(bullet->pos.y);
		lp.z = (long)floor(bullet->pos.z);

		bool collided = voxlap::isvoxelsolid(lp.x, lp.y, lp.z);
		if (((cur_pl->cam_position - bullet->pos) * voxlap::point3d{ 1.0f, 1.0f, 0.0f }).lengthsquared() > 128.0*128.0 // beyond fog
			|| bullet->time + BULLET_LIFESPAN < dtotclk // too old
			|| collided) // colliding with block
		{
			if (collided) {
				// create bullet splash effect!!!
			}

			aos_world_numbullets--;
			aos_world_bullets[i] = aos_world_bullets[aos_world_numbullets];
			*bullet = { 0 };
		}
	}
}

void aos_world_draw_bullets()
{
	voxlap::extension::FloatVector3D str, hei;
	voxlap::vx5sprite sprite;
	sprite.flags = 0;
	for (long i = 0; i < aos_world_numbullets; i++) {
		BulletType *bullet = &aos_world_bullets[i];

		sprite.voxnum = bullet->voxnum;
		set_orientation_vectors(&bullet->ori, &str, &hei);

		sprite.s = str * -BULLET_SCALE;
		sprite.h = bullet->ori * BULLET_SCALE;
		sprite.f = hei *  BULLET_SCALE;

		sprite.p = bullet->pos - hei * BULLET_SCALE / 2.0f + str * BULLET_SCALE / 2.0f;

		voxlap::drawsprite(&sprite);
	}
}
#endif

// i got tired of writing it out
using FVector3D = voxlap::extension::FloatVector3D;

struct GrenadeType : voxlap::vx5sprite
{
	FVector3D v;
	int __pad;
	double fuse;
};

#define MAXGRENADES 512
static GrenadeType aos_world_grenades[MAXGRENADES];
static long aos_world_numgrenades = 0;
#define GRENADE_SCALE (1.0f / 24.0f)

void aos_world_create_grenade(voxlap::point3d pos, voxlap::point3d vel, float fuse)
{
	if (aos_world_numgrenades >= MAXGRENADES) return;
	
	FVector3D p, h, s, f;
	p = pos;

	GrenadeType grenade;
	
	grenade.flags = 0;
	grenade.voxnum = aos_get_model(AOS_MODEL_GRENADE, false, false);

	h = FVector3D(0.0f, 1.0f, 0.0f) * GRENADE_SCALE;
	s = FVector3D(1.0f, 0.0f, 0.0f) * GRENADE_SCALE;
	f = FVector3D(0.0f, 0.0f, 1.0f) * GRENADE_SCALE;

	grenade.h = h;
	grenade.s = s;
	grenade.f = f;

	grenade.p = p - h / 2.0f + s / 2.0f;
	grenade.v = vel;

	grenade.fuse = dtotclk + fuse;

	aos_world_grenades[aos_world_numgrenades++] = grenade;
}

// returns 1 if there was a collision, 2 if sound should be played
long aos_world_move_grenade(GrenadeType *grenade)
{
	voxlap::point3d fpos = grenade->p; //old position
	//do velocity & gravity (friction is negligible)
	float f = fsynctics * 32;
	grenade->v.z += fsynctics;
	grenade->p = grenade->v * f + grenade->p;
	//do rotation
	//FIX ME: Loses orientation after 45 degree bounce off wall
	// if(g->v.x > 0.1f || g->v.x < -0.1f || g->v.y > 0.1f || g->v.y < -0.1f)
	// {
		// f *= -0.5;
	// }
	//make it bounce (accurate)
	voxlap::lpoint3d lp;
	lp.x = (long)floor(grenade->p.x);
	lp.y = (long)floor(grenade->p.y);
	lp.z = (long)floor(grenade->p.z);

	long ret = 0;

	if (aos_vxl_clipbox(lp.x, lp.y, lp.z))  //hit a wall
	{
		#define GRENADE_BOUNCE_SOUND_THRESHOLD 0.1f

		ret = 1;
		if (fabs(grenade->v.x) > GRENADE_BOUNCE_SOUND_THRESHOLD ||
			fabs(grenade->v.y) > GRENADE_BOUNCE_SOUND_THRESHOLD ||
			fabs(grenade->v.z) > GRENADE_BOUNCE_SOUND_THRESHOLD)
			ret = 2; // play sound

		voxlap::lpoint3d lp2;
		lp2.x = (long)floor(fpos.x);
		lp2.y = (long)floor(fpos.y);
		lp2.z = (long)floor(fpos.z);
		if (lp.z != lp2.z && ((lp.x == lp2.x && lp.y == lp2.y) || !aos_vxl_clipbox(lp.x, lp.y, lp2.z)))
			grenade->v.z = -grenade->v.z;
		else if (lp.x != lp2.x && ((lp.y == lp2.y && lp.z == lp2.z) || !aos_vxl_clipbox(lp2.x, lp.y, lp.z)))
			grenade->v.x = -grenade->v.x;
		else if (lp.y != lp2.y && ((lp.x == lp2.x && lp.z == lp2.z) || !aos_vxl_clipbox(lp.x, lp2.y, lp.z)))
			grenade->v.y = -grenade->v.y;
		grenade->p = fpos; //set back to old position
		grenade->v *= 0.36f;
	}
	return ret;
}

void aos_world_update_grenades()
{
	GrenadeType *grenade;
	for (long i = aos_world_numgrenades - 1; i >= 0; i--) {
		grenade = &aos_world_grenades[i];

		FVector3D xyv = grenade->v; xyv.z = 0;
		float f = xyv.length();

		if (f > 0.0f) {
			float rf = sqrtf(xyv.x * xyv.x + xyv.y * xyv.y);
			voxlap::point3d r = { -xyv.y / f, xyv.x / f, 0.0f };

			voxlap::axisrotate(&grenade->s, &r, -fsynctics * 16.0f * f);
			voxlap::axisrotate(&grenade->h, &r, -fsynctics * 16.0f * f);
			voxlap::axisrotate(&grenade->f, &r, -fsynctics * 16.0f * f);
		}

		long collided = aos_world_move_grenade(grenade);
		if (collided == 2) {
			aos_playsound(AOS_SOUND_GRENADE_BOUNCE, config::vol, 1.0, &grenade->p, KSND_3D);
		}

		if (grenade->fuse < dtotclk)
		{
			if (grenade->p.z < 62.0f)
				aos_playsound(AOS_SOUND_GRENADE_EXPLODE, config::vol, 1.0, &grenade->p, KSND_3D);
			else
				aos_playsound(AOS_SOUND_GRENADE_WATER_EXPLODE, config::vol, 1.0, &grenade->p, KSND_3D);

			// create grenade splash effect!!!

			aos_world_numgrenades--;
			aos_world_grenades[i] = aos_world_grenades[aos_world_numgrenades];
		}
	}
}

void aos_world_draw_grenades()
{
	for (long i = aos_world_numgrenades - 1; i >= 0; i--) {
		GrenadeType* grenade = &aos_world_grenades[i];
		voxlap::drawsprite(grenade);
	}
}

#define MAX_BLOCK_DAMAGE     4 // confusing? meh
#define BLOCK_DAMAGE_TIME 10.f // seconds

blockdamage aos_world_damage[MAX_DAMAGED_BLOCKS];
long aos_world_damagenum = 0;

blockdamage *get_existing_block_damage(long x, long y, long z)
{
	blockdamage *p;
	for (int i = 0; i < aos_world_damagenum; i++) {
		p = &aos_world_damage[i];
		if (p->i == AOS_WORLD_DMG_GET_I(x, y, z))
			return p;
	}
	return NULL;
}

#include "networking.h"
#include "packet.h"

bool aos_world_block_damage(long x, long y, long z, aos_ubyte dmg, bool destroy)
{
	if (x < 0 || x > 511 ||
		y < 0 || y > 511 ||
		z < 0 || z >= 62)
		return false;

	blockdamage *data = get_existing_block_damage(x, y, z);

	if (data != NULL) {
		data->time = (float)dtotclk;
		data->damage += dmg;
		aos_ubyte dmg = data->damage;
		data->damage = min(data->damage, MAX_BLOCK_DAMAGE);
		if (!data->destroyed && destroy) {
			if (dmg > MAX_BLOCK_DAMAGE) {
				data->destroyed = true;

				AoSPacket_BlockAction blockaction;
				blockaction.playerID = aos_local_player_id;
				blockaction.x = x;
				blockaction.y = y;
				blockaction.z = z;
				blockaction.type = ID_BLOCK_ACTION_DESTROY;
				aos_network_send(ID_BLOCK_ACTION, &blockaction, sizeof(AoSPacket_BlockAction), true);

				return true;
			}
		}
	} else {
		if (aos_world_damagenum >= MAX_DAMAGED_BLOCKS) return false;
		data = &aos_world_damage[aos_world_damagenum];

		data->i = AOS_WORLD_DMG_GET_I(x, y, z);
		data->time = (float)dtotclk;
		data->damage = dmg;
		data->destroyed = false;
		
		aos_world_damagenum++;
	}

	return false;
}

void aos_world_block_damage_clear(long x, long y, long z)
{
	blockdamage *p;
	for (int i = 0; i < aos_world_damagenum; i++) {
		p = &aos_world_damage[i];
		if (p->i == AOS_WORLD_DMG_GET_I(x, y, z)) {
			aos_world_damagenum--;
			aos_world_damage[i] = aos_world_damage[aos_world_damagenum];
			*p = { 0 };
			if (voxlap::isvoxelsolid(x, y, z))
				aos_vxl_lighting_updateblock(x, y, z);
		}
	}
}

void aos_world_update_block_damage()
{
	blockdamage *dmg;
	long x, y, z;
	for (int i = aos_world_damagenum - 1; i >= 0; i--) {
		dmg = &aos_world_damage[i];

		aos_world_damage_xyz(&x, &y, &z, dmg->i);

		if (dmg->damage == 0 || dmg->time + BLOCK_DAMAGE_TIME < (float)dtotclk ||
			!voxlap::isvoxelsolid(x, y, z)) {
			aos_world_damagenum--;
			aos_world_damage[i] = aos_world_damage[aos_world_damagenum];
			*dmg = { 0 };
			if (voxlap::isvoxelsolid(x, y, z))
				aos_vxl_lighting_updateblock(x, y, z);
		}
	}
}