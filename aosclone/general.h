#pragma once

#include <voxlap\voxlap.h>

typedef char aos_byte;
typedef unsigned char aos_ubyte;

void showError(const char *format, ...);

long make_color(unsigned char, unsigned char, unsigned char);
void get_color(unsigned char *, unsigned char *, unsigned char *, long);

//binary format:

// T: Team (0 = team1, 1 = team2)
// C: Crouched (0/1)

// 0b0TC00101

#define AOS_MODEL_TEAM1_PLAYER_HEAD              0
#define AOS_MODEL_TEAM1_PLAYER_ARMS              1
#define AOS_MODEL_TEAM1_PLAYER_TORSO_UNCROUCHED  2
#define AOS_MODEL_TEAM1_PLAYER_TORSO_CROUCHED   34
#define AOS_MODEL_TEAM1_PLAYER_LEG_UNCROUCHED    3
#define AOS_MODEL_TEAM1_PLAYER_LEG_CROUCHED     35
#define AOS_MODEL_TEAM1_PLAYER_DEAD              4
#define AOS_MODEL_TEAM1_CAPTURE_POINT            5
#define AOS_MODEL_TEAM1_INTEL                    6

#define AOS_MODEL_TEAM2_PLAYER_HEAD             64
#define AOS_MODEL_TEAM2_PLAYER_ARMS             65
#define AOS_MODEL_TEAM2_PLAYER_TORSO_UNCROUCHED 66
#define AOS_MODEL_TEAM2_PLAYER_TORSO_CROUCHED   98
#define AOS_MODEL_TEAM2_PLAYER_LEG_UNCROUCHED   67
#define AOS_MODEL_TEAM2_PLAYER_LEG_CROUCHED     99
#define AOS_MODEL_TEAM2_PLAYER_DEAD             68
#define AOS_MODEL_TEAM2_CAPTURE_POINT           69
#define AOS_MODEL_TEAM2_INTEL                   70

#define AOS_MODEL_TOOL_SPADE                     7
#define AOS_MODEL_TOOL_BLOCK                     8
#define AOS_MODEL_WEAPON_SEMI                    9
#define AOS_MODEL_WEAPON_SMG                    10
#define AOS_MODEL_WEAPON_SHOTGUN                11
#define AOS_MODEL_GRENADE                       12

#define AOS_MODEL_TRACER_SEMI                   13
#define AOS_MODEL_TRACER_SMG                    14
#define AOS_MODEL_TRACER_SHOTGUN                15

#define AOS_MODEL_CASING_SEMI                   16
#define AOS_MODEL_CASING_SMG                    17
#define AOS_MODEL_CASING_SHOTGUN                18
// add more...

#define AOS_SOUND_INTRO                 "wav/intro.wav"
#define AOS_SOUND_BEEP1                 "wav/beep1.wav"
#define AOS_SOUND_BEEP2                 "wav/beep2.wav"
#define AOS_SOUND_BOUNCE                "wav/bounce.wav"
#define AOS_SOUND_BUILD                 "wav/build.wav"
#define AOS_SOUND_CHAT                  "wav/chat.wav"
#define AOS_SOUND_LAND                  "wav/land.wav"
#define AOS_SOUND_WATER_LAND            "wav/waterland.wav"
#define AOS_SOUND_JUMP                  "wav/jump.wav"
#define AOS_SOUND_WATER_JUMP            "wav/waterjump.wav"
#define AOS_SOUND_FALLHURT              "wav/fallhurt.wav"
#define AOS_SOUND_HITPLAYER             "wav/hitplayer.wav"
#define AOS_SOUND_HORN					"wav/horn.wav"
#define AOS_SOUND_IMPACT				"wav/impact.wav"
#define AOS_SOUND_WHACK                 "wav/whack.wav"
#define AOS_SOUND_WOOSH                 "wav/woosh_mono.wav"
#define AOS_SOUND_DEATH                 "wav/death.wav"
#define AOS_SOUND_FOOTSTEP1             "wav/footstep1.wav"
#define AOS_SOUND_FOOTSTEP2             "wav/footstep2.wav"
#define AOS_SOUND_FOOTSTEP3             "wav/footstep3.wav"
#define AOS_SOUND_FOOTSTEP4             "wav/footstep4.wav"
#define AOS_SOUND_WADE1                 "wav/wade1.wav"
#define AOS_SOUND_WADE2                 "wav/wade2.wav"
#define AOS_SOUND_WADE3                 "wav/wade3.wav"
#define AOS_SOUND_WADE4                 "wav/wade4.wav"
#define AOS_SOUND_DEBRIS_BREAK          "wav/debris.wav"
#define AOS_SOUND_DEBRIS_BOUNCE         "wav/bounce.wav"
#define AOS_SOUND_SWITCH                "wav/switch.wav"
#define AOS_SOUND_SHOOT_SEMI            "wav/semishoot.wav"
#define AOS_SOUND_SHOOT_SMG             "wav/smgshoot.wav"
#define AOS_SOUND_SHOOT_SHOTGUN         "wav/shotgunshoot.wav"
#define AOS_SOUND_RELOAD_SEMI           "wav/semireload.wav"
#define AOS_SOUND_RELOAD_SMG            "wav/smgreload.wav"
#define AOS_SOUND_RELOAD_SHOTGUN        "wav/shotgunreload.wav"
#define AOS_SOUND_COCK_SHOTGUN          "wav/cock.wav"
#define AOS_SOUND_GRENADE_PIN           "wav/pin.wav"
#define AOS_SOUND_GRENADE_EXPLODE       "wav/explode.wav"
#define AOS_SOUND_GRENADE_WATER_EXPLODE "wav/waterexplode.wav"
#define AOS_SOUND_GRENADE_BOUNCE        "wav/grenadebounce.wav"
#define AOS_SOUND_HITGROUND             "wav/hitground.wav"
#define AOS_SOUND_HITPLAYER             "wav/hitplayer.wav"
#define AOS_SOUND_EMPTY_CLIP            "wav/empty.wav"
#define AOS_SOUND_INTEL_PICKUP          "wav/pickup.wav"

#define AOS_FOG_DISTANCE 128.0f;

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

bool aos_init_models();
voxlap::kv6data *aos_get_model(aos_ubyte, bool, bool);
void aos_uninit_models();

enum class GameState
{
	ENTER_NAME,
	MAP_LOADING,
	CHOOSE_TEAM,
	CHOOSE_GUN,
	INGAME
};

extern GameState aos_gamestate;

void aos_playsound(const char *, long, float, void *, long);

void set_orientation_vectors(voxlap::point3d *, voxlap::point3d *, voxlap::point3d *);
void replace_sprite_color(voxlap::kv6data *kv, long newcolor);
void aos_strcpycut(char *dest, char *src, size_t len);