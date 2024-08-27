#include "vxl.h"
#include "general.h"
#include "world.h"
#include "config.h"

#include <math.h>
#include <random>

// max() macro

voxlap::lpoint3d aos_vxl_cube_array[CUBE_ARRAY_LENGTH];

size_t aos_vxl_cube_line(long x1, long y1, long z1, long x2, long y2, long z2,
	voxlap::lpoint3d * cube_array)
{
	voxlap::lpoint3d c, d;
	long ixi, iyi, izi, dx, dy, dz, dxi, dyi, dzi;
	size_t count = 0;

	//Note: positions MUST be rounded towards -inf
	c.x = x1;
	c.y = y1;
	c.z = z1;

	d.x = x2 - x1;
	d.y = y2 - y1;
	d.z = z2 - z1;

	if (d.x < 0) ixi = -1;
	else ixi = 1;
	if (d.y < 0) iyi = -1;
	else iyi = 1;
	if (d.z < 0) izi = -1;
	else izi = 1;

	if ((abs(d.x) >= abs(d.y)) && (abs(d.x) >= abs(d.z))) {
		dxi = 1024; dx = 512;
		dyi = (long)(!d.y ? 0x3fffffff / VSID : abs(d.x * 1024 / d.y));
		dy = dyi / 2;
		dzi = (long)(!d.z ? 0x3fffffff / VSID : abs(d.x * 1024 / d.z));
		dz = dzi / 2;
	} else if (abs(d.y) >= abs(d.z)) {
		dyi = 1024; dy = 512;
		dxi = (long)(!d.x ? 0x3fffffff / VSID : abs(d.y * 1024 / d.x));
		dx = dxi / 2;
		dzi = (long)(!d.z ? 0x3fffffff / VSID : abs(d.y * 1024 / d.z));
		dz = dzi / 2;
	} else {
		dzi = 1024; dz = 512;
		dxi = (long)(!d.x ? 0x3fffffff / VSID : abs(d.z * 1024 / d.x));
		dx = dxi / 2;
		dyi = (long)(!d.y ? 0x3fffffff / VSID : abs(d.z * 1024 / d.y));
		dy = dyi / 2;
	}
	if (ixi >= 0) dx = dxi - dx;
	if (iyi >= 0) dy = dyi - dy;
	if (izi >= 0) dz = dzi - dz;

	while (1) {
		cube_array[count] = c;

		if (count++ == CUBE_ARRAY_LENGTH)
			return count;

		if (c.x == x2 &&
			c.y == y2 &&
			c.z == z2)
			return count;

		if ((dz <= dx) && (dz <= dy)) {
			c.z += izi;
			if (c.z < 0 || c.z >= MAXZDIM)
				return count;
			dz += dzi;
		} else {
			if (dx < dy) {
				c.x += ixi;
				if ((unsigned long)c.x >= VSID)
					return count;
				dx += dxi;
			} else {
				c.y += iyi;
				if ((unsigned long)c.y >= VSID)
					return count;
				dy += dyi;
			}
		}
	}
}

long aos_vxl_clipbox(float x, float y, float z)
{
	long sz;

	if (x < 0 || x >= 512 || y < 0 || y >= 512)
		return 1;
	if (z < 0)
		return 0;
	sz = (long)z;
	if (sz == 63)
		sz = 62;
	else if (sz >= 63)
		return 1;
	else if (sz < 0)
		return 0;
	return voxlap::isvoxelsolid(x, y, sz);
}

long aos_vxl_clipbox_spectator(float x, float y, float z)
{
	long sz;

	if (x < 0 || x >= 512 || y < 0 || y >= 512 || z < -64 || z >= 63)
		return 1;
	sz = (long)z;
	if (sz >= 63)
		return 1;
	return voxlap::isvoxelsolid(x, y, sz);
}

static long groucol[9] = { 0x506050, 0x605848, 0x705040, 0x804838, 0x704030, 0x603828, 0x503020, 0x402818, 0x302010};

