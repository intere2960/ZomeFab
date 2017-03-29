#include <string>
#include <fstream>
#include "zomestruc.h"
#include "zomedir.h"
#include "global.h"
#include "glm.h"

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
	surface_d = 100000000000000.0;
	near_dir = -1;
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
	int ball_size = target.at(COLOR_WHITE).size();
	for (unsigned int i = 0; i < source.at(COLOR_WHITE).size(); i += 1){
		zomeconn temp = source.at(COLOR_WHITE).at(i);

		for (unsigned int j = 0; j < source.at(COLOR_WHITE).at(i).connect_stick.size(); j += 1){
			temp.connect_stick.at(j)[1] += target.at(temp.connect_stick.at(j)[0]).size();
		}
		target.at(COLOR_WHITE).push_back(temp);
	}

	for (int i = 0; i < 3; i += 1){
		for (unsigned int j = 0; j < source.at(i).size(); j += 1){
			zomeconn temp = source.at(i).at(j);

			if (temp.fromindex == vec2(-1, -1)){
				vec3 temp_pos = 2 * temp.position - source.at(COLOR_WHITE).at(temp.towardindex[1]).position;

				for (unsigned int k = 0; k < target.at(COLOR_WHITE).size(); k += 1){
					if ((target.at(COLOR_WHITE).at(k).position - temp_pos).length() < 0.001){
						temp.fromindex[0] = COLOR_WHITE;
						temp.fromindex[1] = k;
						break;
					}
				}
			}
			else{
				temp.fromindex[1] += ball_size;
			}			

			if (temp.towardindex == vec2(-1, -1)){
				vec3 temp_pos = 2 * temp.position - source.at(COLOR_WHITE).at(temp.fromindex[1] - ball_size).position;

				for (unsigned int k = 0; k < target.at(COLOR_WHITE).size(); k += 1){
					if ((target.at(COLOR_WHITE).at(k).position - temp_pos).length() < 0.001){
						temp.towardindex[0] = COLOR_WHITE;
						temp.towardindex[1] = k;
						break;
					}
				}
			}
			else{
				temp.towardindex[1] += ball_size;
			}			
			
			target.at(i).push_back(temp);
		}
	}
}

void output_struc(std::vector<std::vector<zomeconn>> &target)
{
	std::ofstream os;
	os.open("fake.txt");
	for (int i = 0; i < 4; i += 1){
		for (unsigned int j = 0; j < target.at(i).size(); j += 1){
			os << target.at(i).at(j).color << " ";
			os << target.at(i).at(j).position[0] << " " << target.at(i).at(j).position[1] << " " << target.at(i).at(j).position[2] << " ";
			if (i < 3){
				os << target.at(i).at(j).size << " ";
				os << target.at(i).at(j).fromface << " " << target.at(i).at(j).towardface << " ";
				os << target.at(i).at(j).fromindex[0] << " " << target.at(i).at(j).fromindex[1] << " ";
				os << target.at(i).at(j).towardindex[0] << " " << target.at(i).at(j).towardindex[1] << " ";
			}
			else{
				os << target.at(i).at(j).connect_stick.size() << " ";
				for (unsigned int k = 0; k < target.at(i).at(j).connect_stick.size(); k += 1){
					os << target.at(i).at(j).connect_stick.at(k)[0] << " " << target.at(i).at(j).connect_stick.at(k)[1] << " ";
				}
			}
			os << std::endl;
		}
	}
	os.close();
}

void struc_parser(std::vector<std::vector<zomeconn>> &target)
{
	std::ifstream is;
	is.open("fake.txt");
	int temp_color;
	vec3 temp_pos;
	while (is >> temp_color >> temp_pos[0] >> temp_pos[1] >> temp_pos[2]){
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
			vec2 temp_connect;
			is >> temp_size;
			for (int i = 0; i < temp_size; i += 1){
				is >> temp_connect[0] >> temp_connect[1];
				temp_ball.connect_stick.push_back(temp_connect);
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
					vec3 temp_t(temp_model->triangles->at(k).vindices[0] + num_v, temp_model->triangles->at(k).vindices[1] + num_v, temp_model->triangles->at(k).vindices[2] + num_v);
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
				vec3 temp_t(temp_model->triangles->at(k).vindices[0] + num_v, temp_model->triangles->at(k).vindices[1] + num_v, temp_model->triangles->at(k).vindices[2] + num_v);
				face_index.at(i).push_back(temp_t);
			}
			num_v += temp_model->numvertices;
		}
	}

	os << std::endl;

	for (unsigned int i = 0; i < output_connect.size(); i += 1){
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