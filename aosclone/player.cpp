#include "player.h"

#include <voxlap\sysmain.h>
#include <stdio.h>

#include "config.h"
#include "vxl.h"
#include "world.h"
#include "packet.h"
#include "networking.h"
#include "ui.h"

#include <string.h>
#include <math.h>

#define SQRT 0.70710678f

extern double dtotclk;
extern float fsynctics;

void PlayerType::init()
{
	this->alive = false;
	this->local = false;
	this->team = PLAYER_UNCONNECTED; // no team
	this->tool = TOOL_GUN;
	this->weapon = WEAPON_SEMI;
	this->health = 0;
	this->block_stock = TOOL_BLOCK_STOCK;
	this->weapon_ammo = WEAPON_SEMI_AMMO;
	this->weapon_stock = WEAPON_SEMI_STOCK;
	this->grenade_stock = TOOL_GRENADE_STOCK;
	this->block_color = 0;
	this->primary = false;
	this->secondary = false;
	this->lastinputstate = 0;
	this->move_for = false;
	this->move_back = false;
	this->move_left = false;
	this->move_right = false;
	this->crouch = false;
	this->sprint = false;
	this->sneak = false;
	this->jump = false;
	this->airborne = false;
	this->wade = false;
	this->lastclimb = 0.0f;
	this->tool_switched = false;
	this->last_tool = this->tool;
	this->lastswitch = 0.0f;
	this->lastdisable = dtotclk;
	this->respawntime = 0.0f;
	this->lastrespawntick = 0.0f;
	this->score = 0;
	this->grenade_active = false;

	float lastpossent = 0.0f;
	float lastorisent = 0.0f;
	float lastrestock = 0.0f;

	this->lclicked = false;
	this->rclicked = false;
	this->luclicked = false;
	this->ruclicked = false;

	this->cam_position = this->position
							= { 256.f,	301.f,	-13.f };
	this->velocity			= {  0.0f,	 0.0f,	 0.0f };
	this->orientation_for	= {  0.0f,  -SQRT,	 SQRT };
	this->orientation_str	= {  1.0f,	 0.0f,	 0.0f };
	this->orientation_hei	= {  0.0f,	 SQRT,	 SQRT };

	this->block_drag = false;

	this->steprate = 0.5f;
	this->laststep = dtotclk;
	this->curstep = 0.0f;
	this->stepstate = -1;

	this->dead_rise = false;

	this->lastaction = 0.0f;
	this->nextaction = 0.0f;
	this->curaction = 0.0f;
	this->nextaction_secondary = 0.0f;
	this->curaction_secondary = 0.0f;

	this->lastreload = 0.0f;
	this->reload_remaining = -1;

	this->render_scale = 1.0f;
	this->head.voxnum = aos_get_model(AOS_MODEL_TEAM1_PLAYER_HEAD, false, false);
	this->head.flags = 0;

	this->torso.voxnum = aos_get_model(AOS_MODEL_TEAM1_PLAYER_TORSO_UNCROUCHED, false, false);
	this->torso.flags = 0;

	this->arms.voxnum = aos_get_model(AOS_MODEL_TEAM1_PLAYER_ARMS, false, false);
	this->arms.flags = 0;

	this->left_leg.voxnum = aos_get_model(AOS_MODEL_TEAM1_PLAYER_LEG_UNCROUCHED, false, false);
	this->left_leg.flags = 0;

	this->right_leg.voxnum = aos_get_model(AOS_MODEL_TEAM1_PLAYER_LEG_UNCROUCHED, false, false);
	this->right_leg.flags = 0;

	this->dead.voxnum = aos_get_model(AOS_MODEL_TEAM1_PLAYER_DEAD, false, false);
	this->dead.flags = 0;

	this->item.voxnum = aos_get_model(AOS_MODEL_WEAPON_SEMI, false, false);
	this->item.flags = 0;

	this->flag.voxnum = aos_get_model(AOS_MODEL_TEAM1_INTEL, false, false);
	this->flag.flags = 0;
}

void PlayerType::existingplayer(aos_byte team, aos_ubyte weapon, aos_ubyte tool, unsigned long kills, long block_color, char *name)
{
	this->init();
	this->alive = true;
	this->health = 100;
	this->set_team(team);
	this->set_weapon(weapon);
	this->set_tool(tool);
	this->last_tool = this->tool;
	this->score = kills;

	this->set_color(block_color, false);
	this->block_stock = TOOL_BLOCK_STOCK;
	this->grenade_stock = TOOL_GRENADE_STOCK;
	
	aos_strcpycut(this->name, name, 16);

	this->lastdisable = dtotclk;
}

bool PlayerType::createplayer(aos_byte team, aos_ubyte weapon, float x, float y, float z, char *name)
{
	bool notSameTeam = team != this->team;
	this->alive = true;
	this->health = 100;
	this->set_team(team);
	this->set_weapon(weapon);
	this->set_tool(TOOL_GUN);
	this->last_tool = this->tool;
	aos_strcpycut(this->name, name, 16);

	this->set_color(0x707070, false);
	this->block_stock = TOOL_BLOCK_STOCK;
	this->grenade_stock = TOOL_GRENADE_STOCK;

	this->lastdisable = dtotclk;

	
	voxlap::extension::FloatVector3D pos(x, y, z);
	if (team == ID_TEAM_SPEC) {
		pos = { 256.f, 301.f, -13.f };
	}
	this->reposition(&pos);

	voxlap::extension::FloatVector3D orient(0);
	switch (team) {
	default:
	case ID_TEAM_SPEC:
		orient.y = -1.f;
		break;
	case ID_TEAM1:
		orient.x = 1.0f;
		break;
	case ID_TEAM2:
		orient.x = -1.0f;
		break;
	}

	this->orientation_hei = voxlap::extension::FloatVector3D(0);
	this->orientation_str = voxlap::extension::FloatVector3D(0);
	this->reorient(&orient);

	this->lastpossent = dtotclk;
	this->lastorisent = dtotclk;

	return notSameTeam;
}

void PlayerType::reposition(voxlap::point3d *position)
{
	float f; /* FIXME meaningful name */

	this->cam_position = this->position = *position;
	f = this->lastclimb - dtotclk; /* FIXME meaningful name */

	if (f>-0.25f)
		this->cam_position.z += (f + 0.25f) / 0.25f;
}

void PlayerType::reposition(float x, float y, float z)
{
	float f; /* FIXME meaningful name */

	this->cam_position = this->position = { x, y, z };
	f = this->lastclimb - dtotclk; /* FIXME meaningful name */

	if (f>-0.25f)
		this->cam_position.z += (f + 0.25f) / 0.25f;
}

void PlayerType::reorient(voxlap::point3d *orientation)
{
	this->orientation_for = *orientation;
	set_orientation_vectors(orientation, &this->orientation_str, &this->orientation_hei);
}

void PlayerType::reorient(float x, float y, float z)
{
	this->orientation_for = { x, y, z };
	set_orientation_vectors(&this->orientation_for, &this->orientation_str, &this->orientation_hei);
}

void PlayerType::reorient_lat(float dx, float dy, float multx, float multy)
{
	voxlap::extension::FloatVector3D up(0.0f, 0.0f, 1.0f);
	voxlap::axisrotate(&this->orientation_for, &up, (dx * PI / 180.0f) * multx);
	voxlap::axisrotate(&this->orientation_str, &up, (dx * PI / 180.0f) * multx);
	voxlap::axisrotate(&this->orientation_hei, &up, (dx * PI / 180.0f) * multx);

	voxlap::extension::FloatVector3D after_rot = this->orientation_hei;
	voxlap::axisrotate(&after_rot, &this->orientation_str, (dy * PI / 180.0f) * multy);
	if (after_rot.z >= 0.05f) {
		voxlap::axisrotate(&this->orientation_for, &this->orientation_str, (dy * PI / 180.0f) * multy);
		this->orientation_hei = after_rot;
	}
}

bool PlayerType::try_uncrouch()
{
	float x1 = this->position.x + 0.45f;
	float x2 = this->position.x - 0.45f;
	float y1 = this->position.y + 0.45f;
	float y2 = this->position.y - 0.45f;
	float z1 = this->position.z + 2.25f;
	float z2 = this->position.z - 1.35f;

	//first check if player can lower feet (in midair)
	if (this->airborne && !(
		aos_vxl_clipbox(x1, y1, z1) ||
		aos_vxl_clipbox(x1, y2, z1) ||
		aos_vxl_clipbox(x2, y1, z1) ||
		aos_vxl_clipbox(x2, y2, z1)))
		return(1);
	//then check if they can raise their head
	else if (!(aos_vxl_clipbox(x1, y1, z2) ||
		aos_vxl_clipbox(x1, y2, z2) ||
		aos_vxl_clipbox(x2, y1, z2) ||
		aos_vxl_clipbox(x2, y2, z2))) {
		this->position.z -= 0.9f;
		this->cam_position.z -= 0.9f;
		return(1);
	}
	return(0);
}

void PlayerType::boxclipmove_player()
{
	float offset, m, f, nx, ny, nz, z;
	long climb = 0;
	long runclimb = 0;

	f = fsynctics*32.f;
	nx = f*this->velocity.x + this->position.x;
	ny = f*this->velocity.y + this->position.y;

	if (this->crouch) {
		offset = 0.45f;
		m = 0.9f;
	} else {
		offset = 0.9f;
		m = 1.35f;
	}

	nz = this->position.z + offset;

	if (this->velocity.x < 0) f = -0.45f;
	else f = 0.45f;
	z = m;
	while (z >= -1.36f && !aos_vxl_clipbox(nx + f, this->position.y - 0.45f, nz + z) && !aos_vxl_clipbox(nx + f, this->position.y + 0.45f, nz + z))
		z -= 0.9f;
	if (z<-1.36f) this->position.x = nx;
	else if (!this->crouch && this->orientation_for.z<0.5f && !this->sprint) {
		z = 0.35f;
		while (z >= -2.36f && !aos_vxl_clipbox(nx + f, this->position.y - 0.45f, nz + z) && !aos_vxl_clipbox(nx + f, this->position.y + 0.45f, nz + z))
			z -= 0.9f;
		if (z<-2.36f) {
			this->position.x = nx;
			climb = 1;
		} else this->velocity.x = 0;
	} else this->velocity.x = 0;

	if (this->velocity.y < 0) f = -0.45f;
	else f = 0.45f;
	z = m;
	while (z >= -1.36f && !aos_vxl_clipbox(this->position.x - 0.45f, ny + f, nz + z) && !aos_vxl_clipbox(this->position.x + 0.45f, ny + f, nz + z))
		z -= 0.9f;
	if (z<-1.36f) this->position.y = ny;
	else if (!this->crouch && this->orientation_for.z<0.5f && !this->sprint && !climb) {
		z = 0.35f;
		while (z >= -2.36f && !aos_vxl_clipbox(this->position.x - 0.45f, ny + f, nz + z) && !aos_vxl_clipbox(this->position.x + 0.45f, ny + f, nz + z))
			z -= 0.9f;
		if (z<-2.36f) {
			this->position.y = ny;
			climb = 1;
		} else this->velocity.y = 0;
	} else if (!climb)
		this->velocity.y = 0;

	if (climb) {
		this->velocity.x *= 0.5f;
		this->velocity.y *= 0.5f;
		this->lastclimb = dtotclk;
		nz--;
		m = -1.35f;
	} else {
		if (this->velocity.z < 0)
			m = -m;
		nz += this->velocity.z*fsynctics*32.f;
	}

	this->airborne = 1;

	if (aos_vxl_clipbox(this->position.x - 0.45f, this->position.y - 0.45f, nz + m) ||
		aos_vxl_clipbox(this->position.x - 0.45f, this->position.y + 0.45f, nz + m) ||
		aos_vxl_clipbox(this->position.x + 0.45f, this->position.y - 0.45f, nz + m) ||
		aos_vxl_clipbox(this->position.x + 0.45f, this->position.y + 0.45f, nz + m)) {
		if (this->velocity.z >= 0) {
			//if (this->local)
			//	this->wade = (this->position.z > 60) && (voxlap::getfloorz(this->position.x, this->position.y, this->position.z) == 63);
			//else
				this->wade = this->position.z > 61;
			this->airborne = 0;
		}
		this->velocity.z = 0;
	} else
		this->position.z = nz - offset;

	this->reposition(&this->position);
}

