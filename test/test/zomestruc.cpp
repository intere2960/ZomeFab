#include <string>
#include <fstream>
#include <omp.h>
#include "zomestruc.h"
#include "zomedir.h"
#include "global.h"

zomeconn::zomeconn()
{
	vec3 zero(0.0, 0.0, 0.0);
	position = zero;
	rotation = zero;
	endposition = zero;
	fromindex = vec2(-1, -1);
	fromface = -1;
	towardindex = vec2(-1, -1);
	towardface = -1;
	surface_d = 100000000000000.0f;
	energy_d = 100000000000000.0f;
	energy_angle = 100000000000000.0f;
	energy_use_stick = 100000000000000.0f;

	exist = true;
	outter = false;
	material_id = -1;
	material_id_energy_angle = -1;
	material_id_energy_use_stick = -1;

	for (int i = 0; i < 62; i += 1){
		connect_stick[i] = vec2(-1.0f, -1.0f);
	}
}

zomeconn::~zomeconn()
{
}

int zomeconn::getmodelnumber()
{
	int modelnumber;
	if (color == COLOR_BLUE)
	{
		if (size == SIZE_S) modelnumber = 1;
		if (size == SIZE_M) modelnumber = 2;
		if (size == SIZE_L) modelnumber = 3;
	}
	else if (color == COLOR_RED)
	{
		if (size == SIZE_S) modelnumber = 4;
		if (size == SIZE_M) modelnumber = 5;
		if (size == SIZE_L) modelnumber = 6;
	}
	else if (color == COLOR_YELLOW)
	{
		if (size == SIZE_S) modelnumber = 7;
		if (size == SIZE_M) modelnumber = 8;
		if (size == SIZE_L) modelnumber = 9;
	}
	return modelnumber;
}


zomestruc::zomestruc()
{
	//	faceConnection = new ZomeConnection*[62];
	//	nextBall = new zomestruc*[62];
	//	for( int i = 0; i < 62; i++ )
	//	{
	//		faceConnection[i] = NULL;
	//		nextBall[i] = NULL;
	//	}
	position.set(0.0, 0.0, 0.0);
}

zomestruc::~zomestruc()
{
	//	delete[] faceConnection;
	//	delete[] nextBall ;
	//	delete[] position ;
}

//VirtualBall::VirtualBall( int i , ZomeConnection* c )
//{
//	index = i;
//	MathHelper::CopyVector( position , c -> endPosition );
//	isBottom = false;
//	connIndex[0] = c -> index;
//	//connectFace[0] = c -> towardFace;
//
//	connectionAmount = 0;//??
//	conn[0] = c;
//}

void combine_zome_ztruc(std::vector<std::vector<zomeconn>> &target, std::vector<std::vector<zomeconn>> &source)
{
	if (source.size() == 0)
		return;

	int ball_size = target.at(COLOR_WHITE).size();
	for (unsigned int i = 0; i < source.at(COLOR_WHITE).size(); i += 1){
		zomeconn temp = source.at(COLOR_WHITE).at(i);

		for (unsigned int j = 0; j < 62; j += 1){
			if (temp.connect_stick[j] != vec2(-1.0f, -1.0f))
				temp.connect_stick[j][1] += target.at((int)temp.connect_stick[j][0]).size();
		}
		target.at(COLOR_WHITE).push_back(temp);
	}

	int origin_size[3] = { target.at(0).size(), target.at(1).size(), target.at(2).size()};
	for (int i = 0; i < 3; i += 1){
		for (unsigned int j = 0; j < source.at(i).size(); j += 1){
			zomeconn temp = source.at(i).at(j);

			if (temp.fromindex == vec2(-1, -1)){
				vec3 temp_pos = 2 * temp.position - source.at(COLOR_WHITE).at((int)temp.towardindex[1]).position;

				for (unsigned int k = 0; k < target.at(COLOR_WHITE).size(); k += 1){
					if ((target.at(COLOR_WHITE).at(k).position - temp_pos).length() < 0.001){
						temp.fromindex[0] = (float)COLOR_WHITE;
						temp.fromindex[1] = (float)k;
						break;
					}
				}
				target.at(COLOR_WHITE).at((int)temp.fromindex[1]).connect_stick[temp.fromface] = vec2((float)i, (float)j + origin_size[i]);
			}
			else{
				temp.fromindex[1] += ball_size;
			}			

			if (temp.towardindex == vec2(-1, -1)){
				vec3 temp_pos = 2 * temp.position - source.at(COLOR_WHITE).at((int)temp.fromindex[1] - ball_size).position;

				for (unsigned int k = 0; k < target.at(COLOR_WHITE).size(); k += 1){
					if ((target.at(COLOR_WHITE).at(k).position - temp_pos).length() < 0.001){
						temp.towardindex[0] = COLOR_WHITE;
						temp.towardindex[1] = (float)k;
						break;
					}
				}

				target.at(COLOR_WHITE).at((int)temp.towardindex[1]).connect_stick[temp.towardface] = vec2((float)i, (float)j + origin_size[i]);
			}
			else{
				temp.towardindex[1] += ball_size;
			}			
			
			target.at(i).push_back(temp);
		}
	}
}

void output_struc(std::vector<std::vector<zomeconn>> &target, std::string &filename)
{
	std::vector<std::vector<int>> convert_index(4);
	for (int i = 0; i < 4; i += 1){
		int number = 0;
		for (unsigned int j = 0; j < target.at(i).size(); j += 1){
			if (target.at(i).at(j).exist){
				convert_index.at(i).push_back(number);
				number += 1;
			}
			else{
				convert_index.at(i).push_back(-1);
			}
		}
	}

	std::ofstream os;
	os.open(filename);
	for (int i = 0; i < 4; i += 1){
		int number = 0;
		for (unsigned int j = 0; j < target.at(i).size(); j += 1){
			if (target.at(i).at(j).exist){
				os << number << " ";
				number += 1;
				os << target.at(i).at(j).color << " ";
				os << target.at(i).at(j).position[0] << " " << target.at(i).at(j).position[1] << " " << target.at(i).at(j).position[2] << " ";
				if (i < 3){
					os << target.at(i).at(j).size << " ";
					os << target.at(i).at(j).fromface << " " << target.at(i).at(j).towardface << " ";
					os << target.at(i).at(j).fromindex[0] << " " << convert_index.at(target.at(i).at(j).fromindex[0]).at(target.at(i).at(j).fromindex[1]) << " ";
					os << target.at(i).at(j).towardindex[0] << " " << convert_index.at(target.at(i).at(j).towardindex[0]).at(target.at(i).at(j).towardindex[1]) << " ";
				}
				else{
					int num = 0;
					for (unsigned int k = 0; k < 62; k += 1){
						if (target.at(i).at(j).connect_stick[k] != vec2(-1.0f, -1.0f))
							num += 1;
					}
					os << num << " ";
					for (unsigned int k = 0; k < 62; k += 1){
						if (target.at(i).at(j).connect_stick[k] != vec2(-1.0f, -1.0f))
							os << k << " " << target.at(i).at(j).connect_stick[k][0] << " " << convert_index.at(target.at(i).at(j).connect_stick[k][0]).at(target.at(i).at(j).connect_stick[k][1]) << " ";
					}
				}
				os << std::endl;
			}
		}
	}
	os.close();
}

