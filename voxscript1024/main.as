// voxscript release test game by muffin
// feel free to screw around with this whole script! :)

// controls. http://www.ee.bgu.ac.il/~microlab/MicroLab/Labs/ScanCodes.htm
const int KEY_FORWARD   = 0x11; // def.: 0x11, W
const int KEY_BACKWARD  = 0x1F; // def.: 0x1F, S
const int KEY_LEFT      = 0x1E; // def.: 0x1E, A
const int KEY_RIGHT     = 0x20; // def.: 0x20, D
const int KEY_JUMP      = 0x39; // def.: 0x39, Space
const int KEY_SPRINT    = 0x2A; // def.: 0x2A, Left Shift
const float SENSITIVITY = 1.33f;

point3d ipos, ivel, istr, ihei, ifor;
point3d cpos;

point3d soundpos;

bool move_forw = false;
bool move_back = false;
bool move_left = false;
bool move_right = false;

bool jump = false;
bool sprint = false;

bool charge = false;

bool airborne = false;
bool wade = false;

double lastclimb = -1.0;
float speedzoom = 1.0f;

const double SHOOTDELAY = 1.5;
const double BOUNCEDELAY = 2.0;
double lastshoot = -BOUNCEDELAY;

float shootcharge = 0.0f;

kv6data@ tracer_model;

const float GUN_SCALE = 1.0f / 32.0f;
vx5sprite gun;
float gunxoff = 0.0f;
float gunyoff = 0.0f;
float gunzoff = 0.0f;

const float PI = 3.14159265f;
const float SQRT = 0.70710678f;

class tracer
{
    vx5sprite model;
    double time;
    point3d end;
    
    tracer(point3d &in start, point3d &in end)
    {
        this.model.p = start;
        this.end = end;
        this.time = dtotclk;
        @this.model.voxnum = @tracer_model;
        this.model.kfatim = 0; this.model.okfatim = 0; this.model.flags = 0;
    }
    
    void update()
    {
        point3d diff, difstr, difhei, diffor; float d, f;
        //diff.set(this.end.x - this.model.p.x, this.end.y - this.model.p.y, this.end.z - this.model.p.z);
        diff.set(this.model.p.x - this.end.x, this.model.p.y - this.end.y, this.model.p.z - this.end.z);
        d = sqrt(diff.x*diff.x + diff.y*diff.y + diff.z*diff.z);
        diffor.set(diff.x / d, diff.y / d, diff.z / d);
        f = sqrt(diffor.x*diffor.x + diffor.y*diffor.y);
        difstr.set(-diffor.y / f, diffor.x / f, 0.0f);
        difhei.set(-diffor.z*difstr.y, diffor.z*difstr.x, diffor.x*difstr.y - diffor.y*difstr.x);
        
        double size = 1.0f - (dtotclk - this.time);
        this.model.s.set(difstr.x * size, difstr.y * size, difstr.z * size);
        this.model.h.set(difhei.x * size, difhei.y * size, difhei.z * size);
        this.model.f.set(diffor.x * (d / 32.0f), diffor.y * (d / 32.0f), diffor.z * (d / 32.0f));
    }
    
    bool shoulddelete()
    {
        return dtotclk > (this.time + 1.0f);
    }
}

class particle {
    point3d pos;
    point3d vel;
    int color;
    double time;
    
    particle(point3d &in pos, point3d &in vel, int color)
    {
        this.pos = pos;
        this.vel = vel;
        this.color = color;
        this.time = dtotclk;
    }
    
    void update(double fsynctics)
    {
        point3d fp = this.pos;
        
        this.vel.z += fsynctics * 64.0;
        this.pos.x += this.vel.x * fsynctics;
        this.pos.y += this.vel.y * fsynctics;
        this.pos.z += this.vel.z * fsynctics;
        
        lpoint3d hpos;
        if (cansee(fp, this.pos, hpos) == 0) {
            this.pos = fp;
            
            estnorm(hpos.x, hpos.y, hpos.z, fp);
            float f = (this.vel.x*fp.x + this.vel.y*fp.y + this.vel.z*fp.z)*2.0f;
            this.vel.x = (this.vel.x - fp.x * f) * 0.66f;
            this.vel.y = (this.vel.y - fp.y * f) * 0.66f;
            this.vel.z = (this.vel.z - fp.z * f) * 0.66f;
        }
    }
    
