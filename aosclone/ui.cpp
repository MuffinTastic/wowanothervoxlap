#include "ui.h"

//#include <SDL\SDL.h>
#include <stdio.h>
#include <math.h>
#include <voxlap\voxlap.h>
#include <voxlap\sysmain.h>

#include "config.h"
#include "general.h"
#include "world.h"
#include "vxl.h"
#include "packet.h"
#include "networking.h"

#define VOXLAP_FONT_WIDTH  6
#define VOXLAP_FONT_HEIGHT 8

struct TileType
{
	long f, p;
	long x, y;
};

#define FIXEDSYS_TILESPERLINE    16

#define FIXEDSYS_12_WIDTH         9
#define FIXEDSYS_12_HEIGHT       18

#define FIXEDSYS_18_WIDTH        13
#define FIXEDSYS_18_HEIGHT       27

#define FIXEDSYS_36_WIDTH        26
#define FIXEDSYS_36_HEIGHT       53

#define BAR_SIZE                  4
#define BAR_SCREEN_SIZE          16

static TileType fixedsys_12[256];
static TileType fixedsys_18[256];
static TileType fixedsys_36[256];

// BMPs
static TileType health_symbol;
static TileType block_symbol;
static TileType semi_symbol;
static TileType smg_symbol;
static TileType shotgun_symbol;
static TileType grenade_symbol;
static TileType medical_symbol;
static TileType command_symbol;
static TileType intel_symbol;
static TileType player_symbol;

// PNGs
static TileType semi_scope_image;
static TileType smg_scope_image;
static TileType shotgun_scope_image;
static TileType target_symbol;
static TileType bar_symbol;
static TileType splash_symbol;


bool aos_ui_mapmode;
bool aos_ui_quitmode;
bool aos_ui_chatmode;
bool aos_ui_statmode;
long aos_ui_switchmode;

static long switchmode_team;
static long switchmode_weapon;

static long mmapsiz = 128;
static long* map_buf;

#define STAT_WINDOW 192
#define STAT_SIZE 48

struct StatType
{
	unsigned short fps;
	unsigned short ping;
	unsigned short inbytes;
	unsigned short outbytes;
};

static long *stat_imgbuf;
static StatType stat_buf[STAT_SIZE];

#define SCANNAME_LEN 16
static char scancode_name[SCANNAME_LEN][SCANNAME_LEN];
static aos_ubyte scanname_i = 0;

const char *aos_ui_get_key_name(aos_ubyte scancode)
{
	if (scancode == 0x00) return "No Key";
	const char *ret;

	int len = GetKeyNameTextA(scancode << 16, scancode_name[scanname_i], SCANNAME_LEN);

	if (!len) return "Unknown Key";

	ret = scancode_name[scanname_i];
	scanname_i++; if (scanname_i == SCANNAME_LEN) scanname_i = 0;
	
	return ret;
}

void aos_init_fonts()
{/*
	voxlap::kpzload("png/kasci9x12.png", &kasci912[32].f, &kasci912[32].p, &kasci912[32].x, &kasci912[32].y);
	for (long i = 33; i<128 + 18; i++) {
		kasci912[i].f = kasci912[i - 1].p * KASCI912_HEIGHT + kasci912[i - 1].f;
		kasci912[i].p = kasci912[32].p; kasci912[i].x = KASCI912_WIDTH; kasci912[i].y = KASCI912_HEIGHT;
	}
	kasci912[32].x = KASCI912_WIDTH; kasci912[32].y = KASCI912_HEIGHT;
	for (long i = 0; i<32; i++) kasci912[i] = kasci912[32]; //Fill these in just in case...

	voxlap::kpzload("png/knumb.png", &knumb[0].f, &knumb[0].p, &knumb[0].x, &knumb[0].y);
	for (long i = 1; i < 11; i++) {
		knumb[i].f = knumb[i - 1].p * KNUMB_HEIGHT + knumb[i - 1].f;
		knumb[i].p = knumb[0].p; knumb[i].x = KNUMB_WIDTH; knumb[i].y = KNUMB_HEIGHT;
	}
	knumb[0].x = KNUMB_WIDTH; knumb[0].y = 32;*/

	long tmp_f, tmp_p, tmp_x, tmp_y;

	voxlap::kpzload("fonts/FixedSys_Bold_12.bmp", &tmp_f, &tmp_p, &tmp_x, &tmp_y);
	for (long y = 0; y < tmp_y; y++) {
		for (long x = 0; x < tmp_x; x++) {
			long* pix = (long*)(tmp_f + x * 4 + y * tmp_p);
			if ((*pix & 0xFF000000) && !(*pix & 0x00FFFFFF))
				*pix &= 0x00FFFFFF;
		}
	}

	for (long y = 0; y < tmp_y / FIXEDSYS_12_HEIGHT; y++) {
		for (long x = 0; x < tmp_x / FIXEDSYS_12_WIDTH; x++) {
			long index = 32 + x + y * FIXEDSYS_TILESPERLINE;
			fixedsys_12[index].f = tmp_f + x * FIXEDSYS_12_WIDTH * 4 + y * FIXEDSYS_12_HEIGHT * tmp_p;
			fixedsys_12[index].p = tmp_p; fixedsys_12[index].x = FIXEDSYS_12_WIDTH; fixedsys_12[index].y = FIXEDSYS_12_HEIGHT;
		}
	}
	for (long i = 0; i<32; i++) fixedsys_12[i] = fixedsys_12[32]; //Fill these in just in case...

	voxlap::kpzload("fonts/FixedSys_Bold_18.bmp", &tmp_f, &tmp_p, &tmp_x, &tmp_y);
	for (long y = 0; y < tmp_y; y++) {
		for (long x = 0; x < tmp_x; x++) {
			long* pix = (long*)(tmp_f + x * 4 + y * tmp_p);
			if ((*pix & 0xFF000000) && !(*pix & 0x00FFFFFF))
				*pix &= 0x00FFFFFF;
		}
	}

	for (long y = 0; y < tmp_y / FIXEDSYS_18_HEIGHT; y++) {
		for (long x = 0; x < tmp_x / FIXEDSYS_18_WIDTH; x++) {
			long index = 32 + x + y * FIXEDSYS_TILESPERLINE;
			fixedsys_18[index].f = tmp_f + x * FIXEDSYS_18_WIDTH * 4 + y * FIXEDSYS_18_HEIGHT * tmp_p;
			fixedsys_18[index].p = tmp_p; fixedsys_18[index].x = FIXEDSYS_18_WIDTH; fixedsys_18[index].y = FIXEDSYS_18_HEIGHT;
		}
	}
	for (long i = 0; i<32; i++) fixedsys_18[i] = fixedsys_18[32];


	voxlap::kpzload("fonts/FixedSys_Bold_36.bmp", &tmp_f, &tmp_p, &tmp_x, &tmp_y);
	for (long y = 0; y < tmp_y; y++) {
		for (long x = 0; x < tmp_x; x++) {
			long* pix = (long*)(tmp_f + x * 4 + y * tmp_p);
			if ((*pix & 0xFF000000) && !(*pix & 0x00FFFFFF))
				*pix &= 0x00FFFFFF;
		}
	}

	for (long y = 0; y < tmp_y / FIXEDSYS_36_HEIGHT; y++) {
		for (long x = 0; x < tmp_x / FIXEDSYS_36_WIDTH; x++) {
			long index = 32 + x + y * FIXEDSYS_TILESPERLINE;
			fixedsys_36[index].f = tmp_f + x * FIXEDSYS_36_WIDTH * 4 + y * FIXEDSYS_36_HEIGHT * tmp_p;
			fixedsys_36[index].p = tmp_p; fixedsys_36[index].x = FIXEDSYS_36_WIDTH; fixedsys_36[index].y = FIXEDSYS_36_HEIGHT;
		}
	}
	for (long i = 0; i<32; i++) fixedsys_36[i] = fixedsys_36[32];
}

void aos_uninit_fonts()
{
	if (fixedsys_12[0].f) {
		free((long *)fixedsys_12[0].f);
		fixedsys_12[0].f = 0;
	}

	if (fixedsys_18[0].f) {
		free((long *)fixedsys_18[0].f);
		fixedsys_18[0].f = 0;
	}

	if (fixedsys_36[0].f) {
		free((long *)fixedsys_36[0].f);
		fixedsys_36[0].f = 0;
	}
}