void struc_parser(std::vector<std::vector<zomeconn>> &target, std::string &filename)
{
	std::ifstream is;
	is.open(filename);
	int number;
	int temp_color;
	vec3 temp_pos;
	while (is >> number >> temp_color >> temp_pos[0] >> temp_pos[1] >> temp_pos[2]){
		if (temp_color != 3){
			int temp_size, temp_fromface, temp_towardface;
			vec2  temp_fromindex, temp_towardindex;
			is >> temp_size;
			is >> temp_fromface >> temp_towardface;
			is >> temp_fromindex[0] >> temp_fromindex[1];
			is >> temp_towardindex[0] >> temp_towardindex[1];

			zomeconn temp_stick;
			temp_stick.color = temp_color;
			temp_stick.position = temp_pos;
			temp_stick.size = temp_size;
			temp_stick.fromface = temp_fromface;
			temp_stick.towardface = temp_towardface;
			temp_stick.fromindex = temp_fromindex;
			temp_stick.towardindex = temp_towardindex;
			target.at(temp_color).push_back(temp_stick);
		}
		else{
			zomeconn temp_ball;
			temp_ball.color = temp_color;
			temp_ball.position = temp_pos;

			int temp_size;
			int temp_index;
			vec2 temp_connect;
			is >> temp_size;
			for (int i = 0; i < temp_size; i += 1){
				is >> temp_index >> temp_connect[0] >> temp_connect[1];
				temp_ball.connect_stick[temp_index] = temp_connect;
			}
			target.at(COLOR_WHITE).push_back(temp_ball);
		}
	}
	is.close();
}

void output_zometool(vec3 &rotation, std::vector<std::vector<zomeconn>> &zome_queue, int piece_id)
{
	if (zome_queue.at(3).size() != 0){
		std::string s = "voxel_zome_" + std::to_string(piece_id) + ".obj";
		std::ofstream os(s);
		GLMmodel *z_b, *b_s, *b_m, *b_l, *r_s, *r_m, *r_l, *y_s, *y_m, *y_l;
		z_b = glmReadOBJ("test_model/zometool/zomeball.obj");
		b_s = glmReadOBJ("test_model/zometool/BlueS.obj");
		b_m = glmReadOBJ("test_model/zometool/BlueM.obj");
		b_l = glmReadOBJ("test_model/zometool/BlueL.obj");
		r_s = glmReadOBJ("test_model/zometool/RedS.obj");
		r_m = glmReadOBJ("test_model/zometool/RedM.obj");
		r_l = glmReadOBJ("test_model/zometool/RedL.obj");
		y_s = glmReadOBJ("test_model/zometool/YellowS.obj");
		y_m = glmReadOBJ("test_model/zometool/YellowM.obj");
		y_l = glmReadOBJ("test_model/zometool/YellowL.obj");

		std::vector<std::vector<vec3>> face_index(4);

		os << "mtllib color.mtl" << std::endl;

		int num_v = 0;
		for (unsigned int i = 0; i < zome_queue.size(); i += 1){
			for (unsigned int j = 0; j < zome_queue.at(i).size(); j += 1){
				GLMmodel *temp_model = NULL;
				if (i == COLOR_WHITE){
					temp_model = glmCopy(z_b);
					glmR(temp_model, rotation);
					glmRT(temp_model, vec3(0.0, 0.0, 0.0), zome_queue.at(i).at(j).position);
				}
				else{
					zomedir t;
					if (i == COLOR_BLUE){
						if (zome_queue.at(i).at(j).size == SIZE_S){
							temp_model = glmCopy(b_s);
						}
						else if (zome_queue.at(i).at(j).size == SIZE_M){
							temp_model = glmCopy(b_m);
						}
						else{
							temp_model = glmCopy(b_l);
						}
					}
					else if (i == COLOR_RED){
						if (zome_queue.at(i).at(j).size == SIZE_S){
							temp_model = glmCopy(r_s);
						}
						else if (zome_queue.at(i).at(j).size == SIZE_M){
							temp_model = glmCopy(r_m);
						}
						else{
							temp_model = glmCopy(r_l);
						}
					}
					else {
						if (zome_queue.at(i).at(j).size == SIZE_S){
							temp_model = glmCopy(y_s);
						}
						else if (zome_queue.at(i).at(j).size == SIZE_M){
							temp_model = glmCopy(y_m);
						}
						else{
							temp_model = glmCopy(y_l);
						}
					}

					glmRT(temp_model, vec3(0.0, t.roll(zome_queue.at(i).at(j).fromface), 0.0), vec3(0.0, 0.0, 0.0));
					glmRT(temp_model, vec3(0.0, t.phi(zome_queue.at(i).at(j).fromface), t.theta(zome_queue.at(i).at(j).fromface)), vec3(0.0, 0.0, 0.0));
					glmR(temp_model, rotation);
					glmRT(temp_model, vec3(0.0, 0.0, 0.0), zome_queue.at(i).at(j).position);
				}

				for (unsigned int k = 1; k <= temp_model->numvertices; k += 1){
					os << "v " << temp_model->vertices->at(3 * k + 0) << " " << temp_model->vertices->at(3 * k + 1) << " " << temp_model->vertices->at(3 * k + 2) << std::endl;
				}

				for (unsigned int k = 0; k < temp_model->numtriangles; k += 1){
					vec3 temp_t((float)temp_model->triangles->at(k).vindices[0] + num_v, (float)temp_model->triangles->at(k).vindices[1] + num_v, (float)temp_model->triangles->at(k).vindices[2] + num_v);
					face_index.at(i).push_back(temp_t);
				}
				num_v += temp_model->numvertices;
			}
		}

		os << std::endl;

		for (unsigned int i = 0; i < zome_queue.size(); i += 1){
			if (i == COLOR_WHITE){
				os << "usemtl white" << std::endl;
			}
			else if (i == COLOR_BLUE){
				os << "usemtl blue" << std::endl;
			}
			else if (i == COLOR_RED){
				os << "usemtl red" << std::endl;
			}
			else {
				os << "usemtl yellow" << std::endl;
			}

			for (unsigned int j = 0; j < face_index.at(i).size(); j += 1){
				os << "f " << face_index.at(i).at(j)[0] << " " << face_index.at(i).at(j)[1] << " " << face_index.at(i).at(j)[2] << std::endl;
			}
		}

		os.close();
	}
}

#include <iostream>
using namespace std;