    bool shoulddelete()
    {
        return dtotclk > (this.time + 1.0);
    }
}

array<tracer@> tracers;
array<particle@> particles;

bool initapp()
{
    xres = 1280;
    yres = 720;
    colbits = 32;
    fullscreen = 0;
    mousmoth = 0;
    
	loadvxl("vxl/untitled.vxl");
	
	ipos.set(128.0f, 256.0f, 16.0f);
	ifor.set(1.0f, 0.0f, 0.0f);
	istr.set(0.0f, 1.0f, 0.0f);
	ihei.set(0.0f, 0.0f, 1.0f);

    ivel.set(0, 0, 0);
    
    soundpos.set(200, 256, 32);
    
	@tracer_model = loadkv6("kv6/tracer.kv6");
	@gun.voxnum = loadkv6("kv6/gun.kv6");
    gun.kfatim = 0; gun.okfatim = 0; gun.flags = 0;
    
    loadsky("png/TOONSKY.JPG");
	vx5.maxscandist = 1024.0f * 1.414f;
	vx5.lightmode = 1;
	setsideshades(0,64,32,32,16,48);
    
    return false;
}

bool clipbox(float x, float y, float z)
{
	int sz;
	if (x < 0.0f || x >= 1024.0f || y < 0.0f || y >= 1024.0f) return true;
    if (z < 0.0f) return false;
	sz = int(z);
	if (sz == 255) sz = 254;
	else if (sz >= 255) return true;
	else if (sz < 0) return false;
    if (z == 255) return isvoxelsolid(x, y, sz) == 1;
    return isvoxelsolid(x, y, sz) == 1;
}

void boxclipmove(double fsynctics)
{
    float offset, m, f, nx, ny, nz, z;
	bool climb = false;

	f = fsynctics*32.f;
	nx = f*ivel.x + ipos.x;
	ny = f*ivel.y + ipos.y;

	offset = 0.9f;
	m = 1.35f;

	nz = ipos.z + offset;

	if (ivel.x < 0) f = -0.45f;
	else f = 0.45f;
	z = m;
	while (z >= -1.36f && !clipbox(nx + f, ipos.y - 0.45f, nz + z) && !clipbox(nx + f, ipos.y + 0.45f, nz + z))
		z -= 0.9f;
	if (z<-1.36f) ipos.x = nx;
	else if (ifor.z<0.5f && !sprint) {
		z = 0.35f;
		while (z >= -2.36f && !clipbox(nx + f, ipos.y - 0.45f, nz + z) && !clipbox(nx + f, ipos.y + 0.45f, nz + z))
			z -= 0.9f;
		if (z<-2.36f) {
			ipos.x = nx;
			climb = true;
		} else ivel.x = 0;
	} else ivel.x = 0;

	if (ivel.y < 0) f = -0.45f;
	else f = 0.45f;
	z = m;
	while (z >= -1.36f && !clipbox(ipos.x - 0.45f, ny + f, nz + z) && !clipbox(ipos.x + 0.45f, ny + f, nz + z))
		z -= 0.9f;
	if (z<-1.36f) ipos.y = ny;
	else if (ifor.z<0.5f && !sprint && !climb) {
		z = 0.35f;
		while (z >= -2.36f && !clipbox(ipos.x - 0.45f, ny + f, nz + z) && !clipbox(ipos.x + 0.45f, ny + f, nz + z))
			z -= 0.9f;
		if (z<-2.36f) {
			ipos.y = ny;
			climb = true;
		} else ivel.y = 0;
	} else if (!climb)
		ivel.y = 0;

	if (climb) {
		ivel.x *= 0.5f;
		ivel.y *= 0.5f;
		lastclimb = dtotclk;
		nz--;
		m = -1.35f;
	} else {
		if (ivel.z < 0)
			m = -m;
		nz +=ivel.z*fsynctics*32.f;
	}

	airborne = true;

	if (clipbox(ipos.x - 0.45f, ipos.y - 0.45f, nz + m) ||
		clipbox(ipos.x - 0.45f, ipos.y + 0.45f, nz + m) ||
		clipbox(ipos.x + 0.45f, ipos.y - 0.45f, nz + m) ||
		clipbox(ipos.x + 0.45f, ipos.y + 0.45f, nz + m)) {
		if (ivel.z >= 0) {
			//if (this->local)
			//	this->wade = (ipos.z > 60) && (voxlap::getfloorz(ipos.x, ipos.y, ipos.z) == 63);
			//else
			wade = ipos.z > 253;
			airborne = false;
		}
		ivel.z = 0;
	} else
		ipos.z = nz - offset;
        
    cpos = ipos;
    
    f = lastclimb - dtotclk;
    if (f > -0.25f)
        cpos.z += (f + 0.25f) / 0.25f;
}