void PlayerType::boxclipmove_spectator()
{
	float f, nx, ny, nz;
	voxlap::point3d op;

	f = fsynctics * 32.0f;

	nx = f*this->velocity.x + this->position.x;
	if (aos_vxl_clipbox_spectator(nx, this->position.y, this->position.z))
		this->velocity.x = 0.0f;

	ny = f*this->velocity.y + this->position.y;
	if (aos_vxl_clipbox_spectator(this->position.x, ny, this->position.z))
		this->velocity.y = 0.0f;

	nz = f*this->velocity.z + this->position.z;
	if (aos_vxl_clipbox_spectator(this->position.x, this->position.y, nz))
		this->velocity.z = 0.0f;

	op = this->position;
	this->position += this->velocity*f;

	if (aos_vxl_clipbox(this->position.x, this->position.y, this->position.z)) {
		this->position = op;
		this->velocity /= 2.0f;
	}

	this->reposition(&this->position);
}

#define PLAYER_DEAD_MIN_BOUNCE_SPEED   0.10f

void deadbounce_helper(voxlap::lpoint3d *p, voxlap::extension::FloatVector3D *v, voxlap::extension::FloatVector3D *n)
{
	float f, nx, ny, nz;
	voxlap::point3d op;

	f = fsynctics * 32.0f;

	nx = f*v->x + p->x;
	if (aos_vxl_clipbox_spectator(nx, p->y, p->z)) {
		if (v->x > 0.0f)
			n->x = -1.0f;
		else
			n->x = 1.0f;
	}

	ny = f*v->y + p->y;
	if (aos_vxl_clipbox_spectator(p->x, ny, p->z)) {
		if (v->y > 0.0f)
			n->y = -1.0f;
		else
			n->y = 1.0f;
	}

	nz = f*v->z + p->z;
	if (aos_vxl_clipbox_spectator(p->x, p->y, nz)) {
		if (v->z > 0.0f)
			n->z = -1.0f;
		else
			n->z = 1.0f;
	}

	*n = n->normalized();
}

void PlayerType::boxclipbounce_dead()
{
	float f;
	voxlap::extension::FloatVector3D op, fp;
	voxlap::lpoint3d lp;

	f = fsynctics * 32.0f;

	op = this->position;
	this->position += this->velocity*f;

	if (!this->dead_rise) {
		if (!voxlap::cansee(&op, &this->position, &lp)) {
			this->position = op;
			//voxlap::estnorm(lp.x, lp.y, lp.z, &fp);
			deadbounce_helper(&lp, &this->velocity, &fp);
			if (this->velocity.length() > PLAYER_DEAD_MIN_BOUNCE_SPEED) {
				f = (this->velocity.x*fp.x + this->velocity.y*fp.y + this->velocity.z*fp.z)*2.0f;
				this->velocity = (this->velocity - fp * f) * 0.66;

				aos_playsound(AOS_SOUND_DEBRIS_BOUNCE, config::vol, 1.0, &this->position, KSND_3D);
			} else {
				this->velocity = 0.0f;
			}
		}
	}

	this->reposition(&this->position);
}

#define FALL_SLOW_DOWN 0.24f
#define FALL_DAMAGE_VELOCITY 0.58f
#define FALL_DAMAGE_SCALAR 4096

long PlayerType::move_player()
{
	float f, f2;
	f = fsynctics; //player acceleration scalar

	voxlap::extension::FloatVector3D for_tmp = this->orientation_for;
	voxlap::extension::FloatVector3D str_tmp = this->orientation_str;
	voxlap::extension::FloatVector3D hei_tmp = this->orientation_hei;

	if (this->jump) {
		this->jump = 0;
		this->velocity.z = -0.36f;
		if (!this->wade)
			aos_playsound(AOS_SOUND_JUMP, config::vol, 1.0, &this->position, KSND_3D | KSND_MOVE);
		else
			aos_playsound(AOS_SOUND_WATER_JUMP, config::vol, 1.0, &this->position, KSND_3D | KSND_MOVE);
	}

	if (this->airborne)
		f *= 0.1f;
	else if (this->crouch)
		f *= 0.3f;
	else if ((this->secondary && this->tool == TOOL_GUN) || this->sneak)
		f *= 0.5f;
	else if (this->sprint)
		f *= 1.3f;

	if ((this->move_for || this->move_back) && (this->move_left || this->move_right))
		f *= SQRT; //if strafe + forward/backwards then limit diagonal velocity

	if (this->move_for) {
		this->velocity.x += for_tmp.x*f;
		this->velocity.y += for_tmp.y*f;
	} else if (this->move_back) {
		this->velocity.x -= for_tmp.x*f;
		this->velocity.y -= for_tmp.y*f;
	}
	if (this->move_right) {
		this->velocity.x += str_tmp.x*f;
		this->velocity.y += str_tmp.y*f;
	} else if (this->move_left) {
		this->velocity.x -= str_tmp.x*f;
		this->velocity.y -= str_tmp.y*f;
	}

	f = fsynctics + 1;
	this->velocity.z += fsynctics;
	this->velocity.z /= f; //air friction
	if (this->wade)
		f = fsynctics*6.f + 1; //water friction
	else if (!this->airborne)
		f = fsynctics*4.f + 1; //ground friction
	this->velocity.x /= f;
	this->velocity.y /= f;
	f2 = this->velocity.z;

	//if (this->team != -1 && this->alive)
		this->boxclipmove_player();
	//else {
	//	voxlap::clipmove_float(&this->position, &this->velocity, 0.05f);
	//	this->reposition(&this->position);
	//}

	//hit ground... check if hurt
	if (!this->velocity.z && (f2 > FALL_SLOW_DOWN)) {
		//slow down on landing
		this->velocity.x *= 0.5f;
		this->velocity.y *= 0.5f;

		//return fall damage
		if (f2 > FALL_DAMAGE_VELOCITY) {
			f2 -= FALL_DAMAGE_VELOCITY;
			return((long)(f2*f2*FALL_DAMAGE_SCALAR));
		}

		return(-1); // no fall damage but play fall sound
	}

	return(0); //no fall damage
}

#define SQRT3 0.57735026

void PlayerType::move_spectator()
{
	float f, f2;

	//move spectator and perform simple physics (momentum, etc)

	f = fsynctics; //player acceleration scalar
	if (this->sprint)
		f *= 2.0f;

	long mov_amount = 0;
	mov_amount += (this->move_for || this->move_back);
	mov_amount += (this->move_left || this->move_right);
	mov_amount += (this->jump || this->crouch);
	switch (mov_amount) {
	case 2:
		f *= SQRT;
		break;
	case 3:
		f *= SQRT3;
		break;
	}

	if (this->move_for) {
		this->velocity.x += this->orientation_for.x*f;
		this->velocity.y += this->orientation_for.y*f;
		this->velocity.z += this->orientation_for.z*f;
	} else if (this->move_back) {
		this->velocity.x -= this->orientation_for.x*f;
		this->velocity.y -= this->orientation_for.y*f;
		this->velocity.z -= this->orientation_for.z*f;
	}
	if (this->move_left) {
		this->velocity.x -= this->orientation_str.x*f;
		this->velocity.y -= this->orientation_str.y*f;
		this->velocity.z -= this->orientation_str.z*f;
	} else if (this->move_right) {
		this->velocity.x += this->orientation_str.x*f;
		this->velocity.y += this->orientation_str.y*f;
		this->velocity.z += this->orientation_str.z*f;
	}
	if (this->jump) {
		this->velocity.x -= this->orientation_hei.x*f;
		this->velocity.y -= this->orientation_hei.y*f;
		this->velocity.z -= this->orientation_hei.z*f;
	} else if (this->crouch) {
		this->velocity.x += this->orientation_hei.x*f;
		this->velocity.y += this->orientation_hei.y*f;
		this->velocity.z += this->orientation_hei.z*f;
	}

	f = fsynctics + 1;
	this->velocity.x /= f;
	this->velocity.y /= f;
	this->velocity.z /= f; //air friction

	this->boxclipmove_spectator();
}

void PlayerType::move_dead()
{
	float f, f2;

	if (this->dead_rise && voxlap::isvoxelsolid(this->position.x, this->position.y, this->position.z)) {
		this->velocity.z -= fsynctics;
	} else {
		this->velocity.z += fsynctics;
		if (this->dead_rise) this->dead_rise = false;
	}

	f = fsynctics + 1;
	this->velocity.z /= f; //air friction
	if (this->wade)
		f = fsynctics*6.f + 1; //water friction
	else if (!this->airborne)
		f = fsynctics*4.f + 1; //ground friction
	this->velocity.x /= f;
	this->velocity.y /= f;

	this->boxclipbounce_dead();
}

void PlayerType::local_handle_input(float fmousx, float fmousy, bool update_mouse_buttons)
{
	if (!sysmain::progactive)
		return;
	if (this->team == PLAYER_UNCONNECTED)
		return;

	float fsmouse = 1.0f / 50.0f; // fsynctics;
	float fmousy_view = (config::inverty) ? -fmousy : fmousy;

	float sens_multiplier = 2.0f / (1 + (this->secondary && this->tool == TOOL_GUN)); // 2's a bit of a magic number, but it makes things bearable

	if (this->team == ID_TEAM_SPEC || !this->alive) {
		voxlap::orthorotate(this->orientation_str.z*.1,
			((fmousy_view * PI / 180.) * fsmouse) * config::mouse_sens_y * sens_multiplier,
			((fmousx * PI / 180.) * fsmouse) * config::mouse_sens_x * sens_multiplier,
			&this->orientation_str, &this->orientation_hei, &this->orientation_for);
	} else {
		if (!config::drunkcam) {
			this->reorient_lat(fmousx, -fmousy_view,
				config::mouse_sens_x * fsmouse * sens_multiplier,
				config::mouse_sens_y * fsmouse * sens_multiplier);
		} else {
			voxlap::extension::FloatVector3D for_after_rot = this->orientation_for;
			voxlap::extension::FloatVector3D str_after_rot = this->orientation_str;
			voxlap::extension::FloatVector3D hei_after_rot = this->orientation_hei;
			voxlap::orthorotate(this->orientation_str.z*.1,
				((fmousy_view * PI / 180.) * fsmouse) * config::mouse_sens_y * sens_multiplier,
				((fmousx * PI / 180.) * fsmouse) * config::mouse_sens_x * sens_multiplier,
				&str_after_rot, &hei_after_rot, &for_after_rot);
			if (hei_after_rot.z >= 0.05) {
				this->orientation_for = for_after_rot;
				this->orientation_str = str_after_rot;
				this->orientation_hei = hei_after_rot;
			}
		}
	}

	if (update_mouse_buttons) {
		bool newlclk = sysmain::ext_mbstatus[0] & 1;
		bool newrclk = sysmain::ext_mbstatus[1] & 1;

		if (this->lclk != newlclk) {
			if (newlclk) // new click
				this->lclicked = true;
			else // new unclick
				this->luclicked = true;
		}

		if (this->rclk != newrclk) {
			if (newrclk) // new click
				this->rclicked = true;
			else // new unclick
				this->ruclicked = true;
		}

		this->lclk = newlclk;
		this->rclk = newrclk;
	}

	if (this->alive) {
		this->fmousx = fmousx;
		this->fmousy = fmousy;

		aos_ubyte new_input, old_input = this->get_inputdata();

		this->move_for = sysmain::keystatus[config::move_forward] && !aos_ui_chatmode;
		this->move_back = sysmain::keystatus[config::move_backward] && !aos_ui_chatmode;
		this->move_left = sysmain::keystatus[config::move_left] && !aos_ui_chatmode;
		this->move_right = sysmain::keystatus[config::move_right] && !aos_ui_chatmode;

		bool crouch_pressed = sysmain::keystatus[config::crouch] && !aos_ui_chatmode;
		bool jump_pressed = sysmain::keystatus[config::jump] && !aos_ui_chatmode;
		if (this->team > ID_TEAM_SPEC) {
			if (crouch_pressed) {
				if (!this->crouch)
					if (!this->airborne)
						this->position.z += 0.9f;

				this->crouch = true;
			} else {
				// attempting to uncrouch
				if (this->crouch)
					if (this->try_uncrouch())
						this->crouch = false;
			}

			if (jump_pressed) {
				if (!this->airborne) {
					this->jump = true;
					sysmain::keystatus[config::jump] = 0;
				}
			}

		} else {
			this->crouch = crouch_pressed && !aos_ui_chatmode;
			this->jump = jump_pressed && !aos_ui_chatmode;
		}

		if (this->team > ID_TEAM_SPEC)
			this->update_m_inputs(true);

		this->sprint = sysmain::keystatus[config::sprint] && !aos_ui_chatmode;

		this->sneak = sysmain::keystatus[config::sneak] && !aos_ui_chatmode;

		new_input = this->get_inputdata();
		if (new_input != old_input) {
			AoSPacket_InputData inputdata;
			inputdata.playerID = this->id;
			inputdata.keystates = new_input;
			aos_network_send(ID_INPUT_DATA, &inputdata, sizeof(AoSPacket_InputData), true);
		}

	} else { 
		if (this->lclicked) {
			do {
				this->specid++;
				if (this->specid == 32)
					this->specid = 0;
			} while (aos_players[this->specid].team == PLAYER_UNCONNECTED || aos_players[this->specid].team != this->team);
		} else if (this->rclicked) {
			do {
				this->specid--;
				if (this->specid == -1)
					this->specid = 31;
			} while (aos_players[this->specid].team == PLAYER_UNCONNECTED || aos_players[this->specid].team != this->team);
		}
	}
}