long aos_setcube_colfunc(voxlap::lpoint3d *p)
{
	long i, j;
	j = groucol[(p->z >> 5) + 1]; i = groucol[p->z >> 5];
	i = ((((((j & 0xff00ff) - (i & 0xff00ff))*(p->z & 31)) >> 5) + i) & 0xff00ff) +
		((((((j & 0x00ff00) - (i & 0x00ff00))*(p->z & 31)) >> 5) + i) & 0x00ff00);
	i += (labs((p->x & 8) - 4) << 16) + (labs((p->y & 8) - 4) << 8) + labs((p->z & 8) - 4);
	j = rand(); i += (j & 7) + ((j & 0x70) << 4) + ((j & 0x700) << 8);
	return(i + 0x80000000);
}

void aos_setcube(long x, long y, long z, long col)
{
	if (col != -1) {
		col = 0x80000000 | col;
		if (!voxlap::isvoxelsolid(x, y, z)) {
			voxlap::vx5.curcol = col;
			voxlap::vx5.colfunc = voxlap::jitcolfunc;
			voxlap::setcube(x, y, z, -2);
		} else {
			voxlap::vx5.fallcheck = 0;
			voxlap::setcube(x, y, z, col);
			voxlap::vx5.fallcheck = 1;
		}
	} else {
		voxlap::vx5.curcol = DEFAULT_COLOR;
		//voxlap::vx5.colfunc = voxlap::manycolfunc;
		voxlap::vx5.colfunc = aos_setcube_colfunc;
		voxlap::setcube(x, y, z, -1);
	}
}

#define AOS_LIGHTING_SCANRAD   64
#define AOS_LIGHTING_SCANRATE  0.01
#define AOS_LIGHTING_SCANDELAY 0.33

static long origin_x;
static long origin_y;

void aos_vxl_lighting_updateblock(long x, long y, long z)
{
	long block_ptr = voxlap::getcube(x, y, z); // returns long* to color of block at x,y,z
	if (block_ptr == 0 || block_ptr == 1) // voxlib.txt: 0:air, 1:unexposed
		return;
	long* block_color = (long *)(block_ptr);   

	if (!config::shadows) {
		*block_color = (*block_color & 0x00FFFFFF) | 0x80 << 24;
		return;
	}

	unsigned char shadow = 0;

	// bad variable name but there's no better one, really
	// c is the index of the first block encountered by the loop
	// effectively giving the space between the 'selected' block and the first block casting a shadow on it

	long c = 0;
	for (long i = -1; i > -10; i--) {
		long ly = y + i;
		long lz = z + i;
		if (ly < 0 || lz < 0)
			break;
		if (voxlap::isvoxelsolid(x, ly, lz)) {
			if (!c) {
				c = i;
			}
			shadow += 6;
		}
	}

	// decrease shadow by amount of air

	for (long i = -1; i > c; i--) {
		shadow = (unsigned char)(shadow * 0.90);
	}
								   // voxlib.txt: 0x80 == no effect
	*block_color = (*block_color & 0x00FFFFFF) | (0x80 - shadow) << 24; // voxlib.txt: &0xFF000000; 4th byte == lighting 
}

void aos_lighting_updateline(long oy, long rad)
{
	long y = origin_y + oy;
	if (y < 0 || y >= 512)
		return;
	for (long ox = -rad; ox <= rad; ox++) {
		if (ox*ox + oy*oy > rad*rad)
			continue;
		long x = origin_x + ox;
		if (x < 0 || x >= 512)
			continue;
		for (long z = 0; z < 64; z++) {
			aos_vxl_lighting_updateblock(x, y, z);
		}
	}
}

extern double dtotclk; // program uptime in seconds
static double lastscan = 0;
static long lastscan_y = AOS_LIGHTING_SCANRAD;

