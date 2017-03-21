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