void move_player(double fsynctics)
{
    float f, f2;
    f = fsynctics * 2.0f; // accel scalar
    
    if (jump) {
        jump = false;
        ivel.z = -0.36f * 2.0f;
        
        // if wade, water sound
        // else regular sound
    }
    
    if (airborne)
        f *= 0.1f;
    else if (sprint)
        f *= 6.0f;
        
    if ((move_forw || move_back) && (move_left && move_right))
        f *= SQRT; // limit diagonal speed
    
    if (move_forw) {
        ivel.x += ifor.x * f;
        ivel.y += ifor.y * f;
    } else if (move_back) {
        ivel.x -= ifor.x * f;
        ivel.y -= ifor.y * f;
    }
    if (move_right) {
        ivel.x += istr.x * f;
        ivel.y += istr.y * f;
    } else if (move_left) {
        ivel.x -= istr.x * f;
        ivel.y -= istr.y * f;
    }
    
    f = fsynctics + 1.0f;
    ivel.z += fsynctics;
    ivel.z /= f; // air friction
    if (wade)
        f = fsynctics*6.0f + 1.0f; // water friction
    else if (!airborne)
        f = fsynctics*4.0f + 1.0f; // ground friction
        
    ivel.x /= f;
    ivel.y /= f;
    f2 = ivel.z;
    
    boxclipmove(fsynctics);
    
    if ((ivel.z == 0.0f) && (f2 > 0.2f)) {
        ivel.x *= 0.5f; // slow down on ground hit
        ivel.y *= 0.5f;
        
        // play fall sound
        
        return;
    }
}

void update_tracers()
{
    for (int i = tracers.length() - 1; i >= 0; i--) {
        tracer@ tr = @tracers[i];
        if (tr.shoulddelete()) {
            @tracers[i] = @tracers[tracers.length() - 1];
            tracers.removeLast();
            continue;
        }
        tr.update();
    }
}

void draw_tracers()
{
    for (int i = 0; i < tracers.length(); i++) {
    
        tracer@ tr = @tracers[i];
        
        double time = dtotclk + (sin(tr.time) * 0.5 + 0.5) * tr.time;
        
        float kv6_red = 64.0f + sin(time * 2.0f) * 64.0f;
        float kv6_green = 64.0f + cos(time * 1.73f) * 64.0f;
        float kv6_blue = 64.0f + sin(time * 1.24f) * 64.0f;
        
        vx5.kv6col = uint8(kv6_red) << 16 | uint8(kv6_green) << 8 | uint8(kv6_blue);
        
        drawsprite(tr.model);
    }
    
    vx5.kv6col = 0x808080;
}

void add_tracer(point3d &in start, point3d &in end) {
    tracers.insertLast(tracer(start, end));
}

void update_particles(double fsynctics)
{
    for (int i = particles.length() - 1; i >= 0; i--) {
        particle@ pr = @particles[i];
        if (pr.shoulddelete()) {
            @particles[i] = @particles[particles.length() - 1];
            particles.removeLast();
            continue;
        }
        pr.update(fsynctics);
    }
}

void draw_particles()
{
    for (int i = 0; i < particles.length(); i++) {
        particle@ pr = @particles[i];
        
        double size = 1.0f - (dtotclk - pr.time);
        drawspherefill(pr.pos.x, pr.pos.y, pr.pos.z, -size * 0.625f, pr.color);
    }
}

void add_particle(point3d &in pos, point3d &in vel, int color)
{
    particles.insertLast(particle(pos, vel, color));
}

uint rand_n = 0;

float randfloat() // not REALLY random.. just random enough, until i get rand() working
{
    return cos(dtotclk * sin(dtotclk * ++rand_n * 1.5) * rand_n * 2.3);
}