void aos_vxl_updatelighting(voxlap::point3d *origin) // 3d point, center of lighting
{
	double tickdiff = dtotclk - lastscan;
	if (tickdiff > AOS_LIGHTING_SCANRATE) {
		lastscan = dtotclk;

		if (lastscan_y <= AOS_LIGHTING_SCANRAD) {
			aos_lighting_updateline(lastscan_y, AOS_LIGHTING_SCANRAD);
			lastscan_y++;
		} else {
			lastscan = dtotclk + AOS_LIGHTING_SCANDELAY;
			lastscan_y = -AOS_LIGHTING_SCANRAD;
			origin_x = (long)origin->x;
			origin_y = (long)origin->y;
		}
	}
}

void aos_vxl_updatelighting_map()
{
	for (long x = 0; x <= 512; x++) {
		for (long y = 0; y <= 512; y++) {
			for (long z = 0; z < 64; z++) {
				aos_vxl_lighting_updateblock(x, y, z);
			}
		}
	}
}

size_t aos_vxl_get_neighbors(long x, long y, long z)
{
	voxlap::lpoint3d *point = NULL;
	size_t i = 0;

	if (voxlap::isvoxelsolid(x, y, z - 1)) {
		point = &aos_vxl_cube_array[i++];
		point->x = x; point->y = y; point->z = z - 1;
	}

	if (voxlap::isvoxelsolid(x, y - 1, z)) {
		point = &aos_vxl_cube_array[i++];
		point->x = x; point->y = y - 1; point->z = z;
	}

	if (voxlap::isvoxelsolid(x, y + 1, z)) {
		point = &aos_vxl_cube_array[i++];
		point->x = x; point->y = y + 1; point->z = z;
	}

	if (voxlap::isvoxelsolid(x - 1, y, z)) {
		point = &aos_vxl_cube_array[i++];
		point->x = x - 1; point->y = y; point->z = z;
	}

	if (voxlap::isvoxelsolid(x + 1, y, z)) {
		point = &aos_vxl_cube_array[i++];
		point->x = x + 1; point->y = y; point->z = z;
	}

	if (voxlap::isvoxelsolid(x, y, z + 1)) {
		point = &aos_vxl_cube_array[i++];
		point->x = x; point->y = y; point->z = z + 1;
	}

	return i;
}

void aos_vxl_set_point(long x, long y, long z, long col)
{
	if (x < 0 || x > 511 ||
		y < 0 || y > 511 ||
		z < 0 || z > 63)
		return;
	if (col < 0) return;
	aos_setcube(x, y, z, col);
}

void aos_vxl_destroy_point(long x, long y, long z)
{
	if (x < 0 || x > 511 ||
		y < 0 || y > 511 ||
		z < 0 || z >= 62)
		return;
	if (!voxlap::isvoxelsolid(x, y, z))
		return;
	aos_setcube(x, y, z, -1);
	aos_world_block_damage_clear(x, y, z);
	size_t num = aos_vxl_get_neighbors(x, y, z);
	for (int i = 0; i < num; i++) {
		voxlap::lpoint3d *pos = &aos_vxl_cube_array[i];
		aos_vxl_lighting_updateblock(pos->x, pos->y, pos->z);
	}
}

void aos_vxl_update_damaged_color()
{
	blockdamage *dmg;
	long x, y, z;

	for (int i = 0; i < aos_world_damagenum; i++) {
		dmg = &aos_world_damage[i];
		aos_world_damage_xyz(&x, &y, &z, dmg->i);
		aos_vxl_lighting_updateblock(x, y, z);

		long block_ptr = voxlap::getcube(x, y, z);
		if (block_ptr == 0 || block_ptr == 1) // voxlib.txt: 0:air, 1:unexposed
			continue;						  // 0 should never happen, but might aswell do it anyway.
		
		long *block_clr = (long*)block_ptr;
		aos_ubyte alpha = (*block_clr >> 24) & 0xFF;

		alpha -= 10 * dmg->damage;
		if (alpha < 16) alpha = 16;

		*block_clr = (*block_clr & 0x00FFFFFF) | (alpha & 0xFF) << 24;
	}
}