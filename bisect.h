#ifndef BISECT_H_INCLUDED
#define BISECT_H_INCLUDED

#include <algorithm>

#include <iostream>
using namespace std;

enum split_state { _none = -1, _align = 0, _split = 1 };

class edge
{
public:
    edge();
    edge(int i1, vec3 &p1, int i2, vec3 &p2);
    float length();
    friend bool operator==(const edge &lhs,const edge &rhs);

    vec3 point[2];
    int index[2];
    int face_id[2];
    vec3 split_point;
    int vertex_push_index;
    int connect_index;
};

class plane
{
public:
    plane();
    plane(float a, float b, float c, float d,int e);
    float plane_par[4]; // ax+by+cz=d  e->cut dir
    int dir;
};

int plane_dir_point(vec3 &point, plane plane);
void plane_dir_edge(edge &temp, plane plane, int dir[2]);
void plane_dist_edge(edge &temp, plane plane, float dist[2]);
void cut_intersection(GLMmodel *myObj, std::vector<plane> planes, std::vector<int> &face_split_by_plane, bool have_dir);
bool split_edge(GLMmodel *myObj, std::vector<edge> &all_edge, int split_tri_id, plane plane);
void split_face(GLMmodel *myObj, std::vector<edge> &all_edge,std::vector<int> &face_split_by_plane, std::vector<plane> &planes);
void tri_poly(GLMmodel *myObj, std::vector<edge> &all_edge, int face_id, edge &splited_edge1, edge &splited_edge2);
void collect_edge(GLMmodel *myObj, std::vector<edge> &all_edge);
void find_loop(GLMmodel *myObj, std::vector<edge> &all_edge, std::vector<plane> &planes);
void process_piece(GLMmodel &temp_piece, GLMmodel *myObj, std::vector<int> &face_split_by_plane);

#endif // BISECT_H_INCLUDED
