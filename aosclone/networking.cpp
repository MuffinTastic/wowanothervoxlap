#include "networking.h"
#include "packet.h"
#include "general.h"
#include "player.h"
#include "ui.h"

#include "other\UriParser.hpp"

#include <enet\enet.h>

#include <Windows.h>

// Ace of Spades
// http://aoswiki.rakiru.com/index.php/Ace_of_Spades_Protocol
#define AOS_MAGIC_VERSION_NUMBER 3 // 0.75

#define ID_DISCONNECT_BANNED  1
#define ID_DISCONNECT_IPLIMIT 2
#define ID_DISCONNECT_VERSION 3
#define ID_DISCONNECT_SRVFULL 4
#define ID_DISCONNECT_KICKED 10

bool			   aos_network_connected;
static ENetAddress aos_address;
static ENetHost   *aos_client;
static ENetPeer   *aos_server;
static ENetPacket *aos_packet;
static ENetEvent   aos_event;

long aos_network_parse_uri(std::string address)
{
	http::url parsed = http::ParseHttpUrl(address);

	printf("parsed: host[%s] port[%d]\n", parsed.host.c_str(), parsed.port);

	if (parsed.protocol != "aos") {
		return 0;
	}
	
	if (strchr(parsed.host.c_str(), '.')) {
		// Treat as URI
		enet_address_set_host(&aos_address, parsed.host.c_str());
	} else {
		errno = 0;
		// Treat as 32-bit IP
		char *end;
		uint32_t host = strtoul(parsed.host.c_str(), &end, 10);
		if (errno != 0 || end != parsed.host.c_str() + parsed.host.length()) // invalid URI
			return 0;

		aos_address.host = host;
	}
	aos_address.port = parsed.port;

	char addrstr[32];
	enet_address_get_host_ip(&aos_address, addrstr, 32);
	printf("host addr [%d], p [%s]\n", aos_address.host, addrstr);

	return 1;
}

long aos_network_init()
{
	if (enet_initialize() != 0) {
		showError("ENet error: Could not init!");
		return 0;
	}

	aos_network_connected = false;

	return 1;
}

void aos_network_uninit()
{
	enet_deinitialize();
}

long aos_network_connect(const char *address)
{
	if (!aos_network_parse_uri(address)) {
		showError("Invalid aos:// URI");
		return -1;
	}

	aos_client = enet_host_create(NULL, 1, 1, 57600 / 8, 14400 / 8);
	
	if (aos_client == NULL) {
		showError("ENet error: Could not create host!");
		return -1;
	}

	enet_host_compress_with_range_coder(aos_client);

	aos_server = enet_host_connect(aos_client, &aos_address, 1, AOS_MAGIC_VERSION_NUMBER);

	if (aos_server == NULL) {
		showError("ENet error: Could not create peer!");
		enet_host_destroy(aos_client);
		return -1;
	}

	if (enet_host_service(aos_client, &aos_event, 3000) != 0 && aos_event.type == ENET_EVENT_TYPE_CONNECT) {
		aos_network_connected = true;
		aos_gamestate = GameState::MAP_LOADING;
	} else {
		enet_peer_reset(aos_server);
		enet_host_destroy(aos_client);
		return 0;
	}

	return 1;
}

