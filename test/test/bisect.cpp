#include <algorithm>
#include <fstream>
#include "glm.h"
#include "bisect.h"
#include "zomestruc.h"
#include "fill.h"
#include "global.h"

neighbor_record::neighbor_record(std::vector<std::vector<plane>> &all_planes, std::string neighbor_file)
{
	std::ifstream neighbor(neighbor_file);
	neighbor_info.resize(all_planes.size());
	neighbor_table.resize(all_planes.size());
	int temp_ne;
	for (unsigned int i = 0; i < all_planes.size(); i += 1){
		for (unsigned int j = 0; j < all_planes.size(); j += 1){
			neighbor >> temp_ne;
			neighbor_table.at(i).push_back(temp_ne);
			if (temp_ne > 2){
				neighbor_info.at(i).push_back(j);
			}
		}
	}
	neighbor.close();

	for (unsigned int i = 0; i < all_planes.size(); i += 1){

		std::vector<std::vector<int>> temp_info(all_planes.at(i).size());

		for (unsigned int j = 0; j < neighbor_info.at(i).size(); j += 1){

			int current_index = neighbor_info.at(i).at(j);

			for (unsigned int k = 0; k < neighbor_info.at(i).size(); k += 1){
				if (j == k)
					continue;

				int check_index = neighbor_info.at(i).at(k);
				if (neighbor_table.at(current_index).at(check_index) > 2){
					temp_info.at(j).push_back(k);
				}
			}
		}
		plane_neighbor.push_back(temp_info);
	}

	for (unsigned int i = 0; i < all_planes.size(); i += 1){
		vec3 sum(0.0f, 0.0f, 0.0f);
		int num = 0;
		std::string temp_file = std::to_string(i) + ".txt";

		std::ifstream is(temp_file);
		vec3 temp_read;
		while (is >> temp_read[0] >> temp_read[1] >> temp_read[2]){
			sum += temp_read;
			num += 1;
		}
		is.close();

		sum /= (float)num;

		piece_center.push_back(sum);
	}
}

void recount_normal(GLMmodel *myObj)
{
	glmFacetNormals(myObj);

	std::vector<int> *point_tri = new std::vector<int>[myObj->numvertices + 1];

	for (unsigned int i = 0; i < myObj->numtriangles; i += 1)
	{
		for (int j = 0; j < 3; j += 1)
		{
			bool add = true;
			for (unsigned int k = 0; k < point_tri[myObj->triangles->at(i).vindices[j]].size(); k += 1)
			{
				GLfloat *temp = &myObj->facetnorms->at(3 * point_tri[myObj->triangles->at(i).vindices[j]][k]);
				if (*temp == myObj->facetnorms->at(3 * (i + 1) + 0) && *(temp + 1) == myObj->facetnorms->at(3 * (i + 1) + 1) && *(temp + 2) == myObj->facetnorms->at(3 * (i + 1) + 2))
				{
					add = false;
					break;
				}
			}
			if (add)
				point_tri[myObj->triangles->at(i).vindices[j]].push_back(i + 1);
		}
	}

	for (unsigned int i = 1; i <= myObj->numvertices; i += 1)
	{
		sort(point_tri[i].begin(), point_tri[i].begin() + point_tri[i].size());
	}

	myObj->numnormals = myObj->numvertices;
	myObj->normals = new GLfloat[3 * (myObj->numnormals + 1)];
	for (unsigned int i = 1; i <= myObj->numnormals; i += 1)
	{
		GLfloat temp[3] = { 0.0, 0.0, 0.0 };
		for (unsigned int j = 0; j < point_tri[i].size(); j += 1)
		{
			temp[0] += myObj->facetnorms->at(3 * point_tri[i].at(j) + 0);
			temp[1] += myObj->facetnorms->at(3 * point_tri[i].at(j) + 1);
			temp[2] += myObj->facetnorms->at(3 * point_tri[i].at(j) + 2);
		}
		GLfloat normal_length = sqrt(pow(temp[0], 2) + pow(temp[1], 2) + pow(temp[2], 2));
		temp[0] /= normal_length;
		temp[1] /= normal_length;
		temp[2] /= normal_length;

		myObj->normals[3 * i + 0] = temp[0];
		myObj->normals[3 * i + 1] = temp[1];
		myObj->normals[3 * i + 2] = temp[2];
	}
}

void combine_inner_outfit(GLMmodel *myObj, GLMmodel *myObj_inner)
{
	for (unsigned int i = 0; i < myObj_inner->numtriangles; i += 1){
		GLMtriangle temp = myObj_inner->triangles->at(i);
		temp.vindices[0] += myObj->numvertices;
		temp.vindices[1] += myObj->numvertices;
		temp.vindices[2] += myObj->numvertices;
		temp.findex += myObj->numfacetnorms;
		temp.tag = INNER;
		myObj->triangles->push_back(temp);
		myObj->numtriangles += 1;
	}

	for (unsigned int i = 1; i <= myObj_inner->numvertices; i += 1){
		myObj->vertices->push_back(myObj_inner->vertices->at(3 * i + 0));
		myObj->vertices->push_back(myObj_inner->vertices->at(3 * i + 1));
		myObj->vertices->push_back(myObj_inner->vertices->at(3 * i + 2));
		myObj->numvertices += 1;

		vertex new_vertex;
		myObj->cut_loop->push_back(new_vertex);
	}

	for (unsigned int i = 1; i <= myObj_inner->numfacetnorms; i += 1){
		myObj->facetnorms->push_back(myObj_inner->facetnorms->at(3 * i + 0));
		myObj->facetnorms->push_back(myObj_inner->facetnorms->at(3 * i + 1));
		myObj->facetnorms->push_back(myObj_inner->facetnorms->at(3 * i + 2));
		myObj->numfacetnorms += 1;
	}
}

void vertex_use_tri(GLMmodel *model)
{
	for (unsigned int i = 0; i < model->numtriangles; i += 1){
		model->cut_loop->at(model->triangles->at(i).vindices[0]).tri_use.push_back(i);
		model->cut_loop->at(model->triangles->at(i).vindices[1]).tri_use.push_back(i);
		model->cut_loop->at(model->triangles->at(i).vindices[2]).tri_use.push_back(i);
	}
}

void cut_intersection(GLMmodel *model, std::vector<plane> &planes, std::vector<int> &face_split_by_plane, int piece_id, bool have_dir)
{
    if(!face_split_by_plane.empty()){
        std::vector<int> v;
        face_split_by_plane.swap(v);
    }

    for(unsigned int i = 0; i < model->numtriangles; i += 1){
        vec3 point_dir;
        int dir_count[3] = {0, 0, 0};
        for(int j = 0; j < 3; j += 1){
            vec3 temp(model->vertices->at(3 * (model->triangles->at(i).vindices[j]) + 0), model->vertices->at(3 * (model->triangles->at(i).vindices[j]) + 1), model->vertices->at(3 * (model->triangles->at(i).vindices[j]) + 2));
            point_dir[j] = plane_dir_point(temp, planes.at(0));
            dir_count[(int)point_dir[j] + 1] += 1;
        }
        if((dir_count[(planes.at(0).dir + 2) % 3] + dir_count[(planes.at(0).dir + 3) % 3]) != 3){
            face_split_by_plane.push_back(i);
            model->triangles->at(i).split_by_process = true;
            if(!model->triangles->at(i).split_plane_id.empty()){
                std::vector<unsigned int> v;
                model->triangles->at(i).split_plane_id.swap(v);
            }

			if (!((dir_count[(planes.at(0).dir) + 1] == 3)))
				model->triangles->at(i).split_plane_id.push_back(0);
        }
    }

    for(unsigned int i = 1; i < planes.size(); i += 1){
        for(unsigned int j = 0; j < face_split_by_plane.size(); j += 1){
            vec3 point_dir;
            int dir_count[3] = {0, 0, 0};
            for(int k = 0; k < 3; k += 1){
                vec3 temp(model->vertices->at(3 * (model->triangles->at(face_split_by_plane.at(j)).vindices[k]) + 0), model->vertices->at(3 * (model->triangles->at(face_split_by_plane.at(j)).vindices[k]) + 1), model->vertices->at(3 * (model->triangles->at(face_split_by_plane.at(j)).vindices[k]) + 2));
                point_dir[k] = plane_dir_point(temp, planes.at(i));
                dir_count[(int)point_dir[k] + 1] += 1;
            }

            if(dir_count[planes.at(i).dir + 1] == 0 && dir_count[1] == 0){
                if(!model->triangles->at(face_split_by_plane.at(j)).split_plane_id.empty()){
                    std::vector<unsigned int> v;
                    model->triangles->at(face_split_by_plane.at(j)).split_plane_id.swap(v);
                }
                model->triangles->at(face_split_by_plane.at(j)).split_by_process = false;
                face_split_by_plane.erase(face_split_by_plane.begin() + j);
                j -= 1;
            }
            else if(dir_count[(planes.at(i).dir) * (-1) + 1] + dir_count[1] == 3){
                if(!model->triangles->at(face_split_by_plane.at(j)).split_plane_id.empty()){
                    std::vector<unsigned int> v;
                    model->triangles->at(face_split_by_plane.at(j)).split_plane_id.swap(v);
                }
                model->triangles->at(face_split_by_plane.at(j)).split_by_process = false;
                face_split_by_plane.erase(face_split_by_plane.begin() + j);
                j -= 1;
            }
            else if(have_dir && planes.size() >= 2 && i == planes.size() - 1 && dir_count[planes.at(i).dir + 1] != 3){
                if(!model->triangles->at(face_split_by_plane.at(j)).split_plane_id.empty()){
                    std::vector<unsigned int> v;
                    model->triangles->at(face_split_by_plane.at(j)).split_plane_id.swap(v);
                }
                model->triangles->at(face_split_by_plane.at(j)).split_by_process = false;
                face_split_by_plane.erase(face_split_by_plane.begin() + j);
                j -= 1;
            }
            else{
                if(i != planes.size() - 1){
					if (!((dir_count[(planes.at(i).dir) + 1] == 3)))
						model->triangles->at(face_split_by_plane.at(j)).split_plane_id.push_back(i);
                }
                else{
                    if(!have_dir)
						if (!((dir_count[(planes.at(i).dir) + 1] == 3)))
							model->triangles->at(face_split_by_plane.at(j)).split_plane_id.push_back(i);
                }
            }
        }
    }

}

void hole_split(GLMmodel *model, std::vector<edge> &all_edge, std::vector<plane> &planes, std::vector<std::vector<int>> &arrange_group)
{
	
	for (unsigned int i = 0; i < arrange_group.size(); i += 1){
		
		std::vector<int> align_plane_id; 
		for (unsigned int j = 0; j < planes.size(); j += 1){
			for (unsigned int k = 0; k < arrange_group.at(i).size(); k += 1){
			
				vec3 point_dir;
				int dir_count[3] = { 0, 0, 0 };
				for (int a = 0; a < 3; a += 1){
					vec3 temp(model->vertices->at(3 * (model->triangles->at(arrange_group.at(i).at(k)).vindices[a]) + 0), model->vertices->at(3 * (model->triangles->at(arrange_group.at(i).at(k)).vindices[a]) + 1), model->vertices->at(3 * (model->triangles->at(arrange_group.at(i).at(k)).vindices[a]) + 2));
					point_dir[a] = plane_dir_point(temp, planes.at(j));
					dir_count[(int)point_dir[a] + 1] += 1;
				}

				if (dir_count[1] == 2){
					align_plane_id.push_back(j);
					break;
				}
			}
		}

		bool split = false;
		for (unsigned int j = 0; j < align_plane_id.size(); j += 1){
			for (unsigned int k = 0; k < arrange_group.at(i).size(); k += 1){

				vec3 point_dir;
				int dir_count[3] = { 0, 0, 0 };
				for (int a = 0; a < 3; a += 1){
					vec3 temp(model->vertices->at(3 * (model->triangles->at(arrange_group.at(i).at(k)).vindices[a]) + 0), model->vertices->at(3 * (model->triangles->at(arrange_group.at(i).at(k)).vindices[a]) + 1), model->vertices->at(3 * (model->triangles->at(arrange_group.at(i).at(k)).vindices[a]) + 2));
					point_dir[a] = plane_dir_point(temp, planes.at(align_plane_id.at(j)));
					dir_count[(int)point_dir[a] + 1] += 1;
				}

				if ((dir_count[planes.at(align_plane_id.at(j)).dir + 1] != 0) && (dir_count[planes.at(align_plane_id.at(j)).dir * (-1) + 1] != 0)){
					model->triangles->at(arrange_group.at(i).at(k)).split_plane_id.push_back(align_plane_id.at(j));
					split = true;
				}
			}
		}

		if (split){
			for (unsigned int k = 0; k < arrange_group.at(i).size(); k += 1){
				model->triangles->at(arrange_group.at(i).at(k)).split_by_process = true;
			}

			split_face(model, all_edge, arrange_group.at(i), planes);

			for (unsigned int j = 0; j < model->cut_loop->size(); j += 1){
				model->cut_loop->at(j).connect_edge.erase(model->cut_loop->at(j).connect_edge.begin(), model->cut_loop->at(j).connect_edge.begin() + model->cut_loop->at(j).connect_edge.size());
				model->cut_loop->at(j).align_plane.erase(model->cut_loop->at(j).align_plane.begin(), model->cut_loop->at(j).align_plane.begin() + model->cut_loop->at(j).align_plane.size());
			}

			for (unsigned int j = 0; j < model->numtriangles; j += 1){
				if (!model->triangles->at(j).split_plane_id.empty()){
					std::vector<unsigned int> v;
					model->triangles->at(j).split_plane_id.swap(v);
				}

				model->triangles->at(j).split_by_process = false;
			}

			find_near_tri(model, all_edge);

			for (unsigned int j = 0; j < arrange_group.at(i).size(); j += 1){
				for (int k = 0; k < 3; k += 1){
					if (model->triangles->at(model->triangles->at(arrange_group.at(i).at(j)).near_tri[k]).material_id_graph_cut == -1){
						int find_index = find(arrange_group.at(i).begin(), arrange_group.at(i).end(), model->triangles->at(arrange_group.at(i).at(j)).near_tri[k]) - arrange_group.at(i).begin(); 
						if (find_index >= arrange_group.at(i).size()){
							arrange_group.at(i).push_back(model->triangles->at(arrange_group.at(i).at(j)).near_tri[k]);
						}
					}
				}
			}
		}
	}
}

