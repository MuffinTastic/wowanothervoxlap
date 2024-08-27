#pragma once

#include "general.h"

#include <voxlap\voxlap.h>
#include <voxlap\extension\FloatVector3D.h>

#define ID_TEAM_SPEC   -1
#define ID_TEAM1        0
#define ID_TEAM2        1
#define ID_TEAM_NEUTRAL 2

#define ID_GAMEMODE_CTF 0
#define ID_GAMEMODE_TC  1

#define TC_MAX_TERRITORIES  16
#define TC_CAPTURE_DISTANCE 16
#define TC_CAPTURE_RATE   0.05
#define TC_CAPTURE_TIME   20.0

#define ID_TEAM1_FLAG 0
#define ID_TEAM2_FLAG 1
#define ID_TEAM1_BASE 2
#define ID_TEAM2_BASE 3

struct TeamType
{
	aos_ubyte score;
	long color;
	char name[10];

	bool ctf_flag_dropped;
	aos_ubyte ctf_flag_playerID; // ^ false

	voxlap::kv6data *ctf_base_voxnum, *ctf_flag_voxnum;
	voxlap::extension::FloatVector3D ctf_base_pos;
	voxlap::extension::FloatVector3D ctf_flag_pos;

	void setName(char*);
};

struct TC_Territory
{
	voxlap::kv6data *voxnum; // for simplicity's sake
	voxlap::extension::FloatVector3D position;
	aos_ubyte team;
	double progress;
	aos_byte rate;
};

extern aos_ubyte aos_gamemode;

extern aos_ubyte aos_ctf_cap_limit;

void aos_tc_update_team_scores();