void explode_effect(lpoint3d &in lpos, int color)
{
    point3d pos, vel; pos.set(lpos.x, lpos.y, lpos.z);
    
    for (int i = 0; i < 64; i++) {
        
        vel.set(randfloat() * 32.0f,
                randfloat() * 32.0f,
                randfloat() * 32.0f);
                
        add_particle(pos, vel, color);
    }
}

void update_gun(float fmousx, float fmousy, double fsynctics)
{   
    // no, the hei/for switching doesnt really make sense.. blame ken silverman, or ben aksoy maybe
    gun.s.set(-istr.x*GUN_SCALE*0.25f, -istr.y*GUN_SCALE*0.25f, -istr.z*GUN_SCALE*0.25f);
    gun.h.set(ifor.x*GUN_SCALE*0.25f, ifor.y*GUN_SCALE*0.25f, ifor.z*GUN_SCALE*0.25f);
    gun.f.set(ihei.x*GUN_SCALE*0.25f, ihei.y*GUN_SCALE*0.25f, ihei.z*GUN_SCALE*0.25f);
    
    gun.p.set(ifor.x*GUN_SCALE*4 + istr.x*GUN_SCALE*2 + ihei.x*GUN_SCALE*2,
              ifor.y*GUN_SCALE*4 + istr.y*GUN_SCALE*2 + ihei.y*GUN_SCALE*2,
              ifor.z*GUN_SCALE*4 + istr.z*GUN_SCALE*2 + ihei.z*GUN_SCALE*2);
              
    float xdest = -fmousx / 32.0f;
    float ydest = -ivel.z + -fmousy / 32.0f;
    
    gunxoff += (xdest - gunxoff) * fsynctics * 8.0f;
    gunyoff += (ydest - gunyoff) * fsynctics * 8.0f;
    
    gun.p.x += istr.x * gunxoff * GUN_SCALE;
    gun.p.y += istr.y * gunxoff * GUN_SCALE;
    gun.p.z += istr.z * gunxoff * GUN_SCALE;
    
    gun.p.x += ihei.x * gunyoff * GUN_SCALE;
    gun.p.y += ihei.y * gunyoff * GUN_SCALE;
    gun.p.z += ihei.z * gunyoff * GUN_SCALE;
    
    if (dtotclk <= lastshoot + SHOOTDELAY) {
        float scalar = (SHOOTDELAY - (dtotclk - lastshoot)) * 2.0f;
        gunzoff += (scalar - gunzoff) * fsynctics * 32.0f;
    } else {
        gunzoff += -gunzoff * fsynctics * 32.0f;
    }
    gun.p.x -= ifor.x * gunzoff * GUN_SCALE;
    gun.p.y -= ifor.y * gunzoff * GUN_SCALE;
    gun.p.z -= ifor.z * gunzoff * GUN_SCALE;
}

int oldbuttons = 0;

