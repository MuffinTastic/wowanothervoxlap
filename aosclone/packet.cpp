#include "packet.h"

#include <zlib.h>
#include <stdio.h>

#include <voxlap\sysmain.h>
#include <voxlap\voxlap.h>
#include <voxlap\extension\FloatVector3D.h>

#include "constants.h"
#include "config.h"
#include "general.h"
#include "vxl.h"
#include "team.h"
#include "player.h"
#include "world.h"
#include "ui.h"
#include "networking.h"

void handleAoSPositionData(AoSPacket_PositionData *data)
{
	PlayerType *player = aos_world_get_local_player();
	player->reposition(data->x, data->y, data->z);
}

void handleAoSOrientationData(AoSPacket_OrientationData *data)
{
	PlayerType *player = aos_world_get_local_player();
	player->reorient(data->x, data->y, data->z);
}

void handleAoSWorldUpdate(AoSPacketData_WorldUpdate *data)
{
	/*
	if (aos_gamestate <= GameState::MAP_LOADING)
		return;
	
	for (int i = 0; i < 32; i++) {
		AoSPacketData_WorldUpdate *update = &data[i];
		if (update->isNull())
			continue;

		PlayerType *player = &aos_players[i];

		if (i == aos_localplayer)
			voxlap::extension::FloatVector3D diff(update->posX - player->position.x,
											 	  update->posY - player->position.y,
												  update->posZ - player->position.z);
			if (diff.lengthsquared() > RUBBERBAND_DISTANCE) {
				player->reposition(update->posX, update->posY, update->posZ);
			}
		} else {
			player->reposition(update->posX, update->posY, update->posZ);
			player->reorient(update->oriX, update->oriY, update->oriZ);
		}
	}
	*/

	for (int i = 0; i < 32; i++) {
		if (i == aos_local_player_id)
			continue;

		AoSPacketData_WorldUpdate *update = &data[i];

		PlayerType *player = &aos_players[i];

		if (update->isNull() || !player->alive)
			continue;

		player->reposition(update->posX, update->posY, update->posZ);
		player->reorient(update->oriX, update->oriY, update->oriZ);
	}
}

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byt)  \
  (byt & 0x80 ? '1' : '0'), \
  (byt & 0x40 ? '1' : '0'), \
  (byt & 0x20 ? '1' : '0'), \
  (byt & 0x10 ? '1' : '0'), \
  (byt & 0x08 ? '1' : '0'), \
  (byt & 0x04 ? '1' : '0'), \
  (byt & 0x02 ? '1' : '0'), \
  (byt & 0x01 ? '1' : '0')

extern double dtotclk;

void handleAoSInputData(AoSPacket_InputData *data)
{
	PlayerType *player = &aos_players[data->playerID];
	player->set_inputdata(data->keystates);
	printf("dt %f #%d keystate " BYTE_TO_BINARY_PATTERN "\n", dtotclk, data->playerID, BYTE_TO_BINARY(data->keystates));
}

void handleAoSWeaponInput(AoSPacket_WeaponInput *data)
{
	PlayerType *player = &aos_players[data->playerID];
	player->set_weaponinput(data->weaponstates);
}

void handleAoSSetHP(AoSPacket_SetHP *data)
{
	PlayerType *cur_pl = aos_world_get_local_player();
	cur_pl->health = data->health;
	if (data->type != ID_SET_HP_FALL)
		aos_playsound(AOS_SOUND_HITPLAYER, config::vol, 1.0, NULL, 0);
}

void handleAoSGrenadePacket(AoSPacket_GrenadePacket *data)
{
	voxlap::point3d pos = { data->posX, data->posY, data->posZ };
	voxlap::point3d vel = { data->velX, data->velY, data->velZ };
	aos_world_create_grenade(pos, vel, data->fuse);
}

void handleAoSSetTool(AoSPacket_SetTool *data)
{
	PlayerType *player = &aos_players[data->playerID];
	player->set_tool(data->tool);
}

void handleAoSExistingPlayer(AoSPacket_ExistingPlayer *data)
{
	aos_players[data->playerID].existingplayer(
		data->team,
		data->weapon,
		data->tool,
		data->score,
		make_color(data->block_red, data->block_green, data->block_blue),
		data->name
	);
}

