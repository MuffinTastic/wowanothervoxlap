#pragma once

#include "general.h"
#include "team.h"

// Cl -> Client
// Sv -> Server

// Ace of Spades
// http://aoswiki.rakiru.com/index.php/Ace_of_Spades_Protocol
/*
#define ID_POSITION_DATA    0
#define ID_ORIENTATION_DATA 1
#define ID_WORLD_UPDATE     2
#define ID_INPUT_DATA       3
#define ID_WEAPON_INPUT     4
#define ID_HIT_PACKET       5 // Cl -> Sv
#define ID_SET_HP           5 // Sv -> Cl
#define ID_GRENADE_PACKET   6
#define ID_SET_TOOL         7
#define ID_SET_COLOR        8 // screw u britain!
#define ID_EXISTING_PLAYER  9
#define ID_SHORT_PLAYER    10
#define ID_MOVE_OBJECT     11
#define ID_CREATE_PLAYER   12
#define ID_BLOCK_ACTION    13
#define ID_BLOCK_LINE      14
#define ID_STATE_DATA      15
#define ID_KILL_ACTION     16
#define ID_CHAT_MESSAGE    17
#define ID_MAP_START       18
#define ID_MAP_CHUNK       19
#define ID_PLAYER_LEFT     20
#define ID_TERRITORY_CAP   21
#define ID_PROGRESS_BAR    22
#define ID_INTEL_CAP       23
#define ID_INTEL_PICKUP    24
#define ID_INTEL_DROP      25
#define ID_RESTOCK         26
#define ID_FOG_COLOR       27 // screw u britain!
#define ID_WEAPON_RELOAD   28
#define ID_CHANGE_TEAM     29
#define ID_CHANGE_WEAPON   30
*/

// OpenSpades
// https://github.com/yvt/openspades/commit/aa62a0
/*
#define ID_HANDSHAKE_INIT  31
#define ID_HANDSHAKE_RTRN  32
#define ID_VERSION_GET     33
#define ID_VERSION_SEND    34
*/

#pragma pack(push, 1)

#define ID_POSITION_DATA    0
struct AoSPacket_PositionData
{
	float x, y, z;
};

#define ID_ORIENTATION_DATA 1
struct AoSPacket_OrientationData
{
	float x, y, z;
};

#define ID_WORLD_UPDATE     2
// actual packet is WorldUpdate[32]
struct AoSPacketData_WorldUpdate
{
	float posX, posY, posZ;
	float oriX, oriY, oriZ;

	bool isNull(); // Convenience function
};

#define ID_INPUT_DATA       3
struct AoSPacket_InputData
{
	aos_ubyte playerID;
	aos_ubyte keystates;
};

#define ID_WEAPON_INPUT     4
struct AoSPacket_WeaponInput
{
	aos_ubyte playerID;
	aos_ubyte weaponstates;
};

#define ID_HIT_PACKET_TORSO 0
#define ID_HIT_PACKET_HEAD  1
#define ID_HIT_PACKET_ARMS  2
#define ID_HIT_PACKET_LEGS  3
#define ID_HIT_PACKET_MELEE 4

#define ID_HIT_PACKET       5
struct AoSPacket_HitPacket
{
	aos_ubyte hitID;
	aos_ubyte hittype;
};

#define ID_SET_HP_FALL      0
#define ID_SET_HP_WEAPON    1

#define ID_SET_HP           5
struct AoSPacket_SetHP
{
	aos_ubyte health;
	aos_ubyte type;
	float x, y, z;
};

#define ID_GRENADE_PACKET   6
struct AoSPacket_GrenadePacket {
	aos_ubyte playerID;
	float fuse;
	float posX, posY, posZ;
	float velX, velY, velZ;
};

#define ID_SET_TOOL_SPADE   0
#define ID_SET_TOOL_BLOCK   1
#define ID_SET_TOOL_GUN     2
#define ID_SET_TOOL_GRENADE 3

#define ID_SET_TOOL         7
struct AoSPacket_SetTool
{
	aos_ubyte playerID;
	aos_ubyte tool;
};

#define ID_SET_COLOR        8
struct AoSPacket_SetColor
{
	aos_ubyte playerID;
	aos_ubyte blue;
	aos_ubyte green;
	aos_ubyte red;
};