void collect_vanish_face(GLMmodel *model, std::vector<edge> &all_edge, std::vector<plane> &planes, std::vector<int> &face_split_by_plane, std::vector<std::vector<int>> &arrange_group, int piece_id)
{
	for (unsigned int i = 0; i < planes.size(); i += 1){
		for (unsigned int j = 0; j < arrange_group.size(); j += 1){
			for (unsigned int k = 0; k < arrange_group.at(j).size(); k += 1){
				vec3 point_dir;
				int dir_count[3] = { 0, 0, 0 };
				for (int a = 0; a < 3; a += 1){
					vec3 temp(model->vertices->at(3 * (model->triangles->at(arrange_group.at(j).at(k)).vindices[a]) + 0), model->vertices->at(3 * (model->triangles->at(arrange_group.at(j).at(k)).vindices[a]) + 1), model->vertices->at(3 * (model->triangles->at(arrange_group.at(j).at(k)).vindices[a]) + 2));
					point_dir[a] = plane_dir_point(temp, planes.at(i));
					dir_count[(int)point_dir[a] + 1] += 1;
				}

				if (dir_count[1] == 2){
					model->triangles->at(arrange_group.at(j).at(k)).split_plane_id.push_back(i);
				}
			}
		}
	}

	std::vector<int> choose_hole;
	for (unsigned int i = 0; i < arrange_group.size(); i += 1){
		std::vector<int> align_face;
		for (unsigned int j = 0; j < arrange_group.at(i).size(); j += 1){
			if (model->triangles->at(arrange_group.at(i).at(j)).split_plane_id.size() > 0){

				if (model->triangles->at(model->triangles->at(arrange_group.at(i).at(j)).near_tri[0]).material_id_graph_cut == piece_id){
					choose_hole.push_back(i);
					break;
				}
				
				if (model->triangles->at(model->triangles->at(arrange_group.at(i).at(j)).near_tri[1]).material_id_graph_cut == piece_id){
					choose_hole.push_back(i);
					break;
				}

				if (model->triangles->at(model->triangles->at(arrange_group.at(i).at(j)).near_tri[2]).material_id_graph_cut == piece_id){
					choose_hole.push_back(i);
					break;
				}
			}
		}
	}

	for (unsigned int i = 0; i < choose_hole.size(); i += 1){
		std::vector<int> use_vertex;
		for (unsigned int j = 0; j < arrange_group.at(choose_hole.at(i)).size(); j += 1){
			for (int k = 0; k < 3; k += 1){
				int edge_index = model->triangles->at(arrange_group.at(choose_hole.at(i)).at(j)).edge_index[k];
				int dir[2];
				for (unsigned int a = 0; a < planes.size(); a += 1){
					plane_dir_edge(all_edge.at(edge_index), planes.at(a), dir);

					if (dir[0] == 0 && dir[1] == 0){

						if ((find(model->cut_loop->at(all_edge.at(edge_index).index[0]).connect_edge.begin(), model->cut_loop->at(all_edge.at(edge_index).index[0]).connect_edge.end(), edge_index) - model->cut_loop->at(all_edge.at(edge_index).index[0]).connect_edge.begin()) >= model->cut_loop->at(all_edge.at(edge_index).index[0]).connect_edge.size()){
							model->cut_loop->at(all_edge.at(edge_index).index[0]).connect_edge.push_back(edge_index);
						}

						if ((find(model->cut_loop->at(all_edge.at(edge_index).index[1]).connect_edge.begin(), model->cut_loop->at(all_edge.at(edge_index).index[1]).connect_edge.end(), edge_index) - model->cut_loop->at(all_edge.at(edge_index).index[1]).connect_edge.begin()) >= model->cut_loop->at(all_edge.at(edge_index).index[1]).connect_edge.size()){
							model->cut_loop->at(all_edge.at(edge_index).index[1]).connect_edge.push_back(edge_index);
						}						

						if ((find(use_vertex.begin(), use_vertex.end(), all_edge.at(edge_index).index[0]) - use_vertex.begin()) >= use_vertex.size()){
							use_vertex.push_back(all_edge.at(edge_index).index[0]);
						}

						if ((find(use_vertex.begin(), use_vertex.end(), all_edge.at(edge_index).index[1]) - use_vertex.begin()) >= use_vertex.size()){
							use_vertex.push_back(all_edge.at(edge_index).index[1]);
						}
					}
				}
			}
		}

		bool delete_hole = true;
		int start_index = -1;
		for (unsigned int j = 0; j < use_vertex.size(); j += 1){
			if (model->cut_loop->at(use_vertex.at(j)).connect_edge.size() >= 2){
				start_index = use_vertex.at(j);
				break;
			}
		}

		std::vector<int> delete_vertex;
		std::vector<int> trace_path;

		if (start_index != -1){
			trace_path.push_back(start_index);
			int current_index = start_index;
			
			while (true){
				bool stop = true;

				for (unsigned int j = 0; j < model->cut_loop->at(current_index).connect_edge.size(); j += 1){
					if (all_edge.at(model->cut_loop->at(current_index).connect_edge.at(j)).index[0] == current_index){
						if (model->cut_loop->at(all_edge.at(model->cut_loop->at(current_index).connect_edge.at(j)).index[1]).connect_edge.size() >= 2){
							int temp_index = all_edge.at(model->cut_loop->at(current_index).connect_edge.at(j)).index[1];
							
							if ((find(trace_path.begin(), trace_path.end(), temp_index) - trace_path.begin()) >= trace_path.size()){
								current_index = temp_index;
								trace_path.push_back(temp_index);
								stop = false;
								break;
							}

							if ((temp_index == start_index) && (trace_path.size() > 2)){
								delete_hole = false;
								break;
							}
						}
						else{
							delete_vertex.push_back(all_edge.at(model->cut_loop->at(current_index).connect_edge.at(j)).index[1]);
						}
					}
					else{
						if (model->cut_loop->at(all_edge.at(model->cut_loop->at(current_index).connect_edge.at(j)).index[0]).connect_edge.size() >= 2){
							int temp_index = all_edge.at(model->cut_loop->at(current_index).connect_edge.at(j)).index[0];
							
							if ((find(trace_path.begin(), trace_path.end(), temp_index) - trace_path.begin()) >= trace_path.size()){
								current_index = temp_index;
								trace_path.push_back(temp_index);
								stop = false;
								break;
							}

							if ((temp_index == start_index) && (trace_path.size() > 2)){
								delete_hole = false;
								break;
							}
						}
						else{
							delete_vertex.push_back(all_edge.at(model->cut_loop->at(current_index).connect_edge.at(j)).index[1]);
						}
					}
				}

				if (stop)
					break;
			}
		}

		if (delete_hole){
			choose_hole.erase(choose_hole.begin() + i);
			i -= 1;
		}
		else{
			if (delete_vertex.size() != 0){
				for (unsigned int j = 0; j < delete_vertex.size(); j += 1){
					int start_delete_tri = -1;
					for (unsigned int k = 0; k < arrange_group.at(choose_hole.at(i)).size(); k += 1){
						if (model->triangles->at(arrange_group.at(choose_hole.at(i)).at(k)).vindices[0] == delete_vertex.at(j)){
							start_delete_tri = arrange_group.at(choose_hole.at(i)).at(k);
							break;
						}
						if (model->triangles->at(arrange_group.at(choose_hole.at(i)).at(k)).vindices[1] == delete_vertex.at(j)){
							start_delete_tri = arrange_group.at(choose_hole.at(i)).at(k);
							break;
						}
						if (model->triangles->at(arrange_group.at(choose_hole.at(i)).at(k)).vindices[2] == delete_vertex.at(j)){
							start_delete_tri = arrange_group.at(choose_hole.at(i)).at(k);
							break;
						}
					}

					if (start_delete_tri != -1){
						std::vector<int> delete_trace;
						delete_trace.push_back(start_delete_tri);

						for (unsigned int k = 0; k < delete_trace.size(); k += 1){
							for (int a = 0; a < 3; a += 1){
								int edge_index = model->triangles->at(delete_trace.at(k)).edge_index[a];
								bool judge1 = (find(trace_path.begin(), trace_path.end(), all_edge.at(edge_index).index[0]) - trace_path.begin()) >= trace_path.size();
								bool judge2 = (find(trace_path.begin(), trace_path.end(), all_edge.at(edge_index).index[1]) - trace_path.begin()) >= trace_path.size();

								if (judge1 || judge2){
									if (all_edge.at(edge_index).face_id[0] == delete_trace.at(k)){
										int temp_face = all_edge.at(edge_index).face_id[1];
										if (model->triangles->at(temp_face).material_id_graph_cut == -1){
											if ((find(delete_trace.begin(), delete_trace.end(), temp_face) - delete_trace.begin()) >= delete_trace.size()){
												delete_trace.push_back(temp_face);
											}
										}
									}
									else{
										int temp_face = all_edge.at(edge_index).face_id[0];
										if (model->triangles->at(temp_face).material_id_graph_cut == -1){
											if ((find(delete_trace.begin(), delete_trace.end(), temp_face) - delete_trace.begin()) >= delete_trace.size()){
												delete_trace.push_back(temp_face);
											}
										}
									}
								}
							}
						}

						arrange_group.push_back(delete_trace);

						for (unsigned int k = 0; k < delete_trace.size(); k += 1){
							int erase_index = find(arrange_group.at(choose_hole.at(i)).begin(), arrange_group.at(choose_hole.at(i)).end(), delete_trace.at(k)) - arrange_group.at(choose_hole.at(i)).begin();
							if (erase_index < arrange_group.at(choose_hole.at(i)).size()){
								arrange_group.at(choose_hole.at(i)).erase(arrange_group.at(choose_hole.at(i)).begin() + erase_index);
							}
						}

						for (unsigned int k = 0; k < delete_vertex.size(); k += 1){
							for (unsigned int a = 0; a < delete_trace.size(); a += 1){
								if (model->triangles->at(delete_trace.at(a)).vindices[0] == delete_vertex.at(k)){
									delete_vertex.erase(delete_vertex.begin() + k);
									k -= 1;
									break;
								}

								if (model->triangles->at(delete_trace.at(a)).vindices[1] == delete_vertex.at(k)){
									delete_vertex.erase(delete_vertex.begin() + k);
									k -= 1;
									break;
								}

								if (model->triangles->at(delete_trace.at(a)).vindices[2] == delete_vertex.at(k)){
									delete_vertex.erase(delete_vertex.begin() + k);
									k -= 1;
									break;
								}
							}
						}
					}
				}
			}
		}
	}

	for (unsigned int i = 0; i < choose_hole.size(); i += 1){
		for (unsigned int j = 0; j < arrange_group.at(choose_hole.at(i)).size(); j += 1){
			model->triangles->at(arrange_group.at(choose_hole.at(i)).at(j)).material_id_graph_cut = piece_id;

			face_split_by_plane.push_back(arrange_group.at(choose_hole.at(i)).at(j));

			if (!model->triangles->at(arrange_group.at(choose_hole.at(i)).at(j)).split_plane_id.empty()){
				std::vector<unsigned int> v;
				model->triangles->at(arrange_group.at(choose_hole.at(i)).at(j)).split_plane_id.swap(v);
			}

			model->triangles->at(arrange_group.at(choose_hole.at(i)).at(j)).split_by_process = true;
		}
	}
}

bool split_edge(GLMmodel *model, std::vector<edge> &all_edge,int split_tri_id, plane plane)
{
    bool judge = false;
    for(unsigned int i = 0; i < 3; i +=1){

        int dir[2];
        float dist[2];
        plane_dir_edge(all_edge.at(model->triangles->at(split_tri_id).edge_index[i]), plane, dir);
        plane_dist_edge(all_edge.at(model->triangles->at(split_tri_id).edge_index[i]), plane, dist);

        if(all_edge.at(model->triangles->at(split_tri_id).edge_index[i]).connect_index != -1){
			if (dir[0] != plane.dir || dir[1] != plane.dir){
				judge = true;
			}
        }
        else{
            if(dir[0] && dir[1] && (dir[0] != dir[1])){
                if(all_edge.at(model->triangles->at(split_tri_id).edge_index[i]).vertex_push_index == -1){
                    float edge_ratio = dist[0] / (dist[0] + dist[1]);
                    vec3 new_point = all_edge.at(model->triangles->at(split_tri_id).edge_index[i]).point[0] + edge_ratio * (all_edge.at(model->triangles->at(split_tri_id).edge_index[i]).point[1] - all_edge.at(model->triangles->at(split_tri_id).edge_index[i]).point[0]);
                    all_edge.at(model->triangles->at(split_tri_id).edge_index[i]).split_point = new_point;
                }
                judge = true;
            }
			else if ((dir[0] == 0 || dir[1] == 0) && (dir[0] != dir[1])){

                vec3 face_vertex1[3];
                for(int j = 0; j < 3; j += 1){
                    for(int k = 0; k < 3; k += 1)
                        face_vertex1[j][k] = model->vertices->at(3 * model->triangles->at(split_tri_id).vindices[j] + k);
                }

                int dir_count1[3] = {0, 0, 0};
                dir_count1[(int)plane_dir_point(face_vertex1[0], plane) + 1] += 1;
                dir_count1[(int)plane_dir_point(face_vertex1[1], plane) + 1] += 1;
                dir_count1[(int)plane_dir_point(face_vertex1[2], plane) + 1] += 1;

                if(dir_count1[0] == 1 && dir_count1[1] == 1 && dir_count1[2] == 1){
                    if(dir[0] == 0){
                        all_edge.at(model->triangles->at(split_tri_id).edge_index[i]).split_point = all_edge.at(model->triangles->at(split_tri_id).edge_index[i]).point[0];
                        all_edge.at(model->triangles->at(split_tri_id).edge_index[i]).vertex_push_index = all_edge.at(model->triangles->at(split_tri_id).edge_index[i]).index[0];
                    }
                    else if(dir[1] == 0){
                        all_edge.at(model->triangles->at(split_tri_id).edge_index[i]).split_point = all_edge.at(model->triangles->at(split_tri_id).edge_index[i]).point[1];
                        all_edge.at(model->triangles->at(split_tri_id).edge_index[i]).vertex_push_index = all_edge.at(model->triangles->at(split_tri_id).edge_index[i]).index[1];
                    }
                    judge = true;
                }
            }
			else if ((dir[0] == 0 || dir[1] == 0) && (dir[0] == dir[1])){
				if (find(model->cut_loop->at(all_edge.at(model->triangles->at(split_tri_id).edge_index[i]).index[0]).connect_edge.begin(), model->cut_loop->at(all_edge.at(model->triangles->at(split_tri_id).edge_index[i]).index[0]).connect_edge.end(), model->triangles->at(split_tri_id).edge_index[i]) - model->cut_loop->at(all_edge.at(model->triangles->at(split_tri_id).edge_index[i]).index[0]).connect_edge.begin() >= model->cut_loop->at(all_edge.at(model->triangles->at(split_tri_id).edge_index[i]).index[0]).connect_edge.size())
					model->cut_loop->at(all_edge.at(model->triangles->at(split_tri_id).edge_index[i]).index[0]).connect_edge.push_back(model->triangles->at(split_tri_id).edge_index[i]);
				if (find(model->cut_loop->at(all_edge.at(model->triangles->at(split_tri_id).edge_index[i]).index[1]).connect_edge.begin(), model->cut_loop->at(all_edge.at(model->triangles->at(split_tri_id).edge_index[i]).index[1]).connect_edge.end(), model->triangles->at(split_tri_id).edge_index[i]) - model->cut_loop->at(all_edge.at(model->triangles->at(split_tri_id).edge_index[i]).index[1]).connect_edge.begin() >= model->cut_loop->at(all_edge.at(model->triangles->at(split_tri_id).edge_index[i]).index[1]).connect_edge.size())
					model->cut_loop->at(all_edge.at(model->triangles->at(split_tri_id).edge_index[i]).index[1]).connect_edge.push_back(model->triangles->at(split_tri_id).edge_index[i]);
			}
        }
    }

    return judge;
}

