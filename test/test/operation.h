#ifndef OPERATION_H_INCLUDED
#define OPERATION_H_INCLUDED

#include <vector>
#include "algebra3.h"
#include "glm.h"
#include "zomedir.h"
#include "zomestruc.h"
#include "global.h"
#include "nanoflann.hpp"

using namespace nanoflann;

// This is an example of a custom data set class
template <typename T>
struct PointCloud
{
	struct Point
	{
		T  x, y, z;
	};

	std::vector<Point>  pts;

	// Must return the number of data points
	inline size_t kdtree_get_point_count() const { return pts.size(); }

	// Returns the distance between the vector "p1[0:size-1]" and the data point with index "idx_p2" stored in the class:
	inline T kdtree_distance(const T *p1, const size_t idx_p2, size_t /*size*/) const
	{
		const T d0 = p1[0] - pts[idx_p2].x;
		const T d1 = p1[1] - pts[idx_p2].y;
		const T d2 = p1[2] - pts[idx_p2].z;
		return d0*d0 + d1*d1 + d2*d2;
	}

	// Returns the dim'th component of the idx'th point in the class:
	// Since this is inlined and the "dim" argument is typically an immediate value, the
	//  "if/else's" are actually solved at compile time.
	inline T kdtree_get_pt(const size_t idx, int dim) const
	{
		if (dim == 0) return pts[idx].x;
		else if (dim == 1) return pts[idx].y;
		else return pts[idx].z;
	}

	// Optional bounding-box computation: return false to default to a standard bbox computation loop.
	//   Return true if the BBOX was already computed by the class and returned in "bb" so it can be avoided to redo it again.
	//   Look at bb.size() to find out the expected dimensionality (e.g. 2 or 3 for point clouds)
	template <class BBOX>
	bool kdtree_get_bbox(BBOX& /*bb*/) const { return false; }

};

void split(std::vector<std::vector<zomeconn>> &test_connect, int s_index, GLMmodel *model, PointCloud<float> &cloud, zometable &splite_table);
void check_merge(std::vector<std::vector<zomeconn>> &test_connect, std::vector<vec4> &can_merge, GLMmodel *model, zometable &merge_table);
void merge(std::vector<std::vector<zomeconn>> &test_connect, vec4 &merge_vector);
void check_bridge(std::vector<std::vector<zomeconn>> &test_connect, std::vector<vec4> &can_bridge, GLMmodel *model, zometable &merge_table);
void bridge(std::vector<std::vector<zomeconn>> &test_connect, vec4 &bridge_vector);
void kill(std::vector<std::vector<zomeconn>> &test_connect, int index);
float forbidden_energy(float dist);
float compute_energy(std::vector<std::vector<zomeconn>> &test_connect, GLMmodel *model, PointCloud<float> &cloud, float *term);

template <typename T>
void generatePointCloud(PointCloud<T> &point, GLMmodel *model)
{
	//std::cout << "Generating " << model->numtriangles << " point cloud...";
	point.pts.resize(model->numtriangles);
	for (size_t i = 0; i < model->numtriangles; i++)
	{
		vec3 p1(model->vertices->at(3 * model->triangles->at(i).vindices[0] + 0), model->vertices->at(3 * model->triangles->at(i).vindices[0] + 1), model->vertices->at(3 * model->triangles->at(i).vindices[0] + 2));
		vec3 p2(model->vertices->at(3 * model->triangles->at(i).vindices[1] + 0), model->vertices->at(3 * model->triangles->at(i).vindices[1] + 1), model->vertices->at(3 * model->triangles->at(i).vindices[1] + 2));
		vec3 p3(model->vertices->at(3 * model->triangles->at(i).vindices[2] + 0), model->vertices->at(3 * model->triangles->at(i).vindices[2] + 1), model->vertices->at(3 * model->triangles->at(i).vindices[2] + 2));
		vec3 test = (p1 + p2 + p3) / 3.0;

		point.pts[i].x = test[0];
		point.pts[i].y = test[1];
		point.pts[i].z = test[2];
	}

	//std::cout << "done\n";
}

template <typename T>
void generatePointCloud(PointCloud<T> &point, std::vector<std::vector<zomeconn>> &test_connect)
{
	//std::cout << "Generating " << model->numtriangles << " point cloud...";
	point.pts.resize(test_connect.at(COLOR_WHITE).size());
	for (size_t i = 0; i < test_connect.at(COLOR_WHITE).size(); i++)
	{
		vec3 test = test_connect.at(COLOR_WHITE).at(i).position;

		point.pts[i].x = test[0];
		point.pts[i].y = test[1];
		point.pts[i].z = test[2];
	}

	//std::cout << "done\n";
}

template <typename num_t>
void kdtree_search(GLMmodel *model, PointCloud<num_t> &cloud, vec3 &test_point, std::vector<int> &near_tri);

void kdtree_near_node(GLMmodel *model, std::vector<std::vector<zomeconn>> &test_connect);
void kdtree_near_node_outter(GLMmodel *model, std::vector<std::vector<zomeconn>> &test_connect);
void kdtree_near_node_colorful(GLMmodel *model, std::vector<std::vector<zomeconn>> &test_connect, std::vector<simple_material> &materials);
void kdtree_near_node_energy_dist(GLMmodel *model, std::vector<std::vector<zomeconn>> &test_connect, std::vector<simple_material> &materials);

#endif // OPERATION_H_INCLUDED