void aos_init_ui_symbols()
{


	voxlap::kpzload("png/health.bmp", &health_symbol.f, &health_symbol.p, &health_symbol.x, &health_symbol.y);
	voxlap::kpzload("png/block.bmp", &block_symbol.f, &block_symbol.p, &block_symbol.x, &block_symbol.y);
	voxlap::kpzload("png/semi.bmp", &semi_symbol.f, &semi_symbol.p, &semi_symbol.x, &semi_symbol.y);
	voxlap::kpzload("png/smg.bmp", &smg_symbol.f, &smg_symbol.p, &smg_symbol.x, &smg_symbol.y);
	voxlap::kpzload("png/shotgun.bmp", &shotgun_symbol.f, &shotgun_symbol.p, &shotgun_symbol.x, &shotgun_symbol.y);
	voxlap::kpzload("png/grenade.bmp", &grenade_symbol.f, &grenade_symbol.p, &grenade_symbol.x, &grenade_symbol.y);
	voxlap::kpzload("png/command.bmp", &command_symbol.f, &command_symbol.p, &command_symbol.x, &command_symbol.y);
	voxlap::kpzload("png/medical.bmp", &medical_symbol.f, &medical_symbol.p, &medical_symbol.x, &medical_symbol.y);
	voxlap::kpzload("png/intel.bmp", &intel_symbol.f, &intel_symbol.p, &intel_symbol.x, &intel_symbol.y);
	voxlap::kpzload("png/player.bmp", &player_symbol.f, &player_symbol.p, &player_symbol.x, &player_symbol.y);

	voxlap::kpzload("png/semi.png", &semi_scope_image.f, &semi_scope_image.p, &semi_scope_image.x, &semi_scope_image.y);
	voxlap::kpzload("png/smg.png", &smg_scope_image.f, &smg_scope_image.p, &smg_scope_image.x, &smg_scope_image.y);
	voxlap::kpzload("png/shotgun.png", &shotgun_scope_image.f, &shotgun_scope_image.p, &shotgun_scope_image.x, &shotgun_scope_image.y);
	voxlap::kpzload("png/white.png", &bar_symbol.f, &bar_symbol.p, &bar_symbol.x, &bar_symbol.y);
	voxlap::kpzload("png/target.png", &target_symbol.f, &target_symbol.p, &target_symbol.x, &target_symbol.y);
	voxlap::kpzload("png/splash.png", &splash_symbol.f, &splash_symbol.p, &splash_symbol.x, &splash_symbol.y);

	for (long y = 0; y < health_symbol.y; y++) {
		for (long x = 0; x < health_symbol.x; x++) {
			long* pix = (long*)(health_symbol.f + x * 4 + y * health_symbol.p);
			if ((*pix & 0xFF000000) && !(*pix & 0x00FFFFFF))
				*pix &= 0x00FFFFFF;
		}
	}

	for (long y = 0; y < block_symbol.y; y++) {
		for (long x = 0; x < block_symbol.x; x++) {
			long* pix = (long*)(block_symbol.f + x * 4 + y * block_symbol.p);
			if ((*pix & 0xFF000000) && !(*pix & 0x00FFFFFF))
				*pix &= 0x00FFFFFF;
		}
	}
	for (long y = 0; y < semi_symbol.y; y++) {
		for (long x = 0; x < semi_symbol.x; x++) {
			long* pix = (long*)(semi_symbol.f + x * 4 + y * semi_symbol.p);
			if ((*pix & 0xFF000000) && !(*pix & 0x00FFFFFF))
				*pix &= 0x00FFFFFF;
		}
	}

	for (long y = 0; y < smg_symbol.y; y++) {
		for (long x = 0; x < smg_symbol.x; x++) {
			long* pix = (long*)(smg_symbol.f + x * 4 + y * smg_symbol.p);
			if ((*pix & 0xFF000000) && !(*pix & 0x00FFFFFF))
				*pix &= 0x00FFFFFF;
		}
	}

	for (long y = 0; y < shotgun_symbol.y; y++) {
		for (long x = 0; x < shotgun_symbol.x; x++) {
			long* pix = (long*)(shotgun_symbol.f + x * 4 + y * shotgun_symbol.p);
			if ((*pix & 0xFF000000) && !(*pix & 0x00FFFFFF))
				*pix &= 0x00FFFFFF;
		}
	}

	for (long y = 0; y < grenade_symbol.y; y++) {
		for (long x = 0; x < grenade_symbol.x; x++) {
			long* pix = (long*)(grenade_symbol.f + x * 4 + y * grenade_symbol.p);
			if ((*pix & 0xFF000000) && !(*pix & 0x00FFFFFF))
				*pix &= 0x00FFFFFF;
		}
	}

	for (long y = 0; y < medical_symbol.y; y++) {
		for (long x = 0; x < medical_symbol.x; x++) {
			long* pix = (long*)(medical_symbol.f + x * 4 + y * medical_symbol.p);
			if ((*pix & 0xFF000000) && !(*pix & 0x00FFFFFF))
				*pix &= 0x00FFFFFF;
		}
	}

	for (long y = 0; y < command_symbol.y; y++) {
		for (long x = 0; x < command_symbol.x; x++) {
			long* pix = (long*)(command_symbol.f + x * 4 + y * command_symbol.p);
			if ((*pix & 0xFF000000) && !(*pix & 0x00FFFFFF))
				*pix &= 0x00FFFFFF;
		}
	}

	for (long y = 0; y < intel_symbol.y; y++) {
		for (long x = 0; x < intel_symbol.x; x++) {
			long* pix = (long*)(intel_symbol.f + x * 4 + y * intel_symbol.p);
			if ((*pix & 0xFF000000) && !(*pix & 0x00FFFFFF))
				*pix &= 0x00FFFFFF;
		}
	}

	for (long y = 0; y < player_symbol.y; y++) {
		for (long x = 0; x < player_symbol.x; x++) {
			long* pix = (long*)(player_symbol.f + x * 4 + y * player_symbol.p);
			if ((*pix & 0xFF000000) && !(*pix & 0x00FFFFFF))
				*pix &= 0x00FFFFFF;
		}
	}

	//for (long y = 0; y < splash_symbol.y; y++) {
	//	for (long x = 0; x < splash_symbol.x; x++) {
	//		long* pix = (long*)(splash_symbol.f + x * 4 + y * splash_symbol.p);
	//		//if (!(*pix & 0xFF000000))
	//		//	*pix = 0x40400000;
	//	}
	//}
}

void aos_uninit_ui_symbols()
{
	if (health_symbol.f) { free((void*)health_symbol.f); health_symbol.f = 0; }
	if (block_symbol.f) { free((void*)block_symbol.f); block_symbol.f = 0; }
	if (semi_symbol.f) { free((void*)semi_symbol.f); semi_symbol.f = 0; }
	if (smg_symbol.f) { free((void*)smg_symbol.f); smg_symbol.f = 0; }
	if (shotgun_symbol.f) { free((void*)shotgun_symbol.f); shotgun_symbol.f = 0; }
	if (grenade_symbol.f) { free((void*)grenade_symbol.f); grenade_symbol.f = 0; }
	if (medical_symbol.f) { free((void*)medical_symbol.f); medical_symbol.f = 0; }
	if (command_symbol.f) { free((void*)command_symbol.f); command_symbol.f = 0; }
	if (intel_symbol.f) { free((void*)intel_symbol.f); intel_symbol.f = 0; }
	if (player_symbol.f) { free((void*)player_symbol.f); player_symbol.f = 0; }

	if (semi_scope_image.f) { free((void*)semi_scope_image.f); semi_scope_image.f = 0; }
	if (smg_scope_image.f) { free((void*)smg_scope_image.f); smg_scope_image.f = 0; }
	if (shotgun_scope_image.f) { free((void*)shotgun_scope_image.f); shotgun_scope_image.f = 0; }
	if (bar_symbol.f) { free((void*)bar_symbol.f); bar_symbol.f = 0; }
	if (target_symbol.f) { free((void*)target_symbol.f); target_symbol.f = 0; }
	if (splash_symbol.f) { free((void*)splash_symbol.f); splash_symbol.f = 0; }
}

void aos_init_ui_switchmode()
{
	voxlap::extension::FloatVector3D tmp, axis = { 0, 0, 1 };

	tmp = { 3, 0, -.25 };
	voxlap::axisrotate(&tmp, &axis, -30 * (PI / 180.));
	aos_players[32].existingplayer(ID_TEAM1, WEAPON_SEMI, TOOL_SPADE, 0, 0, "Blue Deuce");
	//aos_players[32].render_scale = 1 / 2.0f;
	//tmp += voxlap::extension::FloatVector3D(0, 0, -1);
	aos_players[32].reposition(&tmp);
	tmp = -tmp.normalized();
	aos_players[32].reorient(&tmp);

	tmp = { 3, 0, -.25 };
	voxlap::axisrotate(&tmp, &axis, 30 * (PI / 180.));
	aos_players[33].existingplayer(ID_TEAM2, WEAPON_SEMI, TOOL_SPADE, 0, 0, "Green Deuce");
	//aos_players[33].render_scale = 1 / 2.0f;
	aos_players[33].reposition(&tmp);
	tmp = -tmp.normalized();
	aos_players[33].reorient(&tmp);
}

