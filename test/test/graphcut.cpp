#include "graphcut.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include "shell.h"
#include "operation.h"

float color_material[20][3]
= {
	{ 0.140625, 0.140625, 0.140625 }, { 0.9140625, 0.0, 0.0 }, { 1, 0.20703125, 0.6015625 }, { 1.0, 0.0, 1.0 }, { 0.609375, 0.20703125, 1.0 },
	{ 0.4140625, 0.4140625, 1.0 }, { 0.15625, 0.578125, 1.0 }, { 0.0, 1.0, 1.0 }, { 0.1015625, 0.98828125, 0.609375 }, { 0.15625, 1.0, 0.15625 },
	{ 0.65625, 1.0, 0.140625 }, { 0.97265625, 0.97265625, 0.0 }, { 1.0, 0.82421875, 0.0234375 }, { 1.0, 0.625, 0.2578125 }, { 1.0, 0.5, 0.25 },
	{ 0.80859375, 0.6171875, 0.6171875 }, { 0.7578125, 0.7578125, 0.52734375 }, { 0.58203125, 0.7890625, 0.7890625 }, { 0.71875, 0.71875, 0.859375 }, { 0.7890625, 0.5546875, 0.7578125 }
};

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

void test_graph_cut(GLMmodel *model, std::vector<std::vector<zomeconn>> &test_connect, std::vector<simple_material> &materials, float color_material[20][3], std::vector<std::vector<int>>& vertex_color, float wedge, float wnode, float label, float salient)
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
	
	float max_data = 0.0f;
	float max_smooth = 0.0f;

	std::vector<double> dataterm(model->numtriangles * num_labels);
	for (int i = 0; i < model->numtriangles; i += 1){

		vec3 now_n = vec3(model->facetnorms->at(3 * model->triangles->at(i).findex + 0), model->facetnorms->at(3 * model->triangles->at(i).findex + 1), model->facetnorms->at(3 * model->triangles->at(i).findex + 2));
		vec3 p1 = vec3(model->vertices->at(3 * model->triangles->at(i).vindices[0] + 0), model->vertices->at(3 * model->triangles->at(i).vindices[0] + 1), model->vertices->at(3 * model->triangles->at(i).vindices[0] + 2));
		vec3 p2 = vec3(model->vertices->at(3 * model->triangles->at(i).vindices[1] + 0), model->vertices->at(3 * model->triangles->at(i).vindices[1] + 1), model->vertices->at(3 * model->triangles->at(i).vindices[1] + 2));
		vec3 p3 = vec3(model->vertices->at(3 * model->triangles->at(i).vindices[2] + 0), model->vertices->at(3 * model->triangles->at(i).vindices[2] + 1), model->vertices->at(3 * model->triangles->at(i).vindices[2] + 2));
		vec3 now_g = (p1 + p2 + p3) / 3.0f;

		for (int j = 0; j < 3; j += 1){
			if (model->triangles->at(i).near_tri[j] != -1 && !nhd->Adjacent(model->triangles->at(i).near_tri[j], i))
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
				float smooth = dot * wedge / 3.0f + salient * model->triangles->at(i).saliency;
				
				nhd->AddEdge(i, model->triangles->at(i).near_tri[j], smooth);
			}
		}

		for (int j = 0; j < use.size(); j += 1){
			vec3 judge_p = test_connect.at(COLOR_WHITE).at(use.at(j)).position;
			float dist = (now_g - judge_p).length();
			
			dataterm.at(i * num_labels + convert_index.at(use.at(j))) = dist;
			if (max_data < dist){
				max_data = dist;
			}
		}
	}

	for (int i = 0; i < dataterm.size(); i += 1){
		dataterm.at(i) /= max_data;
		dataterm.at(i) = wnode * (0.998f - std::min(1.0, dataterm.at(i)));
	}

	std::cout << "NEdges = " << nhd->NumEdges() << std::endl;

	MRFProblemSmoothness problem(model->numtriangles, num_labels, nhd, dataterm);

	MRFGraphCut mrfSolver(&problem, label);

	mrfSolver.Optimize();

	double Edata, Esmooth, Elabel;
	double E = mrfSolver.Energy(&Edata, &Esmooth, &Elabel);
	std::cout << "E1 = " << E << " = " << Edata << " + " << Esmooth << " + " << Elabel << std::endl;

	std::vector<std::vector<int>> mat(num_labels);

	for (int i = 0; i < model->numtriangles; i += 1){
		mat.at(mrfSolver.Label(i)).push_back(i);
	}

	int material_class = 0;
	for (int i = 0; i < num_labels; i += 1){
		if (mat.at(i).size() > 0){
			for (unsigned int j = 0; j < mat.at(i).size(); j += 1){
				model->triangles->at(mat.at(i).at(j)).material_id_graph_cut = material_class;

				for (int k = 0; k < 3; k += 1){
					int index = find(vertex_color.at(model->triangles->at(mat.at(i).at(j)).vindices[k]).begin(), vertex_color.at(model->triangles->at(mat.at(i).at(j)).vindices[k]).end(), material_class) - vertex_color.at(model->triangles->at(mat.at(i).at(j)).vindices[k]).begin();
					if (index >= vertex_color.at(model->triangles->at(mat.at(i).at(j)).vindices[k]).size())
						vertex_color.at(model->triangles->at(mat.at(i).at(j)).vindices[k]).push_back(material_class);
				}
			}

			simple_material temp_m;
			temp_m.name = std::to_string(materials.size());
			temp_m.diffuse[0] = color_material[material_class % 20][0];
			temp_m.diffuse[1] = color_material[material_class % 20][1];
			temp_m.diffuse[2] = color_material[material_class % 20][2];
			materials.push_back(temp_m);

			material_class += 1;
		}
	}
}

