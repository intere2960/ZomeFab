#include <algorithm>
#include "edge.h"

edge::edge()
{
	vec3 zero(0.0, 0.0, 0.0);
	point[0] = zero;
	point[1] = zero;
	index[0] = 0;
	index[1] = 0;
	face_id[0] = -1;
	face_id[1] = -1;
	vertex_push_index = -1;
}

edge::edge(int i1, vec3 &p1, int i2, vec3 &p2)
{
	index[0] = i1;
	point[0] = p1;
	index[1] = i2;
	point[1] = p2;
	face_id[0] = -1;
	face_id[1] = -1;
	vertex_push_index = -1;
}

bool operator==(const edge &lhs, const edge &rhs){
	if ((lhs.point[0] != rhs.point[0]) || (lhs.point[1] != rhs.point[1]))
		return false;
	if ((lhs.index[0] != rhs.index[0]) || (lhs.index[1] != rhs.index[1]))
		return false;
	if ((lhs.face_id[0] != rhs.face_id[0]) || (lhs.face_id[1] != rhs.face_id[1]))
		return false;
	if (lhs.vertex_push_index != rhs.vertex_push_index)
		return false;
	return true;
}


void collect_edge(GLMmodel *model, std::vector<edge> &all_edge)
{
	int count = 0;
	std::vector<vec2> *temp_edge = new std::vector<vec2>[model->numvertices + 1];
	std::vector<int> *temp_point_tri = new std::vector<int>[model->numvertices + 1];


	for (unsigned int i = 0; i < model->numtriangles; i += 1){

		int min_index = model->triangles->at(i).vindices[0], temp_index = 0;
		model->triangles->at(i).split_by_process = false;

		if ((unsigned int)std::min(min_index, (int)model->triangles->at(i).vindices[1]) == model->triangles->at(i).vindices[1]){
			min_index = model->triangles->at(i).vindices[1];
			temp_index = 1;
		}
		if ((unsigned int)std::min(min_index, (int)model->triangles->at(i).vindices[2]) == model->triangles->at(i).vindices[2]){
			min_index = model->triangles->at(i).vindices[2];
			temp_index = 2;
		}

		unsigned int mid_index = model->triangles->at(i).vindices[(temp_index + 1) % 3], temp_mid_index = (temp_index + 1) % 3;
		if (mid_index > model->triangles->at(i).vindices[(temp_index + 2) % 3]){
			mid_index = model->triangles->at(i).vindices[(temp_index + 2) % 3];
			temp_mid_index = (temp_index + 2) % 3;
		}

		vec2 push_index1((float)min_index, (float)mid_index), push_index2((float)min_index, (float)model->triangles->at(i).vindices[(3 - temp_index - temp_mid_index) % 3]), push_index3((float)mid_index, (float)model->triangles->at(i).vindices[(3 - temp_index - temp_mid_index) % 3]);

		bool add1 = true, add2 = true, add3 = true;

		for (unsigned int j = 0; j < temp_edge[min_index].size(); j += 1){
			if (push_index1[1] == temp_edge[min_index].at(j)[1]){
				add1 = false;
				break;
			}
		}
		for (unsigned int j = 0; j < temp_edge[min_index].size(); j += 1){
			if (push_index2[1] == temp_edge[min_index].at(j)[1]){
				add2 = false;
				break;
			}
		}
		for (unsigned int j = 0; j < temp_edge[mid_index].size(); j += 1){
			if (push_index3[1] == temp_edge[mid_index].at(j)[1]){
				add3 = false;
				break;
			}
		}

		if (add1){
			temp_edge[min_index].push_back(push_index1);
			count += 1;
		}

		if (add2){
			temp_edge[min_index].push_back(push_index2);
			count += 1;
		}

		if (add3){
			temp_edge[mid_index].push_back(push_index3);
			count += 1;
		}

		for (int j = 0; j < 3; j += 1)
		{
			temp_point_tri[model->triangles->at(i).vindices[j]].push_back(i);
			model->triangles->at(i).edge_index[j] = -1;
		}
	}

	if (!all_edge.empty()){
		std::vector<edge> v;
		all_edge.swap(v);
	}

	for (unsigned int i = 0; i < model->numvertices + 1; i += 1){
		for (unsigned int j = 0; j < temp_edge[i].size(); j += 1){
			vec3 p1(model->vertices->at((int)temp_edge[i].at(j)[0] * 3 + 0), model->vertices->at((int)temp_edge[i].at(j)[0] * 3 + 1), model->vertices->at((int)temp_edge[i].at(j)[0] * 3 + 2));
			vec3 p2(model->vertices->at((int)temp_edge[i][j][1] * 3 + 0), model->vertices->at((int)temp_edge[i].at(j)[1] * 3 + 1), model->vertices->at((int)temp_edge[i].at(j)[1] * 3 + 2));
			edge temp((int)temp_edge[i].at(j)[0], p1, (int)temp_edge[i].at(j)[1], p2);
			temp.connect_index = -1;
			all_edge.push_back(temp);
		}
	}

	for (unsigned int i = 1; i <= model->numvertices; i += 1)
		sort(temp_point_tri[i].begin(), temp_point_tri[i].begin() + temp_point_tri[i].size());

	for (unsigned int i = 0; i < all_edge.size(); i += 1){
		std::vector<int> temp_vector(temp_point_tri[all_edge.at(i).index[0]].size() + temp_point_tri[all_edge.at(i).index[1]].size());
		std::vector<int>::iterator it;

		it = set_intersection(temp_point_tri[all_edge.at(i).index[0]].begin(), temp_point_tri[all_edge.at(i).index[0]].begin() + temp_point_tri[all_edge.at(i).index[0]].size(), temp_point_tri[all_edge.at(i).index[1]].begin(), temp_point_tri[all_edge.at(i).index[1]].begin() + temp_point_tri[all_edge[i].index[1]].size(), temp_vector.begin());
		temp_vector.resize(it - temp_vector.begin());

		all_edge.at(i).face_id[0] = temp_vector.at(0);
		if (temp_vector.size() > 1)
			all_edge.at(i).face_id[1] = temp_vector.at(1);

		for (unsigned int k = 0; k < temp_vector.size(); k += 1){
			for (int j = 0; j < 3; j += 1){
				if (model->triangles->at(temp_vector.at(k)).edge_index[j] == -1){
					model->triangles->at(temp_vector.at(k)).edge_index[j] = i;
					break;
				}
			}
		}
	}

	for (unsigned int i = 0; i < model->numtriangles; i += 1){
		int temp_edge_index[3];
		temp_edge_index[0] = model->triangles->at(i).edge_index[0];
		temp_edge_index[1] = model->triangles->at(i).edge_index[1];
		temp_edge_index[2] = model->triangles->at(i).edge_index[2];

		for (int j = 0; j < 3; j += 1){
			bool judge1, judge2;
			judge1 = (model->triangles->at(i).vindices[0] == (unsigned int)all_edge.at(temp_edge_index[j]).index[0]) && (model->triangles->at(i).vindices[1] == (unsigned int)all_edge.at(temp_edge_index[j]).index[1]);
			judge2 = (model->triangles->at(i).vindices[0] == (unsigned int)all_edge.at(temp_edge_index[j]).index[1]) && (model->triangles->at(i).vindices[1] == (unsigned int)all_edge.at(temp_edge_index[j]).index[0]);
			if (judge1 || judge2){
				model->triangles->at(i).edge_index[0] = (int)temp_edge_index[j];
			}

			judge1 = (model->triangles->at(i).vindices[1] == (unsigned int)all_edge.at(temp_edge_index[j]).index[0]) && (model->triangles->at(i).vindices[2] == (unsigned int)all_edge.at(temp_edge_index[j]).index[1]);
			judge2 = (model->triangles->at(i).vindices[1] == (unsigned int)all_edge.at(temp_edge_index[j]).index[1]) && (model->triangles->at(i).vindices[2] == (unsigned int)all_edge.at(temp_edge_index[j]).index[0]);
			if (judge1 || judge2){
				model->triangles->at(i).edge_index[1] = (int)temp_edge_index[j];
			}

			judge1 = (model->triangles->at(i).vindices[2] == (unsigned int)all_edge.at(temp_edge_index[j]).index[0]) && (model->triangles->at(i).vindices[0] == (unsigned int)all_edge.at(temp_edge_index[j]).index[1]);
			judge2 = (model->triangles->at(i).vindices[2] == (unsigned int)all_edge.at(temp_edge_index[j]).index[1]) && (model->triangles->at(i).vindices[0] == (unsigned int)all_edge.at(temp_edge_index[j]).index[0]);
			if (judge1 || judge2){
				model->triangles->at(i).edge_index[2] = (int)temp_edge_index[j];
			}
		}
	}

	for (unsigned int i = 0; i < model->numvertices + 1; i += 1){
		if (!temp_edge[i].empty()){
			std::vector<vec2> v;
			temp_edge[i].swap(v);
		}
		if (!temp_point_tri[i].empty()){
			std::vector<int> v;
			temp_point_tri[i].swap(v);
		}
	}

	/*delete temp_edge;
	delete temp_point_tri;*/
}

void find_near_tri(GLMmodel *model, std::vector<edge> &all_edge)
{
	for (int i = 0; i < model->numtriangles; i += 1){
		for (int j = 0; j < 3; j += 1){
			edge temp = all_edge.at(model->triangles->at(i).edge_index[j]);
			int index = temp.face_id[0];
			if (index == i){
				index = temp.face_id[1];
			}
			model->triangles->at(i).near_tri[j] = index;
		}
	}
}

void inform_vertex(GLMmodel *model, std::vector<edge> &all_edge)
{
	for (unsigned int i = 0; i < all_edge.size(); i += 1){
		model->cut_loop->at(all_edge.at(i).index[0]).connect_edge.push_back(i);
		model->cut_loop->at(all_edge.at(i).index[1]).connect_edge.push_back(i);
	}
}
