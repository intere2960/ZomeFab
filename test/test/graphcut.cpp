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
#include <iostream>

using namespace std;

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
			if (!nhd->Adjacent(model->triangles->at(i).near_tri[j], i) && model->triangles->at(i).near_tri[j] != -1)
			{
				//cout << model->triangles->at(i).near_tri[j] << endl;
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

vec4 easy_plane(GLMmodel *model, int index1, int index2)
{
	vec3 p2(model->vertices->at(3 * index2 + 0), model->vertices->at(3 * index2 + 1), model->vertices->at(3 * index2 + 2));
	vec3 p1(model->vertices->at(3 * index1 + 0), model->vertices->at(3 * index1 + 1), model->vertices->at(3 * index1 + 2));

	vec3 n_p1(model->normals[3 * index1 + 0], model->normals[3 * index1 + 1], model->normals[3 * index1 + 2]);
	vec3 n_p2(model->normals[3 * index2 + 0], model->normals[3 * index2 + 1], model->normals[3 * index2 + 2]);

	vec3 v1 = (p2 - p1).normalize();
	vec3 v2 = ((n_p1 + n_p2) / 2.0).normalize();

	vec3 n_vector = (v1 ^ v2).normalize();
	float d = n_vector * p1;

	vec4 ans(n_vector, d);

	return ans;
}

#include <iostream>
using namespace std;

bool cut_plane_error(GLMmodel *model, std::vector<vec2> &partition_plane, std::vector<cut_plane> &plane_queue, std::vector<int> &material_point, std::vector<vec2> &error_info)
{
	bool ans = false;
	for (int i = 0; i < partition_plane.size(); i += 1){
		plane now_plane = plane_queue.at(partition_plane.at(i)[0]).split_plane;
		for (int j = 0; j < material_point.size(); j += 1){
			vec3 now_point(model->vertices->at(3 * material_point.at(j) + 0), model->vertices->at(3 * material_point.at(j) + 1), model->vertices->at(3 * material_point.at(j) + 2));
			//cout << i << " " << material_point.at(j) << " ";
			int dir = plane_dir_point(now_point, now_plane);
			if (dir == -1.0f * partition_plane.at(i)[1] && dir != 0){
				ans = true;
				//cout << i << " " << material_point.at(j) << " " << dir << endl;
				
				error_info.push_back(vec2(i, material_point.at(j)));
			}
		}
	}

	return ans;
}

void alter_plane(GLMmodel *model, std::vector<std::vector<vec2>> &partition_plane, int piece_id, std::vector<cut_plane> &plane_queue, std::vector<std::vector<int>> &material_point, std::vector<vec2> &error_info)
{
	std::vector<vec2> connect_info;
	
	cout << "all : ";
	for (int i = 0; i < partition_plane.at(piece_id).size(); i += 1){
		cout << partition_plane.at(piece_id).at(i)[0] << " ";
		cout << plane_queue.at(partition_plane.at(piece_id).at(i)[0]).use_vertex[0] << " " << plane_queue.at(partition_plane.at(piece_id).at(i)[0]).use_vertex[1] << endl;
		connect_info.push_back(vec2(plane_queue.at(partition_plane.at(piece_id).at(i)[0]).use_vertex[0], plane_queue.at(partition_plane.at(piece_id).at(i)[0]).use_vertex[1]));
	}
	cout << endl;
	//cout << "size : " << connect_info.size() << endl;

	std::vector<int> point_loop;
	int loop_pre_index = -1;
	int loop_next_index = connect_info.at(0)[1];
	int loop_end_index = connect_info.at(0)[0];
	point_loop.push_back(loop_next_index);

	while (loop_next_index != loop_end_index){
		//cout << "hello1" << endl;
		for (int i = 1; i < connect_info.size(); i += 1){
			if (connect_info.at(i)[0] == loop_next_index){
				if (connect_info.at(i)[1] != loop_pre_index){
					loop_pre_index = loop_next_index;
					loop_next_index = connect_info.at(i)[1];

					point_loop.push_back(loop_next_index);
					break;
				}
			}

			if (connect_info.at(i)[1] == loop_next_index){
				if (connect_info.at(i)[0] != loop_pre_index){
					loop_pre_index = loop_next_index;
					loop_next_index = connect_info.at(i)[0];

					point_loop.push_back(loop_next_index);
					break;
				}
			}
		}
	}

	/*for (int i = 0; i < point_loop.size(); i += 1){
		cout << point_loop.at(i) << " ";
	}
	cout << endl;*/

	std::vector<std::vector<int>> plane_error(partition_plane.at(piece_id).size());
	for (int i = 0; i < error_info.size(); i += 1){
		plane_error.at(error_info.at(i)[0]).push_back(error_info.at(i)[1]);
	}

	int min_error = -1;
	int min_error_number = 10000;
	for (int i = 0; i < plane_error.size(); i += 1){
		if (plane_error.at(i).size() != 0){
			if (plane_error.at(i).size() < min_error_number){
				min_error = i;
				min_error_number = plane_error.at(i).size();
			}
		}
	}

	bool done = false;
	//for (int i = 0; i < plane_error.size(); i += 1){
		/*cout << i << " => ";
		for (int j = 0; j < plane_error.at(i).size(); j += 1){
			cout << plane_error.at(i).at(j) << " ";
		}
		cout << endl;*/
	if (plane_error.at(min_error).size() != 0){
		for (int j = 0; j < 2; j += 1){
			//cout << "hello2" << endl;

			int start_point = plane_queue.at(partition_plane.at(piece_id).at(min_error)[0]).use_vertex[j];
			std::vector<bool> choose(point_loop.size());
			for (int k = 0; k < choose.size(); k += 1){
				choose.at(k) = true;
			}

			int now_index = find(point_loop.begin(), point_loop.end(), start_point) - point_loop.begin();
			int next_index = (now_index + 1) % point_loop.size();
			int pre_index = (now_index - 1 + point_loop.size()) % point_loop.size();

			choose.at(now_index) = false;
			choose.at(next_index) = false;
			choose.at(pre_index) = false;

			for (int k = 0; k < plane_error.at(min_error).size(); k += 1){
				int find_index = find(point_loop.begin(), point_loop.end(), plane_error.at(min_error).at(k)) - point_loop.begin();
				choose.at(find_index) = false;
			}

			//int new_plane_index = -1;
			std::vector<int> new_plane_index;
			for (int k = 0; k < choose.size(); k += 1){
				if (choose.at(k)){
					new_plane_index.push_back(point_loop.at(k));
					//cout << j << " " << point_loop.at(now_index) << " " << new_plane_index << endl;
					//break;
				}
			}
								
			if (new_plane_index.size() != 0){
				for (int a = 0; a < new_plane_index.size(); a += 1){
					vec4 new_plane_par = easy_plane(model, point_loop.at(now_index), new_plane_index.at(a));
					vec3 now_p1 = vec3(model->vertices->at(3 * plane_queue.at(partition_plane.at(piece_id).at(min_error)[0]).use_vertex[0] + 0), model->vertices->at(3 * plane_queue.at(partition_plane.at(piece_id).at(min_error)[0]).use_vertex[0] + 1), model->vertices->at(3 * plane_queue.at(partition_plane.at(piece_id).at(min_error)[0]).use_vertex[0] + 2));
					vec3 now_p2 = vec3(model->vertices->at(3 * plane_queue.at(partition_plane.at(piece_id).at(min_error)[0]).use_vertex[1] + 0), model->vertices->at(3 * plane_queue.at(partition_plane.at(piece_id).at(min_error)[0]).use_vertex[1] + 1), model->vertices->at(3 * plane_queue.at(partition_plane.at(piece_id).at(min_error)[0]).use_vertex[1] + 2));
					plane new_plane(new_plane_par[0], new_plane_par[1], new_plane_par[2], new_plane_par[3], 0);

					bool vaild = true;
					int error_dir;
					for (int k = 0; k < plane_error.at(min_error).size(); k += 1){
						vec3 error_p = vec3(model->vertices->at(3 * plane_error.at(min_error).at(k) + 0), model->vertices->at(3 * plane_error.at(min_error).at(k) + 1), model->vertices->at(3 * plane_error.at(min_error).at(k) + 2));
							
						int dir[3];

						dir[0] = plane_dir_point(now_p1, new_plane);
						dir[1] = plane_dir_point(now_p2, new_plane);
						dir[2] = plane_dir_point(error_p, new_plane);
						error_dir = dir[2];
						//cout << dir[0] << " " << dir[1] << " " << dir[2] << endl;

						if ((dir[0] == dir[2]) || (dir[1] == dir[2])){
							vaild = false;
							break;
						}
					}
						
					if (vaild){
						//cout << "fucking ya" << endl;

						cut_plane new_cut;
						new_cut.split_plane = new_plane;
						new_cut.use_vertex[0] = point_loop.at(now_index);
						new_cut.use_vertex[1] = new_plane_index.at(a);
						new_cut.normal_percent[0] = 0.5;
						new_cut.normal_percent[1] = 0.5;

						plane_queue.push_back(new_cut);

						std::vector<vec2> error_piece;
						std::vector<int> error_point;
							
						std::vector<vec2> normal_piece;
						std::vector<int> normal_point;

						error_piece.push_back(vec2(plane_queue.size() - 1, error_dir));
						error_point.push_back(point_loop.at(now_index));
						error_point.push_back(new_plane_index.at(a));

						normal_piece.push_back(vec2(plane_queue.size() - 1, error_dir * -1));
						normal_point.push_back(point_loop.at(now_index));
						normal_point.push_back(new_plane_index.at(a));

						for (int k = 0; k < partition_plane.at(piece_id).size(); k += 1){
							int test_index1 = plane_queue.at(partition_plane.at(piece_id).at(k)[0]).use_vertex[0];
							vec3 test_point1 = vec3(model->vertices->at(3 * test_index1 + 0), model->vertices->at(3 * test_index1 + 1), model->vertices->at(3 * test_index1 + 2));
							int test_index2 = plane_queue.at(partition_plane.at(piece_id).at(k)[0]).use_vertex[1];
							vec3 test_point2 = vec3(model->vertices->at(3 * test_index2 + 0), model->vertices->at(3 * test_index2 + 1), model->vertices->at(3 * test_index2 + 2));

							int dir[2];
							dir[0] = plane_dir_point(test_point1, new_plane);
							dir[1] = plane_dir_point(test_point2, new_plane);

							if ((dir[0] + dir[1] == error_dir) || (dir[0] + dir[1] == 2 * error_dir)){
								error_piece.push_back(partition_plane.at(piece_id).at(k));

								if (find(error_point.begin(), error_point.end(), test_index1) - error_point.begin() >= error_point.size()){
									error_point.push_back(test_index1);
								}
								if (find(error_point.begin(), error_point.end(), test_index2) - error_point.begin() >= error_point.size()){
									error_point.push_back(test_index2);
								}
							}
							else{
								normal_piece.push_back(partition_plane.at(piece_id).at(k));

								if (find(normal_point.begin(), normal_point.end(), test_index1) - normal_point.begin() >= normal_point.size()){
									normal_point.push_back(test_index1);
								}
								if (find(normal_point.begin(), normal_point.end(), test_index2) - normal_point.begin() >= normal_point.size()){
									normal_point.push_back(test_index2);
								}
							}
						}

						/*cout << "error : ";
						for (int k = 0; k < error_piece.size(); k += 1){
							cout << error_piece.at(k)[0] << " ";
						}
						cout << endl;

						cout << "normal : ";
						for (int k = 0; k < normal_piece.size(); k += 1){
							cout << normal_piece.at(k)[0] << " ";
						}
						cout << endl;

						cout << "error : ";
						for (int k = 0; k < error_point.size(); k += 1){
							cout << error_point.at(k) << " ";
						}
						cout << endl;

						cout << "normal : ";
						for (int k = 0; k < normal_point.size(); k += 1){
							cout << normal_point.at(k) << " ";
						}
						cout << endl;*/

						std::vector<vec2> error_info_error;
						std::vector<vec2> error_info_normal;

						cut_plane_error(model, error_piece, plane_queue, error_point, error_info_error);
						cut_plane_error(model, normal_piece, plane_queue, normal_point, error_info_normal);

						if ((error_info_error.size() <= error_point.size() - 3) && (error_info_normal.size() <= normal_point.size() - 3)){
							partition_plane.push_back(error_piece);
							partition_plane.push_back(normal_piece);

							material_point.push_back(error_point);
							material_point.push_back(normal_point);

							done = true;
							cout << "yaya" << endl;
							break;
						}
						else{
							plane_queue.erase(plane_queue.begin() + plane_queue.size() - 1);
							cout << "fuck" << endl;
						}
					}
				}					
			}

			if (done)
				break;
		}		
	}

		//if (done)
		//	break;
	//}

	//return true;
}