void output_zometool(std::vector<std::vector<zomeconn>> &output_connect, std::string &filename)
{
	std::ofstream os(filename);
	GLMmodel *z_b, *b_s, *b_m, *b_l, *r_s, *r_m, *r_l, *y_s, *y_m, *y_l;
	z_b = glmReadOBJ("test_model/zometool/zomeball.obj");
	b_s = glmReadOBJ("test_model/zometool/BlueS.obj");
	b_m = glmReadOBJ("test_model/zometool/BlueM.obj");
	b_l = glmReadOBJ("test_model/zometool/BlueL.obj");
	r_s = glmReadOBJ("test_model/zometool/RedS.obj");
	r_m = glmReadOBJ("test_model/zometool/RedM.obj");
	r_l = glmReadOBJ("test_model/zometool/RedL.obj");
	y_s = glmReadOBJ("test_model/zometool/YellowS.obj");
	y_m = glmReadOBJ("test_model/zometool/YellowM.obj");
	y_l = glmReadOBJ("test_model/zometool/YellowL.obj");

	std::vector<std::vector<vec3>> face_index(4);

	os << "mtllib color.mtl" << std::endl;

	int num_v = 0;
	for (unsigned int i = 0; i < output_connect.size(); i += 1){
		for (unsigned int j = 0; j < output_connect.at(i).size(); j += 1){
			if (output_connect.at(i).at(j).exist){
				GLMmodel *temp_model = NULL;
				if (i == COLOR_WHITE){
					temp_model = glmCopy(z_b);
					glmR(temp_model, vec3(0.0, 0.0, 0.0));
					glmRT(temp_model, vec3(0.0, 0.0, 0.0), output_connect.at(i).at(j).position);
				}
				else{
					zomedir t;
					if (i == COLOR_BLUE){
						if (output_connect.at(i).at(j).size == SIZE_S){
							temp_model = glmCopy(b_s);
						}
						else if (output_connect.at(i).at(j).size == SIZE_M){
							temp_model = glmCopy(b_m);
						}
						else{
							temp_model = glmCopy(b_l);
						}
					}
					else if (i == COLOR_RED){
						if (output_connect.at(i).at(j).size == SIZE_S){
							temp_model = glmCopy(r_s);
						}
						else if (output_connect.at(i).at(j).size == SIZE_M){
							temp_model = glmCopy(r_m);
						}
						else{
							temp_model = glmCopy(r_l);
						}
					}
					else {
						if (output_connect.at(i).at(j).size == SIZE_S){
							temp_model = glmCopy(y_s);
						}
						else if (output_connect.at(i).at(j).size == SIZE_M){
							temp_model = glmCopy(y_m);
						}
						else{
							temp_model = glmCopy(y_l);
						}
					}

					glmRT(temp_model, vec3(0.0, t.roll(output_connect.at(i).at(j).fromface), 0.0), vec3(0.0, 0.0, 0.0));
					glmRT(temp_model, vec3(0.0, t.phi(output_connect.at(i).at(j).fromface), t.theta(output_connect.at(i).at(j).fromface)), vec3(0.0, 0.0, 0.0));
					glmR(temp_model, vec3(0.0, 0.0, 0.0));
					glmRT(temp_model, vec3(0.0, 0.0, 0.0), output_connect.at(i).at(j).position);
				}

				for (unsigned int k = 1; k <= temp_model->numvertices; k += 1){
					os << "v " << temp_model->vertices->at(3 * k + 0) << " " << temp_model->vertices->at(3 * k + 1) << " " << temp_model->vertices->at(3 * k + 2) << std::endl;
				}

				for (unsigned int k = 0; k < temp_model->numtriangles; k += 1){
					vec3 temp_t((float)temp_model->triangles->at(k).vindices[0] + num_v, (float)temp_model->triangles->at(k).vindices[1] + num_v, (float)temp_model->triangles->at(k).vindices[2] + num_v);
					face_index.at(i).push_back(temp_t);
				}
				num_v += temp_model->numvertices;
			}
		}
	}

	os << std::endl;

	//cout << z_b->numtriangles << endl;
	for (unsigned int i = 0; i < output_connect.size(); i += 1){
		if (i == COLOR_WHITE){
			//cout << face_index.at(i).size() << endl;
			for (unsigned int j = 0; j < face_index.at(i).size(); j += 1){
				if ((j % z_b->numtriangles) == 0){
					if (!output_connect.at(i).at(j / z_b->numtriangles).outter)
						os << "usemtl white" << std::endl;
					else if (output_connect.at(i).at(j / z_b->numtriangles).outter)
						os << "usemtl white" << std::endl;
				}
				os << "f " << face_index.at(i).at(j)[0] << " " << face_index.at(i).at(j)[1] << " " << face_index.at(i).at(j)[2] << std::endl;
			}
		}		
		else{
			if (i == COLOR_BLUE){
				os << "usemtl blue" << std::endl;
			}
			else if (i == COLOR_RED){
				os << "usemtl red" << std::endl;
			}
			else {
				os << "usemtl yellow" << std::endl;
			}

			for (unsigned int j = 0; j < face_index.at(i).size(); j += 1){
				os << "f " << face_index.at(i).at(j)[0] << " " << face_index.at(i).at(j)[1] << " " << face_index.at(i).at(j)[2] << std::endl;
			}
		}
	}

	os.close();
}

