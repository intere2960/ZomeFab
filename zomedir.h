#ifndef ZOMEDIR_H_INCLUDED
#define ZOMEDIR_H_INCLUDED

#define SIZE_S 0
#define SIZE_M 1
#define SIZE_L 2

#define COLOR_BLUE 0
#define COLOR_RED 1
#define COLOR_YELLOW 2
#define COLOR_WHITE 3

#define NODE_DIAMETER 17.7
#define LATCH 4.0
#define SCALE 47.3 //46.3 => 47.3
#define GOLDEN (1.0 + sqrt(5.0)) / 2

#define WEIGHT_BLUE_S 1.7
#define WEIGHT_BLUE_M 3.0
#define WEIGHT_BLUE_L 4.9
#define WEIGHT_RED_S 1.8
#define WEIGHT_RED_M 3.1
#define WEIGHT_RED_L 5.1
#define WEIGHT_YELLOW_S 1.7
#define WEIGHT_YELLOW_M 2.9
#define WEIGHT_YELLOW_L 4.8
#define WEIGHT_WHITE 3.0

#include <vector>
#include <cmath>
#include <GL/glut.h>
#include "algebra3.h"

enum color
{
	_blue, _red, _yellow
};

const double golden((1.0 + sqrt(5.0)) / 2);

class zomedir
{
public:
	zomedir();
	~zomedir();
	GLfloat theta(int index);
	GLfloat phi(int index);
	GLfloat roll(int index);

	std::vector<vec3> *dir;

	GLfloat face_length(int index, int size);
	GLfloat color_length(int index, int size);
	int face_color(int index);
	int opposite_face(int index);

	int dir_face(vec3 &fdir);
	int dir_face(vec3 &vecInitial, vec3 &vecEnd, int size);

	float dir_parameter(int i, int d); //get 'i'th dir[d]
	float weight(int color, int size); //get weight of different color by size
};

#endif // ZOMEDIR_H_INCLUDED