#define ID_EXISTING_PLAYER  9
struct AoSPacket_ExistingPlayer
{
	aos_ubyte playerID;
	aos_byte team;
	aos_ubyte weapon;
	aos_ubyte tool;
	unsigned long score;
	aos_ubyte block_blue;
	aos_ubyte block_green;
	aos_ubyte block_red;
	// [16]: cl->sv
	//    *: sv->cl
	char name[16];
};

#define ID_SHORT_PLAYER    10
struct AoSPacket_ShortPlayerData
{
	aos_ubyte playerID;
	aos_byte team;
	aos_ubyte weapon;
};

#define ID_MOVE_OBJECT     11
struct AoSPacket_MoveObject
{
	aos_ubyte objectID;
	aos_ubyte team;
	float x, y, z;
};

#define ID_CREATE_PLAYER   12
struct AoSPacket_CreatePlayer
{
	aos_ubyte playerID;
	aos_ubyte weapon;
	aos_byte team;
	float x, y, z;
	char name[16];
};

#define ID_BLOCK_ACTION_BUILD   0
#define ID_BLOCK_ACTION_DESTROY 1
#define ID_BLOCK_ACTION_SPADE   2
#define ID_BLOCK_ACTION_GRENADE 3

#define ID_BLOCK_ACTION    13
struct AoSPacket_BlockAction
{
	aos_ubyte playerID;
	aos_ubyte type;
	long x, y, z;
};

#define ID_BLOCK_LINE      14
struct AoSPacket_BlockLine
{
	aos_ubyte playerID;
	long sX, sY, sZ;
	long eX, eY, eZ;
};


union CTFState_IntelData
{
	struct
	{
		aos_ubyte playerID;
		char padding[11];
	};

	struct
	{
		float x, y, z;
	};
};

#define ID_STATE_DATA_CTF   0
struct AoSPacketData_CTFState
{
	aos_ubyte team1_score;
	aos_ubyte team2_score;
	aos_ubyte capture_limit;
	aos_ubyte intel_flags;
	CTFState_IntelData team1_intel;
	CTFState_IntelData team2_intel;
	float team1_base_x, team1_base_y, team1_base_z;
	float team2_base_x, team2_base_y, team2_base_z;
};

struct TCState_TerritoryData
{
	float x, y, z;
	aos_ubyte team;
};

#define ID_STATE_DATA_TC    1
struct AoSPacketData_TCState
{
	aos_ubyte count;
	TCState_TerritoryData territories[TC_MAX_TERRITORIES];
};

#define ID_STATE_DATA      15
struct AoSPacket_StateData
{
	aos_ubyte playerID;
	aos_ubyte fog_blue;
	aos_ubyte fog_green;
	aos_ubyte fog_red;
	aos_ubyte team1_blue;
	aos_ubyte team1_green;
	aos_ubyte team1_red;
	aos_ubyte team2_blue;
	aos_ubyte team2_green;
	aos_ubyte team2_red;
	char team1_name[10];
	char team2_name[10];
	aos_ubyte gamemode;
	// to be continued...
};

#define ID_KILL_ACTION_WEAPON        0
#define ID_KILL_ACTION_HEADSHOT      1
#define ID_KILL_ACTION_MELEE         2
#define ID_KILL_ACTION_GRENADE       3
#define ID_KILL_ACTION_FALL          4
#define ID_KILL_ACTION_TEAM_CHANGE   5
#define ID_KILL_ACTION_WEAPON_CHANGE 6

#define ID_KILL_ACTION     16
struct AoSPacket_KillAction
{
	aos_ubyte playerID;
	aos_ubyte killerID;
	aos_ubyte type;
	aos_ubyte respawn_time;
};

#define ID_CHAT_MESSAGE_ALL    0
#define ID_CHAT_MESSAGE_TEAM   1
#define ID_CHAT_MESSAGE_SYSTEM 2

#define ID_CHAT_MESSAGE    17
#define CHAT_MESSAGE_MAX_LENGTH 94
struct AoSPacket_ChatMessage
{
	aos_ubyte playerID;
	aos_ubyte type;
	char text[CHAT_MESSAGE_MAX_LENGTH];
};

#define ID_MAP_START       18
struct AoSPacket_MapStart
{
	unsigned long map_size;
};

#define ID_MAP_CHUNK       19
// map chunk doesn't work like other packets!
typedef char AoSPacket_MapChunk;

#define ID_PLAYER_LEFT     20
struct AoSPacket_PlayerLeft
{
	aos_ubyte playerID;
};

