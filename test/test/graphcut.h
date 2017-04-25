#ifndef GRAPHCUT_H_INCLUDED
#define GRAPHCUT_H_INCLUDED

#include "MRFOptimization.h"
#include "MRFProblems.h"
#include "MRFGraphCut.h"
#include "glm.h"
#include "zomestruc.h"
#include "global.h"

double AverageRadius(GLMmodel *model, vec3 &centroid);
void test_graph_cut(GLMmodel *model, std::vector<std::vector<zomeconn>> &test_connect, std::vector<simple_material> &materials, float color_material[20][3], std::vector<std::vector<int>>& vertex_color);

#endif // GRAPHCUT_H_INCLUDED