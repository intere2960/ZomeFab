#include <assert.h>
#include <stdlib.h>

#ifndef __VKFUNCTIONS_H
#define __VKFUNCTIONS_H

#define vkMax(a,b) ((a)>(b)?(a):(b))

#define vkMin(a,b) ((a)<(b)?(a):(b))

#define vkAbs(a) ((a)<(0)?(-(a)):(a))

#define vkRandDbl(a) ((double)a*((double)rand()/(double)RAND_MAX))

#define vkRandInt(a) (rand()%((int)a))

#define vkPI 3.14159265359

#endif