#define ID_TERRITORY_CAP   21
struct AoSPacket_TerritoryCapture
{
	aos_ubyte entityID;
	aos_ubyte winning;
	aos_ubyte team;
};

#define ID_PROGRESS_BAR    22
struct AoSPacket_ProgressBar
{
	aos_ubyte entityID;
	aos_ubyte team;
	aos_byte rate;
	float progress;
};

#define ID_INTEL_CAP       23
struct AoSPacket_IntelCapture
{
	aos_ubyte playerID;
	aos_ubyte winning;
};

#define ID_INTEL_PICKUP    24
struct AoSPacket_IntelPickup
{
	aos_ubyte playerID;
};

#define ID_INTEL_DROP      25
struct AoSPacket_IntelDrop
{
	aos_ubyte playerID;
	float x, y, z;
};

#define ID_RESTOCK         26
struct AoSPacket_Restock
{
	aos_ubyte playerID;
};

#define ID_FOG_COLOR       27
struct AoSPacket_FogColor
{
	aos_ubyte pad; // alpha isn't even used
	aos_ubyte fog_blue;
	aos_ubyte fog_green;
	aos_ubyte fog_red;
};

#define ID_WEAPON_RELOAD   28
struct AoSPacket_WeaponReload
{
	aos_ubyte playerID;
	aos_ubyte clip;
	aos_ubyte reserve;
};

#define ID_CHANGE_TEAM     29
struct AoSPacket_ChangeTeam
{
	aos_ubyte playerID;
	aos_ubyte teamID;
};

#define ID_CHANGE_WEAPON   30
struct AoSPacket_ChangeWeapon
{
	aos_ubyte playerID;
	aos_ubyte weapon;
};


#define ID_HANDSHAKE_INIT  31
// same data as RTRN

#define ID_HANDSHAKE_RTRN  32
struct OSPacket_Handshake
{
	int challenge;
};

#define ID_VERSION_GET     33
// no packet-specific data

#define ID_VERSION_SEND    34
struct OSPacket_Version
{
	char client;
	aos_byte major;
	aos_byte minor;
	aos_byte revision;
	char title[128];
};

#pragma pack(pop)

void handleAoSPositionData(AoSPacket_PositionData *);
void handleAoSOrientationData(AoSPacket_OrientationData *);
void handleAoSWorldUpdate(AoSPacketData_WorldUpdate *);
void handleAoSInputData(AoSPacket_InputData *);
void handleAoSWeaponInput(AoSPacket_WeaponInput *);
void handleAoSSetHP(AoSPacket_SetHP *);
void handleAoSGrenadePacket(AoSPacket_GrenadePacket *);
void handleAoSSetTool(AoSPacket_SetTool *);
void handleAoSSetColor(AoSPacket_SetColor *);
void handleAoSExistingPlayer(AoSPacket_ExistingPlayer *);
void handleAoSShortPlayerData(AoSPacket_ShortPlayerData *);
void handleAoSMoveObject(AoSPacket_MoveObject *);
void handleAoSCreatePlayer(AoSPacket_CreatePlayer *);
void handleAoSBlockAction(AoSPacket_BlockAction *);
void handleAoSBlockLine(AoSPacket_BlockLine *);
void handleAoSStateData(AoSPacket_StateData *, void *);
void handleAoSKillAction(AoSPacket_KillAction *);
void handleAoSChatMessage(AoSPacket_ChatMessage *);
void handleAoSMapStart(AoSPacket_MapStart *);
void handleAoSMapChunk(AoSPacket_MapChunk *, size_t);
long handleAoSMapEnd();
void handleAoSPlayerLeft(AoSPacket_PlayerLeft *);
void handleAoSTerritoryCapture(AoSPacket_TerritoryCapture *);
void handleAoSProgressBar(AoSPacket_ProgressBar *);
void handleAoSIntelCapture(AoSPacket_IntelCapture *);
void handleAoSIntelPickup(AoSPacket_IntelPickup *);
void handleAoSIntelDrop(AoSPacket_IntelDrop *);
void handleAoSRestock(AoSPacket_Restock *);
void handleAoSFogColor(AoSPacket_FogColor *);
void handleAoSWeaponReload(AoSPacket_WeaponReload *);

void handleOSHandshake(OSPacket_Handshake *);
void handleOSVersion();

float aos_map_progress();
void aos_map_getvars(long *, long *);