#ifndef GRAPHCUT_H_INCLUDED
#define GRAPHCUT_H_INCLUDED

#include <string>
#include "MRFOptimization.h"
#include "MRFProblems.h"
#include "MRFGraphCut.h"
#include "glm.h"
#include "zomestruc.h"
#include "bisect.h"
#include "global.h"

double AverageRadius(GLMmodel *model, vec3 &centroid);
void test_graph_cut(GLMmodel *model, std::vector<std::vector<zomeconn>> &test_connect, std::vector<simple_material> &materials, float color_material[20][3], std::vector<std::vector<int>>& vertex_color, float wedge, float wnode, float label, float salient);
int have_same_material(int now_material, int p1, int p2, std::vector<std::vector<int>> &vertex_color);
void graph_cut(GLMmodel *model, std::string &model_file, std::string &zome_file, float total_num, float wedge, float wnode, float label, float salient);
void fake_saliency(GLMmodel *model, std::string &model_file);
void saliency_texture(GLMmodel *model, std::string &model_file);

#endif // GRAPHCUT_H_INCLUDED