#ifndef BISECT_H_INCLUDED
#define BISECT_H_INCLUDED

#include <vector>
#include <string>
#include "algebra3.h"
#include "edge.h"
#include "plane.h"

void recount_normal(GLMmodel *myObj);
void combine_inner_outfit(GLMmodel *myObj, GLMmodel *myObj_inner);
void vertex_use_tri(GLMmodel *model);
void cut_intersection(GLMmodel *model, std::vector<plane> &planes, std::vector<int> &face_split_by_plane, int piece_id, bool have_dir);
void hole_split(GLMmodel *model, std::vector<edge> &all_edge, std::vector<plane> &planes, std::vector<std::vector<int>> &arrange_group);
void collect_vanish_face(GLMmodel *model, std::vector<edge> &all_edge, std::vector<plane> &planes, std::vector<int> &face_split_by_plane, std::vector<std::vector<int>> &arrange_group, int piece_id);
bool split_edge(GLMmodel *model, std::vector<edge> &all_edge, int split_tri_id, plane plane);
void split_face(GLMmodel *model, std::vector<edge> &all_edge,std::vector<int> &face_split_by_plane, std::vector<plane> &planes);
void tri_poly(GLMmodel *model, std::vector<edge> &all_edge, int face_id, edge &splited_edge1, edge &splited_edge2);
void find_easy_loop(GLMmodel *model, std::vector<edge> &all_edge, std::vector<int> &single_use);
void find_shell_loop(GLMmodel *model, std::vector<edge> &all_edge, std::vector<plane> &planes);
void process_piece(GLMmodel &temp_piece, GLMmodel *model, std::vector<int> &face_split_by_plane);
void generate_fake_inner(GLMmodel &temp_piece, std::vector<plane> &planes, std::vector<edge> &all_edge);
void cut_mesh(GLMmodel *model, std::string &model_file, std::string &shell_file, std::string &zome_file, int plane_tune);

#endif // BISECT_H_INCLUDED