void handleAoSMoveObject(AoSPacket_MoveObject *data)
{
	aos_world_moveobject(data->objectID, data->team, data->x, data->y, data->z);
}

void handleAoSCreatePlayer(AoSPacket_CreatePlayer *data)
{
	PlayerType *player = &aos_players[data->playerID];

	if (player->createplayer(
			data->team,
			data->weapon,
			data->x, data->y, data->z,
			data->name
		))
	{
		TeamType *team = aos_world_get_team(data->team);
		aos_ui_add_chat_message(0xFF0000, "%s joined the %s team", player->name, team->name);
	}
}

void handleAoSSetColor(AoSPacket_SetColor *data)
{
	PlayerType *player = &aos_players[data->playerID];
	player->set_color(make_color(data->red, data->green, data->blue), false);
}

void handleAoSBlockAction(AoSPacket_BlockAction *data)
{
	if (data->playerID < 32 && (data->x < 0 || data->x >= 512 || data->y < 0 || data->y >= 512 || data->z < 0 || data->z >= 62))
		return;

	PlayerType *player = &aos_players[data->playerID];
	voxlap::extension::FloatVector3D sound_location(data->x + 0.5f, data->y + 0.5f, data->z + 0.5f);

	switch (data->type) {
	case ID_BLOCK_ACTION_BUILD:
		aos_vxl_set_point(data->x, data->y, data->z, player->block_color);
		aos_playsound(AOS_SOUND_BUILD, config::vol, 1.0, &sound_location, KSND_3D);
		break;
	case ID_BLOCK_ACTION_DESTROY:
		aos_vxl_destroy_point(data->x, data->y, data->z);
		//if (player->tool == TOOL_SPADE)
		break;
	case ID_BLOCK_ACTION_SPADE: {
		for (long i = -1; i < 2; i++) {
			long z = data->z + i;
			if (z < 0 || z >= 62)
				continue;
			aos_vxl_destroy_point(data->x, data->y, z);
		}
		aos_playsound(AOS_SOUND_HITGROUND, config::vol, 1.0, &sound_location, KSND_3D);
	}	break;
	case ID_BLOCK_ACTION_GRENADE: {
		
		for (long x = -1; x < 2; x++) {
			for (long y = -1; y < 2; y++) {
				for (long z = -1; z < 2; z++) {
					aos_vxl_destroy_point(data->x + x, data->y + y, data->z + z);
				}
			}
		}

	}	break;
	}
}

void handleAoSBlockLine(AoSPacket_BlockLine *data)
{
	PlayerType *player = &aos_players[data->playerID];
	size_t blocknum = aos_vxl_cube_line(data->sX, data->sY, data->sZ, data->eX, data->eY, data->eZ, aos_vxl_cube_array);
	voxlap::extension::FloatVector3D sound_location;
	for (unsigned int i = 0; i < blocknum; i++) {
		voxlap::lpoint3d *block = &aos_vxl_cube_array[i];
		aos_vxl_set_point(block->x, block->y, block->z, player->block_color);
		sound_location = { block->x + 0.5f, block->y + 0.5f, block->z + 0.5f };
	}
	
	sound_location = { data->eX + 0.5f, data->eY + 0.5f, data->eZ + 0.5f };
	aos_playsound(AOS_SOUND_BUILD, config::vol, 1.0, &sound_location, KSND_3D);
}