void PlayerType::update_m_inputs(bool send)
{
	this->primary = this->lclk && this->get_tool_can_activate();

	if (!this->sprint) {
		switch (this->tool) {
		case TOOL_GUN:
			if (this->rclicked && this->get_weapon_can_zoom()) this->secondary = !this->secondary;
			break;
		default:
			this->secondary = this->rclk;
			break;
		}
	}

	if (this->sprint)
		if (this->secondary && this->tool == TOOL_GUN)
			this->secondary = false;

	if (this->team > ID_TEAM_SPEC && send) {
		aos_ubyte inputstate = this->get_weaponinput();
		if (inputstate != this->lastinputstate) {
			AoSPacket_WeaponInput weaponinput;
			weaponinput.playerID = this->id;
			weaponinput.weaponstates = inputstate;
			aos_network_send(ID_WEAPON_INPUT, &weaponinput, sizeof(AoSPacket_WeaponInput), true);

			this->lastinputstate = inputstate;
		}
	}
}

void PlayerType::local_handle_keypress(long key, aos_ubyte scancode)
{
	if (aos_ui_chatmode)
		return;

	if (this->alive && this->team > ID_TEAM_SPEC) {
		if (scancode == config::cube_color_sample) {
			if (this->tool == TOOL_BLOCK && this->tool_active) {
				if (this->hitcol != NULL && this->hitdistxy < 128.0) {
					this->set_color(*this->hitcol & 0x00FFFFFF, true);
				} else {
					this->set_color(voxlap::vx5.fogcol & 0x00FFFFFF, true);
				}
				this->lastdisable = dtotclk - (PLAYER_SWITCHTIME - TOOL_BLOCK_PICK_RATE); // bit of hack to make picking really fast
				this->nextaction = dtotclk + this->get_tool_primary_delay();

				aos_playsound(AOS_SOUND_SWITCH, config::vol, 1.0, NULL, NULL);
			}
		}

		if (scancode == config::last_weapon) {
			if (this->last_tool == this->tool) return;
			aos_ubyte tmp_tool = this->tool;
			this->set_tool(this->last_tool);
			this->last_tool = tmp_tool;
			this->lastswitch = this->lastdisable = dtotclk;
			this->tool_switched = true;
			aos_playsound(AOS_SOUND_SWITCH, config::vol, 1.0, NULL, NULL);
			return;
		}

		if (scancode == config::reload) {
			if (this->tool == TOOL_GUN)
				this->reload_weapon();
			return;
		}
	}

	aos_ubyte slot_num = aos_ui_get_slot(scancode);
	if (slot_num != 0) {
		slot_num--;

		if (aos_ui_switchmode != AOS_UI_SWITCH_NONE) {
			static aos_byte team;
			static aos_ubyte weapon;

			switch (aos_ui_switchmode) {
			case AOS_UI_SWITCH_TEAM:
				if (slot_num >= 3) break;
				team = (slot_num < 2) ? slot_num : -1;
				if (team == this->team) {
					aos_ui_switchmode = AOS_UI_SWITCH_NONE;
					break;
				}
				if (this->team == PLAYER_UNCONNECTED) {
					if (team == ID_TEAM_SPEC) {
						this->existingplayer(ID_TEAM_SPEC, WEAPON_SEMI, TOOL_GUN, 0, 0x707070, config::name);

						AoSPacket_ExistingPlayer existingplayer;
						memset(&existingplayer, 0, sizeof(AoSPacket_ExistingPlayer));
						this->create_existingplayer_packet(&existingplayer);
						aos_network_send(ID_EXISTING_PLAYER, &existingplayer, sizeof(AoSPacket_ExistingPlayer), true);
						aos_ui_switchmode = AOS_UI_SWITCH_NONE;
					} else {
						aos_ui_switchmode = AOS_UI_SWITCH_WEAPON;
					}
				} else if (this->team == ID_TEAM_SPEC) {
					aos_ui_switchmode = AOS_UI_SWITCH_WEAPON;
				} else {
					AoSPacket_ChangeTeam changeteam;
					changeteam.playerID = this->id;
					changeteam.teamID = team;
					aos_network_send(ID_CHANGE_TEAM, &changeteam, sizeof(AoSPacket_ChangeTeam), true);
					aos_ui_switchmode = AOS_UI_SWITCH_NONE;
				}
				break;
			case AOS_UI_SWITCH_WEAPON:
				if (slot_num >= 3) break;
				weapon = slot_num;
				if (this->team > ID_TEAM_SPEC && weapon == this->weapon) {
					aos_ui_switchmode = AOS_UI_SWITCH_NONE;
					break;
				}
				if (this->team == PLAYER_UNCONNECTED) {
					this->existingplayer(team, weapon, TOOL_GUN, 0, 0x707070, config::name);

					AoSPacket_ExistingPlayer existingplayer;
					memset(&existingplayer, 0, sizeof(AoSPacket_ExistingPlayer));
					this->create_existingplayer_packet(&existingplayer);
					aos_network_send(ID_EXISTING_PLAYER, &existingplayer, sizeof(AoSPacket_ExistingPlayer), true);
				} else if (this->team == ID_TEAM_SPEC) {
					AoSPacket_ShortPlayerData shortplayerdata;
					shortplayerdata.playerID = this->id;
					shortplayerdata.team = team;
					shortplayerdata.weapon = weapon;
					aos_network_send(ID_SHORT_PLAYER, &shortplayerdata, sizeof(AoSPacket_ShortPlayerData), true);
				} else {
					AoSPacket_ChangeWeapon changeweapon;
					changeweapon.playerID = this->id;
					changeweapon.weapon = weapon;
					aos_network_send(ID_CHANGE_WEAPON, &changeweapon, sizeof(AoSPacket_ChangeWeapon), true);
				}
				aos_ui_switchmode = AOS_UI_SWITCH_NONE;
				break;
			}
		} else {
			if (this->alive && this->team > ID_TEAM_SPEC) {
				if (slot_num <= MAX_TOOL) {
					if (slot_num == this->tool) return;
					if (this->local_get_tool_is_valid(slot_num) == false) return;
					this->last_tool = this->tool;
					this->set_tool(slot_num);
					this->lastswitch = this->lastdisable = dtotclk;
					this->tool_switched = true;
					aos_playsound(AOS_SOUND_SWITCH, config::vol, 1.0, NULL, NULL);
					return;
				}
			}
		}
	}

	if (aos_ui_switchmode == AOS_UI_SWITCH_NONE) {
		if (scancode == config::change_team) {
			if (aos_ui_switchmode == AOS_UI_SWITCH_NONE)
				aos_ui_switchmode = AOS_UI_SWITCH_TEAM;
			return;
		} else if (scancode == config::change_weapon) {
			if (aos_ui_switchmode == AOS_UI_SWITCH_NONE) {
				if (this->team <= ID_TEAM_SPEC)
					aos_ui_switchmode = AOS_UI_SWITCH_TEAM;
				else
					aos_ui_switchmode = AOS_UI_SWITCH_WEAPON;
			}
			return;
		}
	}
}

void PlayerType::local_switch_to_valid_tool()
{
	if (!this->local) return;
	aos_ubyte tool = this->tool;
	while (!this->local_get_tool_is_valid(tool))
		tool--;
	if (tool != this->tool) {
		this->last_tool = tool;
		this->set_tool(tool);
		this->lastswitch = this->lastdisable = dtotclk;
		this->tool_switched = true;
		aos_playsound(AOS_SOUND_SWITCH, config::vol, 1.0, NULL, NULL);
	}
}

void PlayerType::local_update()
{
	if (this->team == PLAYER_UNCONNECTED)
		return;

	if (!this->alive) {
		voxlap::lpoint3d collidpt;
		voxlap::extension::FloatVector3D plpos, campos;
		
		if (this->specid == this->id)
			plpos = this->position;
		else
			plpos = aos_players[this->specid].cam_position;

		campos = plpos - (this->orientation_for * 5.0);

		if (!voxlap::cansee(&plpos, &campos, &collidpt)) {
			voxlap::extension::FloatVector3D blockpos = voxlap::extension::FloatVector3D(0.5f, 0.5f, 0.5f) + collidpt;
			float distance = (plpos - blockpos).length();
			campos = plpos - (this->orientation_for * (distance - 1.73205081f));
		}
		this->cam_position = campos;
	}

	if (this->alive && this->team > ID_TEAM_SPEC) {
		if (aos_gamestate == GameState::INGAME) {

#define POSITION_RATE 1.0f // (1.0f / 120.0f)
#define ORIENTATION_RATE (1.0f / 10.0f)

			if (this->lastpossent + POSITION_RATE < dtotclk) {
				aos_network_send(ID_POSITION_DATA, &this->position, sizeof(AoSPacket_PositionData), true);
				this->lastpossent = dtotclk;
			}

			if (this->lastorisent + ORIENTATION_RATE < dtotclk) {
				voxlap::extension::FloatVector3D f = orientation_for;
				aos_network_send(ID_ORIENTATION_DATA, &f, sizeof(AoSPacket_OrientationData), false);
				this->lastorisent = dtotclk;
			}
		}


		if (this->sprint || this->get_weapon_hide_while_reloading()) {
			this->lastdisable = dtotclk;
		}

		if (this->lastswitch + PLAYER_SWITCHTIME < dtotclk) {
			if (this->tool_switched) {
				this->tool_switched = false;
				// change happened, send tool update packet!
				AoSPacket_SetTool settool;
				settool.playerID = this->id;
				settool.tool = this->tool;
				aos_network_send(ID_SET_TOOL, &settool, sizeof(AoSPacket_SetTool), true);

				this->nextaction_secondary = dtotclk + this->get_tool_secondary_delay();
			}
		}

		if (this->tool == TOOL_BLOCK) {
			if (this->nextaction < dtotclk && this->get_tool_can_activate()) {
				if (this->rclicked) {
					if (this->hitdist <= MINE_RANGE && this->hitcol != NULL && this->hitp.z < 62) {
						this->startp = this->hitp;
						this->block_drag = true;
					} else {
						this->block_drag = false;
					}
				}

				if (this->ruclicked && this->block_drag) {
					if (this->hitcol == NULL || this->hitdist > MINE_RANGE || this->block_stock < this->block_drag_num || this->hitp.z >= 62) {
					} else {
						this->endp = this->hitp;
						AoSPacket_BlockLine blockline;
						blockline.playerID = this->id;

						blockline.sX = this->startp.x;
						blockline.sY = this->startp.y;
						blockline.sZ = this->startp.z;

						blockline.eY = this->endp.y;
						blockline.eZ = this->endp.z;
						blockline.eX = this->endp.x;
						aos_network_send(ID_BLOCK_LINE, &blockline, sizeof(AoSPacket_BlockLine), true);

						aos_playsound(AOS_SOUND_SWITCH, config::vol, 1.0, NULL, 0);

						this->block_stock -= block_drag_num;
						this->lastdisable = dtotclk;
						this->nextaction = dtotclk + this->get_tool_primary_delay();
					}
					this->block_drag = false;
				}
			}

			if (this->block_drag) {
				if (this->hitcol != NULL && this->hitdist <= MINE_RANGE && this->hitp.z < 62) {
					this->block_drag_num = (aos_ubyte)aos_vxl_cube_line(this->startp.x, this->startp.y, this->startp.z,
						this->hitp.x, this->hitp.y, this->hitp.z, aos_vxl_cube_array);
					if (this->block_drag_num >= CUBE_ARRAY_LENGTH) {
						this->block_drag = false;
						this->lastdisable = dtotclk;
						this->nextaction = dtotclk + this->get_tool_primary_delay();
					}
				} else {
					this->block_drag_num = 1;
					aos_vxl_cube_array[0] = this->startp;
				}
			}
		}
	}
}

