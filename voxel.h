#ifndef VOXEL_H_INCLUDED
#define VOXEL_H_INCLUDED

#include <vector>
#include "algebra3.h"
#include "glui_internal.h"
#include "zomedir.h"
#include "glm.h"

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
//    int coord_id;
    bool show = true;
    std::vector<int> coord;
    std::vector<vec3> coord_origin;
};

void assign_coord(voxel &judge, vec3 &origin);
int assign_coord_point(vec3 &p, vec3 &origin);
vec2 check_bound(std::vector<voxel> &all_voxel, int max_d);
void addexist_toward(std::vector<voxel> &all_voxel, voxel & check);
void oct_tree(std::vector<voxel> &all_voxel, int start, int end, int depth, vec3 origin, int coord_id);
void voxelization(GLMmodel *model, std::vector<voxel> &all_voxel, vec3 &bounding_max, vec3 &bounding_min, vec3 &bound_center, int v_color, float v_size);

#endif // VOXEL_H_INCLUDED