void split_face(GLMmodel *model, std::vector<edge> &all_edge, std::vector<int> &face_split_by_plane, std::vector<plane> &planes)
{
    unsigned int current = face_split_by_plane.size();
    std::vector<int> edge_split_id;
    std::vector<int> all_process_face;
    int plane_process_time = 0;
    int end_time;

	for (unsigned int plane_count = 0; plane_count < planes.size(); plane_count += 1){
		for (unsigned int i = 0; i < current; i += 1){

			if (model->triangles->at(face_split_by_plane.at(i)).split_plane_id.size() == 0){
				face_split_by_plane.push_back(face_split_by_plane.at(i));
			}
			else{
				int cut_plane = plane_count;
				plane test_plane = planes.at(plane_count);

				int find_plane = find(model->triangles->at(face_split_by_plane.at(i)).split_plane_id.begin(), model->triangles->at(face_split_by_plane.at(i)).split_plane_id.end(), plane_count) - model->triangles->at(face_split_by_plane.at(i)).split_plane_id.begin();
				
				bool test = (find_plane < model->triangles->at(face_split_by_plane.at(i)).split_plane_id.size()) && (split_edge(model, all_edge, face_split_by_plane.at(i), test_plane));

				vec3 point_dir;
				int dir_count[3] = { 0, 0, 0 };
				for (int j = 0; j < 3; j += 1){
					vec3 temp(model->vertices->at(3 * (model->triangles->at(face_split_by_plane.at(i)).vindices[j]) + 0), model->vertices->at(3 * (model->triangles->at(face_split_by_plane.at(i)).vindices[j]) + 1), model->vertices->at(3 * (model->triangles->at(face_split_by_plane.at(i)).vindices[j]) + 2));
					point_dir[j] = plane_dir_point(temp, test_plane);
					dir_count[(int)point_dir[j] + 1] += 1;
				}

				if (!test){
					face_split_by_plane.push_back(face_split_by_plane.at(i));
				}
				else{model->triangles->at(face_split_by_plane.at(i)).split_plane_id.erase(model->triangles->at(face_split_by_plane.at(i)).split_plane_id.begin() + find_plane);
					
					int piece_id = model->triangles->at(face_split_by_plane.at(i)).material_id_graph_cut;

					int tag = model->triangles->at(face_split_by_plane.at(i)).tag;

					int choose_dir = -1;
					if (dir_count[0] == 1 && dir_count[1] == 1 && dir_count[2] == 1)
						choose_dir = 0;
					else if (dir_count[2] == 1)
						choose_dir = 1;

					int choose_index = 0;
					for (int j = 0; j < 3; j += 1){
						if (point_dir[j] == choose_dir)
							choose_index = j;
					}

					vec3 choose_edge(-1, -1, -1);
					vec2 choose_vertex_index(-1, -1);
					vec2 vertex_index(-1, -1);
					int one_split = 3;

					for (int j = 0; j < 3; j += 1){
						bool judge1, judge2;
						if (all_edge.at(model->triangles->at(face_split_by_plane.at(i)).edge_index[j]).connect_index == -1){
							judge1 = (model->triangles->at(face_split_by_plane.at(i)).vindices[choose_index] == (unsigned int)all_edge.at(model->triangles->at(face_split_by_plane.at(i)).edge_index[j]).index[0]) && (model->triangles->at(face_split_by_plane.at(i)).vindices[(choose_index + 1) % 3] == (unsigned int)all_edge.at(model->triangles->at(face_split_by_plane.at(i)).edge_index[j]).index[1]);
							judge2 = (model->triangles->at(face_split_by_plane.at(i)).vindices[choose_index] == (unsigned int)all_edge.at(model->triangles->at(face_split_by_plane.at(i)).edge_index[j]).index[1]) && (model->triangles->at(face_split_by_plane.at(i)).vindices[(choose_index + 1) % 3] == (unsigned int)all_edge.at(model->triangles->at(face_split_by_plane.at(i)).edge_index[j]).index[0]);
							if (judge1){
								choose_edge[0] = (float)model->triangles->at(face_split_by_plane.at(i)).edge_index[j];
								one_split -= j;
								choose_vertex_index[0] = 1;
							}
							if (judge2){
								choose_edge[0] = (float)model->triangles->at(face_split_by_plane.at(i)).edge_index[j];
								one_split -= j;
								choose_vertex_index[0] = 0;
							}

							judge1 = (model->triangles->at(face_split_by_plane.at(i)).vindices[choose_index] == (unsigned int)all_edge.at(model->triangles->at(face_split_by_plane.at(i)).edge_index[j]).index[0]) && (model->triangles->at(face_split_by_plane.at(i)).vindices[(choose_index + 2) % 3] == (unsigned int)all_edge.at(model->triangles->at(face_split_by_plane.at(i)).edge_index[j]).index[1]);
							judge2 = (model->triangles->at(face_split_by_plane.at(i)).vindices[choose_index] == (unsigned int)all_edge.at(model->triangles->at(face_split_by_plane.at(i)).edge_index[j]).index[1]) && (model->triangles->at(face_split_by_plane.at(i)).vindices[(choose_index + 2) % 3] == (unsigned int)all_edge.at(model->triangles->at(face_split_by_plane.at(i)).edge_index[j]).index[0]);
							if (judge1){
								choose_edge[1] = (float)model->triangles->at(face_split_by_plane.at(i)).edge_index[j];
								one_split -= j;
								choose_vertex_index[1] = 1;
							}
							if (judge2){
								choose_edge[1] = (float)model->triangles->at(face_split_by_plane.at(i)).edge_index[j];
								one_split -= j;
								choose_vertex_index[1] = 0;
							}
						}
						else{
							judge1 = (model->triangles->at(face_split_by_plane.at(i)).vindices[choose_index] == (unsigned int)all_edge.at(model->triangles->at(face_split_by_plane.at(i)).edge_index[j]).index[0]) && (model->triangles->at(face_split_by_plane.at(i)).vindices[(choose_index + 1) % 3] == (unsigned int)all_edge.at(all_edge.at(model->triangles->at(face_split_by_plane.at(i)).edge_index[j]).connect_index).index[0]);
							judge2 = (model->triangles->at(face_split_by_plane.at(i)).vindices[choose_index] == (unsigned int)all_edge.at(all_edge.at(model->triangles->at(face_split_by_plane.at(i)).edge_index[j]).connect_index).index[0]) && (model->triangles->at(face_split_by_plane.at(i)).vindices[(choose_index + 1) % 3] == (unsigned int)all_edge.at(model->triangles->at(face_split_by_plane.at(i)).edge_index[j]).index[0]);
							if (judge1){
								choose_edge[0] = (float)model->triangles->at(face_split_by_plane.at(i)).edge_index[j];
								one_split -= j;
								vertex_index[0] = (float)all_edge.at(all_edge.at(model->triangles->at(face_split_by_plane.at(i)).edge_index[j]).connect_index).index[0];
							}
							if (judge2){
								choose_edge[0] = (float)all_edge.at(model->triangles->at(face_split_by_plane.at(i)).edge_index[j]).connect_index;
								one_split -= j;
								vertex_index[0] = (float)all_edge.at(model->triangles->at(face_split_by_plane.at(i)).edge_index[j]).index[0];
							}

							judge1 = (model->triangles->at(face_split_by_plane.at(i)).vindices[choose_index] == (unsigned int)all_edge.at(model->triangles->at(face_split_by_plane.at(i)).edge_index[j]).index[0]) && (model->triangles->at(face_split_by_plane.at(i)).vindices[(choose_index + 2) % 3] == (unsigned int)all_edge.at(all_edge.at(model->triangles->at(face_split_by_plane.at(i)).edge_index[j]).connect_index).index[0]);
							judge2 = (model->triangles->at(face_split_by_plane.at(i)).vindices[choose_index] == (unsigned int)all_edge.at(all_edge.at(model->triangles->at(face_split_by_plane.at(i)).edge_index[j]).connect_index).index[0]) && (model->triangles->at(face_split_by_plane.at(i)).vindices[(choose_index + 2) % 3] == (unsigned int)all_edge.at(model->triangles->at(face_split_by_plane.at(i)).edge_index[j]).index[0]);
							if (judge1){
								choose_edge[1] = (float)model->triangles->at(face_split_by_plane.at(i)).edge_index[j];
								one_split -= j;
								vertex_index[1] = (float)all_edge.at(all_edge.at(model->triangles->at(face_split_by_plane.at(i)).edge_index[j]).connect_index).index[0];
							}
							if (judge2){
								choose_edge[1] = (float)all_edge.at(model->triangles->at(face_split_by_plane.at(i)).edge_index[j]).connect_index;
								one_split -= j;
								vertex_index[1] = (float)all_edge.at(model->triangles->at(face_split_by_plane.at(i)).edge_index[j]).index[0];
							}
						}
					}

					choose_edge[2] = (float)model->triangles->at(face_split_by_plane.at(i)).edge_index[one_split];

					if (choose_dir != 0){
						if (all_edge.at((int)choose_edge[0]).vertex_push_index == -1){
							model->vertices->push_back(all_edge.at((int)choose_edge[0]).split_point[0]);
							model->vertices->push_back(all_edge.at((int)choose_edge[0]).split_point[1]);
							model->vertices->push_back(all_edge.at((int)choose_edge[0]).split_point[2]);
							model->numvertices += 1;
							all_edge.at((int)choose_edge[0]).vertex_push_index = model->numvertices;

							vertex new_vertex;
							model->cut_loop->push_back(new_vertex);
						}

						if (all_edge.at((int)choose_edge[1]).vertex_push_index == -1){
							model->vertices->push_back(all_edge.at((int)choose_edge[1]).split_point[0]);
							model->vertices->push_back(all_edge.at((int)choose_edge[1]).split_point[1]);
							model->vertices->push_back(all_edge.at((int)choose_edge[1]).split_point[2]);
							model->numvertices += 1;
							all_edge.at((int)choose_edge[1]).vertex_push_index = model->numvertices;

							vertex new_vertex;
							model->cut_loop->push_back(new_vertex);						
						}

						GLMtriangle temp1, temp2;
						edge inner_e1, inner_e2;
						edge outer_e1, outer_e2;
						int inner_edge_index[2];
						int outer_edge_index[2];

						temp1.vindices[0] = all_edge.at((int)choose_edge[1]).vertex_push_index;
						temp1.vindices[1] = all_edge.at((int)choose_edge[0]).vertex_push_index;
						if (all_edge.at((int)choose_edge[1]).connect_index == -1)
							temp1.vindices[2] = all_edge.at((int)choose_edge[1]).index[(int)choose_vertex_index[1]];
						else
							temp1.vindices[2] = (GLuint)vertex_index[1];
						temp1.split_plane_id = model->triangles->at(face_split_by_plane.at(i)).split_plane_id;
						temp1.split_by_process = true;

						if (all_edge.at((int)choose_edge[0]).vertex_push_index < all_edge.at((int)choose_edge[1]).vertex_push_index){
							inner_e1.index[0] = all_edge.at((int)choose_edge[0]).vertex_push_index;
							inner_e1.point[0][0] = model->vertices->at(3 * all_edge.at((int)choose_edge[0]).vertex_push_index + 0);
							inner_e1.point[0][1] = model->vertices->at(3 * all_edge.at((int)choose_edge[0]).vertex_push_index + 1);
							inner_e1.point[0][2] = model->vertices->at(3 * all_edge.at((int)choose_edge[0]).vertex_push_index + 2);

							inner_e1.index[1] = all_edge.at((int)choose_edge[1]).vertex_push_index;
							inner_e1.point[1][0] = model->vertices->at(3 * all_edge.at((int)choose_edge[1]).vertex_push_index + 0);
							inner_e1.point[1][1] = model->vertices->at(3 * all_edge.at((int)choose_edge[1]).vertex_push_index + 1);
							inner_e1.point[1][2] = model->vertices->at(3 * all_edge.at((int)choose_edge[1]).vertex_push_index + 2);
						}
						else{
							inner_e1.index[1] = all_edge.at((int)choose_edge[0]).vertex_push_index;
							inner_e1.point[1][0] = model->vertices->at(3 * all_edge.at((int)choose_edge[0]).vertex_push_index + 0);
							inner_e1.point[1][1] = model->vertices->at(3 * all_edge.at((int)choose_edge[0]).vertex_push_index + 1);
							inner_e1.point[1][2] = model->vertices->at(3 * all_edge.at((int)choose_edge[0]).vertex_push_index + 2);

							inner_e1.index[0] = all_edge.at((int)choose_edge[1]).vertex_push_index;
							inner_e1.point[0][0] = model->vertices->at(3 * all_edge.at((int)choose_edge[1]).vertex_push_index + 0);
							inner_e1.point[0][1] = model->vertices->at(3 * all_edge.at((int)choose_edge[1]).vertex_push_index + 1);
							inner_e1.point[0][2] = model->vertices->at(3 * all_edge.at((int)choose_edge[1]).vertex_push_index + 2);
						}
						inner_e1.connect_index = -1;
						inner_e1.face_id[0] = face_split_by_plane.at(i);
						inner_e1.face_id[1] = model->numtriangles;
						all_edge.push_back(inner_e1);
						inner_edge_index[0] = all_edge.size() - 1;
						if (find(model->cut_loop->at(inner_e1.index[0]).connect_edge.begin(), model->cut_loop->at(inner_e1.index[0]).connect_edge.end(), inner_edge_index[0]) - model->cut_loop->at(inner_e1.index[0]).connect_edge.begin() >= model->cut_loop->at(inner_e1.index[0]).connect_edge.size())
							model->cut_loop->at(inner_e1.index[0]).connect_edge.push_back(inner_edge_index[0]);
						if ((unsigned int)(find(model->cut_loop->at(inner_e1.index[0]).align_plane.begin(), model->cut_loop->at(inner_e1.index[0]).align_plane.end(), cut_plane) - model->cut_loop->at(inner_e1.index[0]).align_plane.begin()) >= model->cut_loop->at(inner_e1.index[0]).align_plane.size()){
							model->cut_loop->at(inner_e1.index[0]).align_plane.push_back(cut_plane);
						}
						if (model->cut_loop->at(inner_e1.index[0]).align_plane.size() >= 2)
							model->multi_vertex->push_back(inner_e1.index[0]);

						if (find(model->cut_loop->at(inner_e1.index[1]).connect_edge.begin(), model->cut_loop->at(inner_e1.index[1]).connect_edge.end(), inner_edge_index[0]) - model->cut_loop->at(inner_e1.index[1]).connect_edge.begin() >= model->cut_loop->at(inner_e1.index[1]).connect_edge.size())
							model->cut_loop->at(inner_e1.index[1]).connect_edge.push_back(inner_edge_index[0]);
						if ((unsigned int)(find(model->cut_loop->at(inner_e1.index[1]).align_plane.begin(), model->cut_loop->at(inner_e1.index[1]).align_plane.end(), cut_plane) - model->cut_loop->at(inner_e1.index[1]).align_plane.begin()) >= model->cut_loop->at(inner_e1.index[1]).align_plane.size()){
							model->cut_loop->at(inner_e1.index[1]).align_plane.push_back(cut_plane);
						}
						if (model->cut_loop->at(inner_e1.index[1]).align_plane.size() >= 2)
							model->multi_vertex->push_back(inner_e1.index[1]);

						if (all_edge.at((int)choose_edge[1]).connect_index == -1){
							inner_e2.index[0] = all_edge.at((int)choose_edge[1]).index[(int)choose_vertex_index[1]];
							inner_e2.point[0][0] = model->vertices->at(3 * all_edge.at((int)choose_edge[1]).index[(int)choose_vertex_index[1]] + 0);
							inner_e2.point[0][1] = model->vertices->at(3 * all_edge.at((int)choose_edge[1]).index[(int)choose_vertex_index[1]] + 1);
							inner_e2.point[0][2] = model->vertices->at(3 * all_edge.at((int)choose_edge[1]).index[(int)choose_vertex_index[1]] + 2);
						}
						else{
							inner_e2.index[0] = (int)vertex_index[1];
							inner_e2.point[0][0] = model->vertices->at(3 * (int)vertex_index[1] + 0);
							inner_e2.point[0][1] = model->vertices->at(3 * (int)vertex_index[1] + 1);
							inner_e2.point[0][2] = model->vertices->at(3 * (int)vertex_index[1] + 2);
						}

						inner_e2.index[1] = all_edge.at((int)choose_edge[0]).vertex_push_index;
						inner_e2.point[1][0] = model->vertices->at(3 * all_edge.at((int)choose_edge[0]).vertex_push_index + 0);
						inner_e2.point[1][1] = model->vertices->at(3 * all_edge.at((int)choose_edge[0]).vertex_push_index + 1);
						inner_e2.point[1][2] = model->vertices->at(3 * all_edge.at((int)choose_edge[0]).vertex_push_index + 2);
						inner_e2.connect_index = -1;
						inner_e2.face_id[0] = model->numtriangles;
						inner_e2.face_id[1] = model->numtriangles + 1;
						all_edge.push_back(inner_e2);
						inner_edge_index[1] = all_edge.size() - 1;

						if (all_edge.at((int)choose_edge[1]).connect_index == -1){
							outer_e2.index[0] = all_edge.at((int)choose_edge[1]).index[(int)choose_vertex_index[1]];
							outer_e2.point[0][0] = model->vertices->at(3 * all_edge.at((int)choose_edge[1]).index[(int)choose_vertex_index[1]] + 0);
							outer_e2.point[0][1] = model->vertices->at(3 * all_edge.at((int)choose_edge[1]).index[(int)choose_vertex_index[1]] + 1);
							outer_e2.point[0][2] = model->vertices->at(3 * all_edge.at((int)choose_edge[1]).index[(int)choose_vertex_index[1]] + 2);

							outer_e2.index[1] = all_edge.at((int)choose_edge[1]).vertex_push_index;
							outer_e2.point[1][0] = model->vertices->at(3 * all_edge.at((int)choose_edge[1]).vertex_push_index + 0);
							outer_e2.point[1][1] = model->vertices->at(3 * all_edge.at((int)choose_edge[1]).vertex_push_index + 1);
							outer_e2.point[1][2] = model->vertices->at(3 * all_edge.at((int)choose_edge[1]).vertex_push_index + 2);
							outer_e2.connect_index = (int)choose_edge[1];
							outer_e2.vertex_push_index = all_edge.at((int)choose_edge[1]).vertex_push_index;

							for (int x = 0; x < 2; x += 1){
								if (all_edge.at((int)choose_edge[1]).face_id[x] == face_split_by_plane.at(i))
									outer_e2.face_id[x] = model->numtriangles;
								else
									outer_e2.face_id[x] = all_edge.at((int)choose_edge[1]).face_id[x];
							}

							all_edge.push_back(outer_e2);
							outer_edge_index[1] = all_edge.size() - 1;
							edge_split_id.push_back(outer_edge_index[1]);
							edge_split_id.push_back((int)choose_edge[1]);
						}
						else{
							outer_edge_index[1] = all_edge.at((int)choose_edge[1]).connect_index;
							for (int x = 0; x < 2; x += 1){
								if (all_edge.at(outer_edge_index[1]).face_id[x] == face_split_by_plane.at(i)){
									all_edge.at(outer_edge_index[1]).face_id[x] = model->numtriangles;
								}
							}
						}

						temp1.edge_index[0] = inner_edge_index[0];
						temp1.edge_index[1] = inner_edge_index[1];
						temp1.edge_index[2] = outer_edge_index[1];
						temp1.material_id_graph_cut = -1;
						temp1.tag = tag;

						model->triangles->push_back(temp1);
						glmOneFacetNormals(model, model->numtriangles);
						model->numtriangles += 1;
						all_process_face.push_back(model->numtriangles - 1);

						dir_count[0] = 0;
						dir_count[1] = 0;
						dir_count[2] = 0;
						for (int j = 0; j < 3; j += 1){
							vec3 temp(model->vertices->at(3 * (temp1.vindices[j]) + 0), model->vertices->at(3 * (temp1.vindices[j]) + 1), model->vertices->at(3 * (temp1.vindices[j]) + 2));
							point_dir[j] = plane_dir_point(temp, test_plane);
							dir_count[(int)point_dir[j] + 1] += 1;
						}

						if (dir_count[test_plane.dir * -1 + 1] == 0){
							face_split_by_plane.push_back(model->numtriangles - 1);
							model->triangles->at(model->numtriangles - 1).material_id_graph_cut = piece_id;
						}

						temp2.vindices[0] = all_edge.at((int)choose_edge[0]).vertex_push_index;
						if (all_edge.at((int)choose_edge[0]).connect_index == -1)
							temp2.vindices[1] = all_edge.at((int)choose_edge[0]).index[(int)choose_vertex_index[0]];
						else
							temp2.vindices[1] = (GLuint)vertex_index[0];
						if (all_edge.at((int)choose_edge[1]).connect_index == -1)
							temp2.vindices[2] = all_edge.at((int)choose_edge[1]).index[(int)choose_vertex_index[1]];
						else
							temp2.vindices[2] = (GLuint)vertex_index[1];
						temp2.split_plane_id = model->triangles->at(face_split_by_plane.at(i)).split_plane_id;
						temp2.split_by_process = true;

						if (all_edge.at((int)choose_edge[0]).connect_index == -1){
							outer_e1.index[0] = all_edge.at((int)choose_edge[0]).index[(int)choose_vertex_index[0]];
							outer_e1.point[0][0] = model->vertices->at(3 * all_edge.at((int)choose_edge[0]).index[(int)choose_vertex_index[0]] + 0);
							outer_e1.point[0][1] = model->vertices->at(3 * all_edge.at((int)choose_edge[0]).index[(int)choose_vertex_index[0]] + 1);
							outer_e1.point[0][2] = model->vertices->at(3 * all_edge.at((int)choose_edge[0]).index[(int)choose_vertex_index[0]] + 2);

							outer_e1.index[1] = all_edge.at((int)choose_edge[0]).vertex_push_index;
							outer_e1.point[1][0] = model->vertices->at(3 * all_edge.at((int)choose_edge[0]).vertex_push_index + 0);
							outer_e1.point[1][1] = model->vertices->at(3 * all_edge.at((int)choose_edge[0]).vertex_push_index + 1);
							outer_e1.point[1][2] = model->vertices->at(3 * all_edge.at((int)choose_edge[0]).vertex_push_index + 2);
							outer_e1.connect_index = (int)choose_edge[0];
							outer_e1.vertex_push_index = all_edge.at((int)choose_edge[0]).vertex_push_index;
							for (int x = 0; x < 2; x += 1){
								if (all_edge.at((int)choose_edge[0]).face_id[x] == face_split_by_plane.at(i))
									outer_e1.face_id[x] = model->numtriangles;
								else
									outer_e1.face_id[x] = all_edge.at((int)choose_edge[0]).face_id[x];
							}

							all_edge.push_back(outer_e1);
							outer_edge_index[0] = all_edge.size() - 1;
							edge_split_id.push_back(outer_edge_index[0]);
							edge_split_id.push_back((int)choose_edge[0]);
						}
						else{
							outer_edge_index[0] = all_edge.at((int)choose_edge[0]).connect_index;
							for (int x = 0; x < 2; x += 1){
								if (all_edge.at(outer_edge_index[0]).face_id[x] == face_split_by_plane.at(i)){
									all_edge.at(outer_edge_index[0]).face_id[x] = model->numtriangles;
								}
							}
						}

						temp2.edge_index[0] = outer_edge_index[0];
						temp2.edge_index[1] = (int)choose_edge[2];
						temp2.edge_index[2] = inner_edge_index[1];

						for (int x = 0; x < 2; x += 1){
							if (all_edge.at((int)choose_edge[2]).face_id[x] == face_split_by_plane.at(i)){
								all_edge.at((int)choose_edge[2]).face_id[x] = model->numtriangles;
							}
						}

						temp2.material_id_graph_cut = -1;
						temp2.tag = tag;

						model->triangles->push_back(temp2);
						glmOneFacetNormals(model, model->numtriangles);
						model->numtriangles += 1;
						all_process_face.push_back(model->numtriangles - 1);

						dir_count[0] = 0;
						dir_count[1] = 0;
						dir_count[2] = 0;
						for (int j = 0; j < 3; j += 1){
							vec3 temp(model->vertices->at(3 * (temp2.vindices[j]) + 0), model->vertices->at(3 * (temp2.vindices[j]) + 1), model->vertices->at(3 * (temp2.vindices[j]) + 2));
							point_dir[j] = plane_dir_point(temp, test_plane);
							dir_count[(int)point_dir[j] + 1] += 1;
						}

						if (dir_count[test_plane.dir * -1 + 1] == 0){
							face_split_by_plane.push_back(model->numtriangles - 1);
							model->triangles->at(model->numtriangles - 1).material_id_graph_cut = piece_id;
						}

						if (all_edge.at((int)choose_edge[0]).connect_index == -1){
							int old_index[2];
							old_index[0] = all_edge.at((int)choose_edge[0]).index[0];
							old_index[1] = all_edge.at((int)choose_edge[0]).index[1];

							vec3 old_vertex[2];
							old_vertex[0] = all_edge.at((int)choose_edge[0]).point[0];
							old_vertex[1] = all_edge.at((int)choose_edge[0]).point[1];

							all_edge.at((int)choose_edge[0]).index[0] = model->triangles->at(face_split_by_plane.at(i)).vindices[choose_index];
							all_edge.at((int)choose_edge[0]).point[0][0] = model->vertices->at(3 * model->triangles->at(face_split_by_plane.at(i)).vindices[choose_index] + 0);
							all_edge.at((int)choose_edge[0]).point[0][1] = model->vertices->at(3 * model->triangles->at(face_split_by_plane.at(i)).vindices[choose_index] + 1);
							all_edge.at((int)choose_edge[0]).point[0][2] = model->vertices->at(3 * model->triangles->at(face_split_by_plane.at(i)).vindices[choose_index] + 2);

							all_edge.at((int)choose_edge[0]).index[1] = all_edge.at((int)choose_edge[0]).vertex_push_index;
							all_edge.at((int)choose_edge[0]).point[1][0] = model->vertices->at(3 * all_edge.at((int)choose_edge[0]).vertex_push_index + 0);
							all_edge.at((int)choose_edge[0]).point[1][1] = model->vertices->at(3 * all_edge.at((int)choose_edge[0]).vertex_push_index + 1);
							all_edge.at((int)choose_edge[0]).point[1][2] = model->vertices->at(3 * all_edge.at((int)choose_edge[0]).vertex_push_index + 2);
							all_edge.at((int)choose_edge[0]).connect_index = outer_edge_index[0];

							if (old_index[0] != all_edge.at((int)choose_edge[0]).index[0] && old_index[0] != all_edge.at((int)choose_edge[0]).index[1] && model->cut_loop->at(old_index[0]).connect_edge.size() > 0){
								unsigned int alter_edge = find(model->cut_loop->at(old_index[0]).connect_edge.begin(), model->cut_loop->at(old_index[0]).connect_edge.end(), choose_edge[0]) - model->cut_loop->at(old_index[0]).connect_edge.begin();
								if (alter_edge < model->cut_loop->at(old_index[0]).connect_edge.size()){
									model->cut_loop->at(old_index[0]).connect_edge.at(alter_edge) = all_edge.at((int)choose_edge[0]).connect_index;
								}
							}

							if (old_index[1] != all_edge.at((int)choose_edge[0]).index[0] && old_index[1] != all_edge.at((int)choose_edge[0]).index[1] && model->cut_loop->at(old_index[1]).connect_edge.size() > 0){
								unsigned int alter_edge = find(model->cut_loop->at(old_index[1]).connect_edge.begin(), model->cut_loop->at(old_index[1]).connect_edge.end(), choose_edge[0]) - model->cut_loop->at(old_index[1]).connect_edge.begin();
								if (alter_edge < model->cut_loop->at(old_index[1]).connect_edge.size()){
									model->cut_loop->at(old_index[1]).connect_edge.at(alter_edge) = all_edge.at((int)choose_edge[0]).connect_index;
								}
							}

							int judge_dir[2];
							plane_dir_edge(all_edge.at((int)choose_edge[0]), test_plane, judge_dir);
							if ((judge_dir[0] == 0 || judge_dir[0] == test_plane.dir) && (judge_dir[1] == 0 || judge_dir[1] == test_plane.dir)){
								for (unsigned int j = 0; j < model->cut_loop->at(all_edge.at((int)choose_edge[0]).index[0]).align_plane.size(); j += 1){
									for (unsigned int k = 0; k < model->cut_loop->at(all_edge.at((int)choose_edge[0]).index[0]).connect_edge.size(); k += 1){
										plane_dir_edge(all_edge.at(model->cut_loop->at(all_edge.at((int)choose_edge[0]).index[0]).connect_edge.at(k)), planes.at(model->cut_loop->at(all_edge.at((int)choose_edge[0]).index[0]).align_plane.at(j)), judge_dir);
										if (judge_dir[0] == 0 && judge_dir[1] == 0 && all_edge.at(model->cut_loop->at(all_edge.at((int)choose_edge[0]).index[0]).connect_edge.at(k)).index[1] == all_edge.at((int)choose_edge[0]).vertex_push_index){

											if (find(model->cut_loop->at(all_edge.at((int)choose_edge[0]).vertex_push_index).connect_edge.begin(), model->cut_loop->at(all_edge.at((int)choose_edge[0]).vertex_push_index).connect_edge.end(), model->cut_loop->at(all_edge.at((int)choose_edge[0]).index[0]).connect_edge.at(k)) - model->cut_loop->at(all_edge.at((int)choose_edge[0]).vertex_push_index).connect_edge.begin() >= model->cut_loop->at(all_edge.at((int)choose_edge[0]).vertex_push_index).connect_edge.size())
												model->cut_loop->at(all_edge.at((int)choose_edge[0]).vertex_push_index).connect_edge.push_back(model->cut_loop->at(all_edge.at((int)choose_edge[0]).index[0]).connect_edge.at(k));

											if (find(model->cut_loop->at(all_edge.at((int)choose_edge[0]).vertex_push_index).align_plane.begin(), model->cut_loop->at(all_edge.at((int)choose_edge[0]).vertex_push_index).align_plane.end(), model->cut_loop->at(all_edge.at((int)choose_edge[0]).index[0]).align_plane.at(j)) - model->cut_loop->at(all_edge.at((int)choose_edge[0]).vertex_push_index).align_plane.begin() >= model->cut_loop->at(all_edge.at((int)choose_edge[0]).vertex_push_index).align_plane.size())
												model->cut_loop->at(all_edge.at((int)choose_edge[0]).vertex_push_index).align_plane.push_back(model->cut_loop->at(all_edge.at((int)choose_edge[0]).index[0]).align_plane.at(j));

											if (model->cut_loop->at(all_edge.at((int)choose_edge[0]).vertex_push_index).align_plane.size() >= 2)
												model->multi_vertex->push_back(all_edge.at((int)choose_edge[0]).vertex_push_index);
											break;
										}
									}
								}
							}

							plane_dir_edge(all_edge.at(all_edge.at((int)choose_edge[0]).connect_index), test_plane, judge_dir);
							if ((judge_dir[0] == 0 || judge_dir[0] == test_plane.dir) && (judge_dir[1] == 0 || judge_dir[1] == test_plane.dir)){
								for (unsigned int j = 0; j < model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[0]).connect_index).index[0]).align_plane.size(); j += 1){
									for (unsigned int k = 0; k < model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[0]).connect_index).index[0]).connect_edge.size(); k += 1){
										plane_dir_edge(all_edge.at(model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[0]).connect_index).index[0]).connect_edge.at(k)), planes.at(model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[0]).connect_index).index[0]).align_plane.at(j)), judge_dir);
										if (judge_dir[0] == 0 && judge_dir[1] == 0 && all_edge.at(model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[0]).connect_index).index[0]).connect_edge.at(k)).index[1] == all_edge.at(all_edge.at((int)choose_edge[0]).connect_index).vertex_push_index){
											if (find(model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[0]).connect_index).vertex_push_index).connect_edge.begin(), model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[0]).connect_index).vertex_push_index).connect_edge.end(), model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[0]).connect_index).index[0]).connect_edge.at(k)) - model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[0]).connect_index).vertex_push_index).connect_edge.begin() >= model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[0]).connect_index).vertex_push_index).connect_edge.size())
												model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[0]).connect_index).vertex_push_index).connect_edge.push_back(model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[0]).connect_index).index[0]).connect_edge.at(k));

											if (find(model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[0]).connect_index).vertex_push_index).align_plane.begin(), model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[0]).connect_index).vertex_push_index).align_plane.end(), model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[0]).connect_index).index[0]).align_plane.at(j)) - model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[0]).connect_index).vertex_push_index).align_plane.begin() >= model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[0]).connect_index).vertex_push_index).align_plane.size())
												model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[0]).connect_index).vertex_push_index).align_plane.push_back(model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[0]).connect_index).index[0]).align_plane.at(j));

											if (model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[0]).connect_index).vertex_push_index).align_plane.size() >= 2)
												model->multi_vertex->push_back(all_edge.at(all_edge.at((int)choose_edge[0]).connect_index).vertex_push_index);
											break;
										}
									}
								}
							}

							if (plane_dir_point(old_vertex[0], test_plane) != test_plane.dir && plane_dir_point(old_vertex[0], test_plane) != 0){
								if (!model->cut_loop->at(old_index[0]).connect_edge.empty()){
									std::vector<unsigned int> v;
									model->cut_loop->at(old_index[0]).connect_edge.swap(v);
								}
							}
							if (plane_dir_point(old_vertex[1], test_plane) != test_plane.dir && plane_dir_point(old_vertex[1], test_plane) != 0){
								if (!model->cut_loop->at(old_index[1]).connect_edge.empty()){
									std::vector<unsigned int> v;
									model->cut_loop->at(old_index[1]).connect_edge.swap(v);
								}
							}
						}

						for (unsigned j = 0; j < model->cut_loop->at(all_edge.at((int)choose_edge[0]).vertex_push_index).connect_edge.size(); j += 1){
							int contain_dir[2];
							plane_dir_edge(all_edge.at(model->cut_loop->at(all_edge.at((int)choose_edge[0]).vertex_push_index).connect_edge.at(j)), test_plane, contain_dir);
							if ((contain_dir[0] != 0 && contain_dir[0] != test_plane.dir) || (contain_dir[1] != 0 && contain_dir[1] != test_plane.dir)){
								model->cut_loop->at(all_edge.at((int)choose_edge[0]).vertex_push_index).connect_edge.erase(model->cut_loop->at(all_edge.at((int)choose_edge[0]).vertex_push_index).connect_edge.begin() + j);
								j -= 1;
							}
						}

						if (all_edge.at((int)choose_edge[1]).connect_index == -1){
							int old_index[2];
							old_index[0] = all_edge.at((int)choose_edge[1]).index[0];
							old_index[1] = all_edge.at((int)choose_edge[1]).index[1];

							vec3 old_vertex[2];
							old_vertex[0] = all_edge.at((int)choose_edge[1]).point[0];
							old_vertex[1] = all_edge.at((int)choose_edge[1]).point[1];

							all_edge.at((int)choose_edge[1]).index[0] = model->triangles->at(face_split_by_plane.at(i)).vindices[choose_index];
							all_edge.at((int)choose_edge[1]).point[0][0] = model->vertices->at(3 * model->triangles->at(face_split_by_plane.at(i)).vindices[choose_index] + 0);
							all_edge.at((int)choose_edge[1]).point[0][1] = model->vertices->at(3 * model->triangles->at(face_split_by_plane.at(i)).vindices[choose_index] + 1);
							all_edge.at((int)choose_edge[1]).point[0][2] = model->vertices->at(3 * model->triangles->at(face_split_by_plane.at(i)).vindices[choose_index] + 2);

							all_edge.at((int)choose_edge[1]).index[1] = all_edge.at((int)choose_edge[1]).vertex_push_index;
							all_edge.at((int)choose_edge[1]).point[1][0] = model->vertices->at(3 * all_edge.at((int)choose_edge[1]).vertex_push_index + 0);
							all_edge.at((int)choose_edge[1]).point[1][1] = model->vertices->at(3 * all_edge.at((int)choose_edge[1]).vertex_push_index + 1);
							all_edge.at((int)choose_edge[1]).point[1][2] = model->vertices->at(3 * all_edge.at((int)choose_edge[1]).vertex_push_index + 2);
							all_edge.at((int)choose_edge[1]).connect_index = outer_edge_index[1];
							
							if (old_index[0] != all_edge.at((int)choose_edge[1]).index[0] && old_index[0] != all_edge.at((int)choose_edge[1]).index[1] && model->cut_loop->at(old_index[0]).connect_edge.size() > 0){
								unsigned int alter_edge = find(model->cut_loop->at(old_index[0]).connect_edge.begin(), model->cut_loop->at(old_index[0]).connect_edge.end(), choose_edge[1]) - model->cut_loop->at(old_index[0]).connect_edge.begin();
								if (alter_edge < model->cut_loop->at(old_index[0]).connect_edge.size()){
									model->cut_loop->at(old_index[0]).connect_edge.at(alter_edge) = all_edge.at((int)choose_edge[1]).connect_index;
								}
							}

							if (old_index[1] != all_edge.at((int)choose_edge[1]).index[0] && old_index[1] != all_edge.at((int)choose_edge[1]).index[1] && model->cut_loop->at(old_index[1]).connect_edge.size() > 0){
								unsigned int alter_edge = find(model->cut_loop->at(old_index[1]).connect_edge.begin(), model->cut_loop->at(old_index[1]).connect_edge.end(), choose_edge[1]) - model->cut_loop->at(old_index[1]).connect_edge.begin();
								if (alter_edge < model->cut_loop->at(old_index[1]).connect_edge.size()){
									model->cut_loop->at(old_index[1]).connect_edge.at(alter_edge) = all_edge.at((int)choose_edge[1]).connect_index;
								}
							}

							int judge_dir[2];
							plane_dir_edge(all_edge.at((int)choose_edge[1]), test_plane, judge_dir);
							if ((judge_dir[0] == 0 || judge_dir[0] == test_plane.dir) && (judge_dir[1] == 0 || judge_dir[1] == test_plane.dir)){
								for (unsigned int j = 0; j < model->cut_loop->at(all_edge.at((int)choose_edge[1]).index[0]).align_plane.size(); j += 1){
									for (unsigned int k = 0; k < model->cut_loop->at(all_edge.at((int)choose_edge[1]).index[0]).connect_edge.size(); k += 1){
										plane_dir_edge(all_edge.at(model->cut_loop->at(all_edge.at((int)choose_edge[1]).index[0]).connect_edge.at(k)), planes.at(model->cut_loop->at(all_edge.at((int)choose_edge[1]).index[0]).align_plane.at(j)), judge_dir);
										if (judge_dir[0] == 0 && judge_dir[1] == 0 && all_edge.at(model->cut_loop->at(all_edge.at((int)choose_edge[1]).index[0]).connect_edge.at(k)).index[1] == all_edge.at((int)choose_edge[1]).vertex_push_index){
											if (find(model->cut_loop->at(all_edge.at((int)choose_edge[1]).vertex_push_index).connect_edge.begin(), model->cut_loop->at(all_edge.at((int)choose_edge[1]).vertex_push_index).connect_edge.end(), model->cut_loop->at(all_edge.at((int)choose_edge[1]).index[0]).connect_edge.at(k)) - model->cut_loop->at(all_edge.at((int)choose_edge[1]).vertex_push_index).connect_edge.begin() >= model->cut_loop->at(all_edge.at((int)choose_edge[1]).vertex_push_index).connect_edge.size())
												model->cut_loop->at(all_edge.at((int)choose_edge[1]).vertex_push_index).connect_edge.push_back(model->cut_loop->at(all_edge.at((int)choose_edge[1]).index[0]).connect_edge.at(k));

											if (find(model->cut_loop->at(all_edge.at((int)choose_edge[1]).vertex_push_index).align_plane.begin(), model->cut_loop->at(all_edge.at((int)choose_edge[1]).vertex_push_index).align_plane.end(), model->cut_loop->at(all_edge.at((int)choose_edge[1]).index[0]).align_plane.at(j)) - model->cut_loop->at(all_edge.at((int)choose_edge[1]).vertex_push_index).align_plane.begin() >= model->cut_loop->at(all_edge.at((int)choose_edge[1]).vertex_push_index).align_plane.size())
												model->cut_loop->at(all_edge.at((int)choose_edge[1]).vertex_push_index).align_plane.push_back(model->cut_loop->at(all_edge.at((int)choose_edge[1]).index[0]).align_plane.at(j));

											if (model->cut_loop->at(all_edge.at((int)choose_edge[1]).vertex_push_index).align_plane.size() >= 2)
												model->multi_vertex->push_back(all_edge.at((int)choose_edge[1]).vertex_push_index);
											break;
										}
									}
								}
							}

							plane_dir_edge(all_edge.at(all_edge.at((int)choose_edge[1]).connect_index), test_plane, judge_dir);
							if ((judge_dir[0] == 0 || judge_dir[0] == test_plane.dir) && (judge_dir[1] == 0 || judge_dir[1] == test_plane.dir)){
								for (unsigned int j = 0; j < model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[1]).connect_index).index[0]).align_plane.size(); j += 1){
									for (unsigned int k = 0; k < model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[1]).connect_index).index[0]).connect_edge.size(); k += 1){

										plane_dir_edge(all_edge.at(model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[1]).connect_index).index[0]).connect_edge.at(k)), planes.at(model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[1]).connect_index).index[0]).align_plane.at(j)), judge_dir);

										if (judge_dir[0] == 0 && judge_dir[1] == 0 && all_edge.at(model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[1]).connect_index).index[0]).connect_edge.at(k)).index[1] == all_edge.at(all_edge.at((int)choose_edge[1]).connect_index).vertex_push_index){
											if (find(model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[1]).connect_index).vertex_push_index).connect_edge.begin(), model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[1]).connect_index).vertex_push_index).connect_edge.end(), model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[1]).connect_index).index[0]).connect_edge.at(k)) - model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[1]).connect_index).vertex_push_index).connect_edge.begin() >= model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[1]).connect_index).vertex_push_index).connect_edge.size())
												model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[1]).connect_index).vertex_push_index).connect_edge.push_back(model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[1]).connect_index).index[0]).connect_edge.at(k));

											if (find(model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[1]).connect_index).vertex_push_index).align_plane.begin(), model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[1]).connect_index).vertex_push_index).align_plane.end(), model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[1]).connect_index).index[0]).align_plane.at(j)) - model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[1]).connect_index).vertex_push_index).align_plane.begin() >= model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[1]).connect_index).vertex_push_index).align_plane.size())
												model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[1]).connect_index).vertex_push_index).align_plane.push_back(model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[1]).connect_index).index[0]).align_plane.at(j));

											if (model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[1]).connect_index).vertex_push_index).align_plane.size() >= 2)
												model->multi_vertex->push_back(all_edge.at(all_edge.at((int)choose_edge[1]).connect_index).vertex_push_index);
											break;
										}
									}
								}
							}

							if (plane_dir_point(old_vertex[0], test_plane) != test_plane.dir && plane_dir_point(old_vertex[0], test_plane) != 0){
								if (!model->cut_loop->at(old_index[0]).connect_edge.empty()){
									std::vector<unsigned int> v;
									model->cut_loop->at(old_index[0]).connect_edge.swap(v);
								}
							}
							if (plane_dir_point(old_vertex[1], test_plane) != test_plane.dir && plane_dir_point(old_vertex[1], test_plane) != 0){
								if (!model->cut_loop->at(old_index[1]).connect_edge.empty()){
									std::vector<unsigned int> v;
									model->cut_loop->at(old_index[1]).connect_edge.swap(v);
								}
							}
						}

						for (unsigned j = 0; j < model->cut_loop->at(all_edge.at((int)choose_edge[1]).vertex_push_index).connect_edge.size(); j += 1){
							int contain_dir[2];
							plane_dir_edge(all_edge.at(model->cut_loop->at(all_edge.at((int)choose_edge[1]).vertex_push_index).connect_edge.at(j)), test_plane, contain_dir);
							if ((contain_dir[0] != 0 && contain_dir[0] != test_plane.dir) || (contain_dir[1] != 0 && contain_dir[1] != test_plane.dir)){
								model->cut_loop->at(all_edge.at((int)choose_edge[1]).vertex_push_index).connect_edge.erase(model->cut_loop->at(all_edge.at((int)choose_edge[1]).vertex_push_index).connect_edge.begin() + j);
								j -= 1;
							}
						}

						model->triangles->at(face_split_by_plane.at(i)).edge_index[0] = (int)choose_edge[0];
						model->triangles->at(face_split_by_plane.at(i)).edge_index[1] = inner_edge_index[0];
						model->triangles->at(face_split_by_plane.at(i)).edge_index[2] = (int)choose_edge[1];

						model->triangles->at(face_split_by_plane.at(i)).vindices[0] = model->triangles->at(face_split_by_plane.at(i)).vindices[choose_index];
						model->triangles->at(face_split_by_plane.at(i)).vindices[1] = all_edge.at((int)choose_edge[0]).vertex_push_index;
						model->triangles->at(face_split_by_plane.at(i)).vindices[2] = all_edge.at((int)choose_edge[1]).vertex_push_index;
						model->triangles->at(face_split_by_plane.at(i)).split_by_process = true;

						glmOneFacetNormals(model, face_split_by_plane.at(i));
						all_process_face.push_back(face_split_by_plane.at(i));

						if (all_edge.at((int)choose_edge[0]).face_id[1] != -1){
							if (!model->triangles->at(all_edge.at((int)choose_edge[0]).face_id[0]).split_by_process || !model->triangles->at(all_edge.at((int)choose_edge[0]).face_id[1]).split_by_process){
								if (!model->triangles->at(all_edge.at((int)choose_edge[0]).face_id[0]).split_by_process)
									tri_poly(model, all_edge, all_edge.at((int)choose_edge[0]).face_id[0], all_edge.at((int)choose_edge[0]), all_edge.at(outer_edge_index[0]));
								if (!model->triangles->at(all_edge.at((int)choose_edge[0]).face_id[1]).split_by_process)
									tri_poly(model, all_edge, all_edge.at((int)choose_edge[0]).face_id[1], all_edge.at((int)choose_edge[0]), all_edge.at(outer_edge_index[0]));
							}
						}

						if (all_edge.at((int)choose_edge[1]).face_id[1] != -1){
							if (!model->triangles->at(all_edge.at((int)choose_edge[1]).face_id[0]).split_by_process || !model->triangles->at(all_edge.at((int)choose_edge[1]).face_id[1]).split_by_process){
								if (!model->triangles->at(all_edge.at((int)choose_edge[1]).face_id[0]).split_by_process)
									tri_poly(model, all_edge, all_edge.at((int)choose_edge[1]).face_id[0], all_edge.at((int)choose_edge[1]), all_edge.at(outer_edge_index[1]));
								if (!model->triangles->at(all_edge.at((int)choose_edge[1]).face_id[1]).split_by_process)
									tri_poly(model, all_edge, all_edge.at((int)choose_edge[1]).face_id[1], all_edge.at((int)choose_edge[1]), all_edge.at(outer_edge_index[1]));
							}
						}

						dir_count[0] = 0;
						dir_count[1] = 0;
						dir_count[2] = 0;
						for (int j = 0; j < 3; j += 1){
							vec3 temp(model->vertices->at(3 * (model->triangles->at(face_split_by_plane.at(i)).vindices[j]) + 0), model->vertices->at(3 * (model->triangles->at(face_split_by_plane.at(i)).vindices[j]) + 1), model->vertices->at(3 * (model->triangles->at(face_split_by_plane.at(i)).vindices[j]) + 2));
							point_dir[j] = plane_dir_point(temp, test_plane);
							dir_count[(int)point_dir[j] + 1] += 1;
						}

						if (dir_count[test_plane.dir * -1 + 1] == 0){
							face_split_by_plane.push_back(face_split_by_plane.at(i));
						}
						else{
							model->triangles->at(face_split_by_plane.at(i)).material_id_graph_cut = -1;
						}
					}
					else{
						if (all_edge.at((int)choose_edge[2]).vertex_push_index == -1){
							model->vertices->push_back(all_edge.at((int)choose_edge[2]).split_point[0]);
							model->vertices->push_back(all_edge.at((int)choose_edge[2]).split_point[1]);
							model->vertices->push_back(all_edge.at((int)choose_edge[2]).split_point[2]);
							model->numvertices += 1;
							all_edge.at((int)choose_edge[2]).vertex_push_index = model->numvertices;

							vertex new_vertex;
							model->cut_loop->push_back(new_vertex);
						}

						GLMtriangle temp;
						edge inner_e;
						edge outer_e;
						int inner_edge_index;
						int outer_edge_index;

						temp.vindices[0] = model->triangles->at(face_split_by_plane.at(i)).vindices[choose_index];
						temp.vindices[1] = all_edge.at((int)choose_edge[0]).index[(int)choose_vertex_index[0]];
						temp.vindices[2] = all_edge.at((int)choose_edge[2]).vertex_push_index;
						temp.split_plane_id = model->triangles->at(face_split_by_plane.at(i)).split_plane_id;
						temp.split_by_process = true;

						inner_e.index[0] = model->triangles->at(face_split_by_plane.at(i)).vindices[choose_index];
						inner_e.index[1] = all_edge.at((int)choose_edge[2]).vertex_push_index;
						inner_e.point[0][0] = model->vertices->at(3 * model->triangles->at(face_split_by_plane.at(i)).vindices[choose_index] + 0);
						inner_e.point[0][1] = model->vertices->at(3 * model->triangles->at(face_split_by_plane.at(i)).vindices[choose_index] + 1);
						inner_e.point[0][2] = model->vertices->at(3 * model->triangles->at(face_split_by_plane.at(i)).vindices[choose_index] + 2);
						inner_e.point[1][0] = model->vertices->at(3 * all_edge.at((int)choose_edge[2]).vertex_push_index + 0);
						inner_e.point[1][1] = model->vertices->at(3 * all_edge.at((int)choose_edge[2]).vertex_push_index + 1);
						inner_e.point[1][2] = model->vertices->at(3 * all_edge.at((int)choose_edge[2]).vertex_push_index + 2);
						inner_e.face_id[0] = face_split_by_plane.at(i);
						inner_e.face_id[1] = model->numtriangles;
						inner_e.connect_index = -1;
						all_edge.push_back(inner_e);
						inner_edge_index = all_edge.size() - 1;

						if (find(model->cut_loop->at(inner_e.index[0]).connect_edge.begin(), model->cut_loop->at(inner_e.index[0]).connect_edge.end(), inner_edge_index) - model->cut_loop->at(inner_e.index[0]).connect_edge.begin() >= model->cut_loop->at(inner_e.index[0]).connect_edge.size())
							model->cut_loop->at(inner_e.index[0]).connect_edge.push_back(inner_edge_index);
						if ((unsigned int)(find(model->cut_loop->at(inner_e.index[0]).align_plane.begin(), model->cut_loop->at(inner_e.index[0]).align_plane.end(), cut_plane) - model->cut_loop->at(inner_e.index[0]).align_plane.begin()) >= model->cut_loop->at(inner_e.index[0]).align_plane.size()){
							model->cut_loop->at(inner_e.index[0]).align_plane.push_back(cut_plane);
						}
						if (model->cut_loop->at(inner_e.index[0]).align_plane.size() >= 2)
							model->multi_vertex->push_back(inner_e.index[0]);

						if (find(model->cut_loop->at(inner_e.index[1]).connect_edge.begin(), model->cut_loop->at(inner_e.index[1]).connect_edge.end(), inner_edge_index) - model->cut_loop->at(inner_e.index[1]).connect_edge.begin() >= model->cut_loop->at(inner_e.index[1]).connect_edge.size())
							model->cut_loop->at(inner_e.index[1]).connect_edge.push_back(inner_edge_index);
						if ((unsigned int)(find(model->cut_loop->at(inner_e.index[1]).align_plane.begin(), model->cut_loop->at(inner_e.index[1]).align_plane.end(), cut_plane) - model->cut_loop->at(inner_e.index[1]).align_plane.begin()) >= model->cut_loop->at(inner_e.index[1]).align_plane.size()){
							model->cut_loop->at(inner_e.index[1]).align_plane.push_back(cut_plane);
						}
						if (model->cut_loop->at(inner_e.index[1]).align_plane.size() >= 2)
							model->multi_vertex->push_back(inner_e.index[1]);

						if (all_edge.at((int)choose_edge[2]).connect_index == -1){
							outer_e.index[0] = all_edge.at((int)choose_edge[0]).index[(int)choose_vertex_index[0]];
							outer_e.index[1] = all_edge.at((int)choose_edge[2]).vertex_push_index;
							outer_e.point[0][0] = model->vertices->at(3 * all_edge.at((int)choose_edge[0]).index[(int)choose_vertex_index[0]] + 0);
							outer_e.point[0][1] = model->vertices->at(3 * all_edge.at((int)choose_edge[0]).index[(int)choose_vertex_index[0]] + 1);
							outer_e.point[0][2] = model->vertices->at(3 * all_edge.at((int)choose_edge[0]).index[(int)choose_vertex_index[0]] + 2);
							outer_e.point[1][0] = model->vertices->at(3 * all_edge.at((int)choose_edge[2]).vertex_push_index + 0);
							outer_e.point[1][1] = model->vertices->at(3 * all_edge.at((int)choose_edge[2]).vertex_push_index + 1);
							outer_e.point[1][2] = model->vertices->at(3 * all_edge.at((int)choose_edge[2]).vertex_push_index + 2);
							outer_e.connect_index = (int)choose_edge[2];
							outer_e.vertex_push_index = all_edge.at((int)choose_edge[2]).vertex_push_index;

							for (int x = 0; x < 2; x += 1){
								if (all_edge.at((int)choose_edge[2]).face_id[x] == face_split_by_plane.at(i))
									outer_e.face_id[x] = model->numtriangles;
								else
									outer_e.face_id[x] = all_edge.at((int)choose_edge[2]).face_id[x];
							}

							all_edge.push_back(outer_e);
							outer_edge_index = all_edge.size() - 1;
						}
						else{
							bool test1 = (all_edge.at((int)choose_edge[2]).index[0] == all_edge.at((int)choose_edge[0]).index[(int)choose_vertex_index[0]]) && (all_edge.at((int)choose_edge[2]).index[1] == all_edge.at((int)choose_edge[2]).vertex_push_index);
							bool test2 = (all_edge.at((int)choose_edge[2]).index[1] == all_edge.at((int)choose_edge[0]).index[(int)choose_vertex_index[0]]) && (all_edge.at((int)choose_edge[2]).index[0] == all_edge.at((int)choose_edge[2]).vertex_push_index);
							bool test3 = (all_edge.at(all_edge.at((int)choose_edge[2]).connect_index).index[0] == all_edge.at((int)choose_edge[0]).index[(int)choose_vertex_index[0]]) && (all_edge.at(all_edge.at((int)choose_edge[2]).connect_index).index[1] == all_edge.at((int)choose_edge[2]).vertex_push_index);
							bool test4 = (all_edge.at(all_edge.at((int)choose_edge[2]).connect_index).index[1] == all_edge.at((int)choose_edge[0]).index[(int)choose_vertex_index[0]]) && (all_edge.at(all_edge.at((int)choose_edge[2]).connect_index).index[0] == all_edge.at((int)choose_edge[2]).vertex_push_index);
							if (test1 || test2){
								outer_edge_index = (int)choose_edge[2];
								choose_edge[2] = (float)all_edge.at((int)choose_edge[2]).connect_index;
							}
							if (test3 || test4){
								outer_edge_index = all_edge.at((int)choose_edge[2]).connect_index;
							}

							for (int x = 0; x < 2; x += 1){
								if (all_edge.at(outer_edge_index).face_id[x] == face_split_by_plane.at(i)){
									all_edge.at(outer_edge_index).face_id[x] = model->numtriangles;
								}
							}
						}

						edge_split_id.push_back((int)choose_edge[0]);
						edge_split_id.push_back((int)choose_edge[1]);
						edge_split_id.push_back(outer_edge_index);
						edge_split_id.push_back((int)choose_edge[2]);


						for (int x = 0; x < 2; x += 1){
							if (all_edge.at((int)choose_edge[0]).face_id[x] == face_split_by_plane.at(i)){
								all_edge.at((int)choose_edge[0]).face_id[x] = model->numtriangles;
							}
						}

						temp.edge_index[0] = (int)choose_edge[0];
						temp.edge_index[1] = outer_edge_index;
						temp.edge_index[2] = inner_edge_index;

						temp.material_id_graph_cut = -1;
						temp.tag = tag;

						model->triangles->push_back(temp);
						glmOneFacetNormals(model, model->numtriangles);
						model->numtriangles += 1;
						all_process_face.push_back(model->numtriangles - 1);

						dir_count[0] = 0;
						dir_count[1] = 0;
						dir_count[2] = 0;
						for (int j = 0; j < 3; j += 1){
							vec3 temp2(model->vertices->at(3 * (temp.vindices[j]) + 0), model->vertices->at(3 * (temp.vindices[j]) + 1), model->vertices->at(3 * (temp.vindices[j]) + 2));
							point_dir[j] = plane_dir_point(temp2, test_plane);
							dir_count[(int)point_dir[j] + 1] += 1;
						}

						if (dir_count[test_plane.dir * -1 + 1] == 0){
							face_split_by_plane.push_back(model->numtriangles - 1);
							model->triangles->at(model->numtriangles - 1).material_id_graph_cut = piece_id;
						}

						model->triangles->at(face_split_by_plane.at(i)).vindices[0] = model->triangles->at(face_split_by_plane.at(i)).vindices[choose_index];
						model->triangles->at(face_split_by_plane.at(i)).vindices[1] = all_edge.at((int)choose_edge[2]).vertex_push_index;
						model->triangles->at(face_split_by_plane.at(i)).vindices[2] = all_edge.at((int)choose_edge[1]).index[(int)choose_vertex_index[1]];
						model->triangles->at(face_split_by_plane.at(i)).split_by_process = true;
						glmOneFacetNormals(model, face_split_by_plane.at(i));
						all_process_face.push_back(face_split_by_plane.at(i));

						if (all_edge.at((int)choose_edge[2]).connect_index == -1){
							int old_index[2];
							old_index[0] = all_edge.at((int)choose_edge[2]).index[0];
							old_index[1] = all_edge.at((int)choose_edge[2]).index[1];

							vec3 old_vertex[2];
							old_vertex[0] = all_edge.at((int)choose_edge[2]).point[0];
							old_vertex[1] = all_edge.at((int)choose_edge[2]).point[1];

							all_edge.at((int)choose_edge[2]).index[0] = all_edge.at((int)choose_edge[1]).index[(int)choose_vertex_index[1]];
							all_edge.at((int)choose_edge[2]).point[0][0] = model->vertices->at(3 * all_edge.at((int)choose_edge[1]).index[(int)choose_vertex_index[1]] + 0);
							all_edge.at((int)choose_edge[2]).point[0][1] = model->vertices->at(3 * all_edge.at((int)choose_edge[1]).index[(int)choose_vertex_index[1]] + 1);
							all_edge.at((int)choose_edge[2]).point[0][2] = model->vertices->at(3 * all_edge.at((int)choose_edge[1]).index[(int)choose_vertex_index[1]] + 2);

							all_edge.at((int)choose_edge[2]).index[1] = all_edge.at((int)choose_edge[2]).vertex_push_index;
							all_edge.at((int)choose_edge[2]).point[1][0] = model->vertices->at(3 * all_edge.at((int)choose_edge[2]).vertex_push_index + 0);
							all_edge.at((int)choose_edge[2]).point[1][1] = model->vertices->at(3 * all_edge.at((int)choose_edge[2]).vertex_push_index + 1);
							all_edge.at((int)choose_edge[2]).point[1][2] = model->vertices->at(3 * all_edge.at((int)choose_edge[2]).vertex_push_index + 2);
							all_edge.at((int)choose_edge[2]).connect_index = outer_edge_index;

							int judge_dir[2];
							plane_dir_edge(all_edge.at((int)choose_edge[2]), test_plane, judge_dir);
							if ((judge_dir[0] == 0 || judge_dir[0] == test_plane.dir) && (judge_dir[1] == 0 || judge_dir[1] == test_plane.dir)){
								for (unsigned int j = 0; j < model->cut_loop->at(all_edge.at((int)choose_edge[2]).index[0]).align_plane.size(); j += 1){
									for (unsigned int k = 0; k < model->cut_loop->at(all_edge.at((int)choose_edge[2]).index[0]).connect_edge.size(); k += 1){
										plane_dir_edge(all_edge.at(model->cut_loop->at(all_edge.at((int)choose_edge[2]).index[0]).connect_edge.at(k)), planes.at(model->cut_loop->at(all_edge.at((int)choose_edge[2]).index[0]).align_plane.at(j)), judge_dir);
										if (judge_dir[0] == 0 && judge_dir[1] == 0 && all_edge.at(model->cut_loop->at(all_edge.at((int)choose_edge[2]).index[0]).connect_edge.at(k)).index[1] == all_edge.at((int)choose_edge[2]).vertex_push_index){
											if (find(model->cut_loop->at(all_edge.at((int)choose_edge[2]).vertex_push_index).connect_edge.begin(), model->cut_loop->at(all_edge.at((int)choose_edge[2]).vertex_push_index).connect_edge.end(), model->cut_loop->at(all_edge.at((int)choose_edge[2]).index[0]).connect_edge.at(k)) - model->cut_loop->at(all_edge.at((int)choose_edge[2]).vertex_push_index).connect_edge.begin() >= model->cut_loop->at(all_edge.at((int)choose_edge[2]).vertex_push_index).connect_edge.size())
												model->cut_loop->at(all_edge.at((int)choose_edge[2]).vertex_push_index).connect_edge.push_back(model->cut_loop->at(all_edge.at((int)choose_edge[2]).index[0]).connect_edge.at(k));

											if (find(model->cut_loop->at(all_edge.at((int)choose_edge[2]).vertex_push_index).align_plane.begin(), model->cut_loop->at(all_edge.at((int)choose_edge[2]).vertex_push_index).align_plane.end(), model->cut_loop->at(all_edge.at((int)choose_edge[2]).index[0]).align_plane.at(j)) - model->cut_loop->at(all_edge.at((int)choose_edge[2]).vertex_push_index).align_plane.begin() >= model->cut_loop->at(all_edge.at((int)choose_edge[2]).vertex_push_index).align_plane.size())
												model->cut_loop->at(all_edge.at((int)choose_edge[2]).vertex_push_index).align_plane.push_back(model->cut_loop->at(all_edge.at((int)choose_edge[2]).index[0]).align_plane.at(j));

											if (model->cut_loop->at(all_edge.at((int)choose_edge[2]).vertex_push_index).align_plane.size() >= 2)
												model->multi_vertex->push_back(all_edge.at((int)choose_edge[2]).vertex_push_index);
											break;
										}
									}
								}
							}

							plane_dir_edge(all_edge.at(all_edge.at((int)choose_edge[2]).connect_index), test_plane, judge_dir);
							if ((judge_dir[0] == 0 || judge_dir[0] == test_plane.dir) && (judge_dir[1] == 0 || judge_dir[1] == test_plane.dir)){
								for (unsigned int j = 0; j < model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[2]).connect_index).index[0]).align_plane.size(); j += 1){
									for (unsigned int k = 0; k < model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[2]).connect_index).index[0]).connect_edge.size(); k += 1){
										plane_dir_edge(all_edge.at(model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[2]).connect_index).index[0]).connect_edge.at(k)), planes.at(model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[2]).connect_index).index[0]).align_plane.at(j)), judge_dir);
										if (judge_dir[0] == 0 && judge_dir[1] == 0 && all_edge.at(model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[2]).connect_index).index[0]).connect_edge.at(k)).index[1] == all_edge.at(all_edge.at((int)choose_edge[2]).connect_index).vertex_push_index){
											if (find(model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[2]).connect_index).vertex_push_index).connect_edge.begin(), model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[2]).connect_index).vertex_push_index).connect_edge.end(), model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[2]).connect_index).index[0]).connect_edge.at(k)) - model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[2]).connect_index).vertex_push_index).connect_edge.begin() >= model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[2]).connect_index).vertex_push_index).connect_edge.size())
												model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[2]).connect_index).vertex_push_index).connect_edge.push_back(model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[2]).connect_index).index[0]).connect_edge.at(k));

											if (find(model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[2]).connect_index).vertex_push_index).align_plane.begin(), model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[2]).connect_index).vertex_push_index).align_plane.end(), model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[2]).connect_index).index[0]).align_plane.at(j)) - model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[2]).connect_index).vertex_push_index).align_plane.begin() >= model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[2]).connect_index).vertex_push_index).align_plane.size())
												model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[2]).connect_index).vertex_push_index).align_plane.push_back(model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[2]).connect_index).index[0]).align_plane.at(j));

											if (model->cut_loop->at(all_edge.at(all_edge.at((int)choose_edge[2]).connect_index).vertex_push_index).align_plane.size() >= 2)
												model->multi_vertex->push_back(all_edge.at(all_edge.at((int)choose_edge[2]).connect_index).vertex_push_index);
											break;
										}
									}
								}
							}

							if (old_index[0] != all_edge.at((int)choose_edge[2]).index[0] && old_index[0] != all_edge.at((int)choose_edge[2]).index[1] && model->cut_loop->at(old_index[0]).connect_edge.size() > 0){
								unsigned int alter_edge = find(model->cut_loop->at(old_index[0]).connect_edge.begin(), model->cut_loop->at(old_index[0]).connect_edge.end(), choose_edge[2]) - model->cut_loop->at(old_index[0]).connect_edge.begin();
								if (alter_edge < model->cut_loop->at(old_index[0]).connect_edge.size()){
									model->cut_loop->at(old_index[0]).connect_edge.at(alter_edge) = all_edge.at((int)choose_edge[2]).connect_index;
								}
							}

							if (old_index[1] != all_edge.at((int)choose_edge[2]).index[0] && old_index[1] != all_edge.at((int)choose_edge[2]).index[1] && model->cut_loop->at(old_index[1]).connect_edge.size() > 0){
								unsigned int alter_edge = find(model->cut_loop->at(old_index[1]).connect_edge.begin(), model->cut_loop->at(old_index[1]).connect_edge.end(), choose_edge[2]) - model->cut_loop->at(old_index[1]).connect_edge.begin();
								if (alter_edge < model->cut_loop->at(old_index[1]).connect_edge.size()){
									model->cut_loop->at(old_index[1]).connect_edge.at(alter_edge) = all_edge.at((int)choose_edge[2]).connect_index;
								}
							}
						}

						for (unsigned j = 0; j < model->cut_loop->at(all_edge.at((int)choose_edge[2]).vertex_push_index).connect_edge.size(); j += 1){
							int contain_dir[2];
							plane_dir_edge(all_edge.at(model->cut_loop->at(all_edge.at((int)choose_edge[2]).vertex_push_index).connect_edge.at(j)), test_plane, contain_dir);
							if ((contain_dir[0] != 0 && contain_dir[0] != test_plane.dir) || (contain_dir[1] != 0 && contain_dir[1] != test_plane.dir)){
								model->cut_loop->at(all_edge.at((int)choose_edge[2]).vertex_push_index).connect_edge.erase(model->cut_loop->at(all_edge.at((int)choose_edge[2]).vertex_push_index).connect_edge.begin() + j);
								j -= 1;
							}
						}

						for (unsigned j = 0; j < model->cut_loop->at(model->triangles->at(face_split_by_plane.at(i)).vindices[0]).connect_edge.size(); j += 1){
							int contain_dir[2];
							plane_dir_edge(all_edge.at(model->cut_loop->at(model->triangles->at(face_split_by_plane.at(i)).vindices[0]).connect_edge.at(j)), test_plane, contain_dir);
							if ((contain_dir[0] != 0 && contain_dir[0] != test_plane.dir) || (contain_dir[1] != 0 && contain_dir[1] != test_plane.dir)){
								model->cut_loop->at(model->triangles->at(face_split_by_plane.at(i)).vindices[0]).connect_edge.erase(model->cut_loop->at(model->triangles->at(face_split_by_plane.at(i)).vindices[0]).connect_edge.begin() + j);
								j -= 1;
							}
						}

						model->triangles->at(face_split_by_plane.at(i)).edge_index[0] = (int)choose_edge[1];
						model->triangles->at(face_split_by_plane.at(i)).edge_index[1] = (int)inner_edge_index;
						model->triangles->at(face_split_by_plane.at(i)).edge_index[2] = (int)choose_edge[2];

						dir_count[0] = 0;
						dir_count[1] = 0;
						dir_count[2] = 0;
						for (int j = 0; j < 3; j += 1){
							vec3 temp(model->vertices->at(3 * (model->triangles->at(face_split_by_plane.at(i)).vindices[j]) + 0), model->vertices->at(3 * (model->triangles->at(face_split_by_plane.at(i)).vindices[j]) + 1), model->vertices->at(3 * (model->triangles->at(face_split_by_plane.at(i)).vindices[j]) + 2));
							point_dir[j] = plane_dir_point(temp, test_plane);
							dir_count[(int)point_dir[j] + 1] += 1;
						}

						if (dir_count[test_plane.dir * -1 + 1] == 0){
							face_split_by_plane.push_back(face_split_by_plane.at(i));
						}
						else{
							model->triangles->at(face_split_by_plane.at(i)).material_id_graph_cut = -1;
						}

						if (all_edge.at((int)choose_edge[2]).face_id[1] != -1){
							if (!model->triangles->at(all_edge.at((int)choose_edge[2]).face_id[0]).split_by_process || !model->triangles->at(all_edge.at((int)choose_edge[2]).face_id[1]).split_by_process){
								if (!model->triangles->at(all_edge.at((int)choose_edge[2]).face_id[0]).split_by_process)
									tri_poly(model, all_edge, all_edge.at((int)choose_edge[2]).face_id[0], all_edge.at((int)choose_edge[2]), all_edge.at(outer_edge_index));
								if (!model->triangles->at(all_edge.at((int)choose_edge[2]).face_id[1]).split_by_process)
									tri_poly(model, all_edge, all_edge.at((int)choose_edge[2]).face_id[1], all_edge.at((int)choose_edge[2]), all_edge.at(outer_edge_index));
							}
						}
					}
				}
			}

			if (i == current - 1){

				for (unsigned int j = 0; j < edge_split_id.size(); j += 1){
					all_edge.at(edge_split_id.at(j)).connect_index = -1;
					all_edge.at(edge_split_id.at(j)).vertex_push_index = -1;
				}

				if (!edge_split_id.empty()){
					std::vector<int> v;
					edge_split_id.swap(v);
				}

				for (unsigned int j = 0; j < all_process_face.size(); j += 1){
					model->triangles->at(all_process_face.at(j)).split_by_process = false;
				}

				face_split_by_plane.erase(face_split_by_plane.begin(), face_split_by_plane.begin() + current);

				for (unsigned int j = 0; j < face_split_by_plane.size(); j += 1){
					model->triangles->at(face_split_by_plane.at(j)).split_by_process = true;
				}
				i = -1;
				current = face_split_by_plane.size();
				break;
			}
		}
	}

    for(unsigned int i = 0; i < planes.size(); i += 1){
        for(unsigned int j = 0; j < model->multi_vertex->size(); j += 1){
            vec3 point(model->vertices->at(3 * model->multi_vertex->at(j) + 0), model->vertices->at(3 * model->multi_vertex->at(j) + 1), model->vertices->at(3 * model->multi_vertex->at(j) + 2));
			int judge = (int)plane_dir_point(point, planes.at(i));
            if(judge == planes.at(i).dir * -1){
                if(!model->cut_loop->at(model->multi_vertex->at(j)).align_plane.empty()){
                    std::vector<unsigned int> v;
                    model->cut_loop->at(model->multi_vertex->at(j)).align_plane.swap(v);
                }
                model->multi_vertex->erase(model->multi_vertex->begin() + j);
                j -= 1;
            }
            if(judge == 0){
                if((unsigned int)(find(model->cut_loop->at(model->multi_vertex->at(j)).align_plane.begin(), model->cut_loop->at(model->multi_vertex->at(j)).align_plane.end(), i) - model->cut_loop->at(model->multi_vertex->at(j)).align_plane.begin()) >= model->cut_loop->at(model->multi_vertex->at(j)).align_plane.size()){
                    model->cut_loop->at(model->multi_vertex->at(j)).align_plane.push_back(i);
                }
            }
        }
    }

    for(unsigned int i = 0; i < model->multi_vertex->size(); i += 1){
        sort(model->cut_loop->at(model->multi_vertex->at(i)).align_plane.begin(), model->cut_loop->at(model->multi_vertex->at(i)).align_plane.begin() + model->cut_loop->at(model->multi_vertex->at(i)).align_plane.size());
    }
}

