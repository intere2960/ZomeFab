#ifndef ZOMESTRUC_H_INCLUDED
#define ZOMESTRUC_H_INCLUDED

#include <vector>
#include <string>
#include "algebra3.h"
#include "glm.h"

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/point_generators_3.h>
#include <CGAL/algorithm.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/convex_hull_3.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/AABB_tree.h>
#include <CGAL/AABB_traits.h>
#include <CGAL/boost/graph/graph_traits_Polyhedron_3.h>
#include <CGAL/AABB_face_graph_triangle_primitive.h>
#include <CGAL/Side_of_triangle_mesh.h>

typedef CGAL::Exact_predicates_exact_constructions_kernel K;
typedef K::Point_3 Point;
typedef CGAL::Polyhedron_3<K> Polyhedron_3;
typedef K::Plane_3 Plane_3;
typedef Polyhedron_3::Vertex_iterator Vertex_iterator;
typedef Polyhedron_3::Face_iterator Face_iterator;
typedef Polyhedron_3::Plane_iterator Plane_iterator;
typedef Polyhedron_3::Halfedge_around_facet_circulator Halfedge_facet_circulator;

typedef CGAL::AABB_face_graph_triangle_primitive<Polyhedron_3> Primitive;
typedef CGAL::AABB_traits<K, Primitive> Traits;
typedef CGAL::AABB_tree<Traits> Tree;
typedef CGAL::Side_of_triangle_mesh<Polyhedron_3, K> Point_inside;

class zomeconn
{
public:
	zomeconn();
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
	bool outer;

	int material_id;
	int material_id_energy_angle;
	int material_id_energy_use_stick;
	int material_id_outer;
};

class zomestruc
{
public:
	zomestruc();
	int index;
	vec3 position;
};

void search_near_point(std::vector<std::vector<zomeconn>> &test_connect, std::vector<int> &check_index, int now);
bool pointInside(Polyhedron_3 &polyhedron, Point &query);
bool is_pointInside(Polyhedron_3 &polyhedron, Point &query);
bool check_inside(std::vector<std::vector<zomeconn>> &test_connect, int now);

void combine_zome_ztruc(std::vector<std::vector<zomeconn>> &target, std::vector<std::vector<zomeconn>> &source);
void output_struc(std::vector<std::vector<zomeconn>> &target, std::string &filename);
void struc_parser(std::vector<std::vector<zomeconn>> &target, std::string &filename);

void output_zometool(vec3 &rotation, std::vector<std::vector<zomeconn>> &zome_queue, int piece_id);
void output_zometool(std::vector<std::vector<zomeconn>> &output_connect, std::string &filename);
void output_zometool_color(std::vector<std::vector<zomeconn>> &output_connect, std::string &filename, int color);
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

bool near_middle(GLMmodel *model, vec3 &test_p, std::vector<int> &middle_point);
bool near_outer(GLMmodel *model, vec3 &test_p, std::vector<int> &outer_point);
int near_solt(GLMmodel *model, std::vector<std::vector<zomeconn>> &test_connect, std::vector<std::vector<int>> &solt_ball, std::vector<std::vector<vec3>> &solt_dist);
void generate_tenon(GLMmodel* model, std::vector<std::vector<zomeconn>> &test_connect, int use_solt, std::vector<int> &solt_ball, std::vector<vec3> &solt_dist, std::string &file_name, int piece_id);
void output_color_zome(std::string &model_file, std::string &zome_file);
#endif // ZOMESTRUC_H_INCLUDED
