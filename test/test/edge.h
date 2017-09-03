#ifndef EDGE_H_INCLUDED
#define EDGE_H_INCLUDED

#include <vector>
#include "glm.h"
#include "algebra3.h"

class edge
{
public:
	edge();
	edge(int i1, vec3 &p1, int i2, vec3 &p2);
	float length();
	friend bool operator==(const edge &lhs, const edge &rhs);

	vec3 point[2];
	int index[2];
	int face_id[2];
	vec3 split_point;
	int vertex_push_index;
	int connect_index;
};

void collect_edge(GLMmodel *model, std::vector<edge> &all_edge);
void find_near_tri(GLMmodel *model, std::vector<edge> &all_edge);
void inform_vertex(GLMmodel *model, std::vector<edge> &all_edge);

#endif // EDGE_H_INCLUDED