void aos_print12(long x, long y, char centerX, char centerY, long color, const char *format, ...)
{
	char* argslist;
	char text[256];

	va_start(argslist, format);
	vsprintf(text, format, argslist);
	va_end(argslist);

	long len = strlen(text);
	long cx = 0;
	long cy = 0;

	if (centerX != 1) {
		if (centerX == 0) {
			cx = (long)((len)* FIXEDSYS_12_WIDTH / 2.0);
		} else if (centerX == -1) {
			cx = (long)((len)* FIXEDSYS_12_WIDTH);
		}
	}

	if (centerY != 1) {
		if (centerY == 0) {
			cy = (long)(FIXEDSYS_12_HEIGHT / 2);
		} else if (centerY == -1) {
			cy = (long)(FIXEDSYS_12_HEIGHT);
		}
	}

	for (long i = 0; i<len; i++) {
		voxlap::drawtile(fixedsys_12[text[i]].f, fixedsys_12[text[i]].p, fixedsys_12[text[i]].x, fixedsys_12[text[i]].y, cx << 16, cy << 16,
			((i * FIXEDSYS_12_WIDTH) + x) << 16, (y) << 16, 65536, 65536, 0, 0xFF000000 | color);
	}
}

void aos_print18(long x, long y, char centerX, char centerY, long color, const char *format, ...)
{
	char* argslist;
	char text[256];

	va_start(argslist, format);
	vsprintf(text, format, argslist);
	va_end(argslist);

	long len = strlen(text);
	long cx = 0;
	long cy = 0;

	if (centerX != 1) {
		if (centerX == 0) {
			cx = (long)((len)* FIXEDSYS_18_WIDTH / 2.0);
		} else if (centerX == -1) {
			cx = (long)((len)* FIXEDSYS_18_WIDTH);
		}
	}

	if (centerY != 1) {
		if (centerY == 0) {
			cy = (long)(FIXEDSYS_18_HEIGHT / 2);
		} else if (centerY == -1) {
			cy = (long)(FIXEDSYS_18_HEIGHT);
		}
	}

	for (long i = 0; i<len; i++) {
		voxlap::drawtile(fixedsys_18[text[i]].f, fixedsys_18[text[i]].p, fixedsys_18[text[i]].x, fixedsys_18[text[i]].y, cx << 16, cy << 16,
			((i * FIXEDSYS_18_WIDTH) + x) << 16, (y) << 16, 65536, 65536, 0, 0xFF000000 | color);
	}
}

void aos_print36(long x, long y, char centerX, char centerY, long color, const char *format, ...)
{
	char* argslist;
	char text[256];

	va_start(argslist, format);
	vsprintf(text, format, argslist);
	va_end(argslist);

	long len = strlen(text);
	long cx = 0;
	long cy = 0;

	if (centerX != 1) {
		if (centerX == 0) {
			cx = (long)((len)* FIXEDSYS_36_WIDTH / 2.0);
		} else if (centerX == -1) {
			cx = (long)((len)* FIXEDSYS_36_WIDTH);
		}
	}

	if (centerY != 1) {
		if (centerY == 0) {
			cy = (long)(FIXEDSYS_36_HEIGHT / 2);
		} else if (centerY == -1) {
			cy = (long)(FIXEDSYS_36_HEIGHT);
		}
	}

	for (long i = 0; i<len; i++) {
		voxlap::drawtile(fixedsys_36[text[i]].f, fixedsys_36[text[i]].p, fixedsys_36[text[i]].x, fixedsys_36[text[i]].y, cx << 16, cy << 16,
			((i * FIXEDSYS_36_WIDTH) + x) << 16, (y) << 16, 65536, 65536, 0, 0xFF000000 | color);
	}
}

// Centered
void aos_ui_drawbar(long x, long y, long width, long forecolor, long backcolor, double percent, double max)
{
	long cx = width / 2;
	long cy = 2; // BAR_SIZE / 2

	voxlap::drawtile(bar_symbol.f, bar_symbol.p, bar_symbol.x, bar_symbol.y, 0, (BAR_SIZE / 2) << 16,
		(x - cx) << 16, (y - cy) << 16, (width / BAR_SIZE) << 16, 6 << 16, 0, 0xFF000000 | backcolor);

	long bar_width = ((long)((width * (percent / max))) / BAR_SIZE) << 16;
	if (bar_width) {
		voxlap::drawtile(bar_symbol.f, bar_symbol.p, bar_symbol.x, bar_symbol.y, 0, (BAR_SIZE / 2) << 16,
			(x - cx) << 16, (y - cy) << 16, bar_width, 6 << 16, 0, 0xFF000000 | forecolor);
	}
}

void aos_ui_updateminimap()
{
	for (long x = 0; x < VSID; x++) {
		for (long y = 0; y < VSID; y++) {
			if ((x % 64) == 0 || (y % 64) == 0 || x == VSID-1 || y == VSID-1) {
				map_buf[(x + VSID * y)] = 0xFFCCCCCC;
			} else {
				long pclr = voxlap::getcube(x, y, voxlap::getfloorz(x, y, 0));
				if (pclr == 0 || pclr == 1) {
					map_buf[(x + VSID * y)] = 0xFF000000;
					continue;
				}
				long blockcol = (*(long*)pclr & 0x00FFFFFF);
				unsigned char mapR, mapG, mapB;
				get_color(&mapR, &mapG, &mapB, blockcol); // splits long pixel color val into RGB chars
				float lighting = ((*(long*)pclr & 0xFF000000) >> 24) / 128.0f;
				mapR = min(max(mapR * lighting, 0), 255);
				mapG = min(max(mapG * lighting, 0), 255);
				mapB = min(max(mapB * lighting, 0), 255);
				map_buf[(x + VSID * y)] = make_color(mapR, mapG, mapB) | 0xFF000000;
			}
		}
	}
}

inline voxlap::extension::FloatVector3D rotpoint(voxlap::point3d* f, voxlap::point3d* s, voxlap::point3d p)
{
	voxlap::point3d r;
	r.x = p.x * s->x + p.y * s->y;
	r.y = p.x * f->x + p.y * f->y;
	return r;
}

void aos_ui_minimap_entity_pos(float* x, float* y, bool fullmap)
{
	if (fullmap) {
		*x += sysmain::xres / 2 - VSID / 2;
		*y += sysmain::yres / 2 - VSID / 2;
	} else {
		PlayerType* localply = aos_world_get_local_player();

		long mmaphalf = (mmapsiz / 2);
		long mmx = min(max((long)localply->cam_position.x, mmaphalf), VSID - mmaphalf) - mmaphalf;
		long mmy = min(max((long)localply->cam_position.y, mmaphalf), VSID - mmaphalf) - mmaphalf;
		
		*x += sysmain::xres - mmapsiz - 20 - mmx;
		*y += 20 - mmy;

		*x = min(max(*x, sysmain::xres - mmapsiz - 20), sysmain::xres - 20);
		*y = min(max(*y, 20), mmapsiz + 20);
	}
}

void aos_ui_minimap_drawplayer(long frameptr, long pitch, long xdim, long ydim, PlayerType *player, bool fullmap)
{
	voxlap::extension::FloatVector3D xypos = player->cam_position; xypos.z = 0.0f;
	voxlap::extension::FloatVector3D xyfor = player->orientation_for; xyfor.z = 0.0f; xyfor = xyfor.normalized();
	float f = sqrtf(xyfor.x * xyfor.x + xyfor.y * xyfor.y);
	voxlap::extension::FloatVector3D xystr = { -xyfor.y / f, xyfor.x / f, 0.0f };

	voxlap::extension::FloatVector3D p1(-1.0f, 1.0f, 0.0f), p2(1.0f, 1.0f, 0.0f), p3(1.0f, -1.0f, 0.0f), p4(-1.0f, -1.0f, 0.0f);

	aos_ui_minimap_entity_pos(&xypos.x, &xypos.y, fullmap);

	p1 = rotpoint(&xyfor, &xystr, p1) * player_symbol.x / 2; p1 += xypos;
	p2 = rotpoint(&xyfor, &xystr, p2) * player_symbol.x / 2; p2 += xypos;
	p3 = rotpoint(&xyfor, &xystr, p3) * player_symbol.x / 2; p3 += xypos;
	p4 = rotpoint(&xyfor, &xystr, p4) * player_symbol.x / 2; p4 += xypos;

	long color = 0xFFFFFF;
	if (player->local) color = 0x00FFFF;
	else {
		switch (player->team) {
		case ID_TEAM1:
			color = aos_team1_team.color;
			break;
		case ID_TEAM2:
			color = aos_team2_team.color;
			break;
		//case ID_TEAM_SPEC: // never reached
		//	color = aos_spectator_team.color;
		//	break;
		}
	}

	voxlap::drawpicinquadalpha(player_symbol.f, player_symbol.p, player_symbol.x, player_symbol.y,
		frameptr, pitch, xdim, ydim,
		p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, p4.x, p4.y, color, 0);
}