void tri_poly(GLMmodel *model, std::vector<edge> &all_edge, int face_id, edge &splited_edge1, edge &splited_edge2){
	int tag = model->triangles->at(face_id).tag;
	
	int current_index;
    for(int i = 0; i < 3; i += 1){
        if(all_edge.at(model->triangles->at(face_id).edge_index[i]) == splited_edge1){
            current_index = i;
            break;
        }
    }

    int choose_index[2] = {model->triangles->at(face_id).edge_index[(current_index + 2) % 3], model->triangles->at(face_id).edge_index[(current_index + 1) % 3]};
    int top_index;
    int choose_vertex_index[2] = {-1, -1};
    bool judge1,judge2;
    judge1 = all_edge.at(choose_index[0]).index[0] == all_edge.at(choose_index[1]).index[0];
    judge2 = all_edge.at(choose_index[0]).index[0] == all_edge.at(choose_index[1]).index[1];
    if(judge1){
        top_index = 0;
        choose_vertex_index[0] = 1;
        choose_vertex_index[1] = 1;
    }
    if(judge2){
        top_index = 0;
        choose_vertex_index[0] = 1;
        choose_vertex_index[1] = 0;
    }
    if(choose_vertex_index[0] == -1 && choose_vertex_index[1] == -1){
        judge1 = all_edge.at(choose_index[0]).index[1] == all_edge.at(choose_index[1]).index[0];
        judge2 = all_edge.at(choose_index[0]).index[1] == all_edge.at(choose_index[1]).index[1];
        if(judge1){
            top_index = 1;
            choose_vertex_index[0] = 0;
            choose_vertex_index[1] = 1;
        }
        if(judge2){
            top_index = 1;
            choose_vertex_index[0] = 0;
            choose_vertex_index[1] = 0;
        }
    }

    GLMtriangle temp;

    temp.vindices[0] = all_edge.at(choose_index[0]).index[top_index];
    temp.vindices[1] = all_edge.at(choose_index[0]).index[choose_vertex_index[0]];
    temp.vindices[2] = splited_edge1.vertex_push_index;
    temp.split_by_process = false;

    edge inner_e;
    int inner_edge_index;
    int outer_edge_index;

    inner_e.index[0] = all_edge.at(choose_index[0]).index[top_index];
    inner_e.index[1] = splited_edge1.vertex_push_index;
    inner_e.point[0][0] = model->vertices->at(3 * all_edge.at(choose_index[0]).index[top_index] + 0);
    inner_e.point[0][1] = model->vertices->at(3 * all_edge.at(choose_index[0]).index[top_index] + 1);
    inner_e.point[0][2] = model->vertices->at(3 * all_edge.at(choose_index[0]).index[top_index] + 2);
    inner_e.point[1][0] = model->vertices->at(3 * splited_edge1.vertex_push_index + 0);
    inner_e.point[1][1] = model->vertices->at(3 * splited_edge1.vertex_push_index + 1);
    inner_e.point[1][2] = model->vertices->at(3 * splited_edge1.vertex_push_index + 2);
    inner_e.face_id[0] = face_id;
    inner_e.face_id[1] = model->numtriangles;
    inner_e.connect_index = -1;
    all_edge.push_back(inner_e);
    inner_edge_index = all_edge.size() - 1;

    if(all_edge.at(choose_index[0]).index[1 - top_index] == splited_edge1.index[0] || all_edge.at(choose_index[0]).index[1 - top_index] == splited_edge1.index[1]){
        outer_edge_index = splited_edge2.connect_index;
    }
    else{
        outer_edge_index = splited_edge1.connect_index;
    }

    temp.edge_index[0] = inner_edge_index;
    temp.edge_index[1] = choose_index[0];
    temp.edge_index[2] = outer_edge_index;
	temp.material_id_graph_cut = model->triangles->at(face_id).material_id_graph_cut;

    for(int x = 0; x < 2; x += 1){
        if(all_edge.at(outer_edge_index).face_id[x] == face_id)
            all_edge.at(outer_edge_index).face_id[x] = model->numtriangles;
        if(all_edge.at(choose_index[0]).face_id[x] == face_id)
            all_edge.at(choose_index[0]).face_id[x] = model->numtriangles;
    }

	temp.tag = tag;

    model->triangles->push_back(temp);
    glmOneFacetNormals(model, model->numtriangles);
    model->numtriangles += 1;

    model->triangles->at(face_id).vindices[0] = all_edge.at(choose_index[0]).index[top_index];
    model->triangles->at(face_id).vindices[1] = splited_edge1.vertex_push_index;
    model->triangles->at(face_id).vindices[2] = all_edge.at(choose_index[1]).index[choose_vertex_index[1]];
    model->triangles->at(face_id).split_by_process = false;

    model->triangles->at(face_id).edge_index[0] = inner_edge_index;
    model->triangles->at(face_id).edge_index[1] = all_edge.at(outer_edge_index).connect_index;
    model->triangles->at(face_id).edge_index[2] = choose_index[1];

    glmOneFacetNormals(model, face_id);
}

