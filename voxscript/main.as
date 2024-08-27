
point3d ipos, istr, ihei, ifor;

point3d soundpos;

float textpos = 20;

vx5sprite desklamp;

bool initapp()
{
	xres = 1280;
	yres = 720;
	fullscreen = 0;
	mousmoth = 0;

	loadvxl("vxl/harbor2.vxl");
	loadsky("toonsky.jpg");

	vx5.maxscandist = 256.0f;
	//vx5.fogcol = 0x80E8FF;
	vx5.lightmode = 1;
	setsideshades(0,64,32,32,16,48);

	ipos.set(128.0f, 256.0f, 16.0f);
	ifor.set(1.0f, 0.0f, 0.0f);
	istr.set(0.0f, 1.0f, 0.0f);
	ihei.set(0.0f, 0.0f, 1.0f);

	soundpos.set(250.0f, 256.0f, -32.0f);

	@desklamp.voxnum = loadkv6("kv6\\desklamp.kv6");
	desklamp.p.set(300, 256, -32);
	desklamp.s.set(.4, 0, 0);
	desklamp.h.set(0, .4, 0);
	desklamp.f.set(0, 0, .4);
	desklamp.kfatim = 0; desklamp.okfatim = 0; desklamp.flags = 0;

	return false;
}

const double pi = 3.141592653;

float distance(point3d &in p1, point3d &in p2)
{
	float dx = p2.x - p1.x;
	float dy = p2.y - p1.y;
	float dz = p2.z - p1.z;
	return sqrt(dx*dx + dy*dy + dz*dz);
}

void hitscan_side(lpoint3d &out p, int dir)
{
	switch (dir) {
	case 0:
		p.x--;
		break;
	case 1:
		p.x++;
		break;
		
	case 2:
		p.y--;
		break;
	case 3:
		p.y++;
		break;

	case 4:
		p.z--;
		break;
	case 5:
		p.z++;
		break;
	}
}

int oldbuttons;

void doframe(double fsynctics)
{
	readkeyboard();

	int key;
	do {
		key = keyread();
		if (key == 0) continue;

		uint8 scancode = (key & 0xFF00) >> 8;
		if (scancode == 0x14) {
			playsound("voxdata/wav/SHOOT.WAV", 50, sin(dtotclk / 2.0) + 1.2f, soundpos, 0);
			textpos += (sin(dtotclk / 2.0) + 1.2);
		}
	} while (key > 0);

	if (get_keystatus(0x01) != 0) quitloop();

	bool move_forward = keystatus[0x11] != 0;
	bool move_back = keystatus[0x1F] != 0;
	bool move_left = keystatus[0x1E] != 0;
	bool move_right = keystatus[0x20] != 0;
		
	point3d ivel, nipos;
	ivel.set(0, 0, 0);

	float speed = 32.0f * fsynctics;

	if (move_forward) {
		ivel.x += ifor.x*speed;
		ivel.y += ifor.y*speed;
		ivel.z += ifor.z*speed;
	} else if (move_back) {
		ivel.x -= ifor.x*speed;
		ivel.y -= ifor.y*speed;
		ivel.z -= ifor.z*speed;
	}
	if (move_right) {
		ivel.x += istr.x*speed;
		ivel.y += istr.y*speed;
		ivel.z += istr.z*speed;
	} else if (move_left) {
		ivel.x -= istr.x*speed;
		ivel.y -= istr.y*speed;
		ivel.z -= istr.z*speed;
	}

	clipmove(ipos, ipos, ivel, 1.5f);

	float fmousx, fmousy;
	int buttons;
	readmouse(fmousx, fmousy, buttons);

	orthorotate(istr.z*.1,
		fmousy*fsynctics*.016 * 4.0f,
		fmousx*fsynctics*.016 * 4.0f,
		istr, ihei, ifor, istr, ihei, ifor);

	lpoint3d hitpos; int hitcol; int hitsid;
	hitscan(ipos, ifor, hitpos, hitcol, hitsid);

	if (oldbuttons != buttons) {
		if (buttons & 1 == 1) {
			hitscan_side(hitpos, hitsid);
			vx5.curcol = hitscan_get_color(hitcol);
			setsphere(hitpos, 4, 0);
			playsound("voxdata/wav/SHOOT.WAV", 50, 1.2f, soundpos, 0);
		}
	}

	oldbuttons = buttons;

	int frameptr, pitch, xdim, ydim;
	startdirectdraw(frameptr, pitch, xdim, ydim);
	voxsetframebuffer(frameptr, pitch, xdim, ydim);

	double spintim = dtotclk / 4.0;

	setears3d(ipos, ifor, ihei);
	setcamera(ipos, istr, ihei, ifor, xdim*0.5, ydim*0.5, xdim*0.5);

	opticast();

	drawsprite(desklamp);

	print6x8(2, 2, 0x8D50AC, 0, "fps: " + int(1.0f / fsynctics));
	
	print6x8(textpos, 20, 0xFF0000, -1, "hello world");
	
	if (hitcol != 0) {
		print6x8(20, 30, hitscan_get_color(hitcol), 0, "X " + hitpos.x + ", Y " + hitpos.y + ", Z " + hitpos.z);
	}

	stopdirectdraw();
	nextpage();
}

void uninitapp()
{
	freekv6(desklamp.voxnum);
}