void output_zometool_exp(std::vector<std::vector<zomeconn>> &output_connect, std::string &filename, std::vector<simple_material> &materials, std::string &materials_filename, int mode)
{
	std::ofstream os(filename);
	GLMmodel *z_b, *b_s, *b_m, *b_l, *r_s, *r_m, *r_l, *y_s, *y_m, *y_l;
	z_b = glmReadOBJ("test_model/zometool/zomeball.obj");
	b_s = glmReadOBJ("test_model/zometool/BlueS.obj");
	b_m = glmReadOBJ("test_model/zometool/BlueM.obj");
	b_l = glmReadOBJ("test_model/zometool/BlueL.obj");
	r_s = glmReadOBJ("test_model/zometool/RedS.obj");
	r_m = glmReadOBJ("test_model/zometool/RedM.obj");
	r_l = glmReadOBJ("test_model/zometool/RedL.obj");
	y_s = glmReadOBJ("test_model/zometool/YellowS.obj");
	y_m = glmReadOBJ("test_model/zometool/YellowM.obj");
	y_l = glmReadOBJ("test_model/zometool/YellowL.obj");

	std::vector<std::vector<vec3>> face_index(4);

	os << "mtllib " << materials_filename << std::endl;

	int num_v = 0;
	for (unsigned int i = 0; i < output_connect.size(); i += 1){
		for (unsigned int j = 0; j < output_connect.at(i).size(); j += 1){
			if (output_connect.at(i).at(j).exist){
				GLMmodel *temp_model = NULL;
				if (i == COLOR_WHITE){
					temp_model = glmCopy(z_b);
					glmR(temp_model, vec3(0.0, 0.0, 0.0));
					glmRT(temp_model, vec3(0.0, 0.0, 0.0), output_connect.at(i).at(j).position);
				}
				else{
					zomedir t;
					if (i == COLOR_BLUE){
						if (output_connect.at(i).at(j).size == SIZE_S){
							temp_model = glmCopy(b_s);
						}
						else if (output_connect.at(i).at(j).size == SIZE_M){
							temp_model = glmCopy(b_m);
						}
						else{
							temp_model = glmCopy(b_l);
						}
					}
					else if (i == COLOR_RED){
						if (output_connect.at(i).at(j).size == SIZE_S){
							temp_model = glmCopy(r_s);
						}
						else if (output_connect.at(i).at(j).size == SIZE_M){
							temp_model = glmCopy(r_m);
						}
						else{
							temp_model = glmCopy(r_l);
						}
					}
					else {
						if (output_connect.at(i).at(j).size == SIZE_S){
							temp_model = glmCopy(y_s);
						}
						else if (output_connect.at(i).at(j).size == SIZE_M){
							temp_model = glmCopy(y_m);
						}
						else{
							temp_model = glmCopy(y_l);
						}
					}

					glmRT(temp_model, vec3(0.0, t.roll(output_connect.at(i).at(j).fromface), 0.0), vec3(0.0, 0.0, 0.0));
					glmRT(temp_model, vec3(0.0, t.phi(output_connect.at(i).at(j).fromface), t.theta(output_connect.at(i).at(j).fromface)), vec3(0.0, 0.0, 0.0));
					glmR(temp_model, vec3(0.0, 0.0, 0.0));
					glmRT(temp_model, vec3(0.0, 0.0, 0.0), output_connect.at(i).at(j).position);
				}

				for (unsigned int k = 1; k <= temp_model->numvertices; k += 1){
					os << "v " << temp_model->vertices->at(3 * k + 0) << " " << temp_model->vertices->at(3 * k + 1) << " " << temp_model->vertices->at(3 * k + 2) << std::endl;
				}

				for (unsigned int k = 0; k < temp_model->numtriangles; k += 1){
					vec3 temp_t((float)temp_model->triangles->at(k).vindices[0] + num_v, (float)temp_model->triangles->at(k).vindices[1] + num_v, (float)temp_model->triangles->at(k).vindices[2] + num_v);
					face_index.at(i).push_back(temp_t);
				}
				num_v += temp_model->numvertices;
			}
		}
	}

	os << std::endl;

	if (mode == COLORFUL){
		//cout << z_b->numtriangles << endl;
		for (unsigned int i = 0; i < output_connect.size(); i += 1){
			if (i == COLOR_WHITE){
				//cout << face_index.at(i).size() << endl;
				for (unsigned int j = 0; j < face_index.at(i).size(); j += 1){
					if ((j % z_b->numtriangles) == 0){
						//cout << output_connect.at(i).at(j / z_b->numtriangles).material_id << endl;
						if (output_connect.at(i).at(j / z_b->numtriangles).material_id == -1)
							os << "usemtl initialShadingGroup" << std::endl;
						else
							os << "usemtl " << materials.at(output_connect.at(i).at(j / z_b->numtriangles).material_id).name << std::endl;
					}
					os << "f " << face_index.at(i).at(j)[0] << " " << face_index.at(i).at(j)[1] << " " << face_index.at(i).at(j)[2] << std::endl;
				}
			}
			else{

				os << "usemtl initialShadingGroup" << std::endl;

				for (unsigned int j = 0; j < face_index.at(i).size(); j += 1){
					os << "f " << face_index.at(i).at(j)[0] << " " << face_index.at(i).at(j)[1] << " " << face_index.at(i).at(j)[2] << std::endl;
				}
			}
		}
	}
	else if (mode == ENERGY_ANGLE){
		//cout << z_b->numtriangles << endl;
		for (unsigned int i = 0; i < output_connect.size(); i += 1){
			if (i == COLOR_WHITE){
				//cout << face_index.at(i).size() << endl;
				for (unsigned int j = 0; j < face_index.at(i).size(); j += 1){
					if ((j % z_b->numtriangles) == 0){
						//cout << output_connect.at(i).at(j / z_b->numtriangles).material_id << endl;
						if (output_connect.at(i).at(j / z_b->numtriangles).material_id_energy_angle == -1)
							os << "usemtl initialShadingGroup" << std::endl;
						else
							os << "usemtl " << materials.at(output_connect.at(i).at(j / z_b->numtriangles).material_id_energy_angle).name << std::endl;
					}
					os << "f " << face_index.at(i).at(j)[0] << " " << face_index.at(i).at(j)[1] << " " << face_index.at(i).at(j)[2] << std::endl;
				}
			}
			else{

				os << "usemtl initialShadingGroup" << std::endl;

				for (unsigned int j = 0; j < face_index.at(i).size(); j += 1){
					os << "f " << face_index.at(i).at(j)[0] << " " << face_index.at(i).at(j)[1] << " " << face_index.at(i).at(j)[2] << std::endl;
				}
			}
		}
	}
	else if (mode == ENERGY_USE_STICK){
		//cout << z_b->numtriangles << endl;
		for (unsigned int i = 0; i < output_connect.size(); i += 1){
			if (i == COLOR_WHITE){
				//cout << face_index.at(i).size() << endl;
				for (unsigned int j = 0; j < face_index.at(i).size(); j += 1){
					if ((j % z_b->numtriangles) == 0){
						//cout << output_connect.at(i).at(j / z_b->numtriangles).material_id << endl;
						if (output_connect.at(i).at(j / z_b->numtriangles).material_id_energy_use_stick == -1)
							os << "usemtl initialShadingGroup" << std::endl;
						else
							os << "usemtl " << materials.at(output_connect.at(i).at(j / z_b->numtriangles).material_id_energy_use_stick).name << std::endl;
					}
					os << "f " << face_index.at(i).at(j)[0] << " " << face_index.at(i).at(j)[1] << " " << face_index.at(i).at(j)[2] << std::endl;
				}
			}
			else{

				os << "usemtl initialShadingGroup" << std::endl;

				for (unsigned int j = 0; j < face_index.at(i).size(); j += 1){
					os << "f " << face_index.at(i).at(j)[0] << " " << face_index.at(i).at(j)[1] << " " << face_index.at(i).at(j)[2] << std::endl;
				}
			}
		}
	}

	os.close();
}

float point_surface_dist(GLMmodel *model, vec3 &p)
{
	float surface_d = 100000000000000.0f;

	#pragma omp parallel for
	for (int i = 0; i < model->triangles->size(); i += 1){

		vec3 test[4];
		test[0] = vec3(model->vertices->at(3 * model->triangles->at(i).vindices[0] + 0), model->vertices->at(3 * model->triangles->at(i).vindices[0] + 1), model->vertices->at(3 * model->triangles->at(i).vindices[0] + 2));
		test[1] = vec3(model->vertices->at(3 * model->triangles->at(i).vindices[1] + 0), model->vertices->at(3 * model->triangles->at(i).vindices[1] + 1), model->vertices->at(3 * model->triangles->at(i).vindices[1] + 2));
		test[2] = vec3(model->vertices->at(3 * model->triangles->at(i).vindices[2] + 0), model->vertices->at(3 * model->triangles->at(i).vindices[2] + 1), model->vertices->at(3 * model->triangles->at(i).vindices[2] + 2));
		test[3] = (test[0] + test[1] + test[2]) / 3.0f;

		vec3 v1 = test[0] - test[1];
		vec3 v2 = test[2] - test[1];
		vec3 n = (v2 ^ v1).normalize();
		float d = n * test[0];

		float judge = p * n - d;
		if ((judge < -(NODE_DIAMETER / 2.0f + ERROR_THICKNESS))){
			for (int j = 0; j < 4; j += 1){
				if ((p - test[j]).length() < surface_d){
					surface_d = (p - test[j]).length();
				}
			}
		}
	}		

	if (surface_d < NODE_DIAMETER / 2.0f + ERROR_THICKNESS){
		surface_d = 100000000000000.0f;
	}

	return surface_d;
}