void drawteamsymbol(long frameptr, long pitch, long xdim, long ydim, TileType* symbol, float x, float y, long color, long spec)
{
	voxlap::extension::FloatVector3D p1(-1.0f, -1.0f, 0.0f), p2(1.0f, -1.0f, 0.0f), p3(1.0f, 1.0f, 0.0f), p4(-1.0f, 1.0f, 0.0f);
	voxlap::point3d t;
	//t = p2;
	//p2 = p3;
	//p3 = t;
	
	p1 *= symbol->x / 2;
	p2 *= symbol->x / 2;
	p3 *= symbol->x / 2;
	p4 *= symbol->x / 2;

	p1.x += x; p1.y += y;
	p2.x += x; p2.y += y;
	p3.x += x; p3.y += y;
	p4.x += x; p4.y += y;

	voxlap::drawpicinquadalpha(symbol->f, symbol->p, symbol->x, symbol->y,
		frameptr, pitch, xdim, ydim,
		roundf(p1.x), roundf(p1.y), roundf(p2.x), roundf(p2.y), roundf(p3.x), roundf(p3.y), roundf(p4.x), roundf(p4.y), color, spec);
}

void aos_ui_minimap_draw_team_entities(long frameptr, long pitch, long xdim, long ydim, bool fullmap)
{
	float x, y;

	if (aos_gamemode == ID_GAMEMODE_CTF) {
		x = aos_team1_team.ctf_base_pos.x;
		y = aos_team1_team.ctf_base_pos.y;
		aos_ui_minimap_entity_pos(&x, &y, fullmap);
		drawteamsymbol(frameptr, pitch, xdim, ydim, &medical_symbol, x, y, aos_team1_team.color, 0xFFF0F0F0);

		x = aos_team2_team.ctf_base_pos.x;
		y = aos_team2_team.ctf_base_pos.y;
		aos_ui_minimap_entity_pos(&x, &y, fullmap);
		drawteamsymbol(frameptr, pitch, xdim, ydim, &medical_symbol, x, y, aos_team2_team.color, 0xFFF0F0F0);

		if (aos_team1_team.ctf_flag_dropped) {
			x = aos_team1_team.ctf_flag_pos.x;
			y = aos_team1_team.ctf_flag_pos.y;
			aos_ui_minimap_entity_pos(&x, &y, fullmap);
			drawteamsymbol(frameptr, pitch, xdim, ydim, &intel_symbol, x, y, aos_team1_team.color, 0);
		}

		if (aos_team2_team.ctf_flag_dropped && aos_team1_team.ctf_flag_pos != aos_team2_team.ctf_flag_pos) {
			x = aos_team2_team.ctf_flag_pos.x;
			y = aos_team2_team.ctf_flag_pos.y;
			aos_ui_minimap_entity_pos(&x, &y, fullmap);
			drawteamsymbol(frameptr, pitch, xdim, ydim, &intel_symbol, x, y, aos_team2_team.color, 0);
		}
	} else if (aos_gamemode == ID_GAMEMODE_TC) {
		for (int i = 0; i < aos_tc_territory_count; i++) {
			TC_Territory* territory = &aos_tc_territories[i];
			x = territory->position.x;
			y = territory->position.y;
			long color = 0xFFFFFF;
			switch (territory->team) {
			case ID_TEAM1:
				color = aos_team1_team.color;
				break;
			case ID_TEAM2:
				color = aos_team2_team.color;
				break;
			}
			aos_ui_minimap_entity_pos(&x, &y, fullmap);
			drawteamsymbol(frameptr, pitch, xdim, ydim, &command_symbol, x, y, color, 0);
		}
	}
}

void aos_ui_draw_minimap(long frameptr, long pitch, long xdim, long ydim)
{
	PlayerType *localply = aos_world_get_local_player();
	if (localply->team == PLAYER_UNCONNECTED) return;
	long mmaphalf = (mmapsiz / 2);
	long mmapquart = (mmapsiz / 4);
	if (aos_ui_mapmode) {
		voxlap::drawtile((long)map_buf, VSID * sizeof(long), VSID, VSID, VSID << 15, VSID << 15,
			sysmain::xres / 2 << 16, sysmain::yres / 2 << 16, 65536, 65536, 0, -1);
		for (int i = 0; i < 8; i++) {
			voxlap::print6x8(min(max((sysmain::xres / 2 - VSID / 2) + (mmapquart - VOXLAP_FONT_WIDTH / 2) + (mmaphalf *i), 0), sysmain::xres - VOXLAP_FONT_WIDTH), min(max((sysmain::yres / 2 - VSID / 2) - VOXLAP_FONT_HEIGHT - 4, 0), sysmain::yres - VOXLAP_FONT_HEIGHT), 0xFFFFFF, -1, "%c", 'A' + i);
			voxlap::print6x8(min(max((sysmain::xres / 2 - VSID / 2) - VOXLAP_FONT_WIDTH - 4, 0), sysmain::xres - VOXLAP_FONT_WIDTH), min(max((sysmain::yres / 2 - VSID / 2) + (mmapquart - VOXLAP_FONT_HEIGHT / 2) + (mmaphalf *i), 0), sysmain::yres - VOXLAP_FONT_HEIGHT), 0xFFFFFF, -1, "%c", '1' + i);
		}

	} else {
		long x = min(max((long)localply->cam_position.x, mmaphalf), VSID - mmaphalf) - mmaphalf;
		long y = min(max((long)localply->cam_position.y, mmaphalf), VSID - mmaphalf) - mmaphalf;

		aos_print12((sysmain::xres - mmaphalf - 20), 20, 0, -1, 0xFFFFFF, aos_world_get_sector(localply->position.x, localply->position.y));
		voxlap::drawtile((long)&map_buf[(x + VSID * y)], VSID * sizeof(long), mmapsiz, mmapsiz, 0, 0,
			(sysmain::xres - mmapsiz - 20) << 16, 20 << 16, 65536, 65536, 0, -1);
		
	}

	// draw entities

	aos_ui_minimap_draw_team_entities(frameptr, pitch, xdim, ydim, aos_ui_mapmode);

	for (int i = 0; i < 32; i++) {
		PlayerType* ply = &aos_players[i];
		if (ply->local) continue;
		if (ply->team == PLAYER_UNCONNECTED) continue;
		if (localply->team != ID_TEAM_SPEC && ply->team != localply->team) continue;
		aos_ui_minimap_drawplayer(frameptr, pitch, xdim, ydim, ply, aos_ui_mapmode);
	}

	aos_ui_minimap_drawplayer(frameptr, pitch, xdim, ydim, localply, aos_ui_mapmode);
}

#define STAT_UPDATE_RATE 1.0f
static float stat_last_shuffle = 0.0f;
extern float fsynctics;
extern double dtotclk;

void aos_ui_updatestats()
{
	if (stat_last_shuffle + STAT_UPDATE_RATE < dtotclk) {
		stat_last_shuffle = dtotclk;
		
		for (int x = 0; x < STAT_WINDOW; x++) {
			for (int y = 0; y < STAT_WINDOW; y++) {
				stat_imgbuf[(x + STAT_WINDOW * y)] = 0x80000000;
			}

			stat_imgbuf[(x + STAT_WINDOW * min(max(0, STAT_WINDOW - stat_buf[x / 4].fps), STAT_WINDOW - 1))] = 0xFF00FF21;
			stat_imgbuf[(x + STAT_WINDOW * min(max(0, STAT_WINDOW - stat_buf[x / 4].ping), STAT_WINDOW - 1))] = 0xFFFF0000;
			stat_imgbuf[(x + STAT_WINDOW * min(max(0, STAT_WINDOW - stat_buf[x / 4].inbytes / 8), STAT_WINDOW - 1))] = 0xFFFFD800;
			stat_imgbuf[(x + STAT_WINDOW * min(max(0, STAT_WINDOW - stat_buf[x / 4].outbytes / 8), STAT_WINDOW - 1))] = 0xFFFF6A00;
		}

		for (int i = 0; i < STAT_SIZE - 1; i++)
			stat_buf[i] = stat_buf[i + 1];
		stat_buf[STAT_SIZE - 1].fps = 1.0f / fsynctics;
		stat_buf[STAT_SIZE - 1].ping = aos_network_get_ping();
		stat_buf[STAT_SIZE - 1].inbytes = 0;
		stat_buf[STAT_SIZE - 1].outbytes = 0;
	}
}

