#include "graphcut.h"
#include <algorithm>

double AverageRadius(GLMmodel *model, vec3 &centroid)
{
	float area = 0.0f;
	float distance = 0;
	for (int i = 0; i < model->numtriangles; i += 1){

		vec3 p1 = vec3(model->vertices->at(3 * model->triangles->at(i).vindices[0] + 0), model->vertices->at(3 * model->triangles->at(i).vindices[0] + 1), model->vertices->at(3 * model->triangles->at(i).vindices[0] + 2));
		vec3 p2 = vec3(model->vertices->at(3 * model->triangles->at(i).vindices[1] + 0), model->vertices->at(3 * model->triangles->at(i).vindices[1] + 1), model->vertices->at(3 * model->triangles->at(i).vindices[1] + 2));
		vec3 p3 = vec3(model->vertices->at(3 * model->triangles->at(i).vindices[2] + 0), model->vertices->at(3 * model->triangles->at(i).vindices[2] + 1), model->vertices->at(3 * model->triangles->at(i).vindices[2] + 2));
		vec3 g = (p1 + p2 + p3) / 3.0f;

		vec3 v1 = p1 - p2;
		vec3 v2 = p3 - p2;

		vec3 test_normal = v2 ^ v1;
		float face_area = test_normal.length();
		distance += face_area *	(g - centroid).length();
		area += face_area;

	}

	if (area == 0)
		return 0;
	else
		return distance / area;
}

#include <fstream>