float point_surface_dist_fast(GLMmodel *model, vec3 &p, std::vector<int> &near_tri)
{
	float surface_d = 100000000000000.0f;

	for (int i = 0; i < near_tri.size(); i += 1){

		vec3 test[4];
		test[0] = vec3(model->vertices->at(3 * model->triangles->at(near_tri.at(i)).vindices[0] + 0), model->vertices->at(3 * model->triangles->at(near_tri.at(i)).vindices[0] + 1), model->vertices->at(3 * model->triangles->at(near_tri.at(i)).vindices[0] + 2));
		test[1] = vec3(model->vertices->at(3 * model->triangles->at(near_tri.at(i)).vindices[1] + 0), model->vertices->at(3 * model->triangles->at(near_tri.at(i)).vindices[1] + 1), model->vertices->at(3 * model->triangles->at(near_tri.at(i)).vindices[1] + 2));
		test[2] = vec3(model->vertices->at(3 * model->triangles->at(near_tri.at(i)).vindices[2] + 0), model->vertices->at(3 * model->triangles->at(near_tri.at(i)).vindices[2] + 1), model->vertices->at(3 * model->triangles->at(near_tri.at(i)).vindices[2] + 2));
		test[3] = (test[0] + test[1] + test[2]) / 3.0f;

		vec3 v1 = test[0] - test[1];
		vec3 v2 = test[2] - test[1];
		vec3 n = (v2 ^ v1).normalize();
		float d = n * test[0];

		float judge = p * n - d;
		if ((judge < -(NODE_DIAMETER / 2.0f + ERROR_THICKNESS))){
			for (int j = 0; j < 4; j += 1){
				if ((p - test[j]).length() < surface_d){
					surface_d = (p - test[j]).length();
				}
			}
		}
	}

	if (surface_d < NODE_DIAMETER / 2.0f + ERROR_THICKNESS){
		surface_d = 100000000000000.0f;
	}

	return surface_d;
}

float ball_surface_dist(GLMmodel *model, vec3 &p)
{
	float surface_d = 100000000000000.0f;

	for (unsigned int i = 0; i < model->triangles->size(); i += 1){
		
		vec3 test[4];
		test[0] = vec3(model->vertices->at(3 * model->triangles->at(i).vindices[0] + 0), model->vertices->at(3 * model->triangles->at(i).vindices[0] + 1), model->vertices->at(3 * model->triangles->at(i).vindices[0] + 2));
		test[1] = vec3(model->vertices->at(3 * model->triangles->at(i).vindices[1] + 0), model->vertices->at(3 * model->triangles->at(i).vindices[1] + 1), model->vertices->at(3 * model->triangles->at(i).vindices[1] + 2));
		test[2] = vec3(model->vertices->at(3 * model->triangles->at(i).vindices[2] + 0), model->vertices->at(3 * model->triangles->at(i).vindices[2] + 1), model->vertices->at(3 * model->triangles->at(i).vindices[2] + 2));
		test[3] = (test[0] + test[1] + test[2]) / 3.0f;

		vec3 v1 = test[0] - test[1];
		vec3 v2 = test[2] - test[1];
		vec3 n = (v2 ^ v1).normalize();
		float d = n * test[0];

		for (int j = 0; j < 4; j += 1){
			if (((p - test[j]).length() < surface_d) && ((p - test[j]).length() > 2 * NODE_DIAMETER)){
				surface_d = (p - test[j]).length();
			}
			if ((p - test[j]).length() < 2 * NODE_DIAMETER){
				return 100000000000000.0f;
			}
		}

		float times = p * n - d;

		vec3 insect_p = p + times * n;

		vec3 edge1 = test[1] - test[0];
		vec3 edge2 = test[2] - test[1];
		vec3 edge3 = test[0] - test[2];
		vec3 judge1 = insect_p - test[0];
		vec3 judge2 = insect_p - test[1];
		vec3 judge3 = insect_p - test[2];

		if (((edge1 ^ judge1) * n > 0) && ((edge2 ^ judge2) * n > 0) && ((edge3 ^ judge3) * n > 0)){
			if ((p - insect_p).length() < 2 * NODE_DIAMETER){
				return 100000000000000.0f;
			}
			else{
				if ((p - insect_p).length() < surface_d){
					surface_d = (p - insect_p).length();
				}
			}
		}
	}
	return surface_d;
}

float ball_surface_dist_fast(GLMmodel *model, vec3 &p, std::vector<int> &near_tri)
{
	float surface_d = 100000000000000.0f;

	for (int i = 0; i < near_tri.size(); i += 1){

		vec3 test[4];
		test[0] = vec3(model->vertices->at(3 * model->triangles->at(near_tri.at(i)).vindices[0] + 0), model->vertices->at(3 * model->triangles->at(near_tri.at(i)).vindices[0] + 1), model->vertices->at(3 * model->triangles->at(near_tri.at(i)).vindices[0] + 2));
		test[1] = vec3(model->vertices->at(3 * model->triangles->at(near_tri.at(i)).vindices[1] + 0), model->vertices->at(3 * model->triangles->at(near_tri.at(i)).vindices[1] + 1), model->vertices->at(3 * model->triangles->at(near_tri.at(i)).vindices[1] + 2));
		test[2] = vec3(model->vertices->at(3 * model->triangles->at(near_tri.at(i)).vindices[2] + 0), model->vertices->at(3 * model->triangles->at(near_tri.at(i)).vindices[2] + 1), model->vertices->at(3 * model->triangles->at(near_tri.at(i)).vindices[2] + 2));
		test[3] = (test[0] + test[1] + test[2]) / 3.0f;

		vec3 v1 = test[0] - test[1];
		vec3 v2 = test[2] - test[1];
		vec3 n = (v2 ^ v1).normalize();
		float d = n * test[0];

		for (int j = 0; j < 4; j += 1){
			if (((p - test[j]).length() < surface_d) && ((p - test[j]).length() > 2 * NODE_DIAMETER)){
				surface_d = (p - test[j]).length();
			}
			if ((p - test[j]).length() < 2 * NODE_DIAMETER){
				//cout << "retrun " << 100000000000000.0f << endl;
				return 100000000000000.0f;
			}
		}

		float times = p * n - d;

		vec3 insect_p = p + times * n;

		vec3 edge1 = test[1] - test[0];
		vec3 edge2 = test[2] - test[1];
		vec3 edge3 = test[0] - test[2];
		vec3 judge1 = insect_p - test[0];
		vec3 judge2 = insect_p - test[1];
		vec3 judge3 = insect_p - test[2];

		if (((edge1 ^ judge1) * n > 0) && ((edge2 ^ judge2) * n > 0) && ((edge3 ^ judge3) * n > 0)){
			if ((p - insect_p).length() < 2 * NODE_DIAMETER){
				//cout << "retrun " << 100000000000000.0f << endl;
				return 100000000000000.0f;
			}
			else{
				if ((p - insect_p).length() < surface_d){
					surface_d = (p - insect_p).length();
				}
			}
		}
	}
	//cout << "retrun " << surface_d << endl;
	return surface_d;
}