void update_song()
{
	
}

void aos_ui_draw_stats()
{
	if (aos_ui_statmode) {
		voxlap::drawtile((long)stat_imgbuf, STAT_WINDOW * sizeof(long), STAT_WINDOW, STAT_WINDOW, 0, STAT_WINDOW << 16,
			0 << 16, (sysmain::yres / 2) << 16, 65536, 65536, 0, 0);
		voxlap::print6x8(0, sysmain::yres / 2 + VOXLAP_FONT_HEIGHT, 0x00FF21, 0x80000000,     "fps %d", stat_buf[STAT_SIZE - 2].fps);
		voxlap::print6x8(0, sysmain::yres / 2 + VOXLAP_FONT_HEIGHT * 2, 0xFF0000, 0x80000000, "ping %d", stat_buf[STAT_SIZE - 2].ping);
		voxlap::print6x8(0, sysmain::yres / 2 + VOXLAP_FONT_HEIGHT * 3, 0xFFFFFF, 0x80000000, "per 100ms:");
		voxlap::print6x8(0, sysmain::yres / 2 + VOXLAP_FONT_HEIGHT * 4, 0xFFD800, 0x80000000, "  in  %db", stat_buf[STAT_SIZE - 2].inbytes);
		voxlap::print6x8(0, sysmain::yres / 2 + VOXLAP_FONT_HEIGHT * 5, 0xFF6A00, 0x80000000, "  out %d", stat_buf[STAT_SIZE - 2].outbytes);
	}
}

void aos_ui_add_inbytes(unsigned short in)
{
	stat_buf[STAT_SIZE - 1].inbytes += in;
}

void aos_ui_add_outbytes(unsigned short out)
{
	stat_buf[STAT_SIZE - 1].outbytes += out;
}

void aos_ui_draw_quit()
{
	if (aos_ui_quitmode) {
		aos_print36(sysmain::xres / 2, sysmain::yres / 3, 0, 0, 0xFF0000, "EXIT GAME? Y/N");
	}

	if (!sysmain::progactive) { // I'd put this somewhere else but it works for now
		aos_print36(sysmain::xres / 2, sysmain::yres - sysmain::yres / 3, 0, -1, 0xFFD800, "Input Disabled");
		aos_print18(sysmain::xres / 2, sysmain::yres - sysmain::yres / 3, 0, 1, 0xFFD800, "Use show_mouse key to reenable input");
	}
}


#define CHAT_MESSAGE_TYPE_LENGTH 128

struct chat_message_type
{
	float time;
	long color;
	char text[CHAT_MESSAGE_TYPE_LENGTH];
};

#define UI_MESSAGE_COUNT 10
#define UI_KILLFEED_COUNT 10
#define UI_MESSAGE_DISPLAY_TIME 8.0

static chat_message_type messages[UI_MESSAGE_COUNT];
static chat_message_type killfeed[UI_KILLFEED_COUNT];

static char local_chat_buf[CHAT_MESSAGE_MAX_LENGTH];
static aos_ubyte chat_type;

void chat_enable(bool team)
{	
	switch (team) {
	case false:
		chat_type = ID_CHAT_MESSAGE_ALL;
		break;

	case true:
		chat_type = ID_CHAT_MESSAGE_TEAM;
		break;
	}

	memset(local_chat_buf, 0, CHAT_MESSAGE_MAX_LENGTH);
	aos_ui_chatmode = true;
}

void chat_disable()
{
	aos_ui_chatmode = false;
}

void chat_delete() {
	if (strlen(local_chat_buf) > 0) {
		local_chat_buf[strlen(local_chat_buf) - 1] = '\0';
	}
}

void chat_append(char c)
{
	if (strlen(local_chat_buf) + 1 < CHAT_MESSAGE_MAX_LENGTH) {
		local_chat_buf[strlen(local_chat_buf)] = c;
	}
}

void chat_send()
{
	if (strlen(local_chat_buf) < 1)
		return;

	aos_playsound(AOS_SOUND_CHAT, config::vol, 1.0, NULL, 0);

	AoSPacket_ChatMessage message;
	message.playerID = aos_local_player_id;
	message.type = chat_type;
	aos_strcpycut(message.text, local_chat_buf, CHAT_MESSAGE_MAX_LENGTH);
	aos_network_send(ID_CHAT_MESSAGE, &message, sizeof(AoSPacket_ChatMessage), true);
}

void aos_ui_add_chat_message(long color, char *format, ...)
{
	char* argslist;
	char formatted_text[280];

	for (int i = UI_MESSAGE_COUNT - 1; i > 0; i--) {
		messages[i] = messages[i - 1];
	}

	chat_message_type *message = &messages[0];
	message->time = dtotclk;
	message->color = color;

	va_start(argslist, format);
	vsprintf(formatted_text, format, argslist);
	va_end(argslist);

	aos_strcpycut(message->text, formatted_text, CHAT_MESSAGE_TYPE_LENGTH);
}

void aos_ui_add_killfeed_message(long color, char *format, ...)
{
	char* argslist;
	char formatted_text[280];

	for (int i = UI_KILLFEED_COUNT - 1; i > 0; i--) {
		killfeed[i] = killfeed[i - 1];
	}
	chat_message_type *message = &killfeed[0];
	message->time = dtotclk;
	message->color = color;

	va_start(argslist, format);
	vsprintf(formatted_text, format, argslist);
	va_end(argslist);

	aos_strcpycut(message->text, formatted_text, CHAT_MESSAGE_TYPE_LENGTH);
}

#define SCREEN_SIDE_OFFSET VOXLAP_FONT_WIDTH
#define VOXLAP_FONT_SPACING 4

void aos_ui_draw_chat()
{
	if (aos_ui_chatmode) {
		if (chat_type == ID_CHAT_MESSAGE_ALL) {
			voxlap::print6x8(SCREEN_SIDE_OFFSET, sysmain::yres - SCREEN_SIDE_OFFSET - (UI_MESSAGE_COUNT*(VOXLAP_FONT_HEIGHT + VOXLAP_FONT_SPACING)) - (2 * (VOXLAP_FONT_HEIGHT + VOXLAP_FONT_SPACING)), 0xFFFFFF, -1, "Chat:", local_chat_buf);
		} else {
			voxlap::print6x8(SCREEN_SIDE_OFFSET, sysmain::yres - SCREEN_SIDE_OFFSET - (UI_MESSAGE_COUNT*(VOXLAP_FONT_HEIGHT + VOXLAP_FONT_SPACING)) - (2 * (VOXLAP_FONT_HEIGHT + VOXLAP_FONT_SPACING)), 0xFFFFFF, -1, "Team:", local_chat_buf);
		}
		voxlap::print6x8(SCREEN_SIDE_OFFSET, sysmain::yres - SCREEN_SIDE_OFFSET - (UI_MESSAGE_COUNT*(VOXLAP_FONT_HEIGHT + VOXLAP_FONT_SPACING)) - (VOXLAP_FONT_HEIGHT + VOXLAP_FONT_SPACING), 0xFFFFFF, -1, "%s_", local_chat_buf); // one liner text cursor!
	}

	for (int i = 0; i < UI_MESSAGE_COUNT; i++) {
		chat_message_type *message = &messages[i];
		if (strlen(message->text) == 0 || message->time == 0.0)
			continue;
		if (message->time + UI_MESSAGE_DISPLAY_TIME > dtotclk) {
			voxlap::print6x8(SCREEN_SIDE_OFFSET, sysmain::yres - SCREEN_SIDE_OFFSET - (UI_MESSAGE_COUNT*(VOXLAP_FONT_HEIGHT + VOXLAP_FONT_SPACING)) + (i * (VOXLAP_FONT_HEIGHT + VOXLAP_FONT_SPACING)), message->color, -1, message->text);
		}
	}

	for (int i = 0; i < UI_KILLFEED_COUNT; i++) {
		chat_message_type *message = &killfeed[i];
		if (strlen(message->text) == 0 || message->time == 0.0)
			continue;
		if (message->time + UI_MESSAGE_DISPLAY_TIME > dtotclk) {
			voxlap::print6x8(SCREEN_SIDE_OFFSET, SCREEN_SIDE_OFFSET + (i * (VOXLAP_FONT_HEIGHT + VOXLAP_FONT_SPACING)), message->color, -1, message->text);
		}
	}
}

