#include <fstream>
#include "plane.h"

using namespace std;

plane::plane(){
	plane_par[0] = 0.0;
	plane_par[1] = 0.0;
	plane_par[2] = 0.0;
	plane_par[3] = 0.0;
	dir = 0;
}

plane::plane(float a, float b, float c, float d, int e){
	plane_par[0] = a;
	plane_par[1] = b;
	plane_par[2] = c;
	plane_par[3] = d;
	dir = e;
}

void plane_parser(std::vector<std::vector<plane>> &all_planes){
	ifstream is("training_plane.txt");
	int part, num_plane, plane_dir;
	float plane_info[4];

	is >> part;
	all_planes.resize(part);
	for (int i = 0; i < part; i += 1){
		is >> num_plane;
		for (int j = 0; j < num_plane; j += 1){
			is >> plane_info[0] >> plane_info[1] >> plane_info[2] >> plane_info[3] >> plane_dir;
			plane temp(plane_info[0], plane_info[1], plane_info[2], plane_info[3], plane_dir);
			all_planes.at(i).push_back(temp);
		}
	}

	is.close();
}

float plane_dir_point(vec3 &point, plane plane) //have problem
{
	float judge = plane.plane_par[0] * point[0] + plane.plane_par[1] * point[1] + plane.plane_par[2] * point[2] - plane.plane_par[3];
	//cout << judge << endl;

	if (judge > 0.001f)
		judge = 1;
	else if (judge < -0.001f)
		judge = -1;
	else
		judge = 0;

	return judge;
}

void plane_dir_edge(edge &temp, plane plane, int dir[2])
{
	dir[0] = (int)plane_dir_point(temp.point[0], plane);
	dir[1] = (int)plane_dir_point(temp.point[1], plane);
}

void plane_dist_edge(edge &temp, plane plane, float dist[2])
{
	float judge1 = plane.plane_par[0] * temp.point[0][0] + plane.plane_par[1] * temp.point[0][1] + plane.plane_par[2] * temp.point[0][2] - plane.plane_par[3];
	float judge2 = plane.plane_par[0] * temp.point[1][0] + plane.plane_par[1] * temp.point[1][1] + plane.plane_par[2] * temp.point[1][2] - plane.plane_par[3];

	dist[0] = fabs(judge1) / sqrt(pow(plane.plane_par[0], 2) + pow(plane.plane_par[1], 2) + pow(plane.plane_par[2], 2));
	dist[1] = fabs(judge2) / sqrt(pow(plane.plane_par[0], 2) + pow(plane.plane_par[1], 2) + pow(plane.plane_par[2], 2));
}