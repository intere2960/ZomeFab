
#include "bisect.h"

edge::edge()
{
    vec3 zero(0.0, 0.0, 0.0);
    point[0] = zero;
    point[1] = zero;
    index[0] = 0;
    index[1] = 0;
    l = length();
}

edge::edge(int i1, vec3 &p1, int i2, vec3 &p2)
{
    index[0] = i1;
    point[0] = p1;
    index[1] = i2;
    point[1] = p2;
    l = length();
}

void edge::plane_dir(float plane[4])
{
    float judge1 = plane[0] * point[0][0] + plane[1] * point[0][1] + plane[2] * point[0][2] - plane[3];
    float judge2 = plane[0] * point[1][0] + plane[1] * point[1][1] + plane[2] * point[1][2] - plane[3];

    if(judge1 > 0)
        dir[0] = 1;
    else if(judge1 == 0)
        dir[0] = 0;
    else
        dir[0] = -1;

    if(judge2 > 0)
        dir[1] = 1;
    else if(judge2 == 0)
        dir[1] = 0;
    else
        dir[1] = -1;
}

float edge::length()
{
    vec3 temp = point[1] - point[0];
    return temp.length();
}
