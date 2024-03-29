#ifndef GLOBAL_H_INCLUDED
#define GLOBAL_H_INCLUDED

#define SIZE_S 0
#define SIZE_M 1
#define SIZE_L 2

#define COLOR_BLUE 0
#define COLOR_RED 1
#define COLOR_YELLOW 2
#define COLOR_WHITE 3

#define NODE_DIAMETER 17.7f
#define LATCH 4.0f
#define SCALE 47.3f //46.3 => 47.3
#define GOLDEN (1.0f + sqrt(5.0f)) / 2.0f

#define WEIGHT_BLUE_S 1.7f
#define WEIGHT_BLUE_M 3.0f
#define WEIGHT_BLUE_L 4.9f
#define WEIGHT_RED_S 1.8f
#define WEIGHT_RED_M 3.1f
#define WEIGHT_RED_L 5.1f
#define WEIGHT_YELLOW_S 1.7f
#define WEIGHT_YELLOW_M 2.9f
#define WEIGHT_YELLOW_L 4.8f
#define WEIGHT_WHITE 3.0f

#define ERROR_THICKNESS 3.0f

#define SPLITE 0
#define MERGE 1

#define NORMAL 0
#define COLORFUL 1
#define ENERGY_DIST 2
#define ENERGY_ANGLE 3
#define ENERGY_USE_STICK 4
#define GRAPH_CUT 5
#define SAILENCY 6

#define OUTER 0
#define INNER 1
#define MIDDLE 2

#define VOXELIZATION 0
#define SA 1
#define FAKE_SALIENCY 2
#define GRAPH_CUT_PROCESS 3
#define INNER_SHELL 4
#define CUT_MESH 5
#define OUTPUT_ZOME 6

#endif