int have_same_material(int now_material, int p1, int p2, std::vector<std::vector<int>> &vertex_color)
{
	for (int i = 0; i < vertex_color.at(p1).size(); i += 1){
		for (int j = 0; j < vertex_color.at(p2).size(); j += 1){
			if (vertex_color.at(p1).at(i) == vertex_color.at(p2).at(j) && vertex_color.at(p1).at(i) != now_material)
				return vertex_color.at(p1).at(i);
		}
	}
	return -1;
}

void graph_cut(GLMmodel *model, std::string &model_file, std::string &zome_file, float total_num, float wedge, float wnode, float label, float salient)
{
	if (salient != 0.0f){
		float min_saliency = 100000000.0f;
		float max_saliency = -100000000.0f;

		std::ifstream is(model_file + "_mean_saliency.txt");
		std::vector<float> temp_saliency(1);
		for (int i = 0; i < model->numvertices; i += 1){
			float temp;
			is >> temp;
			temp_saliency.push_back(temp);

			if (temp > max_saliency){
				max_saliency = temp;
			}

			if (temp < min_saliency){
				min_saliency = temp;
			}
		}
		is.close();

		for (int i = 0; i < model->numvertices; i += 1){
			temp_saliency.at(i) -= min_saliency;
			temp_saliency.at(i) /= (max_saliency - min_saliency);
		}

		std::ofstream os(model_file + "_saliency.txt");
		for (int i = 0; i < model->numtriangles; i += 1){
			float temp = 0.0f;

			temp += temp_saliency.at(model->triangles->at(i).vindices[0]);
			temp += temp_saliency.at(model->triangles->at(i).vindices[1]);
			temp += temp_saliency.at(model->triangles->at(i).vindices[2]);

			model->triangles->at(i).saliency = temp / 3.0f;
			os << model->triangles->at(i).saliency << std::endl;
		}
		os.close();

		std::ifstream is2(model_file + "_saliency.txt");
		for (int i = 0; i < model->numtriangles; i += 1){
			float temp;
			is2 >> temp;
			model->triangles->at(i).saliency = temp;
		}
		is2.close();
	}

	std::vector<edge> all_edge;

	recount_normal(model);
	collect_edge(model, all_edge);
	find_near_tri(model, all_edge);

	std::vector<std::vector<zomeconn>> test_connect(4);
	struc_parser(test_connect, zome_file);

	std::string split_name = std::string(strtok(my_strdup(zome_file.c_str()), "."));

	std::fstream fileStream;
	fileStream.open(split_name + std::string("_nearest_point.txt"));
	if (!fileStream.fail()) {
		nearest_point_parser(model, split_name + std::string("_nearest_point.txt"));
	}
	else{
		PointCloud<float> cloud;
		// Generate points:

		generatePointCloud(cloud, model);
		float origin_term[4];
		float origin_e = compute_energy(test_connect, model, cloud, origin_term, total_num);
	}

	std::vector<std::vector<int>> vertex_color(model->numvertices + 1);

	std::vector<simple_material> materials_graph_cut;
	test_graph_cut(model, test_connect, materials_graph_cut, color_material, vertex_color, wedge, wnode, label, salient);

	int num_point = 0;
	for (unsigned int i = 1; i <= model->numvertices; i += 1){
		if (vertex_color.at(i).size() >= 3){
			num_point += 1;
		}
	}

	std::vector<std::vector<int>> material_group(materials_graph_cut.size());

	std::vector<int> convert_index;
	for (unsigned int i = 1; i <= model->numvertices; i += 1){
		if (vertex_color.at(i).size() >= 3){
			convert_index.push_back(i);
		}

		for (unsigned int j = 0; j < vertex_color.at(i).size(); j += 1){
			material_group.at(vertex_color.at(i).at(j)).push_back(i);
		}
	}

	for (unsigned int i = 0; i < materials_graph_cut.size(); i += 1){
		std::string filename = model_file + "_" + std::to_string(i) + ".txt";
		std::ofstream os(filename);

		for (unsigned int j = 0; j < material_group.at(i).size(); j += 1){
			vec3 now(model->vertices->at(3 * material_group.at(i).at(j) + 0), model->vertices->at(3 * material_group.at(i).at(j) + 1), model->vertices->at(3 * material_group.at(i).at(j) + 2));
			os << now[0] << " " << now[1] << " " << now[2] << std::endl;
		}

		os.close();
	}

	std::vector<std::vector<int>> material_point(materials_graph_cut.size());
	for (unsigned int i = 0; i < convert_index.size(); i += 1){
		for (int j = 0; j < vertex_color.at(convert_index.at(i)).size(); j += 1){
			material_point.at(vertex_color.at(convert_index.at(i)).at(j)).push_back(convert_index.at(i));
		}
	}

	std::vector<std::vector<std::vector<int>>> material_boundary(materials_graph_cut.size());
	for (unsigned int i = 0; i < material_boundary.size(); i += 1){
		material_boundary.at(i).resize(materials_graph_cut.size());
	}

	std::ofstream tri_neighbor(model_file + "_tri_neighbor.txt");
	for (unsigned int i = 1; i <= model->numvertices; i += 1){
		if (vertex_color.at(i).size() >= 2){
			for (int j = 0; j < vertex_color.at(i).size(); j += 1){
				for (int k = 0; k < vertex_color.at(i).size(); k += 1){
					if (j != k){
						material_boundary.at(vertex_color.at(i).at(j)).at(vertex_color.at(i).at(k)).push_back(i);
					}
				}
			}
		}

		if (vertex_color.at(i).size() >= 3){
			for (int j = 0; j < vertex_color.at(i).size(); j += 1){
				tri_neighbor << vertex_color.at(i).at(j) << " ";
			}
			tri_neighbor << std::endl;
		}
	}
	tri_neighbor.close();

	std::ofstream neighbor(model_file + "_neighbor.txt");
	for (unsigned int i = 0; i < material_boundary.size(); i += 1){
		for (unsigned int j = 0; j < material_boundary.at(i).size(); j += 1){
			neighbor << material_boundary.at(i).at(j).size() << " ";
		}
		neighbor << std::endl;
	}
	neighbor.close();

	std::string exp_name = model_file + "_" + std::to_string(materials_graph_cut.size()) + "(data_" + std::to_string(wnode) + ",_smooth_" + std::to_string(wedge) + ",_label_" + std::to_string(label) + ",_saliency_" + std::to_string(salient) + ")";
	std::string mtl_name = exp_name + ".mtl";
	std::string obj_name = exp_name + ".obj";
	output_material(materials_graph_cut, mtl_name);
	glmWriteOBJ_EXP(model, my_strdup(obj_name.c_str()), materials_graph_cut, mtl_name, GRAPH_CUT);
}

