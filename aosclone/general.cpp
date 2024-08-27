#include "general.h"

#include <Windows.h>
#include <stdio.h>

#include "constants.h"

GameState aos_gamestate;

void showError(const char *format, ...)
{
	char* argslist;
	char text[256];

	va_start(argslist, format);
	vsprintf(text, format, argslist);
	va_end(argslist);

	MessageBoxA(NULL, text, GAME_TITLE, MB_OK);
}


long make_color(unsigned char red, unsigned char green, unsigned char blue)
{
	return (red << 16) | (green << 8) | blue;
}

void get_color(unsigned char *red, unsigned char *green, unsigned char *blue, long color)
{
	*red   = (unsigned char)((color & 0xFF0000) >> 16);
	*green = (unsigned char)((color & 0x00FF00) >> 8);
	*blue  = (unsigned char)(color & 0x0000FF);
}

voxlap::kv6data *aos_models[256]; // increase number of models if necessary

bool aos_init_models()
{
	aos_models[AOS_MODEL_TEAM1_PLAYER_HEAD] = voxlap::loadkv6("kv6/playerhead.kv6");
	aos_models[AOS_MODEL_TEAM1_PLAYER_ARMS] = voxlap::loadkv6("kv6/playerarms.kv6");
	aos_models[AOS_MODEL_TEAM1_PLAYER_TORSO_UNCROUCHED] = voxlap::loadkv6("kv6/playertorso.kv6");
	aos_models[AOS_MODEL_TEAM1_PLAYER_TORSO_CROUCHED] = voxlap::loadkv6("kv6/playertorsoc.kv6");
	aos_models[AOS_MODEL_TEAM1_PLAYER_LEG_UNCROUCHED] = voxlap::loadkv6("kv6/playerleg.kv6");
	aos_models[AOS_MODEL_TEAM1_PLAYER_LEG_CROUCHED] = voxlap::loadkv6("kv6/playerlegc.kv6");
	aos_models[AOS_MODEL_TEAM1_PLAYER_DEAD] = voxlap::loadkv6("kv6/playerdead.kv6");
	aos_models[AOS_MODEL_TEAM1_CAPTURE_POINT] = voxlap::loadkv6("kv6/cp.kv6");
	aos_models[AOS_MODEL_TEAM1_INTEL] = voxlap::loadkv6("kv6/intel.kv6");

	aos_models[AOS_MODEL_TEAM2_PLAYER_HEAD] = voxlap::loadkv6("kv6/playerhead.kv6");
	aos_models[AOS_MODEL_TEAM2_PLAYER_ARMS] = voxlap::loadkv6("kv6/playerarms.kv6");
	aos_models[AOS_MODEL_TEAM2_PLAYER_TORSO_UNCROUCHED] = voxlap::loadkv6("kv6/playertorso.kv6");
	aos_models[AOS_MODEL_TEAM2_PLAYER_TORSO_CROUCHED] = voxlap::loadkv6("kv6/playertorsoc.kv6");
	aos_models[AOS_MODEL_TEAM2_PLAYER_LEG_UNCROUCHED] = voxlap::loadkv6("kv6/playerleg.kv6");
	aos_models[AOS_MODEL_TEAM2_PLAYER_LEG_CROUCHED] = voxlap::loadkv6("kv6/playerlegc.kv6");
	aos_models[AOS_MODEL_TEAM2_PLAYER_DEAD] = voxlap::loadkv6("kv6/playerdead.kv6");
	aos_models[AOS_MODEL_TEAM2_CAPTURE_POINT] = voxlap::loadkv6("kv6/cp.kv6");
	aos_models[AOS_MODEL_TEAM2_INTEL] = voxlap::loadkv6("kv6/intel.kv6");

	aos_models[AOS_MODEL_TOOL_SPADE] = voxlap::loadkv6("kv6/spade.kv6");
	aos_models[AOS_MODEL_TOOL_BLOCK] = voxlap::loadkv6("kv6/block.kv6");
	aos_models[AOS_MODEL_WEAPON_SEMI] = voxlap::loadkv6("kv6/semi.kv6");
	aos_models[AOS_MODEL_WEAPON_SMG] = voxlap::loadkv6("kv6/smg.kv6");
	aos_models[AOS_MODEL_WEAPON_SHOTGUN] = voxlap::loadkv6("kv6/shotgun.kv6");
	aos_models[AOS_MODEL_GRENADE] = voxlap::loadkv6("kv6/grenade.kv6");

	aos_models[AOS_MODEL_TRACER_SEMI] = voxlap::loadkv6("kv6/semitracer.kv6");
	aos_models[AOS_MODEL_TRACER_SMG] = voxlap::loadkv6("kv6/smgtracer.kv6");
	aos_models[AOS_MODEL_TRACER_SHOTGUN] = voxlap::loadkv6("kv6/shotguntracer.kv6");

	aos_models[AOS_MODEL_CASING_SEMI] = voxlap::loadkv6("kv6/semicasing.kv6");
	aos_models[AOS_MODEL_CASING_SMG] = voxlap::loadkv6("kv6/smgcasing.kv6");
	aos_models[AOS_MODEL_CASING_SHOTGUN] = voxlap::loadkv6("kv6/shotguncasing.kv6");

	if (aos_models[AOS_MODEL_TEAM1_PLAYER_HEAD] == NULL ||
		aos_models[AOS_MODEL_TEAM1_PLAYER_ARMS] == NULL ||
		aos_models[AOS_MODEL_TEAM1_PLAYER_TORSO_UNCROUCHED] == NULL ||
		aos_models[AOS_MODEL_TEAM1_PLAYER_TORSO_CROUCHED] == NULL ||
		aos_models[AOS_MODEL_TEAM1_PLAYER_LEG_UNCROUCHED] == NULL ||
		aos_models[AOS_MODEL_TEAM1_PLAYER_LEG_CROUCHED] == NULL ||
		aos_models[AOS_MODEL_TEAM1_PLAYER_DEAD] == NULL ||
		aos_models[AOS_MODEL_TEAM1_CAPTURE_POINT] == NULL ||
		aos_models[AOS_MODEL_TEAM1_INTEL] == NULL ||
		
		aos_models[AOS_MODEL_TEAM2_PLAYER_HEAD] == NULL ||
		aos_models[AOS_MODEL_TEAM2_PLAYER_ARMS] == NULL ||
		aos_models[AOS_MODEL_TEAM2_PLAYER_TORSO_UNCROUCHED] == NULL ||
		aos_models[AOS_MODEL_TEAM2_PLAYER_TORSO_CROUCHED] == NULL ||
		aos_models[AOS_MODEL_TEAM2_PLAYER_LEG_UNCROUCHED] == NULL ||
		aos_models[AOS_MODEL_TEAM2_PLAYER_LEG_CROUCHED] == NULL ||
		aos_models[AOS_MODEL_TEAM2_PLAYER_DEAD] == NULL ||
		aos_models[AOS_MODEL_TEAM2_CAPTURE_POINT] == NULL ||
		aos_models[AOS_MODEL_TEAM2_INTEL] == NULL ||
		
		aos_models[AOS_MODEL_TOOL_SPADE] == NULL ||
		aos_models[AOS_MODEL_TOOL_BLOCK] == NULL ||
		aos_models[AOS_MODEL_WEAPON_SEMI] == NULL ||
		aos_models[AOS_MODEL_WEAPON_SMG] == NULL ||
		aos_models[AOS_MODEL_WEAPON_SHOTGUN] == NULL ||
		aos_models[AOS_MODEL_GRENADE] == NULL ||

		aos_models[AOS_MODEL_TRACER_SEMI] == NULL ||
		aos_models[AOS_MODEL_TRACER_SMG] == NULL ||
		aos_models[AOS_MODEL_TRACER_SHOTGUN] == NULL ||

		aos_models[AOS_MODEL_CASING_SEMI] == NULL ||
		aos_models[AOS_MODEL_CASING_SMG] == NULL ||
		aos_models[AOS_MODEL_CASING_SHOTGUN] == NULL)
		return false;

	aos_models[AOS_MODEL_TEAM1_PLAYER_HEAD]->lowermip = 0;
	aos_models[AOS_MODEL_TEAM1_PLAYER_ARMS]->lowermip = 0;
	aos_models[AOS_MODEL_TEAM1_PLAYER_TORSO_UNCROUCHED]->lowermip = 0;
	aos_models[AOS_MODEL_TEAM1_PLAYER_TORSO_CROUCHED]->lowermip = 0;
	aos_models[AOS_MODEL_TEAM1_PLAYER_LEG_UNCROUCHED]->lowermip = 0;
	aos_models[AOS_MODEL_TEAM1_PLAYER_LEG_CROUCHED]->lowermip = 0;
	aos_models[AOS_MODEL_TEAM1_PLAYER_DEAD]->lowermip = 0;
	aos_models[AOS_MODEL_TEAM1_CAPTURE_POINT]->lowermip = 0;
	aos_models[AOS_MODEL_TEAM1_INTEL]->lowermip = 0;

	aos_models[AOS_MODEL_TEAM2_PLAYER_HEAD]->lowermip = 0;
	aos_models[AOS_MODEL_TEAM2_PLAYER_ARMS]->lowermip = 0;
	aos_models[AOS_MODEL_TEAM2_PLAYER_TORSO_UNCROUCHED]->lowermip = 0;
	aos_models[AOS_MODEL_TEAM2_PLAYER_TORSO_CROUCHED]->lowermip = 0;
	aos_models[AOS_MODEL_TEAM2_PLAYER_LEG_UNCROUCHED]->lowermip = 0;
	aos_models[AOS_MODEL_TEAM2_PLAYER_LEG_CROUCHED]->lowermip = 0;
	aos_models[AOS_MODEL_TEAM2_PLAYER_DEAD]->lowermip = 0;
	aos_models[AOS_MODEL_TEAM2_CAPTURE_POINT]->lowermip = 0;
	aos_models[AOS_MODEL_TEAM2_INTEL]->lowermip = 0;

	aos_models[AOS_MODEL_TOOL_SPADE]->lowermip = 0;
	aos_models[AOS_MODEL_TOOL_BLOCK]->lowermip = 0;
	aos_models[AOS_MODEL_WEAPON_SEMI]->lowermip = 0;
	aos_models[AOS_MODEL_WEAPON_SMG]->lowermip = 0;
	aos_models[AOS_MODEL_WEAPON_SHOTGUN]->lowermip = 0;
	aos_models[AOS_MODEL_GRENADE]->lowermip = 0;

	aos_models[AOS_MODEL_TRACER_SEMI]->lowermip = 0;
	aos_models[AOS_MODEL_TRACER_SMG]->lowermip = 0;
	aos_models[AOS_MODEL_TRACER_SHOTGUN]->lowermip = 0;

	aos_models[AOS_MODEL_CASING_SEMI]->lowermip = 0;
	aos_models[AOS_MODEL_CASING_SMG]->lowermip = 0;
	aos_models[AOS_MODEL_CASING_SHOTGUN]->lowermip = 0;

	return true;
}