long aos_network_service()
{
	if (aos_client == NULL || aos_server == NULL)
		return 0;
	if (aos_client->connectedPeers == 0)
		return 0;

	while (enet_host_service(aos_client, &aos_event, 0) > 0) {
		switch (aos_event.type) {
		case ENET_EVENT_TYPE_RECEIVE: {	
			unsigned char packetID = aos_event.packet->data[0];
			void *data = (aos_event.packet->data + sizeof(unsigned char));
			aos_ui_add_inbytes(aos_event.packet->dataLength);

			switch (packetID) {
			case ID_POSITION_DATA:
				handleAoSPositionData((AoSPacket_PositionData *)(data));
				break;

			case ID_ORIENTATION_DATA:
				handleAoSOrientationData((AoSPacket_OrientationData *)(data));
				break;

			case ID_WORLD_UPDATE:
				handleAoSWorldUpdate((AoSPacketData_WorldUpdate *)(data));
				break;

			case ID_INPUT_DATA:
				handleAoSInputData((AoSPacket_InputData *)(data));
				break;

			case ID_WEAPON_INPUT:
				handleAoSWeaponInput((AoSPacket_WeaponInput *)(data));
				break;

			case ID_SET_HP:
				handleAoSSetHP((AoSPacket_SetHP *)(data));
				break;

			case ID_GRENADE_PACKET:
				handleAoSGrenadePacket((AoSPacket_GrenadePacket*)(data));
				break;

			case ID_EXISTING_PLAYER:
				handleAoSExistingPlayer((AoSPacket_ExistingPlayer *)(data));
				break;

			case ID_MOVE_OBJECT:
				handleAoSMoveObject((AoSPacket_MoveObject *)(data));
				break;

			case ID_CREATE_PLAYER:
				handleAoSCreatePlayer((AoSPacket_CreatePlayer *)(data));
				break;

			case ID_SET_TOOL:
				handleAoSSetTool((AoSPacket_SetTool *)(data));
				break;

			case ID_SET_COLOR:
				handleAoSSetColor((AoSPacket_SetColor *)(data));
				break;

			case ID_BLOCK_ACTION:
				handleAoSBlockAction((AoSPacket_BlockAction *)(data));
				break;

			case ID_BLOCK_LINE:
				handleAoSBlockLine((AoSPacket_BlockLine *)(data));
				break;

			case ID_STATE_DATA:
				if (!handleAoSMapEnd())
					return 0;

				handleAoSStateData((AoSPacket_StateData *)(data), (char *)(data) + sizeof(AoSPacket_StateData)); // cpp is so strict
				break;

			case ID_KILL_ACTION:
				handleAoSKillAction((AoSPacket_KillAction *)(data));
				break;

			case ID_CHAT_MESSAGE:
				handleAoSChatMessage((AoSPacket_ChatMessage *)(data));
				break;

			case ID_MAP_START: 
				handleAoSMapStart((AoSPacket_MapStart *)(data));
				break;

			case ID_MAP_CHUNK: 
				handleAoSMapChunk((AoSPacket_MapChunk *)(data), aos_event.packet->dataLength - sizeof(unsigned char));
				break;

			case ID_PLAYER_LEFT:
				handleAoSPlayerLeft((AoSPacket_PlayerLeft *)(data));
				break;

			case ID_TERRITORY_CAP:
				handleAoSTerritoryCapture((AoSPacket_TerritoryCapture *)(data));
				break;

			case ID_PROGRESS_BAR:
				handleAoSProgressBar((AoSPacket_ProgressBar *)(data));
				break;

			case ID_INTEL_CAP:
				handleAoSIntelCapture((AoSPacket_IntelCapture *)(data));
				break;

			case ID_INTEL_PICKUP:
				handleAoSIntelPickup((AoSPacket_IntelPickup *)(data));
				break;

			case ID_INTEL_DROP:
				handleAoSIntelDrop((AoSPacket_IntelDrop *)(data));
				break;

			case ID_RESTOCK:
				handleAoSRestock((AoSPacket_Restock *)(data));
				break;

			case ID_FOG_COLOR:
				handleAoSFogColor((AoSPacket_FogColor *)(data));
				break;

			case ID_WEAPON_RELOAD:
				handleAoSWeaponReload((AoSPacket_WeaponReload *)(data));
				break;

			case ID_HANDSHAKE_INIT:
				handleOSHandshake((OSPacket_Handshake *)(data));
				break;

			case ID_VERSION_GET:
				handleOSVersion();
				break;
			}


		}	enet_packet_destroy(aos_event.packet);
			break;
		case ENET_EVENT_TYPE_DISCONNECT:

			aos_network_disconnect();

			switch (aos_event.data) {
			case ID_DISCONNECT_BANNED:
				showError("Banned");
				break;

			case ID_DISCONNECT_IPLIMIT:
				showError("Connection limit exceeded");
				break;

			case ID_DISCONNECT_VERSION:
				showError("Wrong version");
				break;

			case ID_DISCONNECT_SRVFULL:
				showError("Server full");
				break;

			case ID_DISCONNECT_KICKED:
				showError("Kicked");
				break;

			default:
				showError("Connection interrupted");
				break;
			}

			return 0;
		}
	}

	return 1;
}

void aos_network_send(unsigned char packetID, const void *data, size_t size, bool reliable)
{
	_ENetPacketFlag flag;
	if (reliable)
		flag = ENET_PACKET_FLAG_RELIABLE;
	else
		flag = ENET_PACKET_FLAG_UNSEQUENCED;
	aos_packet = enet_packet_create(&packetID, 1, flag);

	enet_packet_resize(aos_packet, size + 1);
	memcpy(aos_packet->data + 1, data, size);

	enet_peer_send(aos_server, 0, aos_packet);
	aos_ui_add_outbytes(size);
}

long aos_network_get_ping()
{
	if (aos_server == NULL)
		return -1;
	return (long)aos_server->roundTripTime;
}

void aos_network_disconnect()
{
	if (!aos_network_connected)
		return;

	enet_peer_disconnect(aos_server, 0);

	while (enet_host_service(aos_client, &aos_event, 3000) > 0) {
		switch (aos_event.type) {
		case ENET_EVENT_TYPE_RECEIVE:
			enet_packet_destroy(aos_event.packet);
			break;
		case ENET_EVENT_TYPE_DISCONNECT: // disconnect succeeded
			return;
		}
	}

	enet_peer_reset(aos_server);
	enet_host_destroy(aos_client);
	aos_network_connected = false;
}