bool check_stick_intersect(GLMmodel *model, vec3 &p, vec3 &origin_p)
{
	vec3 dir = (p - origin_p).normalize();
	bool ans = false;
	for (unsigned int i = 0; i < model->triangles->size(); i += 1){

		vec3 test[3];
		test[0] = vec3(model->vertices->at(3 * model->triangles->at(i).vindices[0] + 0), model->vertices->at(3 * model->triangles->at(i).vindices[0] + 1), model->vertices->at(3 * model->triangles->at(i).vindices[0] + 2));
		test[1] = vec3(model->vertices->at(3 * model->triangles->at(i).vindices[1] + 0), model->vertices->at(3 * model->triangles->at(i).vindices[1] + 1), model->vertices->at(3 * model->triangles->at(i).vindices[1] + 2));
		test[2] = vec3(model->vertices->at(3 * model->triangles->at(i).vindices[2] + 0), model->vertices->at(3 * model->triangles->at(i).vindices[2] + 1), model->vertices->at(3 * model->triangles->at(i).vindices[2] + 2));
		
		vec3 v1 = test[0] - test[1];
		vec3 v2 = test[2] - test[1];
		vec3 n = (v2 ^ v1).normalize();
		float d = n * test[0];

		float dist1 = p * n - d;
		float dist2 = origin_p * n - d;
		
		int judge_p1;
		if (dist1 > 0.001f)
			judge_p1 = 1;
		else if (dist1 < -0.001f)
			judge_p1 = -1;
		else
			judge_p1 = 0;

		int judge_p2;
		if (dist2 > 0.001f)
			judge_p2 = 1;
		else if (dist2 < -0.001f)
			judge_p2 = -1;
		else
			judge_p2 = 0;

		if (judge_p1 != judge_p2){

			float times = (d - (p * n)) / (dir * n);

			//cout << i << " : " << endl;
			//cout << "tri : " << model->triangles->at(i).vindices[0] << " " << model->triangles->at(i).vindices[1] << " " << model->triangles->at(i).vindices[2] << endl;
			//vec3 insect_p = (dist2 * p + dist1 * origin_p) / (dist1 + dist2);
			vec3 insect_p = p + times * dir;
			//cout << "insect_p : " << insect_p[0] << " " << insect_p[1] << " " << insect_p[2] << endl;

			vec3 edge1 = test[1] - test[0];
			vec3 edge2 = test[2] - test[1];
			vec3 edge3 = test[0] - test[2];
			vec3 judge1 = insect_p - test[0];
			vec3 judge2 = insect_p - test[1];
			vec3 judge3 = insect_p - test[2];

			//cout << "judge : " << (((edge1 ^ judge1) * n > 0) && ((edge2 ^ judge2) * n > 0) && ((edge3 ^ judge3) * n > 0)) << endl;
			//cout << endl;

			if (((edge1 ^ judge1) * n > 0) && ((edge2 ^ judge2) * n > 0) && ((edge3 ^ judge3) * n > 0)){
				//cout << "insect true" << endl;
				return true;
			}
		}
		//cout << "i : " << i << endl;
		for (int j = 0; j < 3; j += 1){
			//float dist_times = -(p - test[j]) * dir / (dir * dir);
			//cout << dist_times << endl;
			//if (dist_times > 0.0f){
				//float line_d = (test[j] - (p + dist_times * dir)).length();
			if ((p - test[j]).length() < ERROR_THICKNESS){
				//cout << "short true" << endl;
				return true;
			}
			//}
		}
	}

	return false;
}
 
bool collision_test(std::vector<std::vector<zomeconn>> &test_connect, vec3 & give_up)
{
	zomedir t;
	int ball_ball_count = 0;
	#pragma omp parallel for
	for (int i = 0; i < test_connect.at(COLOR_WHITE).size(); i += 1){
		#pragma omp parallel for
		for (int j = 0; j < test_connect.at(COLOR_WHITE).size(); j += 1){
			if (i != j){
				float judge = (test_connect.at(COLOR_WHITE).at(i).position - test_connect.at(COLOR_WHITE).at(j).position).length();
				if (fabs(judge - NODE_DIAMETER) < 0.5f){
					//cout << fabs(judge - NODE_DIAMETER) << endl;
					ball_ball_count += 1;
				}
			}
		}
	}

	ball_ball_count /= 2;

	int stick_ball_count = 0;

	#pragma omp parallel for
	for (int i = 0; i < 3; i += 1){
		#pragma omp parallel for
		for (int j = 0; j < test_connect.at(i).size(); j += 1){
			vec3 p1 = test_connect.at(COLOR_WHITE).at((int)test_connect.at(i).at(j).fromindex[1]).position;
			vec3 p2 = test_connect.at(COLOR_WHITE).at((int)test_connect.at(i).at(j).towardindex[1]).position;
			float times = t.color_length(test_connect.at(i).at(j).color, test_connect.at(i).at(j).size);

			#pragma omp parallel for
			for (int k = 0; k < test_connect.at(COLOR_WHITE).size(); k += 1){
				if (k != test_connect.at(i).at(j).fromindex[1] && k != test_connect.at(i).at(j).towardindex[1]){
					vec3 o = test_connect.at(COLOR_WHITE).at(k).position;
					vec3 v1 = p2 - p1;
					vec3 v2 = o - p1;
					float o_times = (v2 * v1) / v1.length();
					if ((o_times < times) && (o_times > 0.0f)){
						vec3 check_p = p1 + t.dir->at(test_connect.at(i).at(j).fromface) * o_times;
						float judge = (o - check_p).length() - NODE_DIAMETER / 2.0f;
						if (judge < ERROR_THICKNESS){
							stick_ball_count += 1;
						}
					}
				}
			}
		}
	}

	int stick_stick_count = 0;

	#pragma omp parallel for
	for (int i = 0; i < 3; i += 1){
		#pragma omp parallel for
		for (int j = 0; j < test_connect.at(i).size(); j += 1){
			vec3 p1 = test_connect.at(COLOR_WHITE).at((int)test_connect.at(i).at(j).fromindex[1]).position;
			vec3 p2 = test_connect.at(COLOR_WHITE).at((int)test_connect.at(i).at(j).towardindex[1]).position;
			int fromindex1 = (int)test_connect.at(i).at(j).fromindex[1];
			int towardindex1 = (int)test_connect.at(i).at(j).towardindex[1];

			#pragma omp parallel for
			for (int a = 0; a < 3; a += 1){
			#pragma omp parallel for
				for (int b = 0; b < test_connect.at(a).size(); b += 1){
					if (!((a == i) && (b == j))){
						int fromindex2 = (int)test_connect.at(a).at(b).fromindex[1];
						int towardindex2 = (int)test_connect.at(a).at(b).towardindex[1];
						bool judge1 = (fromindex1 == fromindex2) || (fromindex1 == towardindex2);
						bool judge2 = (towardindex1 == fromindex2) || (towardindex1 == towardindex2);

						if (!(judge1 || judge2)){
							vec3 p3 = test_connect.at(COLOR_WHITE).at((int)test_connect.at(a).at(b).fromindex[1]).position;
							vec3 p4 = test_connect.at(COLOR_WHITE).at((int)test_connect.at(a).at(b).towardindex[1]).position;

							vec3 v1 = p2 - p1;
							vec3 n1 = (p2 - p1).normalize();
							vec3 v2 = p4 - p3;
							vec3 n2 = (p4 - p3).normalize();

							float ap = v1 * v1; //u*u
							float bp = v1 * v2; //u*v
							float cp = v2 * v2; //v*v

							if (!((n1 ^ n2).length() < 0.001f) && !(fabs(n1 * n2) < 0.001f)){
								vec3 v3 = p1 - p3;

								float dp = v1 * v3; //u*w 
								float ep = v2 * v3; //v*w

								float dt = ap * cp - bp * bp;
								float sd = dt;
								float td = dt;

								float sn = bp * ep - cp * dp;
								float tn = ap * ep - bp * dp;

								if (sn < 0.0001f){
									sn = 0.0f;
									tn = ep;
									td = cp;
								}
								else if (sn > sd){
									sn = sd;
									tn = ep + bp;
									td = cp;
								}

								if (tn < 0.0001f){
									tn = 0.0f;
									if (-dp < 0.0001f){
										sn = 0.0f;
									}
									else if (-dp > ap){
										sn = sd;
									}
									else{
										sn = -dp;
										sd = -ap;
									}
								}
								else if (tn > td){
									tn = td;
									if ((-dp + bp) < 0.0001f){
										sn = 0.0f;
									}
									else if ((-dp + bp) > ap){
										sn = sd;
									}
									else{
										sn = (-dp + bp);
										sd = ap;
									}
								}

								float sc = 0.0f;
								float tc = 0.0f;

								if (fabs(sn) < 0.0001f){
									sc = 0.0f;
								}
								else{
									sc = sn / sd;
								}

								if (fabs(tn) < 0.0001f){
									tc = 0.0f;
								}
								else{
									tc = tn / td;
								}

								vec3 dist = v3 + (sc * v1) - (tc * v2);

								if (dist.length() < ERROR_THICKNESS){
									stick_stick_count += 1;
								}
							}
						}

					}
				}
			}
		}
	}
	stick_stick_count /= 2;

	//cout << "error => ball-to-ball : " << ball_ball_count << " ball-to-rod :  " << stick_ball_count << " rod-to-rod :  " << stick_stick_count << endl;

	if (ball_ball_count != 0 || stick_ball_count != 0 || stick_stick_count != 0){
		if (ball_ball_count != 0)
			give_up[0] += 1.0f;
		if (stick_ball_count != 0)
			give_up[1] += 1.0f;
		if (stick_stick_count != 0)
			give_up[2] += 1.0f;
		return false;
	}

	return true;
}

