#ifndef BISECT_H_INCLUDED
#define BISECT_H_INCLUDED

#include <vector>
#include "algebra3.h"
#include "edge.h"
#include "plane.h"

void cut_intersection(GLMmodel *model, std::vector<plane> planes, std::vector<int> &face_split_by_plane, bool have_dir);
bool split_edge(GLMmodel *model, std::vector<edge> &all_edge, int split_tri_id, plane plane);
void split_face(GLMmodel *model, std::vector<edge> &all_edge,std::vector<int> &face_split_by_plane, std::vector<plane> &planes);
void tri_poly(GLMmodel *model, std::vector<edge> &all_edge, int face_id, edge &splited_edge1, edge &splited_edge2);
void find_loop(GLMmodel *model, std::vector<edge> &all_edge, std::vector<plane> &planes);
void find_easy_loop(GLMmodel *model, std::vector<edge> &all_edge, std::vector<int> &single_use);
void find_shell_loop(GLMmodel *model, std::vector<edge> &all_edge, std::vector<plane> &planes);
void process_piece(GLMmodel &temp_piece, GLMmodel *model, std::vector<int> &face_split_by_plane);
bool shell_valid(GLMmodel *model, std::vector<int> face_split_by_plane);
bool split_valid(GLMmodel *model);

#endif // BISECT_H_INCLUDED