void handleAoSStateData(AoSPacket_StateData *data, void *gamestate)
{
	aos_local_player_id = data->playerID;

	aos_ui_switchmode = AOS_UI_SWITCH_TEAM;

	voxlap::vx5.fogcol = make_color(data->fog_red, data->fog_green, data->fog_blue);

	aos_spectator_team.color = 0x000000;
	aos_team1_team.color = make_color(data->team1_red, data->team1_green, data->team1_blue);
	aos_team2_team.color = make_color(data->team2_red, data->team2_green, data->team2_blue);
	aos_spectator_team.setName("Spectator");
	aos_team1_team.setName(data->team1_name);
	aos_team2_team.setName(data->team2_name);

	long local_team1_col = make_color(data->team1_red * 0.5, data->team1_green * 0.5, data->team1_blue * 0.5);
	long local_team2_col = make_color(data->team2_red * 0.5, data->team2_green * 0.5, data->team2_blue * 0.5);

	replace_sprite_color(aos_get_model(AOS_MODEL_TEAM1_PLAYER_HEAD, false, false), local_team1_col);
	replace_sprite_color(aos_get_model(AOS_MODEL_TEAM1_PLAYER_ARMS, false, false), local_team1_col);
	replace_sprite_color(aos_get_model(AOS_MODEL_TEAM1_PLAYER_TORSO_UNCROUCHED, false, false), local_team1_col);
	replace_sprite_color(aos_get_model(AOS_MODEL_TEAM1_PLAYER_TORSO_CROUCHED, false, false), local_team1_col);
	replace_sprite_color(aos_get_model(AOS_MODEL_TEAM1_PLAYER_LEG_UNCROUCHED, false, false), local_team1_col);
	replace_sprite_color(aos_get_model(AOS_MODEL_TEAM1_PLAYER_LEG_CROUCHED, false, false), local_team1_col);
	replace_sprite_color(aos_get_model(AOS_MODEL_TEAM1_PLAYER_DEAD, false, false), local_team1_col);
	replace_sprite_color(aos_get_model(AOS_MODEL_TEAM1_CAPTURE_POINT, false, false), local_team1_col);
	replace_sprite_color(aos_get_model(AOS_MODEL_TEAM1_INTEL, false, false), local_team1_col);

	replace_sprite_color(aos_get_model(AOS_MODEL_TEAM2_PLAYER_HEAD, false, false), local_team2_col);
	replace_sprite_color(aos_get_model(AOS_MODEL_TEAM2_PLAYER_ARMS, false, false), local_team2_col);
	replace_sprite_color(aos_get_model(AOS_MODEL_TEAM2_PLAYER_TORSO_UNCROUCHED, false, false), local_team2_col);
	replace_sprite_color(aos_get_model(AOS_MODEL_TEAM2_PLAYER_TORSO_CROUCHED, false, false), local_team2_col);
	replace_sprite_color(aos_get_model(AOS_MODEL_TEAM2_PLAYER_LEG_UNCROUCHED, false, false), local_team2_col);
	replace_sprite_color(aos_get_model(AOS_MODEL_TEAM2_PLAYER_LEG_CROUCHED, false, false), local_team2_col);
	replace_sprite_color(aos_get_model(AOS_MODEL_TEAM2_PLAYER_DEAD, false, false), local_team2_col);
	replace_sprite_color(aos_get_model(AOS_MODEL_TEAM2_CAPTURE_POINT, false, false), local_team2_col);
	replace_sprite_color(aos_get_model(AOS_MODEL_TEAM2_INTEL, false, false), local_team2_col);

	aos_gamemode = data->gamemode;

	if (aos_gamemode == ID_STATE_DATA_CTF) {
		AoSPacketData_CTFState *ctf_state = (AoSPacketData_CTFState *)gamestate;
		aos_ctf_cap_limit = ctf_state->capture_limit;

		aos_team1_team.score = ctf_state->team1_score;
		aos_team2_team.score = ctf_state->team2_score;

		aos_team1_team.ctf_flag_dropped = !((ctf_state->intel_flags >> 1) & 1);
		aos_team2_team.ctf_flag_dropped = !(ctf_state->intel_flags & 1);

		aos_world_moveobject(ID_TEAM1_BASE, ID_TEAM1, ctf_state->team1_base_x, ctf_state->team1_base_y, ctf_state->team1_base_z);
		aos_world_moveobject(ID_TEAM2_BASE, ID_TEAM2, ctf_state->team2_base_x, ctf_state->team2_base_y, ctf_state->team2_base_z);

		if (aos_team1_team.ctf_flag_dropped) {
			aos_world_moveobject(ID_TEAM1_FLAG, ID_TEAM1, ctf_state->team1_intel.x, ctf_state->team1_intel.y, ctf_state->team1_intel.z);
		} else {
			aos_team1_team.ctf_flag_playerID = ctf_state->team1_intel.playerID;
		}
		
		if (aos_team2_team.ctf_flag_dropped) {
			aos_world_moveobject(ID_TEAM2_FLAG, ID_TEAM2, ctf_state->team2_intel.x, ctf_state->team2_intel.y, ctf_state->team2_intel.z);
		} else {
			aos_team2_team.ctf_flag_playerID = ctf_state->team2_intel.playerID;
		}
			
		aos_team1_team.ctf_base_voxnum = aos_get_model(AOS_MODEL_TEAM1_CAPTURE_POINT, false, false);
		aos_team2_team.ctf_base_voxnum = aos_get_model(AOS_MODEL_TEAM2_CAPTURE_POINT, false, false);

		aos_team1_team.ctf_flag_voxnum = aos_get_model(AOS_MODEL_TEAM1_INTEL, false, false);
		aos_team2_team.ctf_flag_voxnum = aos_get_model(AOS_MODEL_TEAM2_INTEL, false, false);

	} else if (data->gamemode == ID_STATE_DATA_TC) {
		AoSPacketData_TCState *tc_state = (AoSPacketData_TCState *)gamestate;

		aos_tc_territory_count = tc_state->count;
		
		for (int i = 0; i < aos_tc_territory_count; i++) {
			TCState_TerritoryData *territory_state = &tc_state->territories[i];
			aos_world_moveobject(i, territory_state->team, territory_state->x, territory_state->y, territory_state->z);
		}

		aos_tc_update_team_scores();
	} else {
		aos_ctf_cap_limit = 124; // testing..
	}

	aos_gamestate = GameState::INGAME;
	
	aos_playsound(AOS_SOUND_INTRO, config::vol, 1.0, NULL, 0);
}

