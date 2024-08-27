point3d ipos, istr, ihei, ifor;

vx5sprite gun;

const float PI = 3.14159265f;

bool initapp()
{
    xres = 640; yres = 480; colbits = 32; fullscreen = 0;
    loadvxl("vxl/harbor2.vxl");
    
    @gun.voxnum = loadkv6("kv6/gun.kv6");
    gun.p.set(256, 256, -32);
    gun.s.set(-1, 0, 0);
    gun.h.set(0,-1, 0);
    gun.f.set(0, 0, 1);
    gun.kfatim = 0; gun.okfatim = 0; gun.flags = 0;
    
    ipos.set(230, 256, -32);
    ifor.set(1, 0, 0);
    istr.set(0, 1, 0);
    ihei.set(0, 0, 1);
    
    return false;
}

void doframe(double fsynctics)
{
    readkeyboard();
    if (get_keystatus(0x01) != 0)
        quitloop();
        
    axisrotate(gun.s, gun.s, gun.f, -fsynctics);
    axisrotate(gun.h, gun.h, gun.f, -fsynctics);

    int framebuf, pitch, xdim, ydim;
    startdirectdraw(framebuf, pitch, xdim, ydim);
    voxsetframebuffer(framebuf, pitch, xdim, ydim);
    
    setcamera(ipos,istr,ihei,ifor,xdim*.5f,ydim*.5f,xdim*.5f);
    opticast();
    drawsprite(gun);
    
    stopdirectdraw();
    nextpage();
}

void uninitapp()
{
    freekv6(gun.voxnum);
}