void footstep_sound(voxlap::point3d *pos, bool wade)
{
	long rand_num = rand() % 4; // 4 footstep sounds each (wade/ground)
	const char *sound_file;

	if (!wade) {
		switch (rand_num) {
		case 0:
			sound_file = AOS_SOUND_FOOTSTEP1;
			break;
		case 1:
			sound_file = AOS_SOUND_FOOTSTEP2;
			break;
		case 2:
			sound_file = AOS_SOUND_FOOTSTEP3;
			break;
		case 3:
			sound_file = AOS_SOUND_FOOTSTEP4;
			break;
		}
	} else {
		switch (rand_num) {
		case 0:
			sound_file = AOS_SOUND_WADE1;
			break;
		case 1:
			sound_file = AOS_SOUND_WADE2;
			break;
		case 2:
			sound_file = AOS_SOUND_WADE3;
			break;
		case 3:
			sound_file = AOS_SOUND_WADE4;
			break;
		}
	}

	aos_playsound(sound_file, config::vol, 1.0, pos, KSND_3D | KSND_MOVE);
}

void PlayerType::update()
{
	if (this->team == PLAYER_UNCONNECTED)
		return;

	this->check_local();

	voxlap::hitscan_f(&this->cam_position, &this->orientation_for, &this->hitp, &this->hitcol, &this->hitsid);
	if (this->tool == TOOL_BLOCK) voxlap::hitscan_side(&this->hitp, this->hitsid);
	this->hitdist = (this->cam_position - (voxlap::extension::FloatVector3D(0.5f, 0.5f, 0.5f) + this->hitp)).length();
	this->hitdistxy = ((this->cam_position - (voxlap::extension::FloatVector3D(0.5f, 0.5f, 0.5f) + this->hitp)) * voxlap::extension::FloatVector3D(1, 1, 0)).length();

	if (this->team > ID_TEAM_SPEC) {

		if (this->alive) {
			if (this->sprint) this->steprate = 0.4;
			else			  this->steprate = 0.5;

			if (this->laststep + this->steprate < dtotclk) {
				this->laststep = dtotclk;
				this->stepstate = !this->stepstate;
				if (!this->airborne && !this->crouch && !this->sneak && !(this->secondary && this->tool == TOOL_GUN)) {
					float velflat = (this->velocity * voxlap::extension::FloatVector3D(1, 1, 0)).lengthsquared();
					if (velflat > 0.01) {
						footstep_sound(&this->position, this->wade);
					}
				}
			}

			if (!this->grenade_active) {
				if (this->tool == TOOL_SPADE) {
					if (luclicked || rclicked) {
						this->nextaction_secondary = dtotclk + this->get_tool_secondary_delay();
					}
				}

				if (this->primary) {
					if (this->nextaction < dtotclk && this->get_tool_can_activate()) {
						switch (this->tool) {
						case TOOL_SPADE:
							this->nextaction = dtotclk + this->get_tool_primary_delay();
							this->use_spade(false);
							break;
						case TOOL_BLOCK:
							if (this->use_block())
								this->nextaction = dtotclk + this->get_tool_primary_delay();
							break;
						case TOOL_GUN:
							this->fire_weapon();
							this->nextaction = dtotclk + this->get_tool_primary_delay();
							break;
						case TOOL_GRENADE:
							this->grenade_active = true;
							aos_playsound(AOS_SOUND_GRENADE_PIN, config::vol, 1.0, &this->cam_position, KSND_3D | KSND_MOVE);
							this->nextaction = dtotclk + TOOL_GRENADE_TIME;
							break;
						}
					}
				} else if (this->secondary) {
					if (this->nextaction_secondary < dtotclk && this->get_tool_can_activate()) {
						switch (this->tool) {
						case TOOL_SPADE:
							this->nextaction_secondary = dtotclk + this->get_tool_secondary_delay();
							this->use_spade(true);
							break;
						}
					}
				}
			} else {
				if (this->luclicked && this->nextaction > dtotclk && this->grenade_active) {
					aos_playsound(AOS_SOUND_WOOSH, config::vol, 1.0, &this->cam_position, KSND_3D | KSND_MOVE);
					// grenade throw sound
					this->grenade_active = false;

					this->nextaction = dtotclk + this->get_tool_primary_delay();

					if (this->local) {
						this->lastdisable = dtotclk;

						// create & throw grenade
						this->use_grenade(this->curaction);
					}
				}
				if (this->primary && this->nextaction < dtotclk && this->grenade_active) {
					this->grenade_active = false;
					this->nextaction = dtotclk + this->get_tool_primary_delay();
					if (this->local) {
						this->lastdisable = dtotclk;
						this->use_grenade(0.0f);
					}
				}
			}

			if (this->nextaction > dtotclk) {
				this->local_switch_to_valid_tool();
			}

			this->curaction = this->nextaction - dtotclk;
			this->curaction_secondary = this->nextaction_secondary - dtotclk;

			this->curstep = dtotclk - this->laststep;

			if (this->reload_remaining >= 0 && this->lastreload + this->get_weapon_default_reload_delay() < dtotclk) {
				if (this->weapon == WEAPON_SHOTGUN) {
					if (this->reload_remaining > 0) {
						this->weapon_ammo++;
						this->weapon_stock--;
						aos_playsound(AOS_SOUND_RELOAD_SHOTGUN, config::vol, 1.0, &this->cam_position, KSND_3D | KSND_MOVE);

						if (this->local) { // aos is dumb and puts the cock sound on the last reloaded round instead of after all of them (ie, full reload = 5 shells + 1 cock on AoS).
							AoSPacket_WeaponReload reload;
							reload.playerID = this->id;
							reload.clip = this->weapon_ammo;
							reload.reserve = this->weapon_stock;
							aos_network_send(ID_WEAPON_RELOAD, &reload, sizeof(AoSPacket_WeaponReload), true);
						}
					} else {
						aos_playsound(AOS_SOUND_COCK_SHOTGUN, config::vol, 1.0, &this->cam_position, KSND_3D | KSND_MOVE);
					}

					this->reload_remaining--;
					this->lastreload = dtotclk + this->get_weapon_default_reload_delay();

				} else {
					this->weapon_ammo += reload_remaining;
					this->weapon_stock -= reload_remaining;
					this->reload_remaining = -1;

					if (this->local) {
						this->update_m_inputs(true);
					}
				}
			}

			if (this->tool == TOOL_GUN) {
				if (this->lclicked) {
					if (!this->get_tool_can_activate()) {
						aos_playsound(AOS_SOUND_EMPTY_CLIP, config::vol, 1.0, &this->cam_position, KSND_3D | KSND_MOVE);
						if (!this->weapon_ammo && this->tool_active)
							this->lastaction = dtotclk;
					}
				}
			}

			
			long sound = this->move_player();
			if (sound != 0) {
				if (!this->wade)
					aos_playsound(AOS_SOUND_LAND, config::vol, 1.0, &this->position, KSND_3D | KSND_MOVE);
				else
					aos_playsound(AOS_SOUND_WATER_LAND, config::vol, 1.0, &this->position, KSND_3D | KSND_MOVE);
				if (0 < sound)
					aos_playsound(AOS_SOUND_FALLHURT, config::vol, 1.0, &this->position, KSND_3D | KSND_MOVE);
			}

			if (this->lastdisable + PLAYER_SWITCHTIME < dtotclk) {
				this->tool_active = true;
			} else {
				this->tool_active = false;
			}

		} else {
			if (this->lastrespawntick + 1.0f < dtotclk && this->respawntime != 255) {
				this->respawntime--;
				if (this->local) aos_ui_attempt_respawn_sound(this->respawntime);
				this->lastrespawntick = dtotclk;
			}

			this->move_dead();
		}

	} else {
		this->move_spectator();
	}

	this->lclicked = false;
	this->rclicked = false;

	this->luclicked = false;
	this->ruclicked = false;
}

void PlayerType::create_existingplayer_packet(AoSPacket_ExistingPlayer *data)
{
	data->team = this->team;
	data->weapon = this->weapon;
	data->tool = this->tool;
	data->score = this->score;
	get_color(&data->block_red, &data->block_green, &data->block_blue, this->block_color);
	strcpy(data->name, this->name);
}

aos_ubyte PlayerType::get_inputdata()
{
	aos_ubyte input = 0;
	input |= (this->move_for & 1);
	input |= (this->move_back & 1) << 1;
	input |= (this->move_left & 1) << 2;
	input |= (this->move_right & 1) << 3;
	input |= (this->jump & 1) << 4;
	input |= (this->crouch & 1) << 5;
	input |= (this->sneak & 1) << 6;
	input |= (this->sprint & 1) << 7;
	return input;
}

aos_ubyte PlayerType::get_weaponinput()
{
	aos_ubyte input = 0;
	input |= (this->primary & 1);
	input |= (this->secondary & 1) << 1;
	return input;
}

void PlayerType::set_inputdata(aos_ubyte input)
{
	this->move_for = (input) & 1;
	this->move_back = (input >> 1) & 1;
	this->move_left = (input >> 2) & 1;
	this->move_right = (input >> 3) & 1;
	if (((input >> 5) & 1) == 0) {
		if (this->crouch) {
			this->try_uncrouch();
		}
	} else {
		if (!this->crouch) {
			if (!this->airborne)
				this->position.z += 0.9f;
		}
	}

	this->jump = (input >> 4) & 1;
	this->crouch = (input >> 5) & 1;
	this->sneak = (input >> 6) & 1;
	this->sprint = (input >> 7) & 1;

	this->torso.voxnum = aos_get_model(AOS_MODEL_TEAM1_PLAYER_TORSO_UNCROUCHED, this->team, this->crouch);
	this->left_leg.voxnum = aos_get_model(AOS_MODEL_TEAM1_PLAYER_LEG_UNCROUCHED, this->team, this->crouch);
	this->right_leg.voxnum = aos_get_model(AOS_MODEL_TEAM1_PLAYER_LEG_UNCROUCHED, this->team, this->crouch);
}

