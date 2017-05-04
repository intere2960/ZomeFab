
#ifndef _SMESHLIB_IO_IMPORTOBJ_H_
#define _SMESHLIB_IO_IMPORTOBJ_H_

#include "CGAL/Polyhedron_incremental_builder_3.h"
#include "CGAL/Modifier_base.h"
#include "CGAL/exceptions.h"
#include <string>
#include <fstream>
#include <exception>

namespace SMeshLib {
namespace IO       {
;

// The BuildCgalPolyhedronFromObj class builds a CGAL::Polyhedron_3 from Wavefront OBJ file.
// This is very simple reader and only reads vertex coordinates and vertex index for faces.
// Faces can be polygons and doesn't have to be triangles.
template<class HDS>
class BuildCgalPolyhedronFromObj : public CGAL::Modifier_base<HDS>
{
public:
	
	BuildCgalPolyhedronFromObj(const std::string& fileName) : mFileName(fileName) {}
	
	void operator() (HDS& hds)
	{
		typedef typename HDS::Vertex   Vertex;
		typedef typename Vertex::Point Point;
		
		// Open obj file for reading.
		std::ifstream _file(mFileName.c_str());
		if(!_file)
		{
			return;
		}
		
		// Count the number of vertices and facets.
		// This is used to reserve memory in HDS.
		std::string _line;
		int _numVertices = 0;
		int _numFacets   = 0;
		while(_file.good())
		{
			std::getline(_file, _line);
			if(_line.size() > 1)
			{
				if(_line[0]=='v' && _line[1]==' ') {++_numVertices;}
				if(_line[0]=='f' && _line[1]==' ') {++_numFacets;}
			}
		}
		
		// Rewind file to beginning for reading data.
		if(!_file.good())
		{
			_file.clear();
		}
		_file.seekg(0);

		// Postcondition: hds is a valid polyhedral surface.
		CGAL::Polyhedron_incremental_builder_3<HDS> B(hds, true);
		
		// Load the data from OBJ file to HDS.
		B.begin_surface(_numVertices, _numFacets, int((_numVertices + _numFacets - 2)*2.1));
			
			std::string _token;
			while(!_file.eof())
			{
				_token = ""; // Reset token.
				_file >> _token;
				
				// if token is v then its a vertex.
				if(_token=="v")
				{
					double x, y, z;
					_file >> x >> y >> z;
					B.add_vertex(Point(x, y, z));
				}
				
				// There are 4 type of facets.
				// a     only vertex index.
				// a/b   vertex and texture index.
				// a/b/c vertex, texture and normal index.
				// a//c  vertex and normal index.
				else if(_token=="f")
				{
					// Read the remaining line for the facet.
					std::string _line;
					std::getline(_file, _line);
					
					// Split the line into facet's vertices.
					// The length of _vertices is equal to the number of vertices for this face.
					std::istringstream _stream(_line);
					std::vector<std::string> _vertices;
					std::copy(std::istream_iterator<std::string>(_stream), 
							  std::istream_iterator<std::string>(), 
							  std::back_inserter(_vertices));
					
					// For each vertex read only the first number, which is the vertex index.
					B.begin_facet();
					for(size_t i=0 ; i<_vertices.size() ; ++i)
					{
						std::string::size_type _pos = _vertices[i].find('/', 0);
						std::string _indexStr = _vertices[i].substr(0, _pos);
						B.add_vertex_to_facet(stoi(_indexStr)-1); // -1 is because OBJ file uses 1 based index.
					}
					B.end_facet();
				}
			}
			_file.close();
			
		B.end_surface();
	}
	
private:
	
	std::string mFileName;
};


// Import a OBJ file given by fileName to polyhedron.
// TPoly is a type of CGAL::Polyhdeon_3.
template<class TPoly>
void importOBJ(const std::string& fileName, TPoly* polyhedron)
{
	/*if(polyhedron)
	{
		try
		{*/
			// Build Polyhedron_3 from the OBJ file.
			BuildCgalPolyhedronFromObj<TPoly::HalfedgeDS> _buildPolyhedron(fileName);
			
			// Calls is_valid at the end. Throws an exception in debug mode if polyhedron is not
			// manifold.
			polyhedron->delegate(_buildPolyhedron);
			
			// CGAL::Assert_exception is thrown in the debug mode when 
			// CGAL::Polyhedron_incremental_builder_3 is destroyed in BuildCgalPolyhedronFromObj.
			// However, in the release mode assertions is disabled and hence no exception is thrown.
			// Thus for uniform error reporting, if the polyhedron is not valid then throw a dummy 
			// exception in release mode.
			
			/*if(!polyhedron->is_valid())
			{
				throw CGAL::Assertion_exception("", "", "", 0, "");
			}
		}
		catch(const CGAL::Assertion_exception&)
		{
			std::string _msg = "SMeshLib::importOBJ: Error loading " + fileName;
			throw std::exception(_msg.c_str());
		}
	}*/
}

};	// End namespace IO.
};	// End namespace SMeshLib.

