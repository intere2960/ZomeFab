#include "zomestruc.h"
#include "zomedir.h"
#include "global.h"

#include <fstream>

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
		//os << i << " : " << std::endl;
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

#include <iostream>

void struc_parser(std::vector<std::vector<zomeconn>> &target)
{
	std::ifstream is;
	is.open("fake.txt");
	int temp_color;
	vec3 temp_pos;
	while (is >> temp_color >> temp_pos[0] >> temp_pos[1] >> temp_pos[2]){
		//std::cout << temp_color << " " << temp_pos[0] << " " << temp_pos[1] << " " << temp_pos[2] << std::endl;
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

			//std::cout << temp_fromface << " " << temp_towardface << " ";
			//std::cout << temp_fromindex[0] << " " << temp_fromindex[1] << " ";
			//std::cout << towardindex[0] << " " << towardindex[1] << " ";
		}
		else{
			zomeconn temp_ball;
			temp_ball.color = temp_color;
			temp_ball.position = temp_pos;

			int temp_size;
			vec2 temp_connect;
			is >> temp_size;
			//std::cout << temp_size << " ";
			for (int i = 0; i < temp_size; i += 1){
				is >> temp_connect[0] >> temp_connect[1];
				temp_ball.connect_stick.push_back(temp_connect);
				//std::cout << temp_connect[0] << " " << temp_connect[1] << " ";
			}
			target.at(COLOR_WHITE).push_back(temp_ball);
		}
		//std::cout << std::endl;
	}

	is.close();
}