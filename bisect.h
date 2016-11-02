#ifndef BISECT_H_INCLUDED
#define BISECT_H_INCLUDED

#include "glm.h"
#include "algebra3.h"
#include "glui_internal.h"

class edge
{
public:
    edge();
    edge(int i1, vec3 &p1, int i2, vec3 &p2);
    float length();

    vec3 point[2];
    int index[2];
    float l;
};

#endif // BISECT_H_INCLUDED