void PlayerType::set_weaponinput(aos_ubyte input)
{
	if (!this->local) {
		bool newprimary = (input) & 1;
		bool newsecondary = (input >> 1) & 1;

		if (this->primary != newprimary) {
			if (newprimary) // new click
				this->lclicked = true;
			else // new unclick
				this->luclicked = true;
		}

		if (this->secondary != newsecondary) {
			if (newsecondary) // new click
				this->rclicked = true;
			else // new unclick
				this->ruclicked = true;
		}

		this->primary = newprimary;
		this->secondary = newsecondary;
	} else {
		this->primary = (input) & 1;
		this->secondary = (input >> 1) & 1;
	}
}

#define PLAYER_SCALE           (1.0f / 10.0f)
#define THIRDPERSON_ITEM_SCALE (1.0f / 16.0f)
#define ITEM_SCALE             (1.0f / 32.0f)
#define MINI_ITEM_SCALE        (1.0f / 196.0f)

static long prevcol = 0x000000;

voxlap::kv6data *get_tool_voxnum(aos_ubyte tool, aos_ubyte weapon)
{
	switch (tool) {
	case TOOL_SPADE:
		return aos_get_model(AOS_MODEL_TOOL_SPADE, false, false);
		break;
	case TOOL_BLOCK:
		return aos_get_model(AOS_MODEL_TOOL_BLOCK, false, false);
		break;
	case TOOL_GUN:
	default:
		switch (weapon) {
		case WEAPON_SEMI:
		default:
			return aos_get_model(AOS_MODEL_WEAPON_SEMI, false, false);
			break;
		case WEAPON_SMG:
			return aos_get_model(AOS_MODEL_WEAPON_SMG, false, false);
			break;
		case WEAPON_SHOTGUN:
			return aos_get_model(AOS_MODEL_WEAPON_SHOTGUN, false, false);
			break;
		}
		break;
	case TOOL_GRENADE:
		return aos_get_model(AOS_MODEL_GRENADE, false, false);
		break;
	}
}

float fov_ogl_to_voxlap(long xdim, long ydim, float fovy)
{
	float aspect = (float)xdim / (float)ydim;
	float fovx = 2.0f * atanf(tanf(fovy * 0.5f) * aspect);

	return 0.0f;
}

void PlayerType::local_set_camera(long xdim, long ydim)
{
	if (this->alive) {
		float fov = 0.5f;
		bool should_zoom = false;
		if (this->tool == TOOL_GUN && this->secondary) should_zoom = true;
		if (should_zoom)
			fov = 1.0f;
		voxlap::setcamera_float(&this->cam_position, &this->orientation_str, &this->orientation_hei, &this->orientation_for,
			xdim*.5, ydim*.5, xdim*fov);
	} else {
		voxlap::setcamera_float(&this->cam_position, &this->orientation_str, &this->orientation_hei, &this->orientation_for,
			xdim*.5, ydim*.5, xdim*.5);
	}

	sysmain::setears3d(this->cam_position.x, this->cam_position.y, this->cam_position.z,
		this->orientation_for.x, this->orientation_for.y, this->orientation_for.z,
		this->orientation_hei.x, this->orientation_hei.y, this->orientation_hei.z);
}

void PlayerType::draw_local_models()
{
	if (this->team <= ID_TEAM_SPEC)
		return;

	if (!this->alive) {
		this->upd_draw_models();
		return;
	}

	if (this->secondary && this->tool == TOOL_GUN)
		return;
	if (this->tool == TOOL_BLOCK) {
		if (this->tool_active) {
			if (this->block_drag && this->secondary) {
				for (int i = 0; i < this->block_drag_num; i++) {
					if (i > 0) if (this->hitdist > MINE_RANGE) break;
					voxlap::lpoint3d *block = &aos_vxl_cube_array[i];
					if (i + 1 <= this->block_stock) {
						aos_world_render_wirecube(block->x, block->y, block->z, 0xFFFFFF);
					} else {
						aos_world_render_wirecube(block->x, block->y, block->z, 0xFF0000);
					}
				}
			} else {
				if (this->hitcol != NULL && this->hitdist <= MINE_RANGE && this->hitp.z < 62) {
					if (this->block_stock > 0) {
						aos_world_render_wirecube(this->hitp.x, this->hitp.y, this->hitp.z, 0xFFFFFF);
					} else {
						aos_world_render_wirecube(this->hitp.x, this->hitp.y, this->hitp.z, 0xFF0000);
					}
				}
			}
		}
	}

	voxlap::extension::FloatVector3D tmp(0);
	voxlap::setcamera_float(&tmp, &this->orientation_str, &this->orientation_hei, &this->orientation_for,
		sysmain::xres*.5, sysmain::yres*.5, sysmain::xres*.5);

	float stepanim = 0.0;
	if (this->stepstate == 0) {
		stepanim = -this->steprate + (this->curstep * 2.0f);
	} else if (this->stepstate == 1) {
		stepanim = this->steprate - (this->curstep * 2.0f);
	}
	stepanim *= (this->velocity * voxlap::extension::FloatVector3D(1.0f, 1.0f, 0.0f)).length();
	stepanim *= 4.0;

	float activeanim = 0.0;
	if (!this->tool_active) {
		activeanim = 1.0 - (dtotclk - this->lastdisable) / PLAYER_SWITCHTIME;
	}
	activeanim *= 0.5;

	this->item.voxnum = get_tool_voxnum(this->tool, this->weapon);
	item.p = this->orientation_for*ITEM_SCALE * 5 + this->orientation_str*ITEM_SCALE*2.5 + this->orientation_hei*ITEM_SCALE * 3 + /* scaling and positioning */
		this->orientation_hei * -this->velocity.z * ITEM_SCALE + this->orientation_for * stepanim * ITEM_SCALE - /*movement*/
		this->orientation_for * activeanim + this->orientation_hei * activeanim + /*switch anim*/
		this->orientation_str * -fmousx / 64.0f * ITEM_SCALE + this->orientation_hei * -fmousy / 64.0f * ITEM_SCALE;
	item.s = this->orientation_str * -ITEM_SCALE * 0.25;
	item.f = this->orientation_hei * ITEM_SCALE * 0.25;  // f == h..??
	item.h = this->orientation_for * ITEM_SCALE * 0.25; // h == f..??

	float rot_amount_x = 0, rot_amount_y = 0;
	if (this->curaction > 0) {
		float recoil = 0;
		switch (this->tool) {
		case TOOL_SPADE:
			if (this->primary) {
				rot_amount_y = -70 * (this->curaction / TOOL_SPADE_PRIMARY_RATE);
				activeanim = (1.0 - (this->curaction / TOOL_SPADE_PRIMARY_RATE)) * 0.25;
				tmp = item.f; tmp *= -.5;
				voxlap::axisrotate(&tmp, &item.s, -(rot_amount_y * PI) / 180.0);
				item.p = (tmp * item.voxnum->zsiz + (voxlap::extension::FloatVector3D)item.f * item.voxnum->zsiz * 0.5) + item.p;
			}
			break;
			/*case TOOL_BLOCK: // replaced by use_block -> lastdisable
				activeanim = (1.0 - (this->curaction)) * 0.5;
				item.p = this->orientation_for * activeanim + this->orientation_hei * activeanim + item.p;
				break;*/
		case TOOL_GUN:
			recoil = this->get_weapon_default_recoil()*0.5;
			rot_amount_y = 5 * recoil * (this->curaction / this->get_weapon_default_fire_delay());
			item.p = -(this->orientation_for*ITEM_SCALE*recoil * (this->curaction / this->get_weapon_default_fire_delay())) + item.p;
			break;
		case TOOL_GRENADE:
			activeanim = (TOOL_GRENADE_TIME - this->curaction);
			item.p = -this->orientation_hei*ITEM_SCALE * 4 * activeanim + this->orientation_str*ITEM_SCALE * 4 * activeanim + item.p;
		}
	} else if (this->curaction_secondary > 0) {
		switch (this->tool) {
		case TOOL_SPADE:
			if (this->secondary) {
				rot_amount_x = -70 * (this->curaction_secondary / TOOL_SPADE_SECONDARY_RATE);
			}
			break;
		}
	}
	if (rot_amount_y) {
		voxlap::axisrotate(&item.h, &item.s, -(rot_amount_y * PI) / 180.0);
		voxlap::axisrotate(&item.f, &item.s, -(rot_amount_y * PI) / 180.0);
	}

	if (rot_amount_x) {
		voxlap::axisrotate(&item.h, &item.f, -(rot_amount_x * PI) / 180.0);
		voxlap::axisrotate(&item.s, &item.f, -(rot_amount_x * PI) / 180.0);
	}

	if (aos_ui_switchmode == AOS_UI_SWITCH_NONE) {
		if (this->tool == TOOL_BLOCK) {
			voxlap::vx5.kv6col = this->block_color;
			voxlap::drawsprite(&item);
			voxlap::vx5.kv6col = 0x808080;
		} else {
			voxlap::drawsprite(&item);
		}
	}

	if (!this->tool_active && (this->tool_switched || this->get_weapon_hide_while_reloading())) {
		voxlap::extension::FloatVector3D itemstr = this->orientation_str;
		voxlap::extension::FloatVector3D itemfor = this->orientation_for;
		voxlap::axisrotate(&itemstr, &this->orientation_hei, dtotclk);
		voxlap::axisrotate(&itemfor, &this->orientation_hei, dtotclk);

#define ITEM_ACTIVE_SCALE 1.5f

		if (this->local_get_tool_is_valid(TOOL_SPADE)) {
			activeanim = (this->tool == TOOL_SPADE) ? ITEM_ACTIVE_SCALE : 1.0f;
			this->item.voxnum = get_tool_voxnum(TOOL_SPADE, NULL);
			item.p = this->orientation_for * ITEM_SCALE * 5 + this->orientation_str * ITEM_SCALE * -1.8 + this->orientation_hei * ITEM_SCALE * activeanim -
				this->orientation_for * ITEM_SCALE * (activeanim - 1.0f); /* scaling and positioning */
			item.s = itemstr * -MINI_ITEM_SCALE * 0.25 * activeanim;
			item.f = this->orientation_hei * MINI_ITEM_SCALE * 0.25 * activeanim;  // f == h..??
			item.h = itemfor * MINI_ITEM_SCALE * 0.25 * activeanim; // h == f..??

			voxlap::drawsprite(&item);
		}

		if (this->local_get_tool_is_valid(TOOL_BLOCK)) {
			activeanim = (this->tool == TOOL_BLOCK) ? ITEM_ACTIVE_SCALE : 1.0f;
			this->item.voxnum = get_tool_voxnum(TOOL_BLOCK, NULL);
			item.p = this->orientation_for * ITEM_SCALE * 5 + this->orientation_str * ITEM_SCALE * -0.6 + this->orientation_hei * ITEM_SCALE * activeanim -
				this->orientation_for * ITEM_SCALE * (activeanim - 1.0f); /* scaling and positioning */
			item.s = itemstr * -MINI_ITEM_SCALE * 0.25 * activeanim;
			item.f = this->orientation_hei * MINI_ITEM_SCALE * 0.25 * activeanim;  // f == h..??
			item.h = itemfor * MINI_ITEM_SCALE * 0.25 * activeanim; // h == f..??

			voxlap::vx5.kv6col = this->block_color;
			voxlap::drawsprite(&item);
			voxlap::vx5.kv6col = 0x808080;
		}

		if (this->local_get_tool_is_valid(TOOL_GUN)) {
			activeanim = (this->tool == TOOL_GUN) ? ITEM_ACTIVE_SCALE : 1.0f;
			this->item.voxnum = get_tool_voxnum(TOOL_GUN, this->weapon);
			item.p = this->orientation_for * ITEM_SCALE * 5 + this->orientation_str * ITEM_SCALE * 0.6 + this->orientation_hei * ITEM_SCALE * activeanim -
				this->orientation_for * ITEM_SCALE * (activeanim - 1.0f); /* scaling and positioning */
			item.s = itemstr * -MINI_ITEM_SCALE * 0.25 * activeanim;
			item.f = this->orientation_hei * MINI_ITEM_SCALE * 0.25 * activeanim;  // f == h..??
			item.h = itemfor * MINI_ITEM_SCALE * 0.25 * activeanim; // h == f..??

			voxlap::drawsprite(&item);
		}

		if (this->local_get_tool_is_valid(TOOL_GRENADE)) {
			activeanim = (this->tool == TOOL_GRENADE) ? ITEM_ACTIVE_SCALE : 1.0f;
			this->item.voxnum = get_tool_voxnum(TOOL_GRENADE, NULL);
			item.p = this->orientation_for * ITEM_SCALE * 5 + this->orientation_str * ITEM_SCALE * 1.8 + this->orientation_hei * ITEM_SCALE * activeanim -
				this->orientation_for * ITEM_SCALE * (activeanim - 1.0f); /* scaling and positioning */
			item.s = itemstr * -MINI_ITEM_SCALE * 0.25 * activeanim;
			item.f = this->orientation_hei * MINI_ITEM_SCALE * 0.25 * activeanim;  // f == h..??
			item.h = itemfor * MINI_ITEM_SCALE * 0.25 * activeanim; // h == f..??

			voxlap::drawsprite(&item);
		}
	}

	if (aos_gamemode == ID_GAMEMODE_CTF) {
		if (this->ctf_has_enemy_flag()) {
			flag.voxnum = aos_world_get_team(!this->team)->ctf_flag_voxnum;
			flag.p = this->orientation_for*ITEM_SCALE * 5 - this->orientation_str*ITEM_SCALE * 4;
			flag.s = this->orientation_str * -MINI_ITEM_SCALE * 0.6;
			flag.f = this->orientation_hei * MINI_ITEM_SCALE * 0.6;  // f == h..??
			flag.h = this->orientation_for * MINI_ITEM_SCALE * 0.6; // h == f..??
			voxlap::axisrotate(&flag.s, &flag.f, dtotclk);
			voxlap::axisrotate(&flag.h, &flag.f, dtotclk);
			voxlap::drawsprite(&flag);
		}
	} else if (aos_gamemode == ID_GAMEMODE_TC) {
		TC_Territory *territory = aos_world_get_closest_territory(&this->cam_position);
		if (territory != NULL) {
			flag.voxnum = territory->voxnum;
			flag.p = this->orientation_for*ITEM_SCALE * 5 - this->orientation_str*ITEM_SCALE * 4;
			flag.s = this->orientation_str * -MINI_ITEM_SCALE * 0.6;
			flag.f = this->orientation_hei * MINI_ITEM_SCALE * 0.6;  // f == h..??
			flag.h = this->orientation_for * MINI_ITEM_SCALE * 0.6; // h == f..??
			voxlap::axisrotate(&flag.s, &flag.f, dtotclk / 2);
			voxlap::axisrotate(&flag.h, &flag.f, dtotclk / 2);
			voxlap::drawsprite(&flag);
			if (territory->progress) {
				aos_ui_draw_tc_bar(territory->team, territory);
			}
		}
	}
}