voxlap::kv6data *aos_get_model(aos_ubyte modelID, bool team, bool crouched)
{
	if (modelID < 0)
		return 0;

	return aos_models[modelID | team << 6 | crouched << 5];
}

void aos_uninit_models()
{
	voxlap::freekv6(aos_models[AOS_MODEL_TEAM1_PLAYER_HEAD]);
	voxlap::freekv6(aos_models[AOS_MODEL_TEAM1_PLAYER_ARMS]);
	voxlap::freekv6(aos_models[AOS_MODEL_TEAM1_PLAYER_TORSO_UNCROUCHED]);
	voxlap::freekv6(aos_models[AOS_MODEL_TEAM1_PLAYER_TORSO_CROUCHED]);
	voxlap::freekv6(aos_models[AOS_MODEL_TEAM1_PLAYER_LEG_UNCROUCHED]);
	voxlap::freekv6(aos_models[AOS_MODEL_TEAM1_PLAYER_LEG_CROUCHED]);
	voxlap::freekv6(aos_models[AOS_MODEL_TEAM1_PLAYER_DEAD]);
	voxlap::freekv6(aos_models[AOS_MODEL_TEAM1_CAPTURE_POINT]);
	voxlap::freekv6(aos_models[AOS_MODEL_TEAM1_INTEL]);

	voxlap::freekv6(aos_models[AOS_MODEL_TEAM2_PLAYER_HEAD]);
	voxlap::freekv6(aos_models[AOS_MODEL_TEAM2_PLAYER_ARMS]);
	voxlap::freekv6(aos_models[AOS_MODEL_TEAM2_PLAYER_TORSO_UNCROUCHED]);
	voxlap::freekv6(aos_models[AOS_MODEL_TEAM2_PLAYER_TORSO_CROUCHED]);
	voxlap::freekv6(aos_models[AOS_MODEL_TEAM2_PLAYER_LEG_UNCROUCHED]);
	voxlap::freekv6(aos_models[AOS_MODEL_TEAM2_PLAYER_LEG_CROUCHED]);
	voxlap::freekv6(aos_models[AOS_MODEL_TEAM2_PLAYER_DEAD]);
	voxlap::freekv6(aos_models[AOS_MODEL_TEAM2_CAPTURE_POINT]);
	voxlap::freekv6(aos_models[AOS_MODEL_TEAM2_INTEL]);


	voxlap::freekv6(aos_models[AOS_MODEL_TOOL_SPADE]);
	voxlap::freekv6(aos_models[AOS_MODEL_TOOL_BLOCK]);
	voxlap::freekv6(aos_models[AOS_MODEL_WEAPON_SEMI]);
	voxlap::freekv6(aos_models[AOS_MODEL_WEAPON_SMG]);
	voxlap::freekv6(aos_models[AOS_MODEL_WEAPON_SHOTGUN]);
	voxlap::freekv6(aos_models[AOS_MODEL_GRENADE]);
	
	voxlap::freekv6(aos_models[AOS_MODEL_TRACER_SEMI]);
	voxlap::freekv6(aos_models[AOS_MODEL_TRACER_SMG]);
	voxlap::freekv6(aos_models[AOS_MODEL_TRACER_SHOTGUN]);

	voxlap::freekv6(aos_models[AOS_MODEL_CASING_SEMI]);
	voxlap::freekv6(aos_models[AOS_MODEL_CASING_SMG]);
	voxlap::freekv6(aos_models[AOS_MODEL_CASING_SHOTGUN]);
}