//#include<fstream>
//#include<vector>
//#include<string>
//#include<algorithm>
//
//#include "zomestruc.h"
//
////#include<CGAL/Simple_cartesian.h>
//#include<CGAL/Polyhedron_incremental_builder_3.h>
////#include<CGAL/Polyhedron_3.h>
////#include<CGAL/IO/Polyhedron_iostream.h>
////#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
//
////typedef CGAL::Exact_predicates_inexact_constructions_kernel  Read_K;
////typedef CGAL::Polyhedron_3<Read_K>  read_P;
//typedef Polyhedron_3::HalfedgeDS    HalfedgeDS;
//
//// A modifier creating a triangle with the incremental builder.
//template<class HDS>
//class polyhedron_builder : public CGAL::Modifier_base<HDS> {
//public:
//	std::vector<double> &coords;
//	std::vector<int>    &tris;
//	polyhedron_builder(std::vector<double> &_coords, std::vector<int> &_tris) : coords(_coords), tris(_tris) {}
//	void operator()(HDS& hds) {
//		typedef typename HDS::Vertex   Vertex;
//		typedef typename Vertex::Point Point;
//
//		// create a cgal incremental builder
//		CGAL::Polyhedron_incremental_builder_3<HDS> B(hds, true);
//		B.begin_surface(coords.size() / 3, tris.size() / 3);
//
//		// add the polyhedron vertices
//		for (int i = 0; i<(int)coords.size(); i += 3){
//			B.add_vertex(Point(coords[i + 0], coords[i + 1], coords[i + 2]));
//		}
//
//		// add the polyhedron triangles
//		for (int i = 0; i<(int)tris.size(); i += 3){
//			B.begin_facet();
//			B.add_vertex_to_facet(tris[i + 0]);
//			B.add_vertex_to_facet(tris[i + 1]);
//			B.add_vertex_to_facet(tris[i + 2]);
//			B.end_facet();
//		}
//
//		// finish up the surface
//		B.end_surface();
//	}
//};
//
//// reads the first integer from a string in the form
//// "334/455/234" by stripping forward slashes and
//// scanning the result
//int get_first_integer(const char *v){
//	int ival;
//	std::string s(v);
//	std::replace(s.begin(), s.end(), '/', ' ');
//	sscanf(s.c_str(), "%d", &ival);
//	return ival;
//}
//
//// barebones .OFF file reader, throws away texture coordinates, normals, etc.
//// stores results in input coords array, packed [x0,y0,z0,x1,y1,z1,...] and
//// tris array packed [T0a,T0b,T0c,T1a,T1b,T1c,...]
//void load_obj(char *filename, std::vector<double> &coords, std::vector<int> &tris){
//	double x, y, z;
//	char line[1024], v0[1024], v1[1024], v2[1024];
//
//	// open the file, return if open fails
//	FILE *fp = fopen(filename, "r");
//	if (!fp) return;
//
//	// read lines from the file, if the first character of the
//	// line is 'v', we are reading a vertex, otherwise, if the
//	// first character is a 'f' we are reading a facet
//	while (fgets(line, 1024, fp)){
//		if (line[0] == 'v'){
//			sscanf(line, "%*s%lf%lf%lf", &x, &y, &z);
//			coords.push_back(x);
//			coords.push_back(y);
//			coords.push_back(z);
//		}
//		else if (line[0] == 'f'){
//			sscanf(line, "%*s%s%s%s", v0, v1, v2);
//			tris.push_back(get_first_integer(v0) - 1);
//			tris.push_back(get_first_integer(v1) - 1);
//			tris.push_back(get_first_integer(v2) - 1);
//		}
//	}
//	fclose(fp);
//}
//
//void load_model(char *filename, Polyhedron_3 &P){
//	std::vector<double> coords;
//	std::vector<int>    tris;
//
//	// load the input file
//	load_obj(filename, coords, tris);
//
//	// build a polyhedron from the loaded arrays
//	polyhedron_builder<HalfedgeDS> builder(coords, tris);
//	P.delegate(builder);
//}

#endif // _SMESHLIB_IO_IMPORTOBJ_H_