void PlayerType::draw_local_switchmodels()
{
	if (aos_ui_switchmode != AOS_UI_SWITCH_NONE) {
		voxlap::extension::FloatVector3D tmp(0), tmps, tmph, tmpf(1, 0, 0); set_orientation_vectors(&tmpf, &tmps, &tmph);
		voxlap::setcamera_float(&tmp, &tmps, &tmph, &tmpf,
			sysmain::xres*.5, sysmain::yres*.5, sysmain::xres*.5);

		voxlap::vx5sprite sprite; sprite.flags = 0;
		switch (aos_ui_switchmode) {
		case AOS_UI_SWITCH_TEAM:
			aos_players[32].upd_draw_models();
			aos_players[33].upd_draw_models();
			break;
		case AOS_UI_SWITCH_WEAPON:
			sprite.voxnum = get_tool_voxnum(TOOL_GUN, WEAPON_SEMI);
			sprite.p = tmpf*ITEM_SCALE * 2.25 + tmps*ITEM_SCALE*-1.25 + tmph*ITEM_SCALE * 1;
			sprite.s = tmps * -MINI_ITEM_SCALE * 0.25;
			sprite.f = tmph * MINI_ITEM_SCALE * 0.25;  // f == h..??
			sprite.h = tmpf * MINI_ITEM_SCALE * 0.25; // h == f..??

			voxlap::axisrotate(&sprite.s, &sprite.f, dtotclk);
			voxlap::axisrotate(&sprite.h, &sprite.f, dtotclk);

			voxlap::drawsprite(&sprite);

			sprite.voxnum = get_tool_voxnum(TOOL_GUN, WEAPON_SMG);
			sprite.p = tmpf*ITEM_SCALE * 2.25 + tmph*ITEM_SCALE * 1;
			sprite.s = tmps * -MINI_ITEM_SCALE * 0.25;
			sprite.f = tmph * MINI_ITEM_SCALE * 0.25;  // f == h..??
			sprite.h = tmpf * MINI_ITEM_SCALE * 0.25; // h == f..??

			voxlap::axisrotate(&sprite.s, &sprite.f, dtotclk);
			voxlap::axisrotate(&sprite.h, &sprite.f, dtotclk);

			voxlap::drawsprite(&sprite);

			sprite.voxnum = get_tool_voxnum(TOOL_GUN, WEAPON_SHOTGUN);
			sprite.p = tmpf*ITEM_SCALE * 2.25 + tmps*ITEM_SCALE*1.25 + tmph*ITEM_SCALE * 1;
			sprite.s = tmps * -MINI_ITEM_SCALE * 0.25;
			sprite.f = tmph * MINI_ITEM_SCALE * 0.25;  // f == h..??
			sprite.h = tmpf * MINI_ITEM_SCALE * 0.25; // h == f..??

			voxlap::axisrotate(&sprite.s, &sprite.f, dtotclk);
			voxlap::axisrotate(&sprite.h, &sprite.f, dtotclk);

			voxlap::drawsprite(&sprite);
			break;
		}
	}
}

void PlayerType::upd_draw_models()
{
	voxlap::extension::FloatVector3D for_no_z, str_no_z, hei_no_z;

	if (this->alive) {

		for_no_z = (this->orientation_for * voxlap::extension::FloatVector3D(1.0f, 1.0f, 0.0f)).normalized();
		set_orientation_vectors(&for_no_z, &str_no_z, &hei_no_z);

		head.p = this->cam_position + voxlap::extension::FloatVector3D(0.f, 0.f,
			0.3f) * this->render_scale;
		head.s = this->orientation_str * -PLAYER_SCALE * this->render_scale;
		head.f = this->orientation_hei * PLAYER_SCALE * this->render_scale;  // f == h..??
		head.h = this->orientation_for * PLAYER_SCALE * this->render_scale; // h == f..??

		torso.p = this->cam_position + voxlap::extension::FloatVector3D(0.f, 0.f, 0.3f) * this->render_scale;
		torso.s = str_no_z * -PLAYER_SCALE * this->render_scale;
		torso.h = for_no_z * PLAYER_SCALE * this->render_scale;
		torso.f = hei_no_z * PLAYER_SCALE * this->render_scale;

		voxlap::extension::FloatVector3D arm_forward = this->orientation_for;
		voxlap::extension::FloatVector3D arm_height = this->orientation_hei;

		arms.p = this->cam_position + voxlap::extension::FloatVector3D(0.f, 0.f,
			0.5f - (0.1f*this->crouch)) * this->render_scale;
		arms.s = this->orientation_str * -PLAYER_SCALE * this->render_scale;

		if (this->sprint) {
			voxlap::axisrotate(&arm_forward, &arms.s, 45 * (PI / 180.));
			voxlap::axisrotate(&arm_height, &arms.s, 45 * (PI / 180.));
		} else {
			float rot_amount = 0.0f, recoil;
			if (this->curaction > 0) {
				switch (this->tool) {
				case TOOL_SPADE:
				case TOOL_BLOCK:
					rot_amount = 45 * (this->curaction / this->get_tool_primary_delay());
					break;
				case TOOL_GUN:
					recoil = this->get_weapon_default_recoil()*0.5;
					rot_amount = -5 * recoil * (this->curaction / this->get_weapon_default_fire_delay());
					break;
				case TOOL_GRENADE:
					rot_amount = -30 * (TOOL_GRENADE_TIME - this->curaction);
					break;
				}
			} else if (this->curaction_secondary > 0 && this->secondary) {
				switch (this->tool) {
				case TOOL_SPADE:
					rot_amount = 45 * (this->get_tool_secondary_delay() - this->curaction_secondary);
					break;
				}
			}
			if (rot_amount) {
				voxlap::axisrotate(&arm_forward, &arms.s, rot_amount * (PI / 180.));
				voxlap::axisrotate(&arm_height, &arms.s, rot_amount * (PI / 180.));
			}
		}

		arms.f = arm_height * PLAYER_SCALE * this->render_scale;  // f == h..??
		arms.h = arm_forward * PLAYER_SCALE * this->render_scale; // h == f..??

		item.p = (voxlap::extension::FloatVector3D)arms.p +
			(voxlap::extension::FloatVector3D)arm_forward * PLAYER_SCALE * this->render_scale * 9.0f +
			this->orientation_str * PLAYER_SCALE * this->render_scale * 4.0f;

		if (this->tool == TOOL_GUN) {
			item.p = (arm_forward * THIRDPERSON_ITEM_SCALE * this->render_scale * 6.0f) + item.p;
		}

		item.s = this->orientation_str * -THIRDPERSON_ITEM_SCALE * this->render_scale;
		item.f = arm_height * THIRDPERSON_ITEM_SCALE * this->render_scale;  // f == h..??
		item.h = arm_forward * THIRDPERSON_ITEM_SCALE * this->render_scale; // h == f..??

		left_leg.p = this->cam_position + voxlap::extension::FloatVector3D(0.f, 0.f,
			1.1f) * this->render_scale + this->orientation_str * -0.25 * this->render_scale +
			(for_no_z*-0.4f + hei_no_z*-0.4f)*this->crouch * this->render_scale;
		left_leg.s = str_no_z * -PLAYER_SCALE * this->render_scale;
		left_leg.h = for_no_z * PLAYER_SCALE * this->render_scale;
		left_leg.f = hei_no_z * PLAYER_SCALE * this->render_scale;

		right_leg.p = this->cam_position + voxlap::extension::FloatVector3D(0.f, 0.f,
			1.1f) * this->render_scale + this->orientation_str * 0.25 * this->render_scale +
			(for_no_z*-0.4f + hei_no_z*-0.4f)*this->crouch * this->render_scale;
		right_leg.s = str_no_z * -PLAYER_SCALE * this->render_scale;
		right_leg.h = for_no_z * PLAYER_SCALE * this->render_scale;
		right_leg.f = hei_no_z * PLAYER_SCALE * this->render_scale;

		auto velflat = (this->velocity * voxlap::extension::FloatVector3D(1, 1, 0));

		if (velflat.lengthsquared() > 0.01*0.01) {
			float stepanim = 0.0;
			if (this->stepstate == 0) {
				stepanim = -this->steprate + (this->curstep * 2.0);
			} else if (this->stepstate == 1) {
				stepanim = this->steprate - (this->curstep * 2.0);
			}
			stepanim *= (velflat * 2.0).length();
			stepanim *= 4.0f;

			voxlap::extension::FloatVector3D velrot = velflat.normalized();
			voxlap::axisrotate(&velrot, &hei_no_z, 90 * (PI / 180.));


			axisrotate(&left_leg.s, &velrot, stepanim);
			axisrotate(&left_leg.h, &velrot, stepanim);
			axisrotate(&left_leg.f, &velrot, stepanim);


			axisrotate(&right_leg.s, &velrot, -stepanim);
			axisrotate(&right_leg.h, &velrot, -stepanim);
			axisrotate(&right_leg.f, &velrot, -stepanim);
		}

		if (this->ctf_has_enemy_flag()) {
			flag.voxnum = aos_world_get_team(!this->team)->ctf_flag_voxnum;
			flag.p = (voxlap::extension::FloatVector3D)torso.p - for_no_z * PLAYER_SCALE * 3 +
				hei_no_z * PLAYER_SCALE * 6;
			flag.s = torso.s;
			flag.h = torso.h;
			flag.f = torso.f;
			voxlap::drawsprite(&flag);
		}

		voxlap::drawsprite(&head);
		voxlap::drawsprite(&torso);
		voxlap::drawsprite(&arms);
		voxlap::drawsprite(&right_leg);
		voxlap::drawsprite(&left_leg);
		if (this->tool == TOOL_BLOCK) {
			voxlap::vx5.kv6col = this->block_color;
			voxlap::drawsprite(&item);
			voxlap::vx5.kv6col = 0x808080;
		} else {
			voxlap::drawsprite(&item);
		}
	} else {
		for_no_z = (this->orientation_dead * voxlap::extension::FloatVector3D(1.0f, 1.0f, 0.0f)).normalized();
		set_orientation_vectors(&for_no_z, &str_no_z, &hei_no_z);

		dead.p = this->position;
		dead.s = str_no_z * -PLAYER_SCALE * this->render_scale;
		dead.h = for_no_z * PLAYER_SCALE * this->render_scale;
		dead.f = hei_no_z * PLAYER_SCALE * this->render_scale;

		if ((this->position - this->cam_position).length() > 0.5 || !this->local)
			voxlap::drawsprite(&dead);
	}
}