void find_easy_loop(GLMmodel *model, std::vector<edge> &all_edge, std::vector<int> &single_use)
{
	int num_l = 1;
	std::vector<Loop> all_l;

	int start = single_use.at(0);
	int travel = single_use.at(0);
	int pre = -1;
	Loop new_loop;
	new_loop.loop_line = new std::vector<int>();
	new_loop.loop_line->push_back(start);

	while (true){
		for (unsigned int i = 0; i < model->cut_loop->at(travel).connect_edge.size(); i += 1){
			int judge;
			if (all_edge.at(model->cut_loop->at(travel).connect_edge.at(i)).index[0] == travel){
				if (all_edge.at(model->cut_loop->at(travel).connect_edge.at(i)).face_id[1] == -1)
					judge = all_edge.at(model->cut_loop->at(travel).connect_edge.at(i)).index[1];
			}
			else{
				if (all_edge.at(model->cut_loop->at(travel).connect_edge.at(i)).face_id[1] == -1)
					judge = all_edge.at(model->cut_loop->at(travel).connect_edge.at(i)).index[0];
			}
			unsigned int pos = (find(single_use.begin(), single_use.end(), judge) - single_use.begin());
			if (pos < single_use.size()){
				if (judge != pre){
					pre = travel;
					travel = judge;
					new_loop.loop_line->push_back(travel);
					single_use.erase(single_use.begin() + pos);
					break;
				}
			}
		}

		if (travel == start){
			new_loop.loop_line->erase(new_loop.loop_line->begin() + new_loop.loop_line->size() - 1);
			Loop temp_loop;
			all_l.push_back(temp_loop);
			all_l.at(num_l - 1).loop_line = new std::vector<int>();
			for (unsigned int i = 0; i < new_loop.loop_line->size(); i += 1){
				all_l.at(num_l - 1).loop_line->push_back(new_loop.loop_line->at(i));
			}
			num_l += 1;
			delete new_loop.loop_line;
			if (single_use.size() > 0){
				new_loop.loop_line = new std::vector<int>();
				start = single_use.at(0);
				travel = single_use.at(0);
				pre = -1;
				new_loop.loop_line->push_back(start);
			}
			else
				break;
		}
	}

	model->loop = new std::vector<Loop>(all_l.size());
	for (unsigned int i = 0; i < model->loop->size(); i += 1){
		model->loop->at(i).loop_line = new std::vector<int>();
		model->loop->at(i).three_d_point = new std::vector<vec3>();
		for (unsigned int j = 0; j < all_l.at(i).loop_line->size(); j += 1){
			model->loop->at(i).loop_line->push_back(all_l.at(i).loop_line->at(j));
			vec3 p(model->vertices->at(3 * all_l.at(i).loop_line->at(j) + 0), model->vertices->at(3 * all_l.at(i).loop_line->at(j) + 1), model->vertices->at(3 * all_l.at(i).loop_line->at(j) + 2));
			model->loop->at(i).three_d_point->push_back(p);
		}
	}
}

