#ifndef GRAPHCUT_H_INCLUDED
#define GRAPHCUT_H_INCLUDED

#include "MRFOptimization.h"
#include "MRFProblems.h"
#include "MRFGraphCut.h"
#include "glm.h"
#include "zomestruc.h"
#include "bisect.h"
#include "global.h"

class cut_plane
{
public:
	plane split_plane;
	int use_vertex[2];
	float normal_percent[2];
};

double AverageRadius(GLMmodel *model, vec3 &centroid);
void test_graph_cut(GLMmodel *model, std::vector<std::vector<zomeconn>> &test_connect, std::vector<simple_material> &materials, float color_material[20][3], std::vector<std::vector<int>>& vertex_color);
int have_same_material(int now_material, int p1, int p2, std::vector<std::vector<int>> &vertex_color);
vec4 easy_plane(GLMmodel *model, int index1, int index2);
bool cut_plane_error(GLMmodel *model, std::vector<vec2> &partition_plane, std::vector<cut_plane> &plane_queue, std::vector<int> &material_point, std::vector<vec2> &error_info);
void alter_plane(GLMmodel *model, std::vector<std::vector<vec2>> &partition_plane, int piece_id, std::vector<cut_plane> &plane_queue, std::vector<std::vector<int>> &material_point, std::vector<vec2> &error_info);

#endif // GRAPHCUT_H_INCLUDED