#define KILLACTION_TEAM     "%s switched teams"
#define KILLACTION_WEAPON   "%s switched weapons"
#define KILLACTION_FALL     "%s fell too far"
#define KILLACTION_HEADSHOT "%s killed %s (Headshot)"
#define KILLACTION_SPADE    "%s killed %s (Spade)"
#define KILLACTION_BLOCK    "%s killed %s (Block)"
#define KILLACTION_SEMI     "%s killed %s (Rifle)"
#define KILLACTION_SMG      "%s killed %s (SMG)"
#define KILLACTION_SHOTGUN  "%s killed %s (Shotgun)"
#define KILLACTION_GRENADE  "%s killed %s (Grenade)"
#define KILLACTION_WTF_KT   "ERROR: Invalid killtype?"
#define KILLACTION_WTF_WP   "ERROR: Invalid weapon?"
#define KILLACTION_WTF_TL   "ERROR: Invalid tool?"

char *get_killaction_text(PlayerType *killer, aos_ubyte killtype)
{
	switch (killtype) {
	case ID_KILL_ACTION_TEAM_CHANGE:   return KILLACTION_TEAM;     break;
	case ID_KILL_ACTION_WEAPON_CHANGE: return KILLACTION_WEAPON;   break;
	case ID_KILL_ACTION_FALL:          return KILLACTION_FALL;     break;
	case ID_KILL_ACTION_HEADSHOT:      return KILLACTION_HEADSHOT; break;
	case ID_KILL_ACTION_MELEE:         return KILLACTION_SPADE;    break;
	case ID_KILL_ACTION_GRENADE:       return KILLACTION_GRENADE;  break;
	case ID_KILL_ACTION_WEAPON:
		switch (killer->tool) {
		case TOOL_SPADE:               return KILLACTION_SPADE;    break;
		case TOOL_BLOCK:               return KILLACTION_BLOCK;    break;
		case TOOL_GRENADE:             return KILLACTION_GRENADE;  break;
		case TOOL_GUN:
			switch (killer->weapon) {
			case WEAPON_SEMI:          return KILLACTION_SEMI;     break;
			case WEAPON_SMG:           return KILLACTION_SMG;      break;
			case WEAPON_SHOTGUN:       return KILLACTION_SHOTGUN;  break;
			default:                   return KILLACTION_WTF_WP;   break;
			} break;
		default:                       return KILLACTION_WTF_TL;   break;
		} break;
	default:                           return KILLACTION_WTF_KT;   break;
	}
}

void handleAoSKillAction(AoSPacket_KillAction *data)
{
	PlayerType *player = &aos_players[data->playerID];
	PlayerType *killer = &aos_players[data->killerID];
	player->kill(data->respawn_time);
	if (killer != player) killer->score++;
	if (player->local)
		aos_playsound(AOS_SOUND_DEATH, config::vol, 1.0, NULL, 0);

	char *message = get_killaction_text(killer, data->type);

	if (killer->local || player->local) {
		aos_ui_add_killfeed_message(0xFF0000, message, killer->name, player->name);
	} else {
		aos_ui_add_killfeed_message(aos_world_get_team(killer->team)->color, message, killer->name, player->name);
	}
}