void PlayerType::set_team(aos_ubyte team)
{
	this->team = team;
	if (this->team == ID_TEAM_SPEC)
		this->score = 0;

	this->head.voxnum = aos_get_model(AOS_MODEL_TEAM1_PLAYER_HEAD, this->team, false);
	this->torso.voxnum = aos_get_model(AOS_MODEL_TEAM1_PLAYER_TORSO_UNCROUCHED, this->team, false);
	this->arms.voxnum = aos_get_model(AOS_MODEL_TEAM1_PLAYER_ARMS, this->team, false);
	this->left_leg.voxnum = aos_get_model(AOS_MODEL_TEAM1_PLAYER_LEG_UNCROUCHED, this->team, false);
	this->right_leg.voxnum = aos_get_model(AOS_MODEL_TEAM1_PLAYER_LEG_UNCROUCHED, this->team, false);
	this->dead.voxnum = aos_get_model(AOS_MODEL_TEAM1_PLAYER_DEAD, this->team, false);
}

void PlayerType::set_tool(aos_ubyte tool)
{
	if (tool != this->tool)
		this->secondary = false;

	this->grenade_active = false;

	if (this->local) {
		this->nextaction = dtotclk + PLAYER_SWITCHTIME;
		this->nextaction_secondary = dtotclk + PLAYER_SWITCHTIME;
	}

	this->tool = tool;

	if (this->primary) this->lclicked = true;
	if (this->secondary) this->rclicked = true;

	this->item.voxnum = get_tool_voxnum(this->tool, this->weapon);
}

void PlayerType::set_weapon(aos_ubyte weapon)
{
	this->weapon = weapon;

	this->weapon_ammo = this->get_weapon_default_ammo();
	this->weapon_stock = this->get_weapon_default_stock();
}

void PlayerType::set_color(long color, bool update)
{
	this->block_color = color;

	if (this->local && update) {
		AoSPacket_SetColor setcolor;
		setcolor.playerID = this->id;
		get_color(&setcolor.red, &setcolor.green, &setcolor.blue, this->block_color);
		aos_network_send(ID_SET_COLOR, &setcolor, sizeof(AoSPacket_SetColor), true);
	}
}

void PlayerType::kill(aos_ubyte respawn)
{
	this->alive = false;
	this->set_inputdata(0); // quick and dirty way of disabling all bools
	//this->crouch = !this->try_uncrouch();
	this->set_weaponinput(0);
	this->orientation_dead = this->orientation_for;

	this->respawntime = respawn - 1;
	this->lastrespawntick = dtotclk;
	this->lastaction = dtotclk;

	this->reload_remaining = -1;

	this->dead_rise = true;

	if (this->local) {
		this->specid = this->id;
	}
}

void PlayerType::check_restock()
{
	//if (this->lastrestock + PLAYER_RESTOCK_INTERVAL < dtotclk)
	//	this->restock();
}

void PlayerType::restock()
{
	this->lastrestock = dtotclk;

	this->block_stock = TOOL_BLOCK_STOCK;
	this->grenade_stock = TOOL_GRENADE_STOCK;
	this->health = 100;
	this->weapon_stock = this->get_weapon_default_stock();

	if (this->local)
		aos_playsound(AOS_SOUND_SWITCH, config::vol, 1.0, NULL, NULL);
}

long PlayerType::hitscan_test(voxlap::point3d *pos, voxlap::point3d *dir, bool blockscan)
{
	// pos: origin of ray
	// dir: orientation of ray
	// blockscan: "collide" with blocks? (mainly used for looking at teammates to get their nicknames)

	if (this->team == PLAYER_UNCONNECTED)
		return -1;

	voxlap::extension::LongVector3D hitpoint; long *color, face;
	voxlap::kv6voxtype *hitvox;
	float vsc = AOS_FOG_DISTANCE;

	if (blockscan) {
		voxlap::hitscan_f(pos, dir, &hitpoint, &color, &face);
		if (color != NULL) { // if we hit a block at all
			// there has got to be a better way...

			voxlap::extension::FloatVector3D blockvec;
			blockvec.x = hitpoint.x + 0.5f - pos->x;
			blockvec.y = hitpoint.y + 0.5f - pos->y;
			blockvec.z = hitpoint.z + 0.5f - pos->z;

			voxlap::extension::FloatVector3D playervec;
			playervec.x = this->position.x - pos->x;
			playervec.y = this->position.y - pos->y;
			playervec.z = this->position.z - pos->z;

			if (blockvec.length() < playervec.length()) // if block closer than player
				return -1;
		}
	}

	if (this->alive) {

		// prioritizes torso & arms over head
		
		voxlap::sprhitscan_f(pos, dir, &this->torso, &hitpoint, &hitvox, &vsc);
		if (hitvox != NULL)
			return ID_HIT_PACKET_TORSO;

		voxlap::sprhitscan_f(pos, dir, &this->arms, &hitpoint, &hitvox, &vsc);
		if (hitvox != NULL)
			return ID_HIT_PACKET_ARMS;

		voxlap::sprhitscan_f(pos, dir, &this->head, &hitpoint, &hitvox, &vsc);
		if (hitvox != NULL)
			return ID_HIT_PACKET_HEAD;

		voxlap::sprhitscan_f(pos, dir, &this->left_leg, &hitpoint, &hitvox, &vsc);
		if (hitvox != NULL)
			return ID_HIT_PACKET_LEGS;

		voxlap::sprhitscan_f(pos, dir, &this->right_leg, &hitpoint, &hitvox, &vsc);
		if (hitvox != NULL)
			return ID_HIT_PACKET_LEGS;

	} else {
		voxlap::sprhitscan_f(pos, dir, &this->dead, &hitpoint, &hitvox, &vsc);
		if (hitvox != NULL)
			return ID_HIT_DEAD_PLAYER; // not ever send to the server
	}

	return -1;
}

long PlayerType::hitscan_players(long *part, voxlap::point3d *pos, voxlap::point3d *dir, bool blockscan)
{
	long hitid = -1;
	if (part != NULL)
		*part = -1;

	for (int i = 0; i < 32; i++) {
		if (i == this->id) continue;
		PlayerType *loop_pl = &aos_players[i];
		long ret = loop_pl->hitscan_test(pos, dir, blockscan);
		if (ret != -1) {
			hitid = i;
			if (part != NULL)
				*part = ret;
			break;
		}
	}

	return hitid;
}

void PlayerType::check_local()
{
	this->local = (this->id == aos_local_player_id);
}

bool PlayerType::ctf_has_enemy_flag()
{
	if (aos_gamemode != ID_GAMEMODE_CTF) return false;
	if (this->team < ID_TEAM1) return false;
	TeamType *other = aos_world_get_team(!this->team);
	return (!other->ctf_flag_dropped && this->id == other->ctf_flag_playerID);
}

void PlayerType::fire_weapon()
{
	if (!this->alive) return;

	if (this->local) if (!this->weapon_ammo) return;

	AoSPacket_HitPacket hitpacket;
	float spread = this->get_weapon_default_spread();
	if (this->crouch) spread *= 0.66;
	voxlap::point3d fhitp = { this->hitp.x, this->hitp.y, this->hitp.z };
	for (int i = 0; i < this->get_weapon_default_bullets(); i++) {
		voxlap::extension::FloatVector3D spread_orientation = this->orientation_for, spread_ofs_circle(0), up(0, 0, 1);
		spread_ofs_circle.y = (((float)rand() / (float)(RAND_MAX))) * spread;
		voxlap::axisrotate(&spread_ofs_circle, &up, (((float)rand() / (float)(RAND_MAX))) * (PI * 2.0f));
		voxlap::axisrotate(&spread_orientation, &this->orientation_hei, spread_ofs_circle.x * (PI / 180.f));
		voxlap::axisrotate(&spread_orientation, &this->orientation_str, spread_ofs_circle.y * (PI / 180.f));

		aos_world_create_bullet(this->cam_position, spread_orientation, this->weapon);

		long hitpart, hitid = this->hitscan_players(&hitpart, &this->cam_position, &spread_orientation, true);
		PlayerType *hit_pl = &aos_players[hitid];

		if (this->local) {
			if (hitid != -1 && hitpart != -1 && hitpart != ID_HIT_DEAD_PLAYER) {
				hitpacket.hitID = hitid;
				hitpacket.hittype = hitpart;
				aos_network_send(ID_HIT_PACKET, &hitpacket, sizeof(AoSPacket_HitPacket), true);

				if (hitpart == ID_HIT_PACKET_HEAD) {
					aos_playsound(AOS_SOUND_WHACK, config::vol, 1.0, &hit_pl->cam_position, KSND_3D);
				} else {
					aos_playsound(AOS_SOUND_HITPLAYER, config::vol, 1.0, &hit_pl->cam_position, KSND_3D);
				}

				// blood particles
			}
		}

		voxlap::hitscan_f(&this->cam_position, &spread_orientation, &this->hitp, &this->hitcol, &this->hitsid);
		this->hitdist = (this->cam_position - (voxlap::extension::FloatVector3D(0.5f, 0.5f, 0.5f) + this->hitp)).length();
		this->hitdistxy = ((this->cam_position - (voxlap::extension::FloatVector3D(0.5f, 0.5f, 0.5f) + this->hitp)) * voxlap::extension::FloatVector3D(1, 1, 0)).length();

		if (this->hitdistxy < 128.0f) {
			aos_world_block_damage(	this->hitp.x, this->hitp.y, this->hitp.z,
									this->get_tool_default_block_damage(), // assuming tool is weapon..
									(this->local && i == 0)	);

			voxlap::point3d fhitp = { this->hitp.x, this->hitp.y, this->hitp.z };
			aos_playsound(AOS_SOUND_IMPACT, config::vol, 1.0, &fhitp, KSND_3D);

			// block particles
		}
	}

	this->reload_remaining = -1; // reset reload process

	if (this->local) {
		float recoil = this->get_weapon_default_recoil();
		if (this->crouch) recoil *= 0.66;
		else if (this->move_for || this->move_back || this->move_left || this->move_right)
			recoil *= 1.66;
		if (this->airborne) recoil *= 1.66;
		this->reorient_lat(sin(dtotclk * 6.6) * 0.66 * recoil, recoil, 1.0f, 1.0f);

		this->weapon_ammo--;
	}

	const char *sound;
	switch (this->weapon) {
	default:
	case WEAPON_SEMI:
		sound = AOS_SOUND_SHOOT_SEMI;
		break;
	case WEAPON_SMG:
		sound = AOS_SOUND_SHOOT_SMG;
		break;
	case WEAPON_SHOTGUN:
		sound = AOS_SOUND_SHOOT_SHOTGUN;
		break;
	}
	aos_playsound(sound, config::vol, 1.0, &this->cam_position, KSND_3D | KSND_MOVE);
}

