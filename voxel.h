#ifndef VOXEL_H_INCLUDED
#define VOXEL_H_INCLUDED

#include <vector>
#include "algebra3.h"

class voxel
{
public:
    voxel(int t_color, int t_size, float t_scale);
    voxel(int t_color, int t_size, float t_scale, vec3 t_position, vec3 t_rotation);
    voxel(voxel t, vec3 t_position, vec3 t_rotation);

    vec3 position;
    vec3 rotation;
    int color;
    int stick_size;
    float scale;
    int face_toward[6] = {-1, -1, -1, -1, -1, -1};
    std::vector<vec3> toward_vector;
};

vec2 check_bound(std::vector<voxel> &all_voxel, vec3 bounding_max, vec3 bounding_min);
void addexist_toward(std::vector<voxel> &all_voxel, voxel & check);
void voxelization(std::vector<voxel> &all_voxel, vec3 &bounding_max, vec3 &bounding_min, int v_color, float v_size);

//class all_voxel
//{
//    std::vector<voxel> v;
//};

#endif // VOXEL_H_INCLUDED