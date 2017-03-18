#include <fstream>
#include "segment.h"
#include "ImportOBJ.h"
#include "bisect.h"
#include "glui_internal.h"

void sdf_segment(std::vector<GLMmodel> &seg, GLMmodel *model, char *model_source)
{
	// create and read Polyhedron
	Polyhedron mesh;
	SMeshLib::IO::importOBJ(model_source, &mesh);
	/*std::ifstream input("test_model/tricertp.off");
	if (!input || !(input >> mesh) || mesh.empty()) {
		std::cerr << "Not a valid off file." << std::endl;		
	}*/

	// create a property-map for segment-ids
	typedef std::map<Polyhedron::Facet_const_handle, std::size_t> Facet_int_map;
	Facet_int_map internal_segment_map;
	boost::associative_property_map<Facet_int_map> segment_property_map(internal_segment_map);
	// calculate SDF values and segment the mesh using default parameters.
	std::size_t number_of_segments = CGAL::segmentation_via_sdf_values(mesh, segment_property_map);

	std::vector<std::vector<int>> segment_tri(number_of_segments);
	int now = 0;
	for (Polyhedron::Facet_const_iterator facet_it = mesh.facets_begin();
		facet_it != mesh.facets_end(); ++facet_it) {
		segment_tri.at(segment_property_map[facet_it]).push_back(now);
		now += 1;
	}

	std::string s = "piece";
	seg.resize(number_of_segments);

	model->loop = new std::vector<Loop>();
	for (unsigned int i = 0; i < seg.size(); i += 1){		
		process_piece(seg.at(i), model, segment_tri.at(i));
		std::string piece = s + std::to_string(i) + ".obj";
		glmWriteOBJ(&seg.at(i), my_strdup(piece.c_str()), GLM_NONE);
	}
}