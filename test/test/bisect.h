#ifndef BISECT_H_INCLUDED
#define BISECT_H_INCLUDED

#include <vector>
#include <string>
#include "algebra3.h"
#include "edge.h"
#include "plane.h"

class neighbor_record
{
public:
	neighbor_record(){}
	neighbor_record(std::vector<std::vector<plane>> &all_planes, std::string neighbor_file);

	std::vector<std::vector<int>> neighbor_info;
	std::vector<std::vector<int>> neighbor_table;
	std::vector<std::vector<std::vector<int>>> plane_neighbor;
	std::vector<vec3> piece_center;
};

bool plane_dir_face(GLMmodel *model, plane &plane, int face_id);
void vertex_use_tri(GLMmodel *model);
void cut_intersection(GLMmodel *model, std::vector<plane> &planes, std::vector<int> &face_split_by_plane, int piece_id, bool have_dir);
void process_convex(GLMmodel *model, std::vector<plane> &planes, std::vector<int> &face_split_by_plane, int piece_id, neighbor_record &n_info);
void hole_split(GLMmodel *model, std::vector<edge> &all_edge, std::vector<plane> &planes, std::vector<std::vector<int>> &arrange_group);
void collect_vanish_face(GLMmodel *model, std::vector<edge> &all_edge, std::vector<plane> &planes, std::vector<int> &face_split_by_plane, std::vector<std::vector<int>> &arrange_group, int piece_id);
bool split_edge(GLMmodel *model, std::vector<edge> &all_edge, int split_tri_id, plane plane);
void split_face(GLMmodel *model, std::vector<edge> &all_edge,std::vector<int> &face_split_by_plane, std::vector<plane> &planes);
void tri_poly(GLMmodel *model, std::vector<edge> &all_edge, int face_id, edge &splited_edge1, edge &splited_edge2);
void find_loop(GLMmodel *model, std::vector<edge> &all_edge, std::vector<plane> &planes);
void find_easy_loop(GLMmodel *model, std::vector<edge> &all_edge, std::vector<int> &single_use);
void find_shell_loop(GLMmodel *model, std::vector<edge> &all_edge, std::vector<plane> &planes);
void process_piece(GLMmodel &temp_piece, GLMmodel *model, std::vector<int> &face_split_by_plane);
void generate_fake_inner(GLMmodel &temp_piece, std::vector<plane> &planes, std::vector<edge> &all_edge);
bool shell_valid(GLMmodel *model, std::vector<int> &face_split_by_plane);
bool split_valid(GLMmodel *model);
void cut_mesh(GLMmodel *model, std::string &model_file, std::string &shell_file, std::string &zome_file, int plane_tune);

#endif // BISECT_H_INCLUDED