void fake_saliency(GLMmodel *model, std::string &model_file)
{
	std::ifstream is(model_file + "_mean_saliency.txt");
	std::vector<float> temp_saliency(1);
	if (!is.fail()){
		for (int i = 0; i < model->numvertices; i += 1){
			float temp;
			is >> temp;
			temp_saliency.push_back(temp);
		}
		is.close();
	}
	else{
		for (int i = 0; i < model->numvertices; i += 1){
			temp_saliency.push_back(0.0f);
		}
	}

	std::ifstream is1(model_file + "_fake_saliency_tag.txt");
	std::string aaa;
	std::string bbb;
	int a, b, c;

	int count = 0;
	std::vector<int> tag;
	bool record = true;

	while (is1 >> aaa){
		if (aaa == std::string("usemtl")){
			is1 >> bbb;

			if (bbb == std::string("initialShadingGroup")){
				if (count == 0){
					record = false;
				}
				else{
					record = !record;
				}
			}
			else{
				record = true;
			}
		}
		else{
			is1 >> a >> b >> c;

			if (record){
				tag.push_back(count);
				temp_saliency.at(a) = 1.0f;
				temp_saliency.at(b) = 1.0f;
				temp_saliency.at(c) = 1.0f;
			}

			count += 1;
		}
	}
	is.close();

	std::ofstream os(model_file + "_mean_saliency.txt");
	for (int i = 1; i <= model->numvertices; i += 1){
		os << temp_saliency.at(i) << std::endl;
	}
	os.close();
}

