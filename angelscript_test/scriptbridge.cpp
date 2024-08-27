#include "scriptbridge.h"

#include <string>
#include <cstdint>
#include <random>

#include "AngelScript\include\angelscript.h"

#include <voxlap\voxlap5.h>
#include <voxlap\sysmain.h>

extern double dtotclk;

void script_point3d_set(float x, float y, float z, point3d *obj)
{
	obj->x = x;
	obj->y = y;
	obj->z = z;
}

void script_dpoint3d_set(double x, double y, double z, dpoint3d *obj)
{
	obj->x = x;
	obj->y = y;
	obj->z = z;
}

void script_lpoint3d_set(int x, int y, int z, lpoint3d *obj)
{
	obj->x = x;
	obj->y = y;
	obj->z = z;
}

void script_setears3d(const point3d &ipos, const point3d &ifor, const point3d &ihei)
{
	sysmain::setears3d(ipos.x, ipos.y, ipos.z, ifor.x, ifor.y, ifor.z, ihei.x, ihei.y, ihei.z);
}

void script_playsound(const std::string &file, int volume, float frequency, point3d &position, int flags)
{
	sysmain::playsound(file.c_str(), volume, frequency, &position, flags);
}

int script_hitscan_color(int *color)
{
	if (color != nullptr)
		return *color;
	else
		return -1;
}

uint8_t script_keystatus_get(int idx)
{
	if ((uint32_t)idx >= 256) return 0;
	return sysmain::keystatus[idx];
}

void script_keystatus_set(int idx, uint8_t val)
{
	if ((uint32_t)idx >= 256) return;
	sysmain::keystatus[idx] = val;
}

uint8_t script_ext_keystatus_get(int idx)
{
	if ((uint32_t)idx >= 256) return 0;
	return sysmain::ext_keystatus[idx];
}

void script_ext_keystatus_set(int idx, uint8_t val)
{
	if ((uint32_t)idx >= 256) return;
	sysmain::ext_keystatus[idx] = val;
}

int script_loadvxl(const std::string &file)
{
	return loadvxl(file.c_str());
}

int script_savevxl(const std::string &file)
{
	return savevxl(file.c_str());
}

int script_loadsky(const std::string &image)
{
	return loadsky(image.c_str());
}

kv6data *script_loadkv6(const std::string &file)
{
	kv6data *kv6 = loadkv6(file.c_str());
	kv6->lowermip = 0; // ugh
	return kv6;
}

// voxsetframebuffer
// setsideshades
// TODO: setcamera_float
// opticast
// drawpoint2d
// drawpoint3d
// drawline2d
// drawline3d

// drawspherefill
// drawpicinquad
// TODO: drawpicinquad
// TODO: drawpolyquad

void script_print4x6(int x, int y, int foreground, int background, const std::string &message)
{
	print4x6(x, y, foreground, background, message.c_str());
}

void script_print6x8(int x, int y, int foreground, int background, const std::string &message)
{
	print6x8(x, y, foreground, background, message.c_str());
}

// TODO: drawtile

int script_screencapture32bit(const std::string &file)
{
	return screencapture32bit(file.c_str());
}

// TODO: surroundcapture32bit

void script_orthonormalize(point3d *ifor_i, point3d *istr_i, point3d *ihei_i, point3d &istr_o, point3d &ihei_o, point3d &ifor_o)
{
	orthonormalize(istr_i, ihei_i, ifor_i);
	ifor_o = *ifor_i; istr_o = *istr_i; ihei_o = *ihei_i;
}

void script_dorthonormalize(dpoint3d *ifor_i, dpoint3d *istr_i, dpoint3d *ihei_i, dpoint3d &istr_o, dpoint3d &ihei_o, dpoint3d &ifor_o)
{
	dorthonormalize(istr_i, ihei_i, ifor_i);
	ifor_o = *ifor_i; istr_o = *istr_i; ihei_o = *ihei_i;
}

// void orthorotate(float, float, float, point3d &in, point3d &in, point3d &in, point3d &out, point3d &out, point3d &out)
void script_orthorotate(float x, float y, float z, point3d *istr_i, point3d *ihei_i, point3d *ifor_i, point3d &istr_o, point3d &ihei_o, point3d &ifor_o)
{
	orthorotate(x, y, z, istr_i, ihei_i, ifor_i);
	ifor_o = *ifor_i; istr_o = *istr_i; ihei_o = *ihei_i;
}