void aos_ui_init()
{
	//ap_buf = new long[VSID * VSID];

	memset(local_chat_buf, 0, CHAT_MESSAGE_MAX_LENGTH);
	memset(messages, 0, sizeof(chat_message_type) * UI_MESSAGE_COUNT);
	memset(killfeed, 0, sizeof(chat_message_type) * UI_KILLFEED_COUNT);
	chat_type = ID_CHAT_MESSAGE_ALL;

	aos_ui_chatmode = aos_ui_mapmode = aos_ui_quitmode = aos_ui_statmode = false;

	aos_init_fonts();
	aos_init_ui_switchmode();

	aos_init_ui_symbols();

	map_buf = (long *)malloc(VSID * VSID * sizeof(long));
	stat_imgbuf = (long *)malloc(STAT_WINDOW * STAT_WINDOW * sizeof(long));
}

void aos_ui_uninit()
{
	free(map_buf);
	free(stat_imgbuf);

	aos_uninit_ui_symbols();

	aos_uninit_fonts();
}

void aos_ui_attempt_respawn_sound(aos_byte respawncounter)
{
	if (respawncounter <= 0) return;
	
	switch (respawncounter) {
	case 3:
	case 2:
		aos_playsound(AOS_SOUND_BEEP2, config::vol, 1.0, NULL, NULL);
		break;
	case 1:
		aos_playsound(AOS_SOUND_BEEP1, config::vol, 1.0, NULL, NULL);
		break;
	}
}

void aos_ui_draw_health()
{
	PlayerType *cur_pl = aos_world_get_local_player();

	if (cur_pl->team > ID_TEAM_SPEC) {
		if (cur_pl->alive) {
			aos_ubyte hp = cur_pl->health;

			long color = 0xFFFFFF;
			if (hp <= 25) {
				color = 0xFF1420;
			}
			aos_print36(sysmain::xres / 2, sysmain::yres - FIXEDSYS_36_HEIGHT / 2, -1, 0, color, "%d", hp);
			voxlap::drawtile(health_symbol.f, health_symbol.p, health_symbol.x, health_symbol.y, 0, (-health_symbol.y / 3) << 16,
				(sysmain::xres / 2) << 16, (sysmain::yres - FIXEDSYS_36_HEIGHT) << 16, 1 << 16, 1 << 16, 0, 0xFF000000 | color);
		} else {
			if (cur_pl->respawntime && cur_pl->respawntime != 255) {
				aos_print36(sysmain::xres / 2, sysmain::yres - FIXEDSYS_36_HEIGHT / 2, 0, 0, 0xFF0000, "INSERT COIN: %d", cur_pl->respawntime);
			}
		}
	}

	/*if (!cur_pl->alive || cur_pl->team == ID_TEAM_SPEC) {
		aos_print12(sysmain::xres / 2, 0, 0, 1, 0xFFFF00, "Click to change spectated player");
		aos_print12(sysmain::xres / 2, FIXEDSYS_12_HEIGHT, 0, 1, 0xFFFF00, "Press Reload to change spectate mode");

		if (cur_pl->specmode > 0) {
			PlayerType *spec_pl = &aos_players[cur_pl->specid];
			aos_print12(sysmain::xres / 2, FIXEDSYS_12_HEIGHT * 2, 0, 1, aos_world_get_team(spec_pl->team)->color, "%s", spec_pl->name);
		}
	}*/
}

void aos_ui_draw_switch()
{
	if (aos_ui_switchmode != AOS_UI_SWITCH_NONE) {
		switch (aos_ui_switchmode) {
		case AOS_UI_SWITCH_TEAM:
			aos_print12(0 + FIXEDSYS_36_HEIGHT, sysmain::yres - FIXEDSYS_36_HEIGHT, 1, -1, 0xFF0000, "Press 1 to join %s", aos_world_get_team(ID_TEAM1)->name);
			aos_print12(sysmain::xres / 2, sysmain::yres - FIXEDSYS_36_HEIGHT, 0, -1, 0xFF0000, "Press 3 to spectate");
			aos_print12(sysmain::xres - FIXEDSYS_36_HEIGHT, sysmain::yres - FIXEDSYS_36_HEIGHT, -1, -1, 0xFF0000, "Press 2 to join %s", aos_world_get_team(ID_TEAM2)->name);
			break;
		case AOS_UI_SWITCH_WEAPON:
			aos_print12(0 + FIXEDSYS_36_HEIGHT, sysmain::yres - FIXEDSYS_36_HEIGHT, 1, -1, 0xFF0000, "Press 1 to select Rifle");
			aos_print12(sysmain::xres / 2, sysmain::yres - FIXEDSYS_36_HEIGHT, 0, -1, 0xFF0000, "Press 2 to select SMG");
			aos_print12(sysmain::xres - FIXEDSYS_36_HEIGHT, sysmain::yres - FIXEDSYS_36_HEIGHT, -1, -1, 0xFF0000, "Press 3 to select Shotgun");
			break;
		}
	}
}

extern int temp_destroyBlocksIndex;

static PlayerType *t1_l[32], *t2_l[32], *sp_l[32];

void sort_player_array(PlayerType **ar, int l)
{
	PlayerType *t;
	for (int i = 0; i < l; i++) {
		for (int j = i + 1; j < l; j++) {
			if (ar[i]->score < ar[j]->score) {
				t = ar[i];
				ar[i] = ar[j];
				ar[j] = t;
			}
		}
	}
}