#include <math.h>

#include "world.h"
#include <voxlap\sysmain.h>

void aos_playsound(const char *filnam, long volperc, float frqmul, void *bpos, long flags)
{
	if (aos_gamestate <= GameState::MAP_LOADING)
		return;

	if (flags & KSND_3D) {
		voxlap::extension::FloatVector3D tmp(0);
		PlayerType *cur_pl = aos_world_get_local_player();
		if (cur_pl->cam_position == tmp || tmp == *(voxlap::point3d *)bpos)
			return;
		tmp = { 1, 1, 0 };
		float xydist = ((cur_pl->cam_position - *(voxlap::point3d *)bpos) * tmp).length();
		if (xydist > 128.0f) return;
	}

	sysmain::playsound(filnam, volperc, frqmul, bpos, flags);
}

void set_orientation_vectors(voxlap::point3d *o,
	voxlap::point3d *s,
	voxlap::point3d *h)
{
	float f = sqrtf(o->x*o->x + o->y*o->y);
	s->x = -o->y / f;
	s->y = o->x / f;
	h->x = -o->z*s->y;
	h->y = o->z*s->x;
	h->z = o->x*s->y - o->y*s->x;
}

void replace_sprite_color(voxlap::kv6data *kv, long newcolor)
{
	voxlap::kv6voxtype *v, *ve;
	long x, y; // no z necessary..??

	//WARNING: This code will change if Ken re-designs the KV6 format! (lol)
	if (!kv) return;
	v = kv->vox;
	for (x = 0; x<kv->xsiz; x++) {
		for (y = 0; y<kv->ysiz; y++) {
			for (ve = &v[kv->ylen[x*kv->ysiz + y]]; v<ve; v++) {
				//Surface voxels only
				if ((v->col & 0x00FFFFFF) == 0)
					v->col = (newcolor & 0x00FFFFFF);
			}
		}
	}
}

// this is different from the standard library equivalent because it implicitly adds a null terminator
void aos_strcpycut(char *dest, char *src, size_t len)
{
	if (strlen(src) <= len - 1) { // cut if necessary
		memcpy(dest, src, strlen(src) + 1);
	} else {
		memcpy(dest, src, len - 1);
		dest[len - 1] = '\0';
	}
}