#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/boost/graph/graph_traits_Polyhedron_3.h>
#include <CGAL/IO/Polyhedron_iostream.h>
#include <CGAL/mesh_segmentation.h>
#include <CGAL/property_map.h>
#include <vector>
#include "glm.h"

typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef CGAL::Polyhedron_3<Kernel, CGAL::Polyhedron_items_3, CGAL::HalfedgeDS_list> Polyhedron;

void sdf_segment(std::vector<GLMmodel> &seg, GLMmodel *model, char *model_source);


//how to use CGAL::Polyhedron_3
//typedef Polyhedron::Vertex_iterator Vertex_iterator;
//typedef Polyhedron::Face_iterator Face_iterator;
//typedef Polyhedron::Halfedge_around_facet_circulator Halfedge_facet_circulator;
//typedef Kernel::Point_3 Point;
//
//cout << mesh.size_of_vertices() << endl;
//Vertex_iterator v = mesh.vertices_begin();
////for (v = mesh.vertices_begin(); v != mesh.vertices_end(); v++){
//for (int i = 0; i < mesh.size_of_vertices(); i += 1){
//	std::cout << v->point() << std::endl;
//	v++;
//}

/*Face_iterator f;
for (f = mesh.facets_begin(); f != mesh.facets_end(); f++)
std::cout << 1 << std::endl;*/

//for (Face_iterator i = mesh.facets_begin(); i != mesh.facets_end(); ++i) {
//	Halfedge_facet_circulator j = i->facet_begin();
//	// Facets in polyhedral surfaces are at least triangles.
//	CGAL_assertion(CGAL::circulator_size(j) >= 3);
//	std::cout << CGAL::circulator_size(j) << ' ';
//	do {
//		std::cout << ' ' << std::distance(mesh.vertices_begin(), j->vertex());
//	} while (++j != i->facet_begin());
//	std::cout << std::endl;
//}

//ofstream os;
//os.open("sphere.obj");

////os << mesh;
//Vertex_iterator v = mesh.vertices_begin();
//for (int i = 0; i < mesh.size_of_vertices(); i += 1){
//	os << "v " << v->point() << endl;
//	v++;
//}

//os << endl;

//Face_iterator f = mesh.facets_begin();
//for (int i = 0; i < mesh.size_of_facets(); i += 1){
//	Halfedge_facet_circulator edge = f->facet_begin();
//	os << "f ";
//	for (int j = 0; j < CGAL::circulator_size(edge); j += 1){
//		os << distance(mesh.vertices_begin(), edge->vertex()) + 1 << " ";
//		edge++;
//	}
//	os << endl;
//	f++;
//}
//os.close();