void aos_ui_draw_scoreboard()
{
	bool lobby = aos_world_get_local_player()->team == PLAYER_UNCONNECTED;

	if (sysmain::keystatus[config::view_score] || lobby) {
		char cap_limit = 0;
		if (aos_gamemode == ID_GAMEMODE_CTF) cap_limit = aos_ctf_cap_limit;
		else cap_limit = aos_tc_territory_count;

		if (!lobby) {
			aos_print12(sysmain::xres / 2, sysmain::yres / 8, 0, -1, 0xFF0000, "PING: %dms", aos_network_get_ping());
		} else {
			voxlap::drawtile(splash_symbol.f, splash_symbol.p, splash_symbol.x, splash_symbol.y, (splash_symbol.x / 2) << 16, (splash_symbol.y / 2) << 16,
				(sysmain::xres / 2) << 16, (sysmain::yres / 6) << 16, 3 << 13, 3 << 13, 0, 0xFFFFFFFF);
		}

		// Team 1
		aos_print36(sysmain::xres / 4, sysmain::yres / 5, 0, -1, aos_team1_team.color, "%d-%d", aos_team1_team.score, cap_limit);

		// Team 2
		aos_print36(sysmain::xres - (sysmain::xres / 4), sysmain::yres / 5, 0, -1, aos_team2_team.color, "%d-%d", aos_team2_team.score, cap_limit);

		int t1_i = 0, t2_i = 0, sp_i = 0;
		for (int i = 0; i < 32; i++) {
			t1_l[i] = t2_l[i] = sp_l[i] = NULL;
			PlayerType *loop_pl = &aos_players[i];
			if (loop_pl->team != PLAYER_UNCONNECTED) {
				switch (loop_pl->team) {
				case ID_TEAM1:
					t1_l[t1_i] = loop_pl;
					t1_i++;
					break;
				case ID_TEAM2:
					t2_l[t2_i] = loop_pl;
					t2_i++;
					break;
				case ID_TEAM_SPEC:
					sp_l[sp_i] = loop_pl;
					sp_i++;
					break;
				}
			}
		}

		sort_player_array(t1_l, t1_i);
		sort_player_array(t2_l, t2_i);

		for (int i = 0; i < t1_i; i++) {
			PlayerType *loop_pl = t1_l[i];
			long color = 0xFFFFFF;
			if (loop_pl->local) color = 0xFFFF00;
			else if (!loop_pl->alive) color = 0xAFAFAF;
			aos_print12(sysmain::xres / 4, sysmain::yres / 4 + (i * FIXEDSYS_12_HEIGHT), -1, 0, color, "%s", loop_pl->name);
			aos_print12(sysmain::xres / 4 + FIXEDSYS_12_WIDTH, sysmain::yres / 4 + (i * FIXEDSYS_12_HEIGHT), 1, 0, color, "#%d", loop_pl->id);
			aos_print12(sysmain::xres / 4 + FIXEDSYS_12_WIDTH * 5, sysmain::yres / 4 + (i * FIXEDSYS_12_HEIGHT), 1, 0, color, "%d", loop_pl->score);
		}

		for (int i = 0; i < t2_i; i++) {
			PlayerType *loop_pl = t2_l[i];
			long color = 0xFFFFFF;
			if (loop_pl->local) color = 0xFFFF00;
			else if (!loop_pl->alive) color = 0xAFAFAF;
			aos_print12(sysmain::xres - (sysmain::xres / 4), sysmain::yres / 4 + (i * FIXEDSYS_12_HEIGHT), -1, 0, color, "%s", loop_pl->name);
			aos_print12(sysmain::xres - (sysmain::xres / 4) + FIXEDSYS_12_WIDTH, sysmain::yres / 4 + (i * FIXEDSYS_12_HEIGHT), 1, 0, color, "#%d", loop_pl->id);
			aos_print12(sysmain::xres - (sysmain::xres / 4) + FIXEDSYS_12_WIDTH * 5, sysmain::yres / 4 + (i * FIXEDSYS_12_HEIGHT), 1, 0, color, "%d", loop_pl->score);
		}

		if (!lobby) {
			for (int i = 0; i < sp_i; i++) {
				PlayerType *loop_pl = sp_l[i];
				aos_print12(sysmain::xres / 2, sysmain::yres / 4 + (i * FIXEDSYS_12_HEIGHT), 0, 0, 0xFFFFFF, "%s #%d", loop_pl->name, loop_pl->id);
			}
		} else {
			long text_y_pos = (sysmain::yres / 6) + (splash_symbol.y / 5);
			aos_print18(sysmain::xres / 2, text_y_pos, 0, 1, 0xFFFF00, "CONTROLS");
			text_y_pos += FIXEDSYS_18_HEIGHT;
			
			aos_print12(sysmain::xres / 2, text_y_pos, -1, 1, 0xFFFF00, "Movement");
			aos_print12(sysmain::xres / 2 + FIXEDSYS_12_WIDTH, text_y_pos, 1, 1, 0xFFFF00, "%s %s %s %s",
				aos_ui_get_key_name(config::move_forward),
				aos_ui_get_key_name(config::move_backward),
				aos_ui_get_key_name(config::move_left),
				aos_ui_get_key_name(config::move_right));
			text_y_pos += FIXEDSYS_12_HEIGHT;

			aos_print12(sysmain::xres / 2, text_y_pos, -1, 1, 0xFFFF00, "Fire");
			aos_print12(sysmain::xres / 2 + FIXEDSYS_12_WIDTH, text_y_pos, 1, 1, 0xFFFF00, "L. Mouse");
			text_y_pos += FIXEDSYS_12_HEIGHT;

			aos_print12(sysmain::xres / 2, text_y_pos, -1, 1, 0xFFFF00, "Gunsight");
			aos_print12(sysmain::xres / 2 + FIXEDSYS_12_WIDTH, text_y_pos, 1, 1, 0xFFFF00, "R. Mouse");
			text_y_pos += FIXEDSYS_12_HEIGHT;

			aos_print12(sysmain::xres / 2, text_y_pos, -1, 1, 0xFFFF00, "Weapons");
			aos_print12(sysmain::xres / 2 + FIXEDSYS_12_WIDTH, text_y_pos, 1, 1, 0xFFFF00, "1-4");
			text_y_pos += FIXEDSYS_12_HEIGHT;

			aos_print12(sysmain::xres / 2, text_y_pos, -1, 1, 0xFFFF00, "Reload");
			aos_print12(sysmain::xres / 2 + FIXEDSYS_12_WIDTH, text_y_pos, 1, 1, 0xFFFF00, "%s",
				aos_ui_get_key_name(config::reload));
			text_y_pos += FIXEDSYS_12_HEIGHT;

			aos_print12(sysmain::xres / 2, text_y_pos, -1, 1, 0xFFFF00, "Jump");
			aos_print12(sysmain::xres / 2 + FIXEDSYS_12_WIDTH, text_y_pos, 1, 1, 0xFFFF00, "%s",
				aos_ui_get_key_name(config::jump));
			text_y_pos += FIXEDSYS_12_HEIGHT;

			aos_print12(sysmain::xres / 2, text_y_pos, -1, 1, 0xFFFF00, "Crouch");
			aos_print12(sysmain::xres / 2 + FIXEDSYS_12_WIDTH, text_y_pos, 1, 1, 0xFFFF00, "%s",
				aos_ui_get_key_name(config::crouch));
			text_y_pos += FIXEDSYS_12_HEIGHT;

			aos_print12(sysmain::xres / 2, text_y_pos, -1, 1, 0xFFFF00, "Sneak");
			aos_print12(sysmain::xres / 2 + FIXEDSYS_12_WIDTH, text_y_pos, 1, 1, 0xFFFF00, "%s",
				aos_ui_get_key_name(config::sneak));
			text_y_pos += FIXEDSYS_12_HEIGHT;

			aos_print12(sysmain::xres / 2, text_y_pos, -1, 1, 0xFFFF00, "Sprint");
			aos_print12(sysmain::xres / 2 + FIXEDSYS_12_WIDTH, text_y_pos, 1, 1, 0xFFFF00, "%s",
				aos_ui_get_key_name(config::sprint));
			text_y_pos += FIXEDSYS_12_HEIGHT;

			aos_print12(sysmain::xres / 2, text_y_pos, -1, 1, 0xFFFF00, "View Score");
			aos_print12(sysmain::xres / 2 + FIXEDSYS_12_WIDTH, text_y_pos, 1, 1, 0xFFFF00, "%s",
				aos_ui_get_key_name(config::view_score));
			text_y_pos += FIXEDSYS_12_HEIGHT;

			aos_print12(sysmain::xres / 2, text_y_pos, -1, 1, 0xFFFF00, "Map");
			aos_print12(sysmain::xres / 2 + FIXEDSYS_12_WIDTH, text_y_pos, 1, 1, 0xFFFF00, "%s",
				aos_ui_get_key_name(config::view_map));
			text_y_pos += FIXEDSYS_12_HEIGHT;

			aos_print12(sysmain::xres / 2, text_y_pos, -1, 1, 0xFFFF00, "Change Team");
			aos_print12(sysmain::xres / 2 + FIXEDSYS_12_WIDTH, text_y_pos, 1, 1, 0xFFFF00, "%s",
				aos_ui_get_key_name(config::change_team));
			text_y_pos += FIXEDSYS_12_HEIGHT;

			aos_print12(sysmain::xres / 2, text_y_pos, -1, 1, 0xFFFF00, "Change Weapon");
			aos_print12(sysmain::xres / 2 + FIXEDSYS_12_WIDTH, text_y_pos, 1, 1, 0xFFFF00, "%s",
				aos_ui_get_key_name(config::change_weapon));
			text_y_pos += FIXEDSYS_12_HEIGHT;

			aos_print12(sysmain::xres / 2, text_y_pos, -1, 1, 0xFFFF00, "Global Chat");
			aos_print12(sysmain::xres / 2 + FIXEDSYS_12_WIDTH, text_y_pos, 1, 1, 0xFFFF00, "T");
			text_y_pos += FIXEDSYS_12_HEIGHT;

			aos_print12(sysmain::xres / 2, text_y_pos, -1, 1, 0xFFFF00, "Team Chat");
			aos_print12(sysmain::xres / 2 + FIXEDSYS_12_WIDTH, text_y_pos, 1, 1, 0xFFFF00, "Y");
			text_y_pos += FIXEDSYS_12_HEIGHT;

			aos_print12(sysmain::xres / 2, text_y_pos, -1, 1, 0xFFFF00, "Color Select");
			aos_print12(sysmain::xres / 2 + FIXEDSYS_12_WIDTH, text_y_pos, 1, 1, 0xFFFF00, "Unimplemented");
			text_y_pos += FIXEDSYS_12_HEIGHT;

			aos_print12(sysmain::xres / 2, text_y_pos, -1, 1, 0xFFFF00, "Grab Color");
			aos_print12(sysmain::xres / 2 + FIXEDSYS_12_WIDTH, text_y_pos, 1, 1, 0xFFFF00, "%s",
				aos_ui_get_key_name(config::cube_color_sample));
			text_y_pos += FIXEDSYS_12_HEIGHT;

			aos_print12(sysmain::xres / 2, text_y_pos, -1, 1, 0xFFFF00, "Quit");
			aos_print12(sysmain::xres / 2 + FIXEDSYS_12_WIDTH, text_y_pos, 1, 1, 0xFFFF00, "%s",
				aos_ui_get_key_name(config::quit_game));
			text_y_pos += FIXEDSYS_12_HEIGHT;
		}
	}
}

void aos_ui_draw_mapload()
{
	//long imgscale = (long)(splash_symbol.x / (sysmain::xres * 0.66)) << 16;
	//imgscale = (imgscale) ? imgscale : 1 << 16;
	voxlap::drawtile(splash_symbol.f, splash_symbol.p, splash_symbol.x, splash_symbol.y, (splash_symbol.x / 2) << 16, (splash_symbol.y / 2) << 16,
		(sysmain::xres / 2) << 16, ((long)(sysmain::yres / 2.5)) << 16, (1 << 16) / 1.5, (1 << 16) / 1.5, 0, 0xFFFFFFFF);
	aos_ui_drawbar(sysmain::xres / 2, sysmain::yres - (sysmain::yres / 4), splash_symbol.y, 0xFFFF00, 0x404040, aos_map_progress(), 1.0);
	aos_print18(sysmain::xres / 2, sysmain::yres - ((sysmain::yres / 4)) + FIXEDSYS_18_HEIGHT + BAR_SCREEN_SIZE, 0, -1, 0x404040, "Loading Map");
	aos_print18(sysmain::xres / 2, sysmain::yres - ((sysmain::yres / 4)) + FIXEDSYS_18_HEIGHT*2 + BAR_SCREEN_SIZE, 0, -1, 0x404040, "%d%%", (long)(aos_map_progress()*100.0f));
}

