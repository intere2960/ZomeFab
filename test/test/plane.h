#ifndef PLANE_H_INCLUDED
#define PLANE_H_INCLUDED

#include <vector>
#include <string>
#include "algebra3.h"
#include "edge.h"

class plane
{
public:
	plane();
	plane(float a, float b, float c, float d, int e);
	
	vec4 plane_par; // ax+by+cz=d  e->cut dir
	int dir;
	vec3 mid_point;
	float length;
};

class line
{
public:
	vec3 dir;
	vec3 start_point;

	std::vector<unsigned int> use_plane;
	std::vector<vec3> outer_insection; //face_id, ray_t, inside_initial
	std::vector<vec3> inner_insection;
};

void plane_parser_SVM(std::vector<std::vector<plane>> &all_planes, std::string &file_plane);
void output_plane_SVM(std::vector<std::vector<plane>> &all_planes, std::string &model_file, std::string &file_neighbor);
float plane_dir_point(vec3 &point, plane &plane);
float plane_dist_point(vec3 &point, plane &plane);
void plane_dir_edge(edge &temp, plane &plane, int dir[2]);
void plane_dist_edge(edge &temp, plane &plane, float dist[2]);
bool plane_dir_face(GLMmodel *model, plane &plane, int face_id);
vec3 point_project_plane(vec3 &point, plane &plane);
void point_set_project_plane(std::vector<vec3> &point_set, plane &plane);

#endif // PLANE_H_INCLUDED