void handleAoSChatMessage(AoSPacket_ChatMessage *data)
{
	if (data->type == ID_CHAT_MESSAGE_SYSTEM) {
		aos_ui_add_chat_message(0xFF0000, data->text);
		return;
	}

	if (data->playerID > 34 && data->type == ID_CHAT_MESSAGE_TEAM) {
		aos_ui_add_chat_message(aos_team1_team.color, ":%s", data->text);
	} else {
		PlayerType *player = &aos_players[data->playerID];
		TeamType *team = aos_world_get_team(player->team);
		if (data->type == ID_CHAT_MESSAGE_ALL) {
			aos_ui_add_chat_message(0xFFFFFF, "%s (%s): %s", player->name, team->name, data->text);
		} else if (data->type == ID_CHAT_MESSAGE_TEAM) {
			aos_ui_add_chat_message(team->color, "%s: %s", player->name, data->text);
		}
	}
}

#pragma region Map Loading
static unsigned long map_size;
static unsigned long map_read;
static aos_byte *compressed_map = NULL;

void handleAoSMapStart(AoSPacket_MapStart *data)
{
	for (int i = 0; i < 32; i++) {
		aos_players[i].init();
	}

	if (compressed_map != NULL) {
		delete compressed_map;
		compressed_map = NULL;
	}

	map_size = 2 * 1024 * 1024; //data->map_size; // oh god why
	map_read = 0;
	compressed_map = new aos_byte[map_size];
	aos_gamestate = GameState::MAP_LOADING;
}

void handleAoSMapChunk(AoSPacket_MapChunk *compressed_chunk, size_t size)
{
	if (compressed_map == NULL)
		return;

	memcpy(compressed_map + map_read, compressed_chunk, size);
	map_read += size;
}

long handleAoSMapEnd()
{
	// 4x cause it's big and i hate zlib
	aos_byte* decompressed_map = new aos_byte[map_size * 4];
	int status;

	z_stream infstr;
	infstr.zalloc = Z_NULL;
	infstr.zfree = Z_NULL;
	infstr.opaque = Z_NULL;
	infstr.avail_in = map_read;
	infstr.next_in = (Bytef*)compressed_map;
	infstr.avail_out = map_size * 4;
	infstr.next_out = (Bytef*)decompressed_map;

	inflateInit(&infstr);
	status = inflate(&infstr, Z_NO_FLUSH);
	inflateEnd(&infstr);
	
	delete compressed_map;
	compressed_map = NULL; // new aos_byte[map_size];

	if (status >= Z_OK) { // what in the absolute fuck?! this works..
		voxlap::loadvxlbytes(decompressed_map, infstr.total_out);
		delete decompressed_map;
	} else {
		delete decompressed_map;
		switch (status) {
		case Z_STREAM_ERROR:
			showError("zlib error: invalid compression level");
			break;
		case Z_DATA_ERROR:
			showError("zlib error: invalid / incomplete deflate data");
			break;
		case Z_MEM_ERROR:
			showError("zlib error: out of memory");
			break;
		case Z_VERSION_ERROR:
			showError("zlib error: version mismatch");
			break;
		default:
			showError("zlib: unknown return condition '%d'", status);
			break;
		}
		return 0;
	}

	if (config::shadows)
		aos_vxl_updatelighting_map();
	aos_ui_updateminimap();

	return 1;
}

float aos_map_progress()
{
	if (compressed_map == NULL)
		return 0.0f;
	return ((float)(map_read) / map_size);
}
#pragma endregion

void handleAoSPlayerLeft(AoSPacket_PlayerLeft *data)
{
	PlayerType *player = &aos_players[data->playerID];
	aos_ui_add_chat_message(0xFF0000, "%s disconnected", player->name);
	player->init();

	PlayerType *cur_pl = aos_world_get_local_player();
	//if (cur_pl->specid == data->playerID)
	//	cur_pl->local_spec_camera_advance(true);
}

