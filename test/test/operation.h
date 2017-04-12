#ifndef OPERATION_H_INCLUDED
#define OPERATION_H_INCLUDED

#include <vector>
#include "algebra3.h"
#include "glm.h"
#include "zomedir.h"
#include "zomestruc.h"


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

typedef CGAL::Simple_cartesian<double> K;
typedef K::Point_3 Point;
typedef CGAL::Polyhedron_3<K> Polyhedron_3;
typedef CGAL::AABB_face_graph_triangle_primitive<Polyhedron_3> Primitive;
typedef CGAL::AABB_traits<K, Primitive> Traits;
typedef CGAL::AABB_tree<Traits> Tree;
typedef CGAL::Side_of_triangle_mesh<Polyhedron_3, K> Point_inside;

void split(std::vector<std::vector<zomeconn>> &test_connect, int s_index, GLMmodel *model, zometable &splite_table);
void check_merge(std::vector<std::vector<zomeconn>> &test_connect, std::vector<vec4> &can_merge, GLMmodel *model, zometable &merge_table);
void merge(std::vector<std::vector<zomeconn>> &test_connect, vec4 &merge_vector);
void check_bridge(std::vector<std::vector<zomeconn>> &test_connect, std::vector<vec4> &can_bridge, GLMmodel *model, zometable &merge_table);
void bridge(std::vector<std::vector<zomeconn>> &test_connect, vec4 &bridge_vector);
void kill(std::vector<std::vector<zomeconn>> &test_connect, int index);
float forbidden_energy(float dist);
float compute_energy(std::vector<std::vector<zomeconn>> &test_connect, GLMmodel *model, float *term);

void search_near_point(std::vector<std::vector<zomeconn>> &test_connect, std::vector<int> &check_index, int now);
bool pointInside(Polyhedron_3 &polyhedron, Point &query);
bool check_inside(std::vector<std::vector<zomeconn>> &test_connect, int now);

#endif // OPERATION_H_INCLUDED