void doframe(double fsynctics)
{
    readkeyboard();
    int key;
	do {
		key = keyread();
		if (key == 0) continue;

		uint8 scancode = (key & 0xFF00) >> 8;
        
        // this is a thing, if you wanted to have anything here vOv
        // this is the proper way to detect key presses, as it supports key repeating (AoS chatbox is based on this)
	} while (key > 0);
    
    if (get_keystatus(0x01) != 0) quitloop(); // escape
    
    move_forw = get_keystatus(KEY_FORWARD) != 0;
	move_back = get_keystatus(KEY_BACKWARD) != 0;
	move_left = get_keystatus(KEY_LEFT) != 0;
	move_right = get_keystatus(KEY_RIGHT) != 0;
    sprint = get_keystatus(KEY_SPRINT) != 0;
    
    bool n_charge = get_keystatus(0x1D) != 0;
    if (n_charge != charge) {
        if (n_charge) {
            
        } else {
        
        }
    }
    charge = n_charge;
    
    if (!airborne) {
        if (get_keystatus(KEY_JUMP) != 0) {
            jump = true;
            set_keystatus(KEY_JUMP, 0);
        }
    }
    
    point3d loc; loc.set(200 + sin(dtotclk)*8.0f, 256 + cos(dtotclk)*16.0f, 32);

	move_player(fsynctics);
    
    float fmousx, fmousy;
	int buttons;
	readmouse(fmousx, fmousy, buttons);

	orthorotate(istr.z*.1,
		(fmousy*PI/180.0f)*fsynctics*SENSITIVITY*2.0f,
		(fmousx*PI/180.0f)*fsynctics*SENSITIVITY*2.0f,
		istr, ihei, ifor, istr, ihei, ifor);
    
    if (buttons != oldbuttons) { // clicks
        if (buttons & 1 == 1) {
            
        }
    }
    
    lpoint3d hitposl; int hitcol; int hitsid;
    hitscan(ipos, ifor, hitposl, hitcol, hitsid);
    
    if (buttons & 1 != 0) { // hold
        if (dtotclk > lastshoot + SHOOTDELAY) {
            lastshoot = dtotclk;
            
            point3d tracerend;
            point3d tracerstart;
            tracerstart.set(cpos.x + istr.x*1.0f + ihei.x + ifor.x * 4.0f,
                            cpos.y + istr.y*1.0f + ihei.y + ifor.y * 4.0f,
                            cpos.z + istr.z*1.0f + ihei.z + ifor.z * 4.0f);
            
            if (hitcol != 0) {
                tracerend.set(hitposl.x, hitposl.y, hitposl.z);
                if (hitposl.z < 255) {
                    setsphere(hitposl, 5, -1);
                    explode_effect(hitposl, hitscan_get_color(hitcol));
                    playsound("wav/explode.wav", 100, 1.0f, tracerend, 1);
                }
            } else {
                tracerend.set(cpos.x + ifor.x * 128.0f,
                            cpos.y + ifor.y * 128.0f,
                            cpos.z + ifor.z * 128.0f);
            }
            
            playsound("wav/gun_fire.wav", 20, 1.0f/* + sin(dtotclk * 1.5) * 0.07f*/, soundpos, 0);
            add_tracer(tracerstart, tracerend);
        }
    } else if (buttons & 2 != 0) {
        if (dtotclk > lastshoot + BOUNCEDELAY) {
            
            if (hitcol != 0) {
                point3d dif;
                dif.set(ipos.x - hitposl.x, ipos.y - hitposl.y, ipos.z - hitposl.z);
                float dist = sqrt(dif.x*dif.x + dif.y*dif.y + dif.z*dif.z);
                
                if (dist < 32.0f) {
                    lastshoot = dtotclk;
                    playsound("wav/gun_fire.wav", 20, 0.75f, soundpos, 0);
                    
                    float scalar = fsynctics * (2048.0f / (dist*dist));
                    ivel.x += dif.x*scalar;
                    ivel.y += dif.y*scalar;
                    ivel.z += dif.z*scalar;
                    
                    double r_tim = dtotclk + (sin(dtotclk*0.3f) * 0.5 + 0.5) * dtotclk;
                    
                    float r_red = 64.0f + sin(r_tim * 2.0f) * 64.0f;
                    float r_green = 64.0f + cos(r_tim * 1.73f) * 64.0f;
                    float r_blue = 64.0f + sin(r_tim * 1.24f) * 64.0f;
                    
                    int color = uint8(r_red) << 16 | uint8(r_green) << 8 | uint8(r_blue);
                    
                    explode_effect(hitposl,color);
                }
            }
            
        }
    }
    
    update_tracers();
    update_particles(fsynctics);
    update_gun(fmousx, fmousy, fsynctics);
    
    oldbuttons = buttons;
        
    int frameptr, pitch, xdim, ydim;
	startdirectdraw(frameptr, pitch, xdim, ydim);
	voxsetframebuffer(frameptr, pitch, xdim, ydim);

	setears3d(cpos, ifor, ihei);

	float zoomdif = 1.0f - (sqrt(ivel.x*ivel.x + ivel.y*ivel.y) * 0.20f);

	speedzoom += (zoomdif - speedzoom) * fsynctics * 8.0f;

	setcamera(cpos, istr, ihei, ifor, xdim*0.5, ydim*0.5, (xdim * 0.5f) * speedzoom);
    
	opticast();
    
    draw_particles();
    draw_tracers();
    
    cpos.set(0, 0, 0); // this is ok -- cpos is set every frame by boxclipmove
	setcamera(cpos, istr, ihei, ifor, xdim*0.5, ydim*0.5, (xdim * 0.5f) * speedzoom);
    
    drawsprite(gun);
    
	print6x8(2, 2, 0xFF0000, 0, "fps: " + int(1.0f / fsynctics));
    
	stopdirectdraw();
	nextpage();
}

void uninitapp()
{
	freekv6(@tracer_model);
	freekv6(@gun.voxnum);
}