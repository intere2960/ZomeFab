#ifndef ZOMESTRUC_H_INCLUDED
#define ZOMESTRUC_H_INCLUDED

#include <GL/glut.h>
#include "algebra3.h"
#include "zomedir.h"

class zomeconn
{
public:
	zomeconn();
	~zomeconn();
	int getmodelnumber();

	int index;
	int color;

	vec3 position;
	vec3 rotation;
	vec3 endposition;

	int type; //0 none 1 single 2 double
	vec2 fromindex;
	vec2 towardindex;
	int fromface;
	int towardface;
	int size; // S M L
//	ModelObject* thisModel;

	bool isAnalyzed; //analyze force
	float force;

//	zomeconn* link;
};


class zomestruc
{
public:
	zomestruc();
	~zomestruc();
	int index;
	vec3 position;
//	zomestruc* originalBall;
//	ZomeConnection** faceConnection;//62
//	zomestruc** nextBall;//62
//
//	zomestruc* link;
//	ModelObject* thisModel;
//
//	int CheckConnection( int faceIndex );// -1 0 1 2
//	bool CheckBallExist( int faceIndex );
};

//class VirtualBall
//{
//public:
//	VirtualBall( int i , ZomeConnection* c );
//	int index;
//	GLfloat position[3];
//	bool isBottom;
//	int bottomForceIndex;
//	int connectionAmount;
//	int connIndex[3];
//	int connectFace[3];
//	ZomeConnection* conn[3];
//
//	void AddConn( ZomeConnection* c);
//};

#endif // ZOMESTRUC_H_INCLUDED