void aos_ui_draw_tc_bar(aos_byte team, TC_Territory *territory)
{
	aos_ui_drawbar(sysmain::xres / 2, sysmain::yres - (sysmain::yres / 4), sysmain::xres / 1.5f, aos_world_get_team(!team)->color, aos_world_get_team(team)->color, territory->progress, 1.0);
}

void aos_ui_draw_hovernames()
{
	PlayerType *cur_pl = aos_world_get_local_player();

	long hitid = cur_pl->hitscan_players(NULL, &cur_pl->cam_position, &cur_pl->orientation_for, false);
	if (hitid != -1) {
		PlayerType *hit_pl = &aos_players[hitid];
		if (hit_pl->team != ID_TEAM_SPEC && hit_pl->team != PLAYER_UNCONNECTED) {
			if (cur_pl->team == ID_TEAM_SPEC || cur_pl->team == hit_pl->team) {
				aos_print12(sysmain::xres / 2, sysmain::yres - (sysmain::yres / 4), 0, 0, aos_world_get_team(hit_pl->team)->color, "%s", hit_pl->name);
			}
		}
	}
}

void aos_ui_draw_target()
{
	voxlap::drawtile(target_symbol.f, target_symbol.p, target_symbol.x, target_symbol.y, (target_symbol.x) / 2 << 16, (target_symbol.y / 2) << 16,
		(sysmain::xres / 2) << 16, (sysmain::yres) / 2 << 16, 1 << 16, 1 << 16, 0, -1);
}

#define PLAYER_RELOAD_TEXT_TIME 0.5

void aos_ui_draw_ammo()
{
	/*PlayerType *player;
	PlayerType *cur_pl = aos_world_get_local_player();
	if ((!cur_pl->alive || cur_pl->team == ID_TEAM_SPEC) && cur_pl->specmode > 1 && cur_pl->specid != cur_pl->id) {
		player = &aos_players[cur_pl->specid];
	} else {
		player = cur_pl;
	}*/
	PlayerType *player = aos_world_get_local_player();
	TileType *symbol = &health_symbol;
	long color = 0xFFFF00;
	long offset = 0;

	if (player->alive && player->team > ID_TEAM_SPEC) {
		switch (player->tool) {
		case TOOL_BLOCK:
			offset = 100;
		case TOOL_SPADE:
			symbol = &block_symbol;
			color = (player->block_stock > 0) ? 0xFFFF00 : 0xFF0000;
			aos_print36(sysmain::xres - FIXEDSYS_36_WIDTH - symbol->x - offset, sysmain::yres - FIXEDSYS_36_HEIGHT / 2, -1, 0, color, "%d", player->block_stock);
			break;
		case TOOL_GUN:
			switch (player->weapon) {
			case WEAPON_SEMI:
				symbol = &semi_symbol;
				break;
			case WEAPON_SMG:
				symbol = &smg_symbol;
				break;
			case WEAPON_SHOTGUN:
				symbol = &shotgun_symbol;
				break;
			}
			color = (player->weapon_ammo > 0) ? 0xFFFF00 : 0xFF0000;
			aos_print36(sysmain::xres - FIXEDSYS_36_WIDTH - symbol->x, sysmain::yres - FIXEDSYS_36_HEIGHT / 2, -1, 0, color, "%d-%d", player->weapon_ammo, player->weapon_stock);
			if (player->weapon_ammo == 0)
				if (player->lastaction + PLAYER_RELOAD_TEXT_TIME >= dtotclk)
					aos_print36(sysmain::xres / 2, sysmain::yres / 2 + FIXEDSYS_18_HEIGHT, 0, 0, 0xFF0000, "RELOAD");
			break;
		case TOOL_GRENADE:
			symbol = &grenade_symbol;
			color = (player->grenade_stock > 0) ? 0xFFFF00 : 0xFF0000;
			aos_print36(sysmain::xres - FIXEDSYS_36_WIDTH - symbol->x, sysmain::yres - FIXEDSYS_36_HEIGHT / 2, -1, 0, color, "%d", player->grenade_stock);
		}

		voxlap::drawtile(symbol->f, symbol->p, symbol->x, symbol->y, 0, (-symbol->y / 2) << 16,
			(sysmain::xres - symbol->x - FIXEDSYS_36_HEIGHT / 2 - offset) << 16, (sysmain::yres - symbol->y - FIXEDSYS_36_HEIGHT / 2) << 16, 1 << 16, 1 << 16, 0, 0xFF000000 | color);
	}
}

void aos_ui_keypress(long key, aos_ubyte scancode)
{
	if (scancode == config::show_mouse) {
		sysmain::muffin_setactive(!sysmain::progactive);
	} else if (sysmain::progactive) {
		if (aos_ui_quitmode) {
			if (scancode == 0x15) { // Y
				sysmain::quitloop();
			} else if (scancode == 0x31) { // N
				aos_ui_quitmode = false;
			}
		} else if (aos_ui_chatmode) {
			if (scancode == config::quit_game) { // ESC
				chat_disable();
			} else if (scancode == 0x0E) { // backspace
				chat_delete();
			} else if (scancode == 0x1C) { // enter
				chat_disable();
				chat_send();
			} else {
				chat_append(key & 0xFF); // utf8 code
			}
		} else {
			if (scancode == config::quit_game) { // ESC
				if (aos_ui_switchmode != AOS_UI_SWITCH_NONE) {
					aos_ui_switchmode = AOS_UI_SWITCH_NONE;
				} else aos_ui_quitmode = true;
			} else if (scancode == 0x14) { // global; T
				chat_enable(false);
			} else if (scancode == 0x15) { // team; Y
				chat_enable(true);
			} else if (scancode == 0x35) { // global; command; /
				chat_enable(false);
				chat_append('/');
			} else if (scancode == config::view_map) {
				aos_ui_updateminimap();
				aos_ui_mapmode = !aos_ui_mapmode;
			} else if (scancode == config::toggle_graph) {
				aos_ui_statmode = !aos_ui_statmode;
			}
		}
	}
}

aos_ubyte aos_ui_get_slot(aos_ubyte scancode)
{
	aos_byte slot = 0;
	if (scancode == config::slot1)
		slot = 1;
	else if (scancode == config::slot2)
		slot = 2;
	else if (scancode == config::slot3)
		slot = 3;
	else if (scancode == config::slot4)
		slot = 4;
	else if (scancode == config::slot5)
		slot = 5;
	else if (scancode == config::slot6)
		slot = 6;
	else if (scancode == config::slot7)
		slot = 7;
	else if (scancode == config::slot8)
		slot = 8;
	else if (scancode == config::slot9)
		slot = 9;
	else if (scancode == config::slot10)
		slot = 10;
	return slot;
}

/*PlayerType *cur_pl = aos_world_get_local_player();
voxlap::extension::FloatVector3D beam = (cur_pl->orientation_for * voxlap::extension::FloatVector3D(1.0f, 1.0f, 0.0f)).normalized();
voxlap::extension::FloatVector3D rot(0, 0, 1);
voxlap::extension::FloatVector3D topl(0, 0, 0);
voxlap::extension::FloatVector3D topr(0, 0, 0);
voxlap::extension::FloatVector3D botl(0, 0, 0);
voxlap::extension::FloatVector3D botr(0, 0, 0);
voxlap::axisrotate(&beam, &rot, -45 * (PI / 180.f));
topl = beam * player.x;
voxlap::axisrotate(&beam, &rot, 90 * (PI / 180.f));
botl = beam * player.x;
voxlap::axisrotate(&beam, &rot, 90 * (PI / 180.f));
botr = beam * player.x;
voxlap::axisrotate(&beam, &rot, 90 * (PI / 180.f));
topr = beam * player.x;
voxlap::drawpicinquad(player.f, player.p, player.x, player.y, framebuf, bytesperline, xdim, ydim,
sysmain::xres / 2 + topl.x, sysmain::yres / 2 + topl.y,
sysmain::xres / 2 + botl.x, sysmain::yres / 2 + botl.y,
sysmain::xres / 2 + botr.x, sysmain::yres / 2 + botr.y,
sysmain::xres / 2 + topr.x, sysmain::yres / 2 + topr.y);*/