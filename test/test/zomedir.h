#ifndef ZOMEDIR_H_INCLUDED
#define ZOMEDIR_H_INCLUDED

#include <vector>
#include "algebra3.h"

const double golden((1.0 + sqrt(5.0)) / 2);

class zomedir
{
public:
	zomedir();
	~zomedir();
	float theta(int index);
	float phi(int index);
	float roll(int index);

	std::vector<vec3> *dir;
	std::vector<std::vector<std::vector<int>>> near_dir;
	std::vector<std::vector<float>> near_angle;

	float face_length(int index, int size);
	float color_length(int color, int size);
	int face_color(int index);
	int opposite_face(int index);

	int dir_face(vec3 &fdir);
	int dir_face(vec3 &vecInitial, vec3 &vecEnd, int size);
	int find_near_dir(vec3 input);
	void find_best_zome(vec3 start, vec3 end, int near_index, int &choose_size, int &choose_dir);

	float dir_parameter(int i, int d); //get 'i'th dir[d]
	float weight(int color, int size); //get weight of different color by size
};

class zomerecord
{
public:
    vec3 origin;
    vec3 travel_1;
    vec3 travel_2;
};

//split:
//        *
//  (t1) ^ ^ (t2)
//      0-->0
//       (o)
//
//merge:
//        *
//  (t2) ^ ^ (t1)
//      0-->0
//       (o)

class zometable
{
public:
    zometable(int type);
    void find_zomevector(int type);
    void vector_parser(int type, std::ifstream &is);

    std::vector<std::vector<std::vector<zomerecord>>> table;
};

#endif // ZOMEDIR_H_INCLUDED
