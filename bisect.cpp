
#include "bisect.h"

edge::edge()
{
    vec3 zero(0.0, 0.0, 0.0);
    point[0] = zero;
    point[1] = zero;
    index[0] = 0;
    index[1] = 0;
    face_id[0] = -1;
    face_id[1] = -1;
    is_split = false;
    l = length();
}

edge::edge(int i1, vec3 &p1, int i2, vec3 &p2)
{
    index[0] = i1;
    point[0] = p1;
    index[1] = i2;
    point[1] = p2;
    face_id[0] = -1;
    face_id[1] = -1;
    is_split = false;
    l = length();
}

float edge::length()
{
    vec3 temp = point[1] - point[0];
    return temp.length();
}