void combine_loop(GLMmodel *model, plane &plane, std::vector<int> &final_outer, std::vector<int> &final_inner)
{
	bool inverse = false;
	vec3 outer_last = vec3(model->vertices->at(3 * final_outer.at(final_outer.size() - 1) + 0), model->vertices->at(3 * final_outer.at(final_outer.size() - 1) + 1), model->vertices->at(3 * final_outer.at(final_outer.size() - 1) + 2));
	vec3 inner_first = vec3(model->vertices->at(3 * final_inner.at(0) + 0), model->vertices->at(3 * final_inner.at(0) + 1), model->vertices->at(3 * final_inner.at(0) + 2));
	vec3 inner_last = vec3(model->vertices->at(3 * final_inner.at(final_inner.size() - 1) + 0), model->vertices->at(3 * final_inner.at(final_inner.size() - 1) + 1), model->vertices->at(3 * final_inner.at(final_inner.size() - 1) + 2));

	float ol_if, ol_il;
	ol_if = (outer_last - inner_first).length();
	ol_il = (outer_last - inner_last).length();

	if (ol_if > ol_il)
		inverse = true;

	std::vector<int> final_loop;
	for (int j = 0; j < final_outer.size(); j += 1){
		final_loop.push_back(final_outer.at(j));
	}

	if (!inverse){
		for (int j = 0; j < final_inner.size(); j += 1){
			final_loop.push_back(final_inner.at(j));
		}
	}
	else{
		for (int j = final_inner.size() - 1; j >= 0; j -= 1){
			final_loop.push_back(final_inner.at(j));
		}
	}

	Loop temp;
	temp.loop_line = new std::vector<int>();

	for (int j = 0; j < final_loop.size(); j += 1){
		temp.loop_line->push_back(final_loop.at(j));
	}

	temp.plane_normal = vec3(plane.plane_par[0], plane.plane_par[1], plane.plane_par[2]);

	model->loop->push_back(temp);
}

