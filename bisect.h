#ifndef BISECT_H_INCLUDED
#define BISECT_H_INCLUDED

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

float plane_dir_point(vec3 &point, float plane[4]);
void plane_dir(edge &temp, float plane[4], int dir[2]);
void plane_dist(edge &temp, float plane[4], float dist[2]);
void split_all_edge(GLMmodel *myObj, std::vector<edge> &all_edge,std::vector<bool> &is_face_split, float plane[4]);
void split_face(GLMmodel *myObj, std::vector<edge> &all_edge,std::vector<bool> &is_face_split, float plane[4]);

#endif // BISECT_H_INCLUDED
