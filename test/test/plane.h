#ifndef PLANE_H_INCLUDED
#define PLANE_H_INCLUDED

#include <vector>
#include "algebra3.h"
#include "edge.h"

class plane
{
public:
	plane();
	plane(float a, float b, float c, float d, int e);
	vec4 plane_par; // ax+by+cz=d  e->cut dir
	int dir;
};

void plane_parser(std::vector<std::vector<plane>> &all_planes);
float plane_dir_point(vec3 &point, plane plane);
void plane_dir_edge(edge &temp, plane plane, int dir[2]);
void plane_dist_edge(edge &temp, plane plane, float dist[2]);

#endif // PLANE_H_INCLUDED