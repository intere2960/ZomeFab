#ifndef BISECT_H_INCLUDED
#define BISECT_H_INCLUDED

#include <iostream>

class edge
{
public:
    edge();
    edge(int i1, vec3 &p1, int i2, vec3 &p2);
    float length();

    vec3 point[2];
    int index[2];
    int face_id[2];
    float l;
    vec3 split_point;
    int vertex_push_index;
};

class plane
{
public:
    plane();
    plane(float a, float b, float c, float d,int e);
    float plane_par[4]; // ax+by+cz=d  e->cut dir
    int dir;
};

float plane_dir_point(vec3 &point, plane plane);
void plane_dir_edge(edge &temp, plane plane, int dir[2]);
void plane_dist_edge(edge &temp, plane plane, float dist[2]);
void split_all_edge(GLMmodel *myObj, std::vector<edge> &all_edge,std::vector<bool> &is_face_split, plane plane);
bool split_edge_test(GLMmodel *myObj, std::vector<edge> &all_edge,std::vector<bool> &is_face_split,int split_tri_id, plane plane);
void split_face(GLMmodel *myObj, std::vector<edge> &all_edge,std::vector<bool> &is_face_split, plane plane);
void split_face_test(GLMmodel *myObj, std::vector<edge> &all_edge,std::vector<bool> &is_face_split,std::vector<int> &face_split_by_plane, std::vector<plane> &planes);

#endif // BISECT_H_INCLUDED
