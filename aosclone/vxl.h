#pragma once

#include <voxlap\voxlap.h>

#define CUBE_ARRAY_LENGTH 64
extern voxlap::lpoint3d aos_vxl_cube_array[CUBE_ARRAY_LENGTH];

size_t aos_vxl_cube_line(long, long, long, long, long, long, voxlap::lpoint3d *);

long aos_vxl_clipbox(float, float, float);
long aos_vxl_clipbox_spectator(float, float, float);

#define DEFAULT_COLOR 0xFF674028
void aos_vxl_set_point(long, long, long, long);
void aos_vxl_destroy_point(long, long, long);
size_t aos_vxl_get_neighbors(long, long, long);

void aos_vxl_lighting_updateblock(long, long, long);
void aos_vxl_updatelighting(voxlap::point3d *);
void aos_vxl_updatelighting_map();

void aos_vxl_update_damaged_color();