void count_struct(std::vector<std::vector<zomeconn>> &test_connect, vec3 *count)
{
	for (unsigned int i = 0; i < 4; i += 1){
		if (i != 3){
			for (unsigned int j = 0; j < test_connect.at(i).size(); j += 1){
				if (test_connect.at(i).at(j).exist){
					if (test_connect.at(i).at(j).size == SIZE_S){
						count[i][SIZE_S] += 1;
					}
					else if (test_connect.at(i).at(j).size == SIZE_M){
						count[i][SIZE_M] += 1;
					}
					else{
						count[i][SIZE_L] += 1;
					}
				}
			}
		}
		else{
			for (unsigned int j = 0; j < test_connect.at(i).size(); j += 1){
				if (test_connect.at(i).at(j).exist){
					count[i][SIZE_S] += 1;
				}
			}
		}
	}	
}

void search_near_point(std::vector<std::vector<zomeconn>> &test_connect, std::vector<int> &check_index, int now)
{
	check_index.push_back(now);
	for (unsigned int i = 0; i < test_connect.at(COLOR_WHITE).size(); i += 1){
		if (i != now){
			if ((test_connect.at(COLOR_WHITE).at(now).position - test_connect.at(COLOR_WHITE).at(i).position).length() < SCALE * 1.5f){
				check_index.push_back(i);
			}
		}
	}
}

struct Plane_from_facet {
	Polyhedron_3::Plane_3 operator()(Polyhedron_3::Facet& f) {
		Polyhedron_3::Halfedge_handle h = f.halfedge();
		return Polyhedron_3::Plane_3(h->vertex()->point(),
			h->next()->vertex()->point(),
			h->opposite()->vertex()->point());
	}
};

bool pointInside(Polyhedron_3 &polyhedron, Point &query)
{
	std::transform(polyhedron.facets_begin(), polyhedron.facets_end(), polyhedron.planes_begin(), Plane_from_facet());

	Plane_iterator p = polyhedron.planes_begin();
	Face_iterator f = polyhedron.facets_begin();
	for (int i = 0; i < polyhedron.size_of_facets(); i += 1){
		if (query[0] * p->a() + query[1] * p->b() + query[2] * p->c() + p->d() == 0)
			return false;
		p++;
	}

	Vertex_iterator v = polyhedron.vertices_begin();
	for (int i = 0; i < polyhedron.size_of_vertices(); i += 1){
		if (v->point() == query)
			return false;
		v++;
	}
	return true;
}

bool check_inside(std::vector<std::vector<zomeconn>> &test_connect, int now)
{
	std::vector<int> check_index;
	search_near_point(test_connect, check_index, now);
	std::vector<Point> points;

	for (unsigned int i = 0; i < check_index.size(); i += 1){
		Point temp(test_connect.at(COLOR_WHITE).at(check_index.at(i)).position[0], test_connect.at(COLOR_WHITE).at(check_index.at(i)).position[1], test_connect.at(COLOR_WHITE).at(check_index.at(i)).position[2]);
		points.push_back(temp);
	}

	Polyhedron_3 poly;
	if (points.size() > 3){
		CGAL::convex_hull_3(points.begin(), points.end(), poly);
	}
	else{
		return true;
	}

	////cout << poly << endl;

	//ofstream os;
	//os.open("sphere.obj");

	////os << mesh;
	//Vertex_iterator v = poly.vertices_begin();
	//for (int i = 0; i < poly.size_of_vertices(); i += 1){
	//	os << "v " << v->point() << endl;
	//	cout << "v " << v->point() << endl;
	//	v++;
	//}

	//os << endl;
	//Face_iterator f = poly.facets_begin();
	//for (int i = 0; i < poly.size_of_facets(); i += 1){
	//	Halfedge_facet_circulator edge = f->facet_begin();
	//	os << "f ";
	//	for (int j = 0; j < CGAL::circulator_size(edge); j += 1){
	//		os << distance(poly.vertices_begin(), edge->vertex()) + 1 << " ";
	//		edge++;
	//	}
	//	os << endl;
	//	f++;
	//}
	//os.close();

	//std::cout << now  << " : The convex hull contains " << poly.size_of_vertices() << " vertices" << std::endl;

	return pointInside(poly, points.at(0));
}

void judge_outer(std::vector<std::vector<zomeconn>> &test_connect)
{
	for (unsigned int i = 0; i < test_connect.at(COLOR_WHITE).size(); i += 1){
		//cout << i << endl;
		bool judge = check_inside(test_connect, i);
		//cout << judge << endl;
		if (!judge)
			test_connect.at(COLOR_WHITE).at(i).outter = true;
		else
			test_connect.at(COLOR_WHITE).at(i).outter = false;
	}
}

