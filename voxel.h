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

//class all_voxel
//{
//    std::vector<voxel> v;
//};

#endif // VOXEL_H_INCLUDED
