#ifndef OPERATION_H_INCLUDED
#define OPERATION_H_INCLUDED

#include <vector>
#include "algebra3.h"
#include "glm.h"
#include "zomedir.h"
#include "zomestruc.h"

void split(std::vector<std::vector<zomeconn>> &test_connect, int s_index, GLMmodel *model, zometable &splite_table);
void check_merge(std::vector<std::vector<zomeconn>> &test_connect, std::vector<vec4> &can_merge, GLMmodel *model, zometable &merge_table);
void merge(std::vector<std::vector<zomeconn>> &test_connect, vec4 &merge_vector);
void check_bridge(std::vector<std::vector<zomeconn>> &test_connect, std::vector<vec4> &can_bridge, GLMmodel *model, zometable &merge_table);
void bridge(std::vector<std::vector<zomeconn>> &test_connect, vec4 &bridge_vector);
float forbidden_energy(float dist);
float compute_energy(std::vector<std::vector<zomeconn>> &test_connect, GLMmodel *model, float *term);

#endif // OPERATION_H_INCLUDED