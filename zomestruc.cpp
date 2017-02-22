#include "zomestruc.h"


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
	if ( color == COLOR_BLUE )
	{
		if ( size == SIZE_S ) modelnumber = 1;
		if ( size == SIZE_M ) modelnumber = 2;
		if ( size == SIZE_L ) modelnumber = 3;
	}
	else if ( color == COLOR_RED )
	{
		if ( size == SIZE_S ) modelnumber = 4;
		if ( size == SIZE_M ) modelnumber = 5;
		if ( size == SIZE_L ) modelnumber = 6;
	}
	else if ( color == COLOR_YELLOW )
	{
		if ( size == SIZE_S ) modelnumber = 7;
		if ( size == SIZE_M ) modelnumber = 8;
		if ( size == SIZE_L ) modelnumber = 9;
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