void test_graph_cut(GLMmodel *model, std::vector<std::vector<zomeconn>> &test_connect, std::vector<simple_material> &materials, float color_material[20][3], std::vector<std::vector<int>>& vertex_color)
{
	int num_labels = 0;

	std::vector<std::vector<int>> index(test_connect.at(COLOR_WHITE).size());
	std::vector<int> use;
	for (int i = 0; i < model->numtriangles; i += 1){
		index.at(model->triangles->at(i).near_node).push_back(i);
		int test_index = find(use.begin(), use.end(), model->triangles->at(i).near_node) - use.begin();
		if (test_index == use.size()){
			use.push_back(model->triangles->at(i).near_node);
		}
	}

	std::vector<int> convert_index(test_connect.at(COLOR_WHITE).size());
	for (unsigned int i = 0; i < test_connect.at(COLOR_WHITE).size(); i += 1){
		if (index.at(i).size() != 0){
			convert_index.at(i) = num_labels;
			num_labels += 1;
		}
	}

	VKGraph * nhd = new VKGraph(model->numtriangles);

	double radius = AverageRadius(model, glmCentroid(model));
	//cout << "radius : " << radius << endl;

	float max_data = 0.0f;
	float max_smooth = 0.0f;

	float wedge = 0.1f;
	float wnode = 1.0f;
	float label = 0.0f;

	std::vector<double> dataterm(model->numtriangles * num_labels);
	for (int i = 0; i < model->numtriangles; i += 1){

		vec3 now_n = vec3(model->facetnorms->at(3 * model->triangles->at(i).findex + 0), model->facetnorms->at(3 * model->triangles->at(i).findex + 1), model->facetnorms->at(3 * model->triangles->at(i).findex + 2));
		vec3 p1 = vec3(model->vertices->at(3 * model->triangles->at(i).vindices[0] + 0), model->vertices->at(3 * model->triangles->at(i).vindices[0] + 1), model->vertices->at(3 * model->triangles->at(i).vindices[0] + 2));
		vec3 p2 = vec3(model->vertices->at(3 * model->triangles->at(i).vindices[1] + 0), model->vertices->at(3 * model->triangles->at(i).vindices[1] + 1), model->vertices->at(3 * model->triangles->at(i).vindices[1] + 2));
		vec3 p3 = vec3(model->vertices->at(3 * model->triangles->at(i).vindices[2] + 0), model->vertices->at(3 * model->triangles->at(i).vindices[2] + 1), model->vertices->at(3 * model->triangles->at(i).vindices[2] + 2));
		vec3 now_g = (p1 + p2 + p3) / 3.0f;

		for (int j = 0; j < 3; j += 1){
			if (!nhd->Adjacent(model->triangles->at(i).near_tri[j], i))
			{
				vec3 judge_n = vec3(model->facetnorms->at(3 * model->triangles->at(model->triangles->at(i).near_tri[j]).findex + 0), model->facetnorms->at(3 * model->triangles->at(model->triangles->at(i).near_tri[j]).findex + 1), model->facetnorms->at(3 * model->triangles->at(model->triangles->at(i).near_tri[j]).findex + 2));
				vec3 judge_p1 = vec3(model->vertices->at(3 * model->triangles->at(model->triangles->at(i).near_tri[j]).vindices[0] + 0), model->vertices->at(3 * model->triangles->at(model->triangles->at(i).near_tri[j]).vindices[0] + 1), model->vertices->at(3 * model->triangles->at(model->triangles->at(i).near_tri[j]).vindices[0] + 2));
				vec3 judge_p2 = vec3(model->vertices->at(3 * model->triangles->at(model->triangles->at(i).near_tri[j]).vindices[1] + 0), model->vertices->at(3 * model->triangles->at(model->triangles->at(i).near_tri[j]).vindices[1] + 1), model->vertices->at(3 * model->triangles->at(model->triangles->at(i).near_tri[j]).vindices[1] + 2));
				vec3 judge_p3 = vec3(model->vertices->at(3 * model->triangles->at(model->triangles->at(i).near_tri[j]).vindices[2] + 0), model->vertices->at(3 * model->triangles->at(model->triangles->at(i).near_tri[j]).vindices[2] + 1), model->vertices->at(3 * model->triangles->at(model->triangles->at(i).near_tri[j]).vindices[2] + 2));
				vec3 judge_g = (judge_p1 + judge_p2 + judge_p3) / 3.0f;

				vec2 neighbor_index = tri_find_neigbor(model->triangles->at(i), model->triangles->at(model->triangles->at(i).near_tri[j]));

				vec3 neighbor_v1 = vec3(model->vertices->at(3 * neighbor_index[0] + 0), model->vertices->at(3 * neighbor_index[0] + 1), model->vertices->at(3 * neighbor_index[0] + 2));
				vec3 neighbor_v2 = vec3(model->vertices->at(3 * neighbor_index[1] + 0), model->vertices->at(3 * neighbor_index[1] + 1), model->vertices->at(3 * neighbor_index[1] + 2));

				float edist = 10 * (neighbor_v1 - neighbor_v2).length() / radius;
				float dot = edist * min(fabs(now_n * judge_n), 1.0f);
				float smooth = dot * wedge / 3.0f;

				//cout << smooth << endl;

				/*float dist = (now_g - judge_g).length();
				float edist = 10 * dist / radius;
				float angle;
				if (fabs(now_n * judge_n - 1) < 0.001f){
				angle = 0.0f + numeric_limits<float>::epsilon();
				}
				else{
				angle = acos(now_n * judge_n);
				}
				float smooth = -log(angle / M_PI) * edist;*/

				//cout << i << " : " << now_n * judge_n << " " << angle << " " << dist << " " << -log(angle / M_PI) << " " << -log(angle / M_PI) * dist << endl;
				//cout << i << " : " << model->triangles->at(i).near_tri[j] << " : " << -log(angle / M_PI) * dist << endl;

				nhd->AddEdge(i, model->triangles->at(i).near_tri[j], smooth);

				/*if (max_smooth < smooth){
				max_smooth = smooth;
				}*/
			}
		}

		for (int j = 0; j < use.size(); j += 1){
			vec3 judge_p = test_connect.at(COLOR_WHITE).at(use.at(j)).position;
			float dist = (now_g - judge_p).length();
			//cout << dist << endl;
			dataterm.at(i * num_labels + convert_index.at(use.at(j))) = dist;
			if (max_data < dist){
				max_data = dist;
			}
		}
	}

	/*for (int i = 0; i < model->numtriangles; i += 1){
	for (int j = 0; j < nhd->Node(i)->edges.size(); j += 1){
	nhd->Node(i)->edges.at(j)->weight /= max_smooth;
	}
	}*/
	//std::ofstream os;
	//os.open("dataterm.txt");
	//os << model->numtriangles << " " << num_labels << std::endl;

	for (int i = 0; i < dataterm.size(); i += 1){
		//os << dataterm.at(i) << " ";
		dataterm.at(i) /= max_data;
		//dataterm.at(i) /= 1.0;
		dataterm.at(i) = wnode * (0.998f - std::min(1.0, dataterm.at(i)));
		//if ((i + 1) % num_labels == 0){
		//	os << std::endl;
		//}
	}
	//os.close();

	std::cout << "NEdges = " << nhd->NumEdges() << std::endl;

	MRFProblemSmoothness problem(model->numtriangles, num_labels, nhd, dataterm);

	MRFGraphCut mrfSolver(&problem, label);

	mrfSolver.Optimize();

	double Edata, Esmooth, Elabel;
	double E = mrfSolver.Energy(&Edata, &Esmooth, &Elabel);
	std::cout << "E1 = " << E << " = " << Edata << " + " << Esmooth << " + " << Elabel << std::endl;

	/*cout << "max data : " << max_data << endl;
	cout << "max smooth : " << max_smooth << endl;*/

	std::vector<std::vector<int>> mat(num_labels);

	for (int i = 0; i < model->numtriangles; i += 1){
		mat.at(mrfSolver.Label(i)).push_back(i);
		//cout << i << " : " << mrfSolver.Label(i) << endl;
	}

	int material_class = 0;
	for (int i = 0; i < num_labels; i += 1){
		if (mat.at(i).size() > 0){
			//cout << i << " : " << mat.at(i).size() << endl;
			for (unsigned int j = 0; j < mat.at(i).size(); j += 1){
				model->triangles->at(mat.at(i).at(j)).material_id_graph_cut = material_class;

				for (int k = 0; k < 3; k += 1){
					int index = find(vertex_color.at(model->triangles->at(mat.at(i).at(j)).vindices[k]).begin(), vertex_color.at(model->triangles->at(mat.at(i).at(j)).vindices[k]).end(), material_class) - vertex_color.at(model->triangles->at(mat.at(i).at(j)).vindices[k]).begin();
					if (index >= vertex_color.at(model->triangles->at(mat.at(i).at(j)).vindices[k]).size())
						vertex_color.at(model->triangles->at(mat.at(i).at(j)).vindices[k]).push_back(material_class);
				}
				/*vertex_color.at(model->triangles->at(mat.at(i).at(j)).vindices[1]).push_back(material_class);
				vertex_color.at(model->triangles->at(mat.at(i).at(j)).vindices[2]).push_back(material_class);*/
			}

			simple_material temp_m;
			temp_m.name = std::to_string(materials.size());
			/*temp_m.diffuse[0] = (float)rand() / (float)RAND_MAX;
			temp_m.diffuse[1] = (float)rand() / (float)RAND_MAX;
			temp_m.diffuse[2] = (float)rand() / (float)RAND_MAX;*/
			temp_m.diffuse[0] = color_material[material_class % 20][0];
			temp_m.diffuse[1] = color_material[material_class % 20][1];
			temp_m.diffuse[2] = color_material[material_class % 20][2];
			materials.push_back(temp_m);

			material_class += 1;
		}
	}
}