void handleAoSTerritoryCapture(AoSPacket_TerritoryCapture *data)
{
	if (data->entityID >= aos_tc_territory_count) return;
	TC_Territory *territory = &aos_tc_territories[data->entityID];

	aos_world_moveobject(data->entityID, data->team);
	territory->progress = 0.0;
	territory->rate = 0;

	aos_playsound(AOS_SOUND_INTEL_PICKUP, config::vol, 1.0, NULL, 0);
	aos_ui_add_chat_message(0xFF0000, "%s has captured %s", aos_world_get_team(data->team)->name, aos_world_get_sector(territory->position.x, territory->position.y));

	aos_tc_update_team_scores();

	// do stuff with data->winning
}

void handleAoSProgressBar(AoSPacket_ProgressBar *data)
{
	if (data->entityID >= aos_tc_territory_count) return;
	TC_Territory *territory = &aos_tc_territories[data->entityID];
	
	if (territory->rate == 0 && data->rate > 0) {
		if (territory->team != data->team) {
			aos_ui_add_chat_message(0xFF0000, "%s is capturing %s", aos_world_get_team(data->team)->name, aos_world_get_sector(territory->position.x, territory->position.y));
		}
	}

	territory->progress = data->progress;
	territory->rate = data->rate;
}

void handleAoSIntelCapture(AoSPacket_IntelCapture *data)
{
	PlayerType *player = &aos_players[data->playerID];
	if (player->team < ID_TEAM1) return;
	TeamType *team = aos_world_get_team(player->team);
	team->score++;
	player->score += 10;

	TeamType *other = aos_world_get_team(!player->team);
	other->ctf_flag_dropped = true;

	aos_playsound(AOS_SOUND_INTEL_PICKUP, config::vol, 1.0, NULL, 0);
	aos_ui_add_chat_message(0xFF0000, "%s has captured the %s Intel", player->name, other->name);

	// do stuff with data->winning
}

void handleAoSIntelPickup(AoSPacket_IntelPickup *data)
{
	PlayerType *player = &aos_players[data->playerID];
	if (player->team < ID_TEAM1) return;
	TeamType *other = aos_world_get_team(!player->team);
	other->ctf_flag_dropped = false;
	other->ctf_flag_playerID = data->playerID;

	aos_playsound(AOS_SOUND_INTEL_PICKUP, config::vol, 1.0, NULL, 0);
	aos_ui_add_chat_message(0xFF0000, "%s has the %s Intel", player->name, other->name);
}

void handleAoSIntelDrop(AoSPacket_IntelDrop *data)
{
	PlayerType *player = &aos_players[data->playerID];
	if (player->team < ID_TEAM1) return;
	TeamType *other = aos_world_get_team(!player->team);
	other->ctf_flag_dropped = true;
	other->ctf_flag_pos = { data->x, data->y, data->z };
	
	aos_ui_add_chat_message(0xFF0000, "%s has dropped the %s Intel", player->name, other->name);
}

void handleAoSRestock(AoSPacket_Restock * data)
{
	PlayerType *player = aos_world_get_local_player();
	player->restock();
}

void handleAoSFogColor(AoSPacket_FogColor *data)
{
	voxlap::vx5.fogcol = make_color(data->fog_red, data->fog_green, data->fog_blue);
}

void handleAoSWeaponReload(AoSPacket_WeaponReload *data)
{
	PlayerType *player = &aos_players[data->playerID];
	if (player->local) {
		player->weapon_ammo = data->clip;
		player->weapon_stock = data->reserve;
	} else player->reload_weapon();
}

void handleOSHandshake(OSPacket_Handshake *data)
{
#ifdef SEND_GAME_VERSION
	aos_network_send(ID_HANDSHAKE_RTRN, data, sizeof(OSPacket_Handshake), true);
#endif
}

void handleOSVersion()
{
#ifdef SEND_GAME_VERSION
	OSPacket_Version version;
	version.client = GAME_LETTR;
	version.major = GAME_MAJOR;
	version.minor = GAME_MINOR;
	version.revision = GAME_REVSN;
	strcpy(version.title, "Windows");
	aos_network_send(ID_VERSION_SEND, &version, sizeof(OSPacket_Version), true);
#endif
}

#pragma region Convenience

bool AoSPacketData_WorldUpdate::isNull()
{
	if (this == nullptr) return true;
	// orientation should be a normalized vector; normalized vectors [should] never have all-0 values
	return ((this->posX == 0 &&
		this->posY == 0 &&
		this->posZ == 0) ||
		(this->oriX == 0 &&
		this->oriY == 0 &&
		this->oriZ == 0));
}

#pragma endregion