void saliency_texture(GLMmodel *model, std::string &model_file)
{
	std::ifstream is(model_file + "_mean_saliency.txt");
	std::vector<float> temp_saliency(1);
	for (int i = 0; i < model->numvertices; i += 1){
		float temp;
		is >> temp;
		temp_saliency.push_back(temp);
	}
	is.close();

	std::vector<simple_material> materials;
	std::vector<vec3> material_queue;

	for (int i = 1; i <= model->numvertices; i += 1){
		vec2 test(i, temp_saliency.at(i));
		material_queue.push_back(test);
	}

	std::sort(material_queue.begin(),
		material_queue.end(),
		[](vec2 a, vec2 b){
		return b[1] > a[1];
	});

	model->numtexcoords = model->numvertices;

	model->texcoords = (GLfloat*)malloc(sizeof(GLfloat) *
		2 * (model->numtexcoords + 1));

	int num_class = 0;
	for (unsigned int i = 0; i < material_queue.size(); i += 1){
		if (i == 0){
			model->cut_loop->at(material_queue.at(i)[0]).material_id_saliency = num_class;
			num_class += 1;
		}
		else{
			if (material_queue.at(i)[1] == material_queue.at(i - 1)[1]){
				num_class -= 1;
				model->cut_loop->at(material_queue.at(i)[0]).material_id_saliency = num_class;
				num_class += 1;
			}
			else{
				model->cut_loop->at(material_queue.at(i)[0]).material_id_saliency = num_class;
				num_class += 1;
			}
		}
	}

	for (unsigned int i = 0; i < material_queue.size(); i += 1){
		model->texcoords[2 * (int)material_queue.at(i)[0] + 0] = (float)model->cut_loop->at(material_queue.at(i)[0]).material_id_saliency / num_class;
		model->texcoords[2 * (int)material_queue.at(i)[0] + 1] = 0.0f;
	}

	for (unsigned int i = 0; i < model->numtriangles; i += 1){
		for (int j = 0; j < 3; j += 1){
			model->triangles->at(i).tindices[j] = model->triangles->at(i).vindices[j];
		}
	}

	glmWriteOBJ(model, my_strdup(std::string(model_file + "_saliency.obj").c_str()), GLM_TEXTURE);
}