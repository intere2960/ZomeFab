#ifndef ZOMESTRUC_H_INCLUDED
#define ZOMESTRUC_H_INCLUDED

#include <vector>
#include "algebra3.h"
#include "glm.h"

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/point_generators_3.h>
#include <CGAL/algorithm.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/convex_hull_3.h>

class zomeconn
{
public:
	zomeconn();
	~zomeconn();
	int getmodelnumber();

	int index;
	int color;

	vec3 position;
	vec3 rotation;
	vec3 endposition;

	vec2 fromindex;
	vec2 towardindex;
	int fromface;
	int towardface;
	int size; // S M L
	vec2 connect_stick[62];

	float surface_d;
	float energy_d;
	float energy_angle;
	float energy_use_stick;

	bool exist;
	bool outter;

	int material_id;
	int material_id_energy_angle;
	int material_id_energy_use_stick;
};

class zomestruc
{
public:
	zomestruc();
	~zomestruc();
	int index;
	vec3 position;
//	zomestruc* originalBall;
//	ZomeConnection** faceConnection;//62
//	zomestruc** nextBall;//62
//
//	zomestruc* link;
//	ModelObject* thisModel;
//
//	int CheckConnection( int faceIndex );// -1 0 1 2
//	bool CheckBallExist( int faceIndex );
};

typedef CGAL::Exact_predicates_exact_constructions_kernel K;
typedef K::Point_3 Point;
typedef CGAL::Polyhedron_3<K> Polyhedron_3;
typedef K::Plane_3 Plane_3;
typedef Polyhedron_3::Vertex_iterator Vertex_iterator;
typedef Polyhedron_3::Face_iterator Face_iterator;
typedef Polyhedron_3::Plane_iterator Plane_iterator;
typedef Polyhedron_3::Halfedge_around_facet_circulator Halfedge_facet_circulator;

void search_near_point(std::vector<std::vector<zomeconn>> &test_connect, std::vector<int> &check_index, int now);
bool pointInside(Polyhedron_3 &polyhedron, Point &query);
bool check_inside(std::vector<std::vector<zomeconn>> &test_connect, int now);

void combine_zome_ztruc(std::vector<std::vector<zomeconn>> &target, std::vector<std::vector<zomeconn>> &source);
void output_struc(std::vector<std::vector<zomeconn>> &target, std::string &filename);
void struc_parser(std::vector<std::vector<zomeconn>> &target, std::string &filename);

void output_zometool(vec3 &rotation, std::vector<std::vector<zomeconn>> &zome_queue, int piece_id);
void output_zometool(std::vector<std::vector<zomeconn>> &output_connect, std::string &filename);
void output_zometool_exp(std::vector<std::vector<zomeconn>> &output_connect, std::string &filename, std::vector<simple_material> &materials, std::string &materials_filename, int mode);

float point_surface_dist(GLMmodel *model, vec3 &p);
float point_surface_dist_fast(GLMmodel *model, vec3 &p, std::vector<int> &near_tri);
float ball_surface_dist(GLMmodel *model, vec3 &p);
float ball_surface_dist_fast(GLMmodel *model, vec3 &p, std::vector<int> &near_tri);

bool check_stick_intersect(GLMmodel *model, vec3 &p, vec3 &origin_p);
bool collision_test(std::vector<std::vector<zomeconn>> &test_connect, vec3 & give_up);
void count_struct(std::vector<std::vector<zomeconn>> &test_connect, vec3 *count);
void judge_outer(std::vector<std::vector<zomeconn>> &test_connect);

void energy_material(std::vector<std::vector<zomeconn>> &test_connect, std::vector<simple_material> &materials, int mode);

void generate_tenon(GLMmodel* model, std::vector<std::vector<zomeconn>> &test_connect, int use_solt);
#endif // ZOMESTRUC_H_INCLUDED