void energy_material(std::vector<std::vector<zomeconn>> &test_connect, std::vector<simple_material> &materials, int mode)
{
	std::vector<vec2> material_queue;
	for (unsigned int i = 0; i < test_connect.at(COLOR_WHITE).size(); i += 1){
		if (test_connect.at(COLOR_WHITE).at(i).exist){
			vec2 temp;
			if (mode == ENERGY_ANGLE){
				temp = vec2(i, test_connect.at(COLOR_WHITE).at(i).energy_angle);
			}
			else if (mode == ENERGY_USE_STICK){
				temp = vec2(i, test_connect.at(COLOR_WHITE).at(i).energy_use_stick);
			}
			material_queue.push_back(temp);
		}
	}

	std::sort(material_queue.begin(),
		material_queue.end(),
		[](vec2 a, vec2 b){
		return b[1] > a[1];
	});

	int num_class = 0;
	for (unsigned int i = 0; i < material_queue.size(); i += 1){
		//cout << material_queue.at(i)[0] << " " << material_queue.at(i)[1] << endl;
		if (i == 0){
			if (mode == ENERGY_ANGLE){
				test_connect.at(COLOR_WHITE).at(material_queue.at(i)[0]).material_id_energy_angle = num_class;
			}
			else if (mode == ENERGY_USE_STICK){
				test_connect.at(COLOR_WHITE).at(material_queue.at(i)[0]).material_id_energy_use_stick= num_class;
			}
			num_class += 1;
		}
		else{
			if (material_queue.at(i)[1] == material_queue.at(i - 1)[1]){
				num_class -= 1;
				if (mode == ENERGY_ANGLE){
					test_connect.at(COLOR_WHITE).at(material_queue.at(i)[0]).material_id_energy_angle = num_class;
				}
				else if (mode == ENERGY_USE_STICK){
					test_connect.at(COLOR_WHITE).at(material_queue.at(i)[0]).material_id_energy_use_stick = num_class;
				}
				num_class += 1;
			}
			else{
				if (mode == ENERGY_ANGLE){
					test_connect.at(COLOR_WHITE).at(material_queue.at(i)[0]).material_id_energy_angle = num_class;
				}
				else if (mode == ENERGY_USE_STICK){
					test_connect.at(COLOR_WHITE).at(material_queue.at(i)[0]).material_id_energy_use_stick = num_class;
				}
				num_class += 1;
			}
		}
	}

	vec3 deleta = vec3(1.0f, 0.0f, -1.0f) / num_class;
	for (int i = 0; i < num_class; i += 1){
		simple_material temp_m;
		temp_m.name = std::to_string(materials.size());
		temp_m.diffuse[0] = 0.0f + deleta[0] * i;
		temp_m.diffuse[1] = 0.0f + deleta[1] * i;
		temp_m.diffuse[2] = 1.0f + deleta[2] * i;
		materials.push_back(temp_m);
		//cout << "m " << i << " : " << temp_m.diffuse[0] << " " << temp_m.diffuse[1] << " " << temp_m.diffuse[2] << endl;
	}
}

void generate_tenon(GLMmodel* model, std::vector<std::vector<zomeconn>> &test_connect , int use_solt)
{
	zomedir t;

	std::vector<int> use_ball;
	std::vector<vec3> use_ball_insect;

	for (int i = 0; i < test_connect.at(COLOR_WHITE).size(); i += 1){

		vec3 p = test_connect.at(COLOR_WHITE).at(i).position;
		vec3 ball_n = t.dir->at(use_solt);

		for (int j = 0; j < model->triangles->size(); j += 1){

			vec3 p1(model->vertices->at(3 * model->triangles->at(j).vindices[0] + 0), model->vertices->at(3 * model->triangles->at(j).vindices[0] + 1), model->vertices->at(3 * model->triangles->at(j).vindices[0] + 2));
			vec3 p2(model->vertices->at(3 * model->triangles->at(j).vindices[1] + 0), model->vertices->at(3 * model->triangles->at(j).vindices[1] + 1), model->vertices->at(3 * model->triangles->at(j).vindices[1] + 2));
			vec3 p3(model->vertices->at(3 * model->triangles->at(j).vindices[2] + 0), model->vertices->at(3 * model->triangles->at(j).vindices[2] + 1), model->vertices->at(3 * model->triangles->at(j).vindices[2] + 2));
			vec3 v1 = p1 - p2;
			vec3 v2 = p3 - p2;
			vec3 n = (v2 ^ v1).normalize();
			float d = n * p1;

			vec3 edge1 = p2 - p1;
			vec3 edge2 = p3 - p2;
			vec3 edge3 = p1 - p3;
			vec3 insect_p;
			vec3 judge1;
			vec3 judge2;
			vec3 judge3;

			float td = (d - (test_connect.at(COLOR_WHITE).at(i).position * n)) / (n * ball_n);
			if (td > 0){
				insect_p = test_connect.at(COLOR_WHITE).at(i).position + ball_n * td;

				judge1 = insect_p - p1;
				judge2 = insect_p - p2;
				judge3 = insect_p - p3;

				if (((edge1 ^ judge1) * n > 0) && ((edge2 ^ judge2) * n > 0) && ((edge3 ^ judge3) * n > 0)){
					if ((p - insect_p).length() < 1.0 * SCALE){
						if ((find(use_ball.begin(), use_ball.end(), i) - use_ball.begin()) >= use_ball.size()){
							use_ball.push_back(i);
							use_ball_insect.push_back(insect_p);
						}
					}
				}
			}

		}
	}
	//cout << use_ball.size();

	for (int i = 0; i < use_ball.size(); i += 1){
		//for (int i = 0; i < 1; i += 1){
		GLMmodel *xxx = NULL;

		int color = t.face_color(use_solt);
		if (color == COLOR_BLUE){
			xxx = glmReadOBJ("test_model/zometool/blue.obj");
		}
		else if (color == COLOR_RED){
			xxx = glmReadOBJ("test_model/zometool/red.obj");
		}
		else if (color == COLOR_YELLOW){
			xxx = glmReadOBJ("test_model/zometool/yellow.obj");
		}

		cout << use_ball.at(i) << endl;
		vec3 p = test_connect.at(COLOR_WHITE).at(use_ball.at(i)).position;
		vec3 inserct_p = use_ball_insect.at(i);

		vec3 new_p = (p + inserct_p) / 2;

		float l = ((p - inserct_p).length() - ERROR_THICKNESS) / 2.5f;

		glmScale_y(xxx, l);
		glmRT(xxx, vec3(0.0, t.roll(t.opposite_face(use_solt)), 0.0), vec3(0.0, 0.0, 0.0));
		glmRT(xxx, vec3(0.0, t.phi(t.opposite_face(use_solt)), t.theta(t.opposite_face(use_solt))), vec3(0.0, 0.0, 0.0));
		glmR(xxx, vec3(0.0, 0.0, 0.0));
		glmRT(xxx, vec3(0.0, 0.0, 0.0), new_p);

		glmCombine(model, xxx);
	}
	glmWriteOBJ(model, "test_model/out/out_p-zome.obj", GLM_NONE);
}