void script_dorthorotate(float x, float y, float z, dpoint3d *istr_i, dpoint3d *ihei_i, dpoint3d *ifor_i, dpoint3d &istr_o, dpoint3d &ihei_o, dpoint3d &ifor_o)
{
	dorthorotate(x, y, z, istr_i, ihei_i, ifor_i);
	ifor_o = *ifor_i; istr_o = *istr_i; ihei_o = *ihei_i;
}

void script_axisrotate(point3d *in, point3d &out, point3d *ax, float rot)
{
	axisrotate(in, ax, rot);
	out = *in;
}

// void slerp(point3d &in, point3d &in, point3d &in, point3d &in, point3d &in, point3d &in, point3d &in, point3d &in, point3d &in, point3d &out, point3d &out, point3d &out, float)
void script_slerp(point3d *str1, point3d *hei1, point3d *for1, point3d *str2, point3d *hei2, point3d *for2, point3d *istr_i, point3d *ihei_i, point3d *ifor_i, point3d &istr_o, point3d &ihei_o, point3d &ifor_o, float rot)
{
	slerp(str1, hei1, for1, str2, hei2, for2, istr_i, ihei_i, ifor_i, rot);
	ifor_o = *ifor_i; istr_o = *istr_i; ihei_o = *ihei_i;
}

void script_clipmove(point3d *pos_i, point3d &pos_o, point3d *vel, float rad)
{
	clipmove_float(pos_i, vel, rad);
	pos_o = *pos_i;
}

void script_dclipmove(dpoint3d *pos_i, dpoint3d &pos_o, dpoint3d *vel, double rad)
{
	clipmove(pos_i, vel, rad);
	pos_o = *pos_i;
}

#define BRIDGE_REGISTER_FUNC(decl, func) \
	ret = engine->RegisterGlobalFunction(decl, asFUNCTION(func), asCALL_CDECL); \
	if (ret < 0) { \
		err_funcname = decl; \
		goto wtf; \
	}

#define BRIDGE_REGISTER_VAR(decl, addr) \
	ret = engine->RegisterGlobalProperty(decl, addr); \
	if (ret < 0) { \
		err_funcname = decl; \
		return 0; \
	}