void find_shell_loop(GLMmodel *model, std::vector<edge> &all_edge, std::vector<plane> &planes)
{
	for (int i = 0; i < planes.size(); i += 1){
		for (int j = 0; j < all_edge.size(); j += 1){
			int dir[2];
			plane_dir_edge(all_edge.at(j), planes.at(i), dir);
			if (dir[0] == 0 && dir[1] == 0){
				if (all_edge.at(j).face_id[0] == -1 || all_edge.at(j).face_id[1] == -1){
					if (find(model->cut_loop->at(all_edge.at(j).index[0]).connect_edge.begin(), model->cut_loop->at(all_edge.at(j).index[0]).connect_edge.end(), j) - model->cut_loop->at(all_edge.at(j).index[0]).connect_edge.begin() >= model->cut_loop->at(all_edge.at(j).index[0]).connect_edge.size())
						model->cut_loop->at(all_edge.at(j).index[0]).connect_edge.push_back(j);
					if (find(model->cut_loop->at(all_edge.at(j).index[0]).align_plane.begin(), model->cut_loop->at(all_edge.at(j).index[0]).align_plane.end(), i) - model->cut_loop->at(all_edge.at(j).index[0]).align_plane.begin() >= model->cut_loop->at(all_edge.at(j).index[0]).align_plane.size())
						model->cut_loop->at(all_edge.at(j).index[0]).align_plane.push_back(i);

					if (find(model->cut_loop->at(all_edge.at(j).index[1]).connect_edge.begin(), model->cut_loop->at(all_edge.at(j).index[1]).connect_edge.end(), j) - model->cut_loop->at(all_edge.at(j).index[1]).connect_edge.begin() >= model->cut_loop->at(all_edge.at(j).index[1]).connect_edge.size())
						model->cut_loop->at(all_edge.at(j).index[1]).connect_edge.push_back(j);
					if (find(model->cut_loop->at(all_edge.at(j).index[1]).align_plane.begin(), model->cut_loop->at(all_edge.at(j).index[1]).align_plane.end(), i) - model->cut_loop->at(all_edge.at(j).index[1]).align_plane.begin() >= model->cut_loop->at(all_edge.at(j).index[1]).align_plane.size())
						model->cut_loop->at(all_edge.at(j).index[1]).align_plane.push_back(i);
				}
			}
		}
	}

	std::vector<int> outer_index;
	std::vector<int> inner_index;

	for (int i = 1; i <= model->numvertices; i += 1){
		if (model->cut_loop->at(i).tag == OUTER){
			if (model->cut_loop->at(i).align_plane.size() > 0)
				outer_index.push_back(i);
		}
		else{
			if (model->cut_loop->at(i).align_plane.size() > 0)
				inner_index.push_back(i);
		}
	}
	for (int i = 0; i < planes.size(); i += 1){
		std::vector<std::vector<int>> outer_loop_stack;
		std::vector<std::vector<int>> inner_loop_stack;

		std::vector<int> outer_loop;
		std::vector<int> inner_loop;

		//outer
		for (int j = 0; j < outer_index.size(); j += 1){
			if (find(model->cut_loop->at(outer_index.at(j)).align_plane.begin(), model->cut_loop->at(outer_index.at(j)).align_plane.end(), i) - model->cut_loop->at(outer_index.at(j)).align_plane.begin() < model->cut_loop->at(outer_index.at(j)).align_plane.size()){
				outer_loop.push_back(outer_index.at(j));
			}
		}

		if (outer_loop.size() == 0){
			continue;
		}

		while (outer_loop.size() != 0){

			int outer_start_index = -1;
			int outer_end_index = -1;

			bool outer_get_start = false;

			for (int j = 0; j < outer_loop.size(); j += 1){
				bool get_index = false;

				for (int k = 0; k < model->cut_loop->at(outer_loop.at(j)).connect_edge.size(); k += 1){
					edge temp = all_edge.at(model->cut_loop->at(outer_loop.at(j)).connect_edge.at(k));
					if (find(outer_loop.begin(), outer_loop.end(), temp.index[0]) - outer_loop.begin() >= outer_loop.size()){
						get_index = true;
						break;
					}
					if (find(outer_loop.begin(), outer_loop.end(), temp.index[1]) - outer_loop.begin() >= outer_loop.size()){
						get_index = true;
						break;
					}
				}
				if (get_index){
					outer_start_index = outer_loop.at(j);
					outer_get_start = true;
					break;
				}
			}

			if (outer_start_index == -1){
				break;
			}

			std::vector<int> final_outer;
			int outer_next = outer_start_index;
			final_outer.push_back(outer_next);

			int outer_loop_start_index = find(outer_loop.begin(), outer_loop.end(), outer_next) - outer_loop.begin();
			outer_loop.erase(outer_loop.begin() + outer_loop_start_index);

			while (true){
				bool end_time = true;

				for (int j = 0; j < model->cut_loop->at(outer_next).connect_edge.size(); j += 1){
					edge temp = all_edge.at(model->cut_loop->at(outer_next).connect_edge.at(j));
					if ((temp.index[0] != outer_next) && (find(outer_loop.begin(), outer_loop.end(), temp.index[0]) - outer_loop.begin() < outer_loop.size())){
						outer_next = temp.index[0];
						final_outer.push_back(outer_next);

						int index = find(outer_loop.begin(), outer_loop.end(), temp.index[0]) - outer_loop.begin();
						outer_loop.erase(outer_loop.begin() + index);

						end_time = false;
						break;
					}
					if ((temp.index[1] != outer_next) && (find(outer_loop.begin(), outer_loop.end(), temp.index[1]) - outer_loop.begin() < outer_loop.size())){
						outer_next = temp.index[1];
						final_outer.push_back(outer_next);

						int index = find(outer_loop.begin(), outer_loop.end(), temp.index[1]) - outer_loop.begin();
						outer_loop.erase(outer_loop.begin() + index);

						end_time = false;
						break;
					}
				}

				if (end_time){
					break;
				}
			}

			outer_loop_stack.push_back(final_outer);
		}

		//inner
		for (int j = 0; j < inner_index.size(); j += 1){
			if (find(model->cut_loop->at(inner_index.at(j)).align_plane.begin(), model->cut_loop->at(inner_index.at(j)).align_plane.end(), i) - model->cut_loop->at(inner_index.at(j)).align_plane.begin() < model->cut_loop->at(inner_index.at(j)).align_plane.size()){
				inner_loop.push_back(inner_index.at(j));
			}
		}

		if (inner_loop.size() == 0){
			continue;
		}
				
		while (inner_loop.size() != 0){
			int inner_start_index = -1;
			int inner_end_index = -1;

			bool inner_get_start = false;

			for (int j = 0; j < inner_loop.size(); j += 1){
				bool get_index = false;

				for (int k = 0; k < model->cut_loop->at(inner_loop.at(j)).connect_edge.size(); k += 1){
					edge temp = all_edge.at(model->cut_loop->at(inner_loop.at(j)).connect_edge.at(k));
					if (find(inner_loop.begin(), inner_loop.end(), temp.index[0]) - inner_loop.begin() >= inner_loop.size()){
						get_index = true;
						break;
					}
					if (find(inner_loop.begin(), inner_loop.end(), temp.index[1]) - inner_loop.begin() >= inner_loop.size()){
						get_index = true;
						break;
					}
				}
				if (get_index){
					inner_start_index = inner_loop.at(j);
					inner_get_start = true;
					break;
				}
			}

			if (inner_start_index == -1){
				if (inner_loop.size() == 1){
					inner_start_index = inner_loop.at(0);
				}
				else{
					break;
				}				
			}

			std::vector<int> final_inner;
			int inner_next = inner_start_index;
			final_inner.push_back(inner_next);

			int inner_loop_start_index = find(inner_loop.begin(), inner_loop.end(), inner_next) - inner_loop.begin();
			inner_loop.erase(inner_loop.begin() + inner_loop_start_index);

			while (true){
				bool end_time = true;

				for (int j = 0; j < model->cut_loop->at(inner_next).connect_edge.size(); j += 1){
					edge temp = all_edge.at(model->cut_loop->at(inner_next).connect_edge.at(j));
					if ((temp.index[0] != inner_next) && (find(inner_loop.begin(), inner_loop.end(), temp.index[0]) - inner_loop.begin() < inner_loop.size())){
						inner_next = temp.index[0];
						final_inner.push_back(inner_next);

						int index = find(inner_loop.begin(), inner_loop.end(), temp.index[0]) - inner_loop.begin();
						inner_loop.erase(inner_loop.begin() + index);

						end_time = false;
						break;
					}
					if ((temp.index[1] != inner_next) && (find(inner_loop.begin(), inner_loop.end(), temp.index[1]) - inner_loop.begin() < inner_loop.size())){
						inner_next = temp.index[1];
						final_inner.push_back(inner_next);

						int index = find(inner_loop.begin(), inner_loop.end(), temp.index[1]) - inner_loop.begin();
						inner_loop.erase(inner_loop.begin() + index);

						end_time = false;
						break;
					}
				}

				if (end_time)
					break;
			}

			inner_loop_stack.push_back(final_inner);
		}

		std::vector<vec3> outer_loop_center;
		std::vector<vec3> inner_loop_center;

		for (int j = 0; j < outer_loop_stack.size(); j += 1){
			vec3 sum = vec3(0.0f, 0.0f, 0.0f);
			for (int k = 0; k < outer_loop_stack.at(j).size(); k += 1){
				vec3 temp(model->vertices->at(3 * outer_loop_stack.at(j).at(k) + 0), model->vertices->at(3 * outer_loop_stack.at(j).at(k) + 1), model->vertices->at(3 * outer_loop_stack.at(j).at(k) + 2));
				sum += temp;
			}
			sum /= outer_loop_stack.at(j).size();
			outer_loop_center.push_back(sum);
		}

		for (int j = 0; j < inner_loop_stack.size(); j += 1){
			vec3 sum = vec3(0.0f, 0.0f, 0.0f);
			for (int k = 0; k < inner_loop_stack.at(j).size(); k += 1){
				vec3 temp(model->vertices->at(3 * inner_loop_stack.at(j).at(k) + 0), model->vertices->at(3 * inner_loop_stack.at(j).at(k) + 1), model->vertices->at(3 * inner_loop_stack.at(j).at(k) + 2));
				sum += temp;
			}
			sum /= inner_loop_stack.at(j).size();
			inner_loop_center.push_back(sum);
		}

		vec3 p1(model->vertices->at(3 * outer_loop_stack.at(0).at(0) + 0), model->vertices->at(3 * outer_loop_stack.at(0).at(0) + 1), model->vertices->at(3 * outer_loop_stack.at(0).at(0) + 2));
		vec3 p2(model->vertices->at(3 * outer_loop_stack.at(0).at(outer_loop_stack.at(0).size() - 1) + 0), model->vertices->at(3 * outer_loop_stack.at(0).at(outer_loop_stack.at(0).size() - 1) + 1), model->vertices->at(3 * outer_loop_stack.at(0).at(outer_loop_stack.at(0).size() - 1) + 2));

		while (outer_loop_stack.size() > 0 && inner_loop_stack.size() > 0){
				vec3 o_center = outer_loop_center.at(0);
				int min_index = 0;
				int min_dist = 100000;
				for (int j = 0; j < inner_loop_stack.size(); j += 1){
					vec3 temp_i_center = inner_loop_center.at(j);
					float now_dist = (o_center - temp_i_center).length();
					if (now_dist < min_dist){
						min_dist = now_dist;
						min_index = j;
					}
				}

				combine_loop(model, planes.at(i), outer_loop_stack.at(0), inner_loop_stack.at(min_index));

				outer_loop_stack.erase(outer_loop_stack.begin() + 0);
				inner_loop_stack.erase(inner_loop_stack.begin() + min_index);

				outer_loop_center.erase(outer_loop_center.begin() + 0);
				inner_loop_center.erase(inner_loop_center.begin() + min_index);
		}

		while (outer_loop.size() != 0){

			int outer_start_index = -1;
			int outer_end_index = -1;

			bool outer_get_start = false;

			for (int j = 0; j < outer_loop.size(); j += 1){
				bool get_index = false;

				for (int k = 0; k < model->cut_loop->at(outer_loop.at(j)).connect_edge.size(); k += 1){
					edge temp = all_edge.at(model->cut_loop->at(outer_loop.at(j)).connect_edge.at(k));
					if (find(outer_loop.begin(), outer_loop.end(), temp.index[0]) - outer_loop.begin() < outer_loop.size()){
						get_index = true;
						break;
					}
					if (find(outer_loop.begin(), outer_loop.end(), temp.index[1]) - outer_loop.begin() < outer_loop.size()){
						get_index = true;
						break;
					}
				}
				if (get_index){
					outer_start_index = outer_loop.at(j);
					outer_get_start = true;
					break;
				}
			}

			if (outer_start_index == -1){
				break;
			}

			std::vector<int> final_outer;
			int outer_next = outer_start_index;
			final_outer.push_back(outer_next);

			int outer_loop_start_index = find(outer_loop.begin(), outer_loop.end(), outer_next) - outer_loop.begin();
			outer_loop.erase(outer_loop.begin() + outer_loop_start_index);

			while (true){
				bool end_time = true;

				for (int j = 0; j < model->cut_loop->at(outer_next).connect_edge.size(); j += 1){
					edge temp = all_edge.at(model->cut_loop->at(outer_next).connect_edge.at(j));
					if ((temp.index[0] != outer_next) && (find(outer_loop.begin(), outer_loop.end(), temp.index[0]) - outer_loop.begin() < outer_loop.size())){
						outer_next = temp.index[0];
						final_outer.push_back(outer_next);

						int index = find(outer_loop.begin(), outer_loop.end(), temp.index[0]) - outer_loop.begin();
						outer_loop.erase(outer_loop.begin() + index);

						end_time = false;
						break;
					}
					if ((temp.index[1] != outer_next) && (find(outer_loop.begin(), outer_loop.end(), temp.index[1]) - outer_loop.begin() < outer_loop.size())){
						outer_next = temp.index[1];
						final_outer.push_back(outer_next);

						int index = find(outer_loop.begin(), outer_loop.end(), temp.index[1]) - outer_loop.begin();
						outer_loop.erase(outer_loop.begin() + index);

						end_time = false;
						break;
					}
				}

				if (end_time){
					break;
				}
			}

			outer_loop_stack.push_back(final_outer);
		}

		while (inner_loop.size() != 0){
			int inner_start_index = -1;
			int inner_end_index = -1;

			bool inner_get_start = false;

			for (int j = 0; j < inner_loop.size(); j += 1){
				bool get_index = false;

				for (int k = 0; k < model->cut_loop->at(inner_loop.at(j)).connect_edge.size(); k += 1){
					edge temp = all_edge.at(model->cut_loop->at(inner_loop.at(j)).connect_edge.at(k));
					if (find(inner_loop.begin(), inner_loop.end(), temp.index[0]) - inner_loop.begin() < inner_loop.size()){
						get_index = true;
						break;
					}
					if (find(inner_loop.begin(), inner_loop.end(), temp.index[1]) - inner_loop.begin() < inner_loop.size()){
						get_index = true;
						break;
					}
				}
				if (get_index){
					inner_start_index = inner_loop.at(j);
					inner_get_start = true;
					break;
				}
			}
			
			if (inner_start_index == -1){
				break;

			}

			std::vector<int> final_inner;
			int inner_next = inner_start_index;
			final_inner.push_back(inner_next);

			int inner_loop_start_index = find(inner_loop.begin(), inner_loop.end(), inner_next) - inner_loop.begin();
			inner_loop.erase(inner_loop.begin() + inner_loop_start_index);

			while (true){
				bool end_time = true;

				for (int j = 0; j < model->cut_loop->at(inner_next).connect_edge.size(); j += 1){
					edge temp = all_edge.at(model->cut_loop->at(inner_next).connect_edge.at(j));
					if ((temp.index[0] != inner_next) && (find(inner_loop.begin(), inner_loop.end(), temp.index[0]) - inner_loop.begin() < inner_loop.size())){
						inner_next = temp.index[0];
						final_inner.push_back(inner_next);

						int index = find(inner_loop.begin(), inner_loop.end(), temp.index[0]) - inner_loop.begin();
						inner_loop.erase(inner_loop.begin() + index);

						end_time = false;
						break;
					}
					if ((temp.index[1] != inner_next) && (find(inner_loop.begin(), inner_loop.end(), temp.index[1]) - inner_loop.begin() < inner_loop.size())){
						inner_next = temp.index[1];
						final_inner.push_back(inner_next);

						int index = find(inner_loop.begin(), inner_loop.end(), temp.index[1]) - inner_loop.begin();
						inner_loop.erase(inner_loop.begin() + index);

						end_time = false;
						break;
					}
				}

				if (end_time)
					break;
			}

			inner_loop_stack.push_back(final_inner);
		}

		for (int j = 0; j < outer_loop_stack.size(); j += 1){
			Loop temp;
			temp.loop_line = new std::vector<int>();
			for (int k = 0; k < outer_loop_stack.at(j).size(); k += 1){
				temp.loop_line->push_back(outer_loop_stack.at(j).at(k));
			}

			temp.plane_normal = vec3(planes.at(i).plane_par[0], planes.at(i).plane_par[1], planes.at(i).plane_par[2]);
			model->loop->push_back(temp);
		}

		for (int j = 0; j < inner_loop_stack.size(); j += 1){
			Loop temp;
			temp.loop_line = new std::vector<int>();
			for (int k = 0; k < inner_loop_stack.at(j).size(); k += 1){
				temp.loop_line->push_back(inner_loop_stack.at(j).at(k));
			}

			temp.plane_normal = vec3(planes.at(i).plane_par[0], planes.at(i).plane_par[1], planes.at(i).plane_par[2]);
			model->loop->push_back(temp);
		}		
	}
}

