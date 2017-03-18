#ifndef FILL_H_INCLUDED
#define FILL_H_INCLUDED

#include "glm.h"

enum point_type { _concave = -1, _tangential = 0, _convex = 1 };

void convert_2d(GLMmodel &temp_piece, Loop &loop);
int span_tri(Loop &loop, int index1, int index2, int index3, bool two_d);
void judge_inverse(Loop &loop);
void fill_prepare(GLMmodel &temp_piece, bool two_d);
bool check_ear_index(Loop &loop, int current, bool two_d);
int find_ear_index(Loop &loop, int current, bool two_d);
void triangulate(Loop &loop, bool two_d);
void fill_hole(GLMmodel &temp_piece, bool two_d);

#endif // FILL_H_INCLUDED