int script_setup_bridge(asIScriptEngine *engine, char *err_funcname)
{
	int ret;

	ret = engine->RegisterObjectType("point3d", sizeof(point3d), asOBJ_VALUE | asOBJ_POD);
	if (ret < 0) {
		err_funcname = "type point3d";
		return 0;
	}

	ret = engine->RegisterObjectProperty("point3d", "float x", asOFFSET(point3d, x)) |
		engine->RegisterObjectProperty("point3d", "float y", asOFFSET(point3d, y)) |
		engine->RegisterObjectProperty("point3d", "float z", asOFFSET(point3d, z)) |
		engine->RegisterObjectMethod("point3d", "void set(float, float, float)", asFUNCTION(script_point3d_set), asCALL_CDECL_OBJLAST);
	if (ret < 0) {
		err_funcname = "type point3d";
		return 0;
	}

	ret = engine->RegisterObjectType("dpoint3d", sizeof(point3d), asOBJ_VALUE | asOBJ_POD);
	if (ret < 0) {
		err_funcname = "type dpoint3d";
		return 0;
	}

	ret = engine->RegisterObjectProperty("dpoint3d", "double x", asOFFSET(point3d, x)) |
		engine->RegisterObjectProperty("dpoint3d", "double y", asOFFSET(point3d, y)) |
		engine->RegisterObjectProperty("dpoint3d", "double z", asOFFSET(point3d, z)) |
		engine->RegisterObjectMethod("dpoint3d", "void set(double, double, double)", asFUNCTION(script_dpoint3d_set), asCALL_CDECL_OBJLAST);
	if (ret < 0) {
		err_funcname = "type dpoint3d";
		return 0;
	}

	ret = engine->RegisterObjectType("lpoint3d", sizeof(point3d), asOBJ_VALUE | asOBJ_POD);
	if (ret < 0) {
		err_funcname = "type lpoint3d";
		return 0;
	}

	ret = engine->RegisterObjectProperty("lpoint3d", "int x", asOFFSET(point3d, x)) |
		engine->RegisterObjectProperty("lpoint3d", "int y", asOFFSET(point3d, y)) |
		engine->RegisterObjectProperty("lpoint3d", "int z", asOFFSET(point3d, z)) |
		engine->RegisterObjectMethod("lpoint3d", "void set(int, int, int)", asFUNCTION(script_lpoint3d_set), asCALL_CDECL_OBJLAST);
	if (ret < 0) {
		err_funcname = "type lpoint3d";
		return 0;
	}

	ret = engine->RegisterObjectType("kv6data", sizeof(kv6data), asOBJ_REF | asOBJ_NOCOUNT);
	if (ret < 0) {
		err_funcname = "type kv6data";
		return 0;
	}

	ret = engine->RegisterObjectProperty("kv6data", "int leng", asOFFSET(kv6data, leng)) |
		engine->RegisterObjectProperty("kv6data", "int xsiz", asOFFSET(kv6data, xsiz)) |
		engine->RegisterObjectProperty("kv6data", "int ysiz", asOFFSET(kv6data, ysiz)) |
		engine->RegisterObjectProperty("kv6data", "int zsiz", asOFFSET(kv6data, zsiz)) |
		engine->RegisterObjectProperty("kv6data", "float xpiv", asOFFSET(kv6data, xpiv)) |
		engine->RegisterObjectProperty("kv6data", "float ypiv", asOFFSET(kv6data, ypiv)) |
		engine->RegisterObjectProperty("kv6data", "float zpiv", asOFFSET(kv6data, zpiv)) |
		engine->RegisterObjectProperty("kv6data", "uint numvoxs", asOFFSET(kv6data, numvoxs));
	if (ret < 0) {
		err_funcname = "type kv6data";
		return 0;
	}

	ret = engine->RegisterObjectType("vx5sprite", sizeof(vx5sprite), asOBJ_VALUE | asOBJ_POD);
	if (ret < 0) {
		err_funcname = "type vx5sprite";
		return 0;
	}

	ret = engine->RegisterObjectProperty("vx5sprite", "point3d p", asOFFSET(vx5sprite, p)) |
		engine->RegisterObjectProperty("vx5sprite", "int flags", asOFFSET(vx5sprite, flags)) |
		engine->RegisterObjectProperty("vx5sprite", "point3d s", asOFFSET(vx5sprite, s)) |
		engine->RegisterObjectProperty("vx5sprite", "point3d x", asOFFSET(vx5sprite, x)) |
		engine->RegisterObjectProperty("vx5sprite", "kv6data@ voxnum", asOFFSET(vx5sprite, voxnum)) |
		engine->RegisterObjectProperty("vx5sprite", "point3d h", asOFFSET(vx5sprite, h)) |
		engine->RegisterObjectProperty("vx5sprite", "point3d y", asOFFSET(vx5sprite, y)) |
		engine->RegisterObjectProperty("vx5sprite", "int kfatim", asOFFSET(vx5sprite, kfatim)) |
		engine->RegisterObjectProperty("vx5sprite", "point3d f", asOFFSET(vx5sprite, f)) |
		engine->RegisterObjectProperty("vx5sprite", "point3d z", asOFFSET(vx5sprite, z)) |
		engine->RegisterObjectProperty("vx5sprite", "int okfatim", asOFFSET(vx5sprite, okfatim));
	if (ret < 0) {
		err_funcname = "type vx5sprite";
		return 0;
	}

	ret = engine->RegisterObjectType("vx5_struct", sizeof(vx5_struct), asOBJ_VALUE | asOBJ_POD);
	if (ret < 0) {
		err_funcname = "type vx5_struct";
		return 0;
	}

	ret = engine->RegisterObjectProperty("vx5_struct", "int globalmass", asOFFSET(vx5_struct, globalmass)) |
		engine->RegisterObjectProperty("vx5_struct", "int anginc", asOFFSET(vx5_struct, anginc)) | 
		engine->RegisterObjectProperty("vx5_struct", "int sideshademode", asOFFSET(vx5_struct, sideshademode)) |
		engine->RegisterObjectProperty("vx5_struct", "int mipscandist", asOFFSET(vx5_struct, mipscandist)) |
		engine->RegisterObjectProperty("vx5_struct", "int maxscandist", asOFFSET(vx5_struct, maxscandist)) |
		engine->RegisterObjectProperty("vx5_struct", "int vxlmipuse", asOFFSET(vx5_struct, vxlmipuse)) |
		engine->RegisterObjectProperty("vx5_struct", "int fogcol", asOFFSET(vx5_struct, fogcol)) |
		engine->RegisterObjectProperty("vx5_struct", "int kv6mipfactor", asOFFSET(vx5_struct, kv6mipfactor)) |
		engine->RegisterObjectProperty("vx5_struct", "int kv6col", asOFFSET(vx5_struct, kv6col)) |
		engine->RegisterObjectProperty("vx5_struct", "int xplanemin", asOFFSET(vx5_struct, xplanemin)) |
		engine->RegisterObjectProperty("vx5_struct", "int xplanemax", asOFFSET(vx5_struct, xplanemax)) |
		engine->RegisterObjectProperty("vx5_struct", "int curcol", asOFFSET(vx5_struct, curcol)) |
		engine->RegisterObjectProperty("vx5_struct", "int currad", asOFFSET(vx5_struct, currad)) |
		engine->RegisterObjectProperty("vx5_struct", "int curhei", asOFFSET(vx5_struct, curhei)) |
		engine->RegisterObjectProperty("vx5_struct", "float curpow", asOFFSET(vx5_struct, curpow)) |
		engine->RegisterObjectProperty("vx5_struct", "int lightmode", asOFFSET(vx5_struct, lightmode)) |
		engine->RegisterObjectProperty("vx5_struct", "int fallcheck", asOFFSET(vx5_struct, fallcheck));
	if (ret < 0) {
		err_funcname = "type vx5_struct";
		return 0;
	}

	BRIDGE_REGISTER_VAR("const double dtotclk", &dtotclk);
	BRIDGE_REGISTER_VAR("int xres", &sysmain::xres);
	BRIDGE_REGISTER_VAR("int yres", &sysmain::yres);
	BRIDGE_REGISTER_VAR("int colbits", &sysmain::colbits);
	BRIDGE_REGISTER_VAR("int fullscreen", &sysmain::fullscreen);
	BRIDGE_REGISTER_VAR("int mousmoth", &sysmain::mousmoth);
	BRIDGE_REGISTER_VAR("float mousper", &sysmain::mousper);

	BRIDGE_REGISTER_VAR("vx5_struct vx5", &vx5);

	BRIDGE_REGISTER_FUNC("void quitloop()", sysmain::quitloop);
	BRIDGE_REGISTER_FUNC("int startdirectdraw(int &out, int &out, int &out, int &out)", sysmain::startdirectdraw);
	BRIDGE_REGISTER_FUNC("void stopdirectdraw()", sysmain::stopdirectdraw);
	BRIDGE_REGISTER_FUNC("void nextpage()", sysmain::nextpage);
	BRIDGE_REGISTER_FUNC("void clearscreen(int)", sysmain::clearscreen);
	BRIDGE_REGISTER_FUNC("int changeres(int, int, int, int)", sysmain::changeres);
	BRIDGE_REGISTER_FUNC("int readkeyboard()", sysmain::readkeyboard);
	BRIDGE_REGISTER_FUNC("int keyread()", sysmain::keyread);
	BRIDGE_REGISTER_FUNC("void readmouse(float &out, float &out, int &out)", sysmain::readmouse);
	BRIDGE_REGISTER_FUNC("int ismouseout(int, int)", sysmain::ismouseout);
	BRIDGE_REGISTER_FUNC("void setalwaysactive(int)", sysmain::setalwaysactive);
	BRIDGE_REGISTER_FUNC("void smartsleep(int)", sysmain::smartsleep);
	BRIDGE_REGISTER_FUNC("int canrender()", sysmain::canrender);

	BRIDGE_REGISTER_FUNC("int randint()", rand());

	BRIDGE_REGISTER_FUNC("void setvolume(int)", sysmain::setvolume);
	BRIDGE_REGISTER_FUNC("void playsound(const string &in, int, float, point3d &in, int)", script_playsound);
	BRIDGE_REGISTER_FUNC("void setears3d(const point3d &in, const point3d &in, const point3d &in)", script_setears3d);

	BRIDGE_REGISTER_FUNC("uint8 get_keystatus(int)", script_keystatus_get);
	BRIDGE_REGISTER_FUNC("void set_keystatus(int, uint8)", script_keystatus_set);

	BRIDGE_REGISTER_FUNC("uint8 get_ext_keystatus(int)", script_ext_keystatus_get);
	BRIDGE_REGISTER_FUNC("void set_ext_keystatus(int, uint8)", script_ext_keystatus_set);

	BRIDGE_REGISTER_FUNC("int loadvxl(const string &in)", script_loadvxl);
	BRIDGE_REGISTER_FUNC("void loadnul(point3d &out, point3d &out, point3d &out, point3d &out)", script_loadvxl);
	BRIDGE_REGISTER_FUNC("int savevxl(const string &in)", script_loadvxl);
	BRIDGE_REGISTER_FUNC("int loadsky(const string &in)", script_loadsky);

	BRIDGE_REGISTER_FUNC("void voxsetframebuffer(int, int, int, int)", voxsetframebuffer);
	BRIDGE_REGISTER_FUNC("void setsideshades(uint8, uint8, uint8, uint8, uint8, uint8)", setsideshades);
	BRIDGE_REGISTER_FUNC("void setcamera(point3d &in, point3d &in, point3d &in, point3d &in, float, float, float)", setcamera_float);
	BRIDGE_REGISTER_FUNC("void opticast()", opticast);
	BRIDGE_REGISTER_FUNC("void drawpoint2d(int, int, int)", drawpoint2d);
	BRIDGE_REGISTER_FUNC("void drawpoint3d(float, float, float, int)", drawpoint3d);
	BRIDGE_REGISTER_FUNC("void drawline2d(float, float, float, float, int)", drawline2d);
	BRIDGE_REGISTER_FUNC("void drawline3d(float, float, float, float, float, float, int)", drawline3d);
	BRIDGE_REGISTER_FUNC("int project2d(float, float, float, float &out, float &out, float &out)", project2d);
	BRIDGE_REGISTER_FUNC("void drawspherefill(float, float, float, float, int)", drawspherefill);
	BRIDGE_REGISTER_FUNC("void print6x8(int, int, int, int, const string &in)", script_print6x8);
	BRIDGE_REGISTER_FUNC("void print4x6(int, int, int, int, const string &in)", script_print4x6);
	BRIDGE_REGISTER_FUNC("int screencapture32bit(const string &in)", script_screencapture32bit);

	BRIDGE_REGISTER_FUNC("void orthonormalize(point3d &in, point3d &in, point3d &in, point3d &out, point3d &out, point3d &out)", script_orthonormalize);
	BRIDGE_REGISTER_FUNC("void dorthonormalize(dpoint3d &in, dpoint3d &in, dpoint3d &in, dpoint3d &out, dpoint3d &out, dpoint3d &out)", script_dorthonormalize);
	BRIDGE_REGISTER_FUNC("void orthorotate(float, float, float, point3d &in, point3d &in, point3d &in, point3d &out, point3d &out, point3d &out)", script_orthorotate);
	BRIDGE_REGISTER_FUNC("void dorthorotate(double, double, double, dpoint3d &in, dpoint3d &in, dpoint3d &in, dpoint3d &out, dpoint3d &out, dpoint3d &out)", script_dorthorotate);
	BRIDGE_REGISTER_FUNC("void axisrotate(point3d &in, point3d &out, point3d &in, float)", script_axisrotate);
	BRIDGE_REGISTER_FUNC("void slerp(point3d &in, point3d &in, point3d &in, point3d &in, point3d &in, point3d &in, point3d &in, point3d &in, point3d &in, point3d &out, point3d &out, point3d &out, float)", slerp);
	BRIDGE_REGISTER_FUNC("int cansee(point3d &in, point3d &in, lpoint3d &out)", cansee);
	BRIDGE_REGISTER_FUNC("void hitscan(point3d &in, point3d &in, lpoint3d &out, int &out, int &out)", hitscan_f);
	BRIDGE_REGISTER_FUNC("void dhitscan(dpoint3d &in, dpoint3d &in, lpoint3d &out, int &out, int &out)", hitscan);
	BRIDGE_REGISTER_FUNC("int hitscan_get_color(int)", script_hitscan_color);
	BRIDGE_REGISTER_FUNC("void clipmove(point3d &in, point3d &out, point3d &in, float)", script_clipmove);
	BRIDGE_REGISTER_FUNC("void dclipmove(dpoint3d &in, dpoint3d &out, dpoint3d &in, double)", script_dclipmove);
	BRIDGE_REGISTER_FUNC("void estnorm(int, int, int, point3d &out)", estnorm);

	BRIDGE_REGISTER_FUNC("kv6data@ loadkv6(const string &in)", script_loadkv6);
	BRIDGE_REGISTER_FUNC("void freekv6(kv6data@)", freekv6);
	BRIDGE_REGISTER_FUNC("void drawsprite(vx5sprite &in)", drawsprite);
	BRIDGE_REGISTER_FUNC("void meltsphere(vx5sprite &in, lpoint3d &in, int)", meltsphere);

	BRIDGE_REGISTER_FUNC("int isvoxelsolid(int, int, int)", isvoxelsolid);
	BRIDGE_REGISTER_FUNC("int anyvoxelsolid(int, int, int, int)", anyvoxelsolid);
	BRIDGE_REGISTER_FUNC("int anyvoxelempty(int, int, int, int)", anyvoxelempty);
	BRIDGE_REGISTER_FUNC("int getfloorz(int, int, int)", getfloorz);
	BRIDGE_REGISTER_FUNC("int getcube(int, int, int)", getcube);

	BRIDGE_REGISTER_FUNC("void setcube(int, int, int, int)", setcube);
	BRIDGE_REGISTER_FUNC("void setsphere(lpoint3d &in, int, int)", setsphere);
	BRIDGE_REGISTER_FUNC("void setellipsoid(lpoint3d &in, lpoint3d &in, int, int, int)", setellipsoid);
	BRIDGE_REGISTER_FUNC("void setcylinder(lpoint3d &in, lpoint3d &in, int, int, int)", setcylinder);
	BRIDGE_REGISTER_FUNC("void setrect(lpoint3d &in, lpoint3d &in, int)", setrect);
	BRIDGE_REGISTER_FUNC("void settri(point3d &in, point3d &in, point3d &in, int)", settri);
	BRIDGE_REGISTER_FUNC("void setkv6(vx5sprite &in, int)", setkv6);

	BRIDGE_REGISTER_FUNC("void sethull3d(point3d &in, int, int, int)", sethull3d);
	BRIDGE_REGISTER_FUNC("void setlathe(point3d &in, int, int, int)", setlathe);
	BRIDGE_REGISTER_FUNC("void setblobs(point3d &in, int, int, int)", setblobs);
	BRIDGE_REGISTER_FUNC("void setfloodfill3d(int, int, int, int, int, int, int, int, int)", setfloodfill3d);
	BRIDGE_REGISTER_FUNC("void sethollowfill()", sethollowfill);
	BRIDGE_REGISTER_FUNC("void setflash(float, float, float, int, int, int)", setflash);
	BRIDGE_REGISTER_FUNC("void setnormflash(float, float, float, int, int)", setnormflash);

	BRIDGE_REGISTER_FUNC("void updatebbox(int, int, int, int, int, int, int)", updatebbox);
	BRIDGE_REGISTER_FUNC("void updatevxl()", updatevxl);
	BRIDGE_REGISTER_FUNC("void genmipvxl(int, int, int, int)", genmipvxl);
	BRIDGE_REGISTER_FUNC("void updatelighting(int, int, int, int, int, int)", updatelighting);

	BRIDGE_REGISTER_FUNC("void checkfloatinbox(int, int, int, int, int, int)", checkfloatinbox);
	BRIDGE_REGISTER_FUNC("void startfalls()", startfalls);
	BRIDGE_REGISTER_FUNC("void dofall(int)", dofall);
	BRIDGE_REGISTER_FUNC("int meltfall(vx5sprite &out, int, int)", meltfall);
	BRIDGE_REGISTER_FUNC("void finishfalls()", finishfalls);

	return 1;
wtf:;

	return 0;
}