void process_piece(GLMmodel &temp_piece, GLMmodel *model, std::vector<int> &face_split_by_plane)
{
    std::vector<int> vertex_map(model->numvertices + 1, -1);
    std::vector<int> use_vertex;
    temp_piece.numtriangles = face_split_by_plane.size();
    temp_piece.numvertices = 0;
    temp_piece.position[0] = 0.0;
    temp_piece.position[1] = 0.0;
    temp_piece.position[2] = 0.0;

    temp_piece.vertices = new std::vector<GLfloat>();
    temp_piece.vertices->push_back(-10000000000000000000.0f);
    temp_piece.vertices->push_back(-10000000000000000000.0f);
    temp_piece.vertices->push_back(-10000000000000000000.0f);

	temp_piece.cut_loop = new std::vector<vertex>(1);
	
	temp_piece.triangles = new std::vector<GLMtriangle>();
	
    int current_index = 1;
    for(unsigned int i = 0; i < face_split_by_plane.size(); i += 1){
        int temp_index[3];
        temp_index[0] = model->triangles->at(face_split_by_plane.at(i)).vindices[0];
        temp_index[1] = model->triangles->at(face_split_by_plane.at(i)).vindices[1];
        temp_index[2] = model->triangles->at(face_split_by_plane.at(i)).vindices[2];

        if(vertex_map.at(temp_index[0]) == -1){
            vertex_map.at(temp_index[0]) = current_index;
            current_index += 1;
            temp_piece.numvertices += 1;
            temp_piece.vertices->push_back(model->vertices->at(3 * temp_index[0] + 0));
            temp_piece.vertices->push_back(model->vertices->at(3 * temp_index[0] + 1));
            temp_piece.vertices->push_back(model->vertices->at(3 * temp_index[0] + 2));

			vertex temp_v;
			temp_v.tag = model->triangles->at(face_split_by_plane.at(i)).tag;
			temp_piece.cut_loop->push_back(temp_v);
        }
        if(vertex_map.at(temp_index[1]) == -1){
            vertex_map.at(temp_index[1]) = current_index;
            current_index += 1;
            temp_piece.numvertices += 1;
            temp_piece.vertices->push_back(model->vertices->at(3 * temp_index[1] + 0));
            temp_piece.vertices->push_back(model->vertices->at(3 * temp_index[1] + 1));
			temp_piece.vertices->push_back(model->vertices->at(3 * temp_index[1] + 2));

			vertex temp_v;
			temp_v.tag = model->triangles->at(face_split_by_plane.at(i)).tag;
			temp_piece.cut_loop->push_back(temp_v);
        }
        if(vertex_map.at(temp_index[2]) == -1){
            vertex_map.at(temp_index[2]) = current_index;
            current_index += 1;
            temp_piece.numvertices += 1;
            temp_piece.vertices->push_back(model->vertices->at(3 * temp_index[2] + 0));
            temp_piece.vertices->push_back(model->vertices->at(3 * temp_index[2] + 1));
			temp_piece.vertices->push_back(model->vertices->at(3 * temp_index[2] + 2));

			vertex temp_v;
			temp_v.tag = model->triangles->at(face_split_by_plane.at(i)).tag;
			temp_piece.cut_loop->push_back(temp_v);
        }

        GLMtriangle temp_t;
        temp_t.vindices[0] = vertex_map.at(temp_index[0]);
        temp_t.vindices[1] = vertex_map.at(temp_index[1]);
        temp_t.vindices[2] = vertex_map.at(temp_index[2]);

		temp_t.tag = model->triangles->at(face_split_by_plane.at(i)).tag;

        temp_piece.triangles->push_back(temp_t);
    }	

	temp_piece.loop = new std::vector<Loop>();

	temp_piece.facetnorms = new std::vector<GLfloat>();
}

void generate_fake_inner(GLMmodel &temp_piece, std::vector<plane> &planes, std::vector<edge> &all_edge)
{
	bool fake = true;

	for (unsigned int i = 0; i < temp_piece.triangles->size(); i += 1){
		if (temp_piece.triangles->at(i).tag == INNER){
			fake = false;
			break;
		}
	}

	if (fake){
		vec3 p1_n(planes.at(0).plane_par[0], planes.at(0).plane_par[1], planes.at(0).plane_par[2]);
		vec3 p2_n(planes.at(1).plane_par[0], planes.at(1).plane_par[1], planes.at(1).plane_par[2]);

		float p1_d = planes.at(0).plane_par[3];
		float p2_d = planes.at(1).plane_par[3];

		vec3 l_dir = (p1_n ^ p2_n).normalize();

		vec3 temp_p;
		temp_p[2] = 0.0f;
		temp_p[0] = (p2_n[1] * p1_d - p1_n[1] * p2_d) / (p1_n[0] * p2_n[1] - p1_n[1] * p2_n[0]);
		temp_p[1] = (p2_n[0] * p1_d - p1_n[0] * p2_d) / (p1_n[1] * p2_n[0] - p1_n[0] * p2_n[1]);

		vec3 p3_n(planes.at(2).plane_par[0], planes.at(2).plane_par[1], planes.at(2).plane_par[2]);

		float p3_d = planes.at(2).plane_par[3];

		float t = (p3_d - p3_n[0] * temp_p[0] - p3_n[1] * temp_p[1] - p3_n[2] * temp_p[2]) / (p3_n[0] * l_dir[0] + p3_n[1] * l_dir[1] + p3_n[2] * l_dir[2]);
		
		vec3 new_p = temp_p + t * l_dir;

		vertex temp_vertex;
		temp_vertex.tag = INNER;
		temp_piece.cut_loop->push_back(temp_vertex);

		temp_piece.numvertices += 1;

		temp_piece.vertices->push_back(new_p[0]);
		temp_piece.vertices->push_back(new_p[1]);
		temp_piece.vertices->push_back(new_p[2]);

		edge temp_edge;
		temp_edge.index[0] = temp_piece.numvertices;
		temp_edge.index[1] = temp_piece.numvertices;
		temp_edge.face_id[0] = -1;
		temp_edge.face_id[1] = -1;
		temp_edge.point[0] = new_p;
		temp_edge.point[1] = new_p;
		all_edge.push_back(temp_edge);
	}
}

void cut_mesh(GLMmodel *model, std::string &model_file, std::string &shell_file, std::string &zome_file, int plane_tune)
{
	std::vector<std::vector<plane>> all_planes;
	std::vector<edge> all_edge;
	std::vector<int> face_split_by_plane;
	std::vector<int> face_inner_split_by_plane;

	glmFacetNormals(model);

	GLMmodel *model_inner = glmReadOBJ(my_strdup(shell_file.c_str()));
	glmFacetNormals(model_inner);
	combine_inner_outfit(model, model_inner);

	collect_edge(model, all_edge);

	find_near_tri(model, all_edge);

	std::string plane_file = "training_plane_" + model_file;
	if (plane_tune != 0){
		plane_file += "_tune.txt";
	}
	else{
		plane_file += ".txt";
	}

	plane_parser_SVM(all_planes, plane_file);

	output_plane_SVM(all_planes, model_file, std::string(model_file + "_neighbor.txt"));

	for (unsigned int i = 0; i < all_planes.size(); i += 1){

		std::vector<int> temp_face_split_by_plane;
		cut_intersection(model, all_planes.at(i), temp_face_split_by_plane, i, false);
		split_face(model, all_edge, temp_face_split_by_plane, all_planes.at(i));

		for (unsigned int j = 0; j < model->cut_loop->size(); j += 1){
			model->cut_loop->at(j).connect_edge.erase(model->cut_loop->at(j).connect_edge.begin(), model->cut_loop->at(j).connect_edge.begin() + model->cut_loop->at(j).connect_edge.size());
			model->cut_loop->at(j).align_plane.erase(model->cut_loop->at(j).align_plane.begin(), model->cut_loop->at(j).align_plane.begin() + model->cut_loop->at(j).align_plane.size());
		}

		find_near_tri(model, all_edge);

		for (unsigned int j = 0; j < model->numtriangles; j += 1){
			if (!model->triangles->at(j).split_plane_id.empty()){
				std::vector<unsigned int> v;
				model->triangles->at(j).split_plane_id.swap(v);
			}

			model->triangles->at(j).split_by_process = false;
		}
	}

	std::vector<std::vector<int>> all_face_split_by_plane(all_planes.size());
	for (unsigned int i = 0; i < all_planes.size(); i += 1){
		cut_intersection(model, all_planes.at(i), all_face_split_by_plane.at(i), i, false);

		for (unsigned int j = 0; j < all_face_split_by_plane.at(i).size(); j += 1){
			model->triangles->at(all_face_split_by_plane.at(i).at(j)).material_id_graph_cut = i;
		}
	}

	std::vector<int> not_arrange_tri;
	std::vector<int> not_arrange_tri_convert_index(model->numtriangles);
	std::vector<int> group_tag;
	std::vector<std::vector<int>> arrange_group;

	for (int i = 0; i < model->numtriangles; i += 1){
		if (model->triangles->at(i).material_id_graph_cut == -1){
			not_arrange_tri_convert_index.at(i) = not_arrange_tri.size();
			not_arrange_tri.push_back(i);
			group_tag.push_back(-1);
		}
	}

	for (unsigned int i = 0; i < not_arrange_tri.size(); i += 1){
		if (group_tag.at(i) == -1){
			std::vector<int> temp_group;
			temp_group.push_back(not_arrange_tri.at(i));
			group_tag.at(i) = arrange_group.size();

			for (unsigned int j = 0; j < temp_group.size(); j += 1){
				for (int k = 0; k < 3; k += 1){
					if (group_tag.at(not_arrange_tri_convert_index.at(model->triangles->at(temp_group.at(j)).near_tri[k])) == -1){
						group_tag.at(not_arrange_tri_convert_index.at(model->triangles->at(temp_group.at(j)).near_tri[k])) = arrange_group.size();
						temp_group.push_back(model->triangles->at(temp_group.at(j)).near_tri[k]);
					}
				}
			}

			arrange_group.push_back(temp_group);
		}
	}

	for (unsigned int a = 0; a < all_planes.size(); a += 1){
		hole_split(model, all_edge, all_planes.at(a), arrange_group);
	}

	for (unsigned int j = 0; j < model->numtriangles; j += 1){
		if (model->triangles->at(j).material_id_graph_cut != -1){
			int index = find(all_face_split_by_plane.at(model->triangles->at(j).material_id_graph_cut).begin(), all_face_split_by_plane.at(model->triangles->at(j).material_id_graph_cut).end(), j) - all_face_split_by_plane.at(model->triangles->at(j).material_id_graph_cut).begin();
			if (index >= all_face_split_by_plane.at(model->triangles->at(j).material_id_graph_cut).size())
				all_face_split_by_plane.at(model->triangles->at(j).material_id_graph_cut).push_back(j);
		}
	}

	std::vector<std::vector<zomeconn>> test_connect(4);
	struc_parser(test_connect, zome_file);

	for (unsigned int i = plane_tune; i < all_planes.size(); i += 1){

		if (plane_tune == 0){
			collect_vanish_face(model, all_edge, all_planes.at(i), all_face_split_by_plane.at(i), arrange_group, i);

			for (unsigned int j = 0; j < model->cut_loop->size(); j += 1){
				model->cut_loop->at(j).connect_edge.erase(model->cut_loop->at(j).connect_edge.begin(), model->cut_loop->at(j).connect_edge.begin() + model->cut_loop->at(j).connect_edge.size());
				model->cut_loop->at(j).align_plane.erase(model->cut_loop->at(j).align_plane.begin(), model->cut_loop->at(j).align_plane.begin() + model->cut_loop->at(j).align_plane.size());
			}

			for (unsigned int j = 0; j < model->numtriangles; j += 1){
				if (!model->triangles->at(j).split_plane_id.empty()){
					std::vector<unsigned int> v;
					model->triangles->at(j).split_plane_id.swap(v);
				}

				model->triangles->at(j).split_by_process = false;
			}
		}

		GLMmodel piece;
		process_piece(piece, model, all_face_split_by_plane.at(i));

		std::vector<edge> fill_edge;
		collect_edge(&piece, fill_edge);

		generate_fake_inner(piece, all_planes.at(i), fill_edge);

		find_shell_loop(&piece, fill_edge, all_planes.at(i));
		fill_hole(piece, true);

		if (plane_tune == 0){
			std::vector<std::vector<int>> solt_ball(62);
			std::vector<std::vector<vec3>> solt_dist(62);

			int solt = near_solt(&piece, test_connect, solt_ball, solt_dist);
			generate_tenon(&piece, test_connect, solt, solt_ball.at(solt), solt_dist.at(solt), std::string("test_model/out/" + model_file + "_tenon"), i);
		}

		std::string file_name = "test_model/out/" + model_file + "_" + std::to_string(i) + ".obj";
		glmWriteOBJ(&piece, my_strdup(file_name.c_str()), GLM_NONE);
	}
}