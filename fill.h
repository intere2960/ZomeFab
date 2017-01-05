#ifndef FILL_H_INCLUDED
#define FILL_H_INCLUDED

enum point_type { _concave = -1, _tangential = 0, _convex = 1 };

void convert_2d(GLMmodel &temp_piece, Loop &loop);
int span_tri(Loop &loop, int index1, int index2, int index3);
void judge_inverse(Loop &loop);
void fill_prepare(GLMmodel &temp_piece);
bool check_ear_index(Loop &loop, int current);
int find_ear_index(Loop &loop, int current);
void triangulate(Loop &loop);
void fill_hole(GLMmodel &temp_piece);

#endif // FILL_H_INCLUDED