void PlayerType::reload_weapon()
{
	if (!this->alive) return;
	if (this->weapon_ammo == this->get_weapon_default_ammo() || !this->weapon_stock) return;
	if (this->weapon != WEAPON_SHOTGUN && this->reload_remaining > 0) return;

	this->reload_remaining = min(this->get_weapon_default_ammo() - this->weapon_ammo, this->weapon_stock);
	
	this->lastreload = dtotclk;
	if (this->weapon == WEAPON_SEMI || this->weapon == WEAPON_SMG)
		this->secondary = false;

	if (this->local) {
		AoSPacket_WeaponReload reload;
		reload.playerID = this->id;
		reload.clip = this->weapon_ammo;
		reload.reserve = this->weapon_stock;
		aos_network_send(ID_WEAPON_RELOAD, &reload, sizeof(AoSPacket_WeaponReload), true);

		this->update_m_inputs(true);
	}

	const char *sound;
	switch (this->weapon) {
	default:
	case WEAPON_SEMI:
		sound = AOS_SOUND_RELOAD_SEMI;
		break;
	case WEAPON_SMG:
		sound = AOS_SOUND_RELOAD_SMG;
		break;
	case WEAPON_SHOTGUN:
		//sound = AOS_SOUND_RELOAD_SHOTGUN;
		return;
		break;
	}
	aos_playsound(sound, config::vol, 1.0, &this->cam_position, KSND_3D | KSND_MOVE);
}

bool PlayerType::use_block()
{
	if (this->hitcol != NULL && this->hitdist < MINE_RANGE && this->block_stock && this->hitp.z < 62) {
		this->block_stock--;
		this->lastdisable = dtotclk;
		if (this->local) {
			AoSPacket_BlockAction blockaction;
			blockaction.playerID = this->id;
			blockaction.type = ID_BLOCK_ACTION_BUILD;
			blockaction.x = this->hitp.x;
			blockaction.y = this->hitp.y;
			blockaction.z = this->hitp.z;
			aos_network_send(ID_BLOCK_ACTION, &blockaction, sizeof(AoSPacket_BlockAction), true);

			aos_playsound(AOS_SOUND_SWITCH, config::vol, 1.0, NULL, 0);
		}
		return true;
	}
	return false;
}

static int spadeuses = 0;

void PlayerType::use_spade(bool secondary)
{
	voxlap::point3d hp = { this->hitp.x + 0.5f, this->hitp.y + 0.5f, this->hitp.z + 0.5f };
	if (!secondary) {
		long hitpart, hitid = this->hitscan_players(&hitpart, &this->cam_position, &this->orientation_for, true);
		PlayerType *hit_pl = NULL;
		if (hitid != -1 && hitpart != -1 && hitpart != ID_HIT_DEAD_PLAYER) {
			hit_pl = &aos_players[hitid];
		}
		if (hit_pl == NULL) {
			if (this->hitcol != NULL && this->hitdist < MINE_RANGE) {
				if (this->hitp.z < 62) {
					bool destroyed = aos_world_block_damage(this->hitp.x, this->hitp.y, this->hitp.z, this->get_tool_default_block_damage(), this->local);
					if (this->local) {
						if (destroyed && this->block_stock < 50)
							this->block_stock++;
					}
				}
				aos_playsound(AOS_SOUND_HITGROUND, config::vol, 1.0, &hp, KSND_3D);
			} else {
				aos_playsound(AOS_SOUND_WOOSH, config::vol, 1.0, &this->cam_position, KSND_3D | KSND_MOVE);
			}
		} else {
			if ((this->cam_position - hit_pl->cam_position).length() < MINE_RANGE) {
				if (this->local) {
					AoSPacket_HitPacket hitpacket;
					hitpacket.hitID = hitid;
					hitpacket.hittype = ID_HIT_PACKET_MELEE;
					aos_network_send(ID_HIT_PACKET, &hitpacket, sizeof(AoSPacket_HitPacket), true);
					aos_playsound(AOS_SOUND_WHACK, config::vol, 1.0, &hit_pl->cam_position, KSND_3D | KSND_MOVE);
				} else {
					aos_playsound(AOS_SOUND_WOOSH, config::vol, 1.0, &hit_pl->cam_position, KSND_3D | KSND_MOVE);
				}
			}
		}
	} else {
		if (this->hitcol != NULL && this->hitdist < MINE_RANGE && this->hitp.z < 62) {
			if (this->local) {
				AoSPacket_BlockAction blockaction;
				blockaction.x = this->hitp.x;
				blockaction.y = this->hitp.y;
				blockaction.z = this->hitp.z;
				blockaction.type = ID_BLOCK_ACTION_SPADE;
				aos_network_send(ID_BLOCK_ACTION, &blockaction, sizeof(AoSPacket_BlockAction), true);
			}
		} else {
			aos_playsound(AOS_SOUND_WOOSH, config::vol, 1.0, &this->cam_position, KSND_3D | KSND_MOVE);
		}
	}
}

void PlayerType::use_grenade(float time)
{
	if (this->grenade_stock <= 0)
		return;

	if (this->local) {
		this->grenade_stock--;

		voxlap::point3d pos = this->cam_position;
		voxlap::extension::FloatVector3D vel = this->velocity;
		
		if (time > 0.0f)
			vel += this->orientation_for;

		AoSPacket_GrenadePacket grenadepacket;
		grenadepacket.playerID = this->id;
		grenadepacket.posX = pos.x;
		grenadepacket.posY = pos.y;
		grenadepacket.posZ = pos.z;
		grenadepacket.velX = vel.x;
		grenadepacket.velY = vel.y;
		grenadepacket.velZ = vel.z;
		grenadepacket.fuse = time;
		aos_network_send(ID_GRENADE_PACKET, &grenadepacket, sizeof(AoSPacket_GrenadePacket), true);

		aos_world_create_grenade(this->cam_position, this->orientation_for + this->velocity, time);
	}
}

aos_ubyte PlayerType::get_weapon_default_stock()
{
	switch (this->weapon) {
	case WEAPON_SEMI:
	default:
		return WEAPON_SEMI_STOCK;
		break;
	case WEAPON_SMG:
		return WEAPON_SMG_STOCK;
		break;
	case WEAPON_SHOTGUN:
		return WEAPON_SHOTGUN_STOCK;
		break;
	}
}

aos_ubyte PlayerType::get_weapon_default_ammo()
{
	switch (this->weapon) {
	case WEAPON_SEMI:
	default:
		return WEAPON_SEMI_AMMO;
		break;
	case WEAPON_SMG:
		return WEAPON_SMG_AMMO;
		break;
	case WEAPON_SHOTGUN:
		return WEAPON_SHOTGUN_AMMO;
		break;
	}
}

float PlayerType::get_weapon_default_fire_delay()
{
	switch (this->weapon) {
	case WEAPON_SEMI:
	default:
		return WEAPON_SEMI_DELAY;
		break;
	case WEAPON_SMG:
		return WEAPON_SMG_DELAY;
		break;
	case WEAPON_SHOTGUN:
		return WEAPON_SHOTGUN_DELAY;
		break;
	}
}

float PlayerType::get_weapon_default_reload_delay()
{
	switch (this->weapon) {
	case WEAPON_SEMI:
	default:
		return WEAPON_SEMI_RELOAD;
		break;
	case WEAPON_SMG:
		return WEAPON_SMG_RELOAD;
		break;
	case WEAPON_SHOTGUN:
		return WEAPON_SHOTGUN_RELOAD;
		break;
	}
}

float PlayerType::get_weapon_default_spread()
{
	switch (this->weapon) {
	case WEAPON_SEMI:
	default:
		return WEAPON_SEMI_SPREAD;
		break;
	case WEAPON_SMG:
		return WEAPON_SMG_SPREAD;
		break;
	case WEAPON_SHOTGUN:
		return WEAPON_SHOTGUN_SPREAD;
		break;
	}
}

float PlayerType::get_weapon_default_recoil()
{
	switch (this->weapon) {
	case WEAPON_SEMI:
	default:
		return WEAPON_SEMI_RECOIL;
		break;
	case WEAPON_SMG:
		return WEAPON_SMG_RECOIL;
		break;
	case WEAPON_SHOTGUN:
		return WEAPON_SHOTGUN_RECOIL;
		break;
	}
}

float PlayerType::get_weapon_default_bullets()
{
	switch (this->weapon) {
	case WEAPON_SEMI:
	default:
		return WEAPON_SEMI_BULLETS;
		break;
	case WEAPON_SMG:
		return WEAPON_SMG_BULLETS;
		break;
	case WEAPON_SHOTGUN:
		return WEAPON_SHOTGUN_BULLETS;
		break;
	}
}

aos_ubyte PlayerType::get_tool_default_block_damage()
{
	switch (this->tool) {
	case TOOL_SPADE:
		return TOOL_SPADE_BLOCK_DMG;
		break;
	case TOOL_GUN:
		switch (this->weapon) {
		case WEAPON_SEMI:
		default:
			return WEAPON_SEMI_BLOCK_DMG;
			break;
		case WEAPON_SMG:
			return WEAPON_SMG_BLOCK_DMG;
			break;
		case WEAPON_SHOTGUN:
			return WEAPON_SHOTGUN_BLOCK_DMG;
			break;
		}
		break;
	default:
		return 0;
		break;
	}
}

float PlayerType::get_tool_primary_delay()
{
	switch (this->tool) {
	case TOOL_SPADE:
		return TOOL_SPADE_PRIMARY_RATE;
		break;
	case TOOL_BLOCK:
		return TOOL_BLOCK_RATE;
		break;
	case TOOL_GUN:
	default:
		return this->get_weapon_default_fire_delay();
		break;
	case TOOL_GRENADE:
		return TOOL_GRENADE_DELAY;
		break;
	}
}

float PlayerType::get_tool_secondary_delay()
{
	switch (this->tool) {
	case TOOL_SPADE:
		return TOOL_SPADE_SECONDARY_RATE;
		break;
	default:
		return 0.0f;
		break;
	}
}

bool PlayerType::local_get_tool_is_valid(aos_ubyte tool)
{
	switch (tool) {
	case TOOL_SPADE:
		return true;
		break;
	case TOOL_BLOCK:
		return (this->block_stock > 0);
		break;
	case TOOL_GUN:
		return (this->weapon_stock > 0);
		break;
	case TOOL_GRENADE:
		return (this->grenade_stock > 0);
		break;
	default:
		return true;
		break;
	}
}

bool PlayerType::get_tool_can_activate()
{
	if (!this->tool_active && this->local || this->sprint) return false;
	switch (this->tool) {
	case TOOL_SPADE:
		return true;
		break;
	case TOOL_BLOCK:
		return (this->block_stock > 0);
		break;
	case TOOL_GUN:
		return (this->weapon_ammo > 0 && (this->reload_remaining <= 0 || this->weapon == WEAPON_SHOTGUN));
		break;
	case TOOL_GRENADE:
		return (this->grenade_stock > 0);
		break;
	default:
		return true;
		break;
	}
}

bool PlayerType::get_weapon_can_zoom()
{
	if (!this->tool_active && this->local || this->sprint) return false;
	return (this->reload_remaining <= 0 || this->weapon == WEAPON_SHOTGUN);
}

bool PlayerType::get_weapon_hide_while_reloading()
{
	if (this->reload_remaining <= 0) return false;
	return (this->tool == TOOL_GUN) && (this->weapon != WEAPON_SHOTGUN);
}