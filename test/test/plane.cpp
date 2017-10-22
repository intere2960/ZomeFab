#include <fstream>
#include "plane.h"

#include <CGAL/Cartesian_d.h>
#include <iostream>
#include <cstdlib>
#include <CGAL/Random.h>
#include <CGAL/Min_sphere_annulus_d_traits_d.h>
#include <CGAL/Min_sphere_d.h>
typedef CGAL::Cartesian_d<double>              K;
typedef CGAL::Min_sphere_annulus_d_traits_d<K> Traits;
typedef CGAL::Min_sphere_d<Traits>             Min_sphere;
typedef K::Point_d                             Point;

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

void plane_parser_SVM(std::vector<std::vector<plane>> &all_planes, std::string &file_plane){
	ifstream is(file_plane);
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

void plane_parser_voronoi(std::vector<std::vector<plane>> &all_planes, std::string &file_plane){
	ifstream is(file_plane);
	int part, num_plane, plane_dir;
	float plane_info[4];

	is >> part;
	all_planes.resize(part);
	for (int i = 0; i < part; i += 1){
		is >> num_plane;
		for (int j = 0; j < num_plane; j += 1){
			is >> plane_info[0] >> plane_info[1] >> plane_info[2] >> plane_info[3] >> plane_dir;

			float length = plane_info[0] * plane_info[0] + plane_info[1] * plane_info[1] + plane_info[2] * plane_info[2];
			plane temp(plane_info[0] / length, plane_info[1] / length, plane_info[2] / length, plane_info[3] / length, plane_dir);
			is >> temp.mid_point[0] >> temp.mid_point[1] >> temp.mid_point[2] >> temp.length;
			all_planes.at(i).push_back(temp);
		}
	}

	is.close();
}

void output_plane_voronoi(std::vector<std::vector<plane>> &all_planes, std::string &file_tag)
{
	for (unsigned int i = 0; i < all_planes.size(); i += 1){
		GLMmodel *planes = NULL;
		for (unsigned int j = 0; j < all_planes.at(i).size(); j += 1){
			
			GLMmodel *temp_plane = glmReadOBJ("test_model/cube.obj");
			
			glmScale_y(temp_plane, all_planes.at(i).at(j).length / 2.0f);
			glmScale_z(temp_plane, all_planes.at(i).at(j).length / 2.0f);

			vec3 n_vector;
			n_vector[0] = all_planes.at(i).at(j).plane_par[0];
			n_vector[1] = all_planes.at(i).at(j).plane_par[1];
			n_vector[2] = all_planes.at(i).at(j).plane_par[2];
			float d = all_planes.at(i).at(j).plane_par[3];
			
			vec3 axis = vec3(1.0f, 0.0f, 0.0f) ^ n_vector;
			axis = axis.normalize();
			float dot_angle = vec3(1.0f, 0.0f, 0.0f) * n_vector;
			float angle = acos(dot_angle);
			
			mat4 R = rotation3Drad(axis, angle);

			for (unsigned int a = 1; a <= temp_plane->numvertices; a += 1) {
				vec3 temp(temp_plane->vertices->at(3 * a + 0), temp_plane->vertices->at(3 * a + 1), temp_plane->vertices->at(3 * a + 2));
				temp = R * temp;
				temp_plane->vertices->at(3 * a + 0) = temp[0];
				temp_plane->vertices->at(3 * a + 1) = temp[1];
				temp_plane->vertices->at(3 * a + 2) = temp[2];
			}

			glmT(temp_plane, all_planes.at(i).at(j).mid_point);

			if (j == 0){
				planes = glmCopy(temp_plane);
			}
			else{
				glmCombine(planes, temp_plane);
			}
		}

		std::string filename = file_tag + "_" + to_string(i) + ".obj";
		glmWriteOBJ(planes, my_strdup(filename.c_str()), GLM_NONE);
	}
}

void output_plane_SVM(std::vector<std::vector<plane>> &all_planes, std::string &file_neighbor)
{
	ifstream neighbor(file_neighbor);
	std::vector<std::vector<int>> neighbor_info(all_planes.size());
	std::vector<std::vector<vec3>> vertex_info(all_planes.size());
	int temp_ne;
	for (unsigned int i = 0; i < all_planes.size(); i += 1){
		for (unsigned int j = 0; j < all_planes.size(); j += 1){
			neighbor >> temp_ne;
			if (temp_ne > 2){
				neighbor_info.at(i).push_back(j);
			}
		}

		string file_vertex = to_string(i) + ".txt";
		ifstream vertex(file_vertex);
		vec3 temp_v;
		while (vertex >> temp_v[0] >> temp_v[1] >> temp_v[2])
		{
			vertex_info.at(i).push_back(temp_v);
		}
		vertex.close();
	}
	neighbor.close();

	for (unsigned int i = 0; i < neighbor_info.size(); i += 1){
		GLMmodel *planes = NULL;
		for (unsigned int j = 0; j < neighbor_info.at(i).size(); j += 1){
			std::vector<vec3> temp_vector = vertex_info.at(i);
			for (unsigned int k = 0; k < vertex_info.at(neighbor_info.at(i).at(j)).size(); k += 1){
				temp_vector.push_back(vertex_info.at(neighbor_info.at(i).at(j)).at(k));
			}
			point_set_project_plane(temp_vector, all_planes.at(i).at(j));

			Point *p = new Point[temp_vector.size()];
			for (unsigned a = 0; a < temp_vector.size(); a += 1){
				float data[3];
				data[0] = temp_vector.at(a)[0];
				data[1] = temp_vector.at(a)[1];
				data[2] = temp_vector.at(a)[2];
				p[a] = Point(3, data, data + 3);
			}

			Min_sphere ms(p, p + temp_vector.size());
			vec3 center(ms.center()[0], ms.center()[1], ms.center()[2]);
			float r = sqrt(ms.squared_radius());

			GLMmodel *temp_plane = glmReadOBJ("test_model/cube.obj");

			glmScale_y(temp_plane, r / 1.5);
			glmScale_z(temp_plane, r / 1.5);

			vec3 n_vector;
			n_vector[0] = all_planes.at(i).at(j).plane_par[0];
			n_vector[1] = all_planes.at(i).at(j).plane_par[1];
			n_vector[2] = all_planes.at(i).at(j).plane_par[2];
			float d = all_planes.at(i).at(j).plane_par[3];

			vec3 axis = vec3(1.0f, 0.0f, 0.0f) ^ n_vector;
			float dot_angle = vec3(1.0f, 0.0f, 0.0f) * n_vector;
			float angle = acos(dot_angle);
			mat4 R = rotation3Drad(axis, angle);

			for (unsigned int a = 1; a <= temp_plane->numvertices; a += 1) {
				vec3 temp(temp_plane->vertices->at(3 * a + 0), temp_plane->vertices->at(3 * a + 1), temp_plane->vertices->at(3 * a + 2));
				temp = R * temp;
				temp_plane->vertices->at(3 * a + 0) = temp[0];
				temp_plane->vertices->at(3 * a + 1) = temp[1];
				temp_plane->vertices->at(3 * a + 2) = temp[2];
			}

			glmT(temp_plane, center);

			if (j == 0){
				planes = glmCopy(temp_plane);
			}
			else{
				glmCombine(planes, temp_plane);
			}			
		}

		std::string filename = "SVM_plane" + to_string(i) + ".obj";
		glmWriteOBJ(planes, my_strdup(filename.c_str()), GLM_NONE);
	}
}

float plane_dir_point(vec3 &point, plane &plane)
{
	float judge = plane.plane_par[0] * point[0] + plane.plane_par[1] * point[1] + plane.plane_par[2] * point[2] - plane.plane_par[3];

	if (judge > 0.001f)
		judge = 1;
	else if (judge < -0.001f)
		judge = -1;
	else
		judge = 0;

	return judge;
}

float plane_dist_point(vec3 &point, plane &plane)
{
	float judge = plane.plane_par[0] * point[0] + plane.plane_par[1] * point[1] + plane.plane_par[2] * point[2] - plane.plane_par[3];

	return judge;
}

void plane_dir_edge(edge &temp, plane &plane, int dir[2])
{
	dir[0] = (int)plane_dir_point(temp.point[0], plane);
	dir[1] = (int)plane_dir_point(temp.point[1], plane);
}

void plane_dist_edge(edge &temp, plane &plane, float dist[2])
{
	float judge1 = plane.plane_par[0] * temp.point[0][0] + plane.plane_par[1] * temp.point[0][1] + plane.plane_par[2] * temp.point[0][2] - plane.plane_par[3];
	float judge2 = plane.plane_par[0] * temp.point[1][0] + plane.plane_par[1] * temp.point[1][1] + plane.plane_par[2] * temp.point[1][2] - plane.plane_par[3];

	dist[0] = fabs(judge1) / sqrt(pow(plane.plane_par[0], 2) + pow(plane.plane_par[1], 2) + pow(plane.plane_par[2], 2));
	dist[1] = fabs(judge2) / sqrt(pow(plane.plane_par[0], 2) + pow(plane.plane_par[1], 2) + pow(plane.plane_par[2], 2));
}

vec3 point_project_plane(vec3 &point, plane &plane)
{
	float dist = plane_dist_point(point, plane);

	vec3 new_point = point + dist * -vec3(plane.plane_par[0], plane.plane_par[1], plane.plane_par[2]);

	return new_point;
}

void point_set_project_plane(std::vector<vec3> &point_set, plane &plane)
{
	for (unsigned int i = 0; i < point_set.size(); i += 1){
		point_set.at(i) = point_project_plane(point_set.at(i), plane);
	}
}