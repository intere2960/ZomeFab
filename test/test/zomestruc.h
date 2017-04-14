#ifndef ZOMESTRUC_H_INCLUDED
#define ZOMESTRUC_H_INCLUDED

#include <vector>
#include "algebra3.h"
#include "glm.h"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/point_generators_3.h>
#include <CGAL/algorithm.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/convex_hull_3.h>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/AABB_tree.h>
#include <CGAL/AABB_traits.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/boost/graph/graph_traits_Polyhedron_3.h>
#include <CGAL/AABB_face_graph_triangle_primitive.h>
#include <CGAL/algorithm.h>
#include <CGAL/Side_of_triangle_mesh.h>

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

	int type; //0 none 1 single 2 double
	vec2 fromindex;
	vec2 towardindex;
	int fromface;
	int towardface;
	int size; // S M L
//	ModelObject* thisModel;
	vec2 connect_stick[62];

	bool isAnalyzed; //analyze force
	float force;

	float surface_d;
	float energy_d;

	bool exist;
	bool outter;

	int material_id;
//	zomeconn* link;
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

//class VirtualBall
//{
//public:
//	VirtualBall( int i , ZomeConnection* c );
//	int index;
//	GLfloat position[3];
//	bool isBottom;
//	int bottomForceIndex;
//	int connectionAmount;
//	int connIndex[3];
//	int connectFace[3];
//	ZomeConnection* conn[3];
//
//	void AddConn( ZomeConnection* c);
//};

typedef CGAL::Simple_cartesian<double> K;
typedef K::Point_3 Point;
typedef CGAL::Polyhedron_3<K> Polyhedron_3;
typedef Polyhedron_3::Vertex_iterator Vertex_iterator;
//typedef Polyhedron_3::Face_iterator Face_iterator;
//typedef Polyhedron_3::Halfedge_around_facet_circulator Halfedge_facet_circulator;
typedef CGAL::Side_of_triangle_mesh<Polyhedron_3, K> Point_inside;

void search_near_point(std::vector<std::vector<zomeconn>> &test_connect, std::vector<int> &check_index, int now);
bool pointInside(Polyhedron_3 &polyhedron, Point &query);
bool check_inside(std::vector<std::vector<zomeconn>> &test_connect, int now);

void combine_zome_ztruc(std::vector<std::vector<zomeconn>> &target, std::vector<std::vector<zomeconn>> &source);
void output_struc(std::vector<std::vector<zomeconn>> &target, std::string &filename);
void struc_parser(std::vector<std::vector<zomeconn>> &target, std::string &filename);
void output_zometool(vec3 &rotation, std::vector<std::vector<zomeconn>> &zome_queue, int piece_id);
void output_zometool(std::vector<std::vector<zomeconn>> &output_connect, std::string &filename);
void output_zometool_colorful(std::vector<std::vector<zomeconn>> &output_connect, std::string &filename, std::vector<simple_material> &materials, std::string &materials_filename);
float point_surface_dist(GLMmodel *model, vec3 &p);
float point_surface_dist_fast(GLMmodel *model, vec3 &p, std::vector<int> &near_tri);
float ball_surface_dist(GLMmodel *model, vec3 &p);
float ball_surface_dist_fast(GLMmodel *model, vec3 &p, std::vector<int> &near_tri);
bool check_stick_intersect(GLMmodel *model, vec3 &p, vec3 &origin_p);
bool collision_test(std::vector<std::vector<zomeconn>> &test_connect, vec3 & give_up);
void count_struct(std::vector<std::vector<zomeconn>> &test_connect, vec3 *count);
void judge_outter(std::vector<std::vector<zomeconn>> &test_connect);

#endif // ZOMESTRUC_H_INCLUDED
