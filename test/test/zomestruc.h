#ifndef ZOMESTRUC_H_INCLUDED
#define ZOMESTRUC_H_INCLUDED

#include <vector>
#include "algebra3.h"
#include "glm.h"

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
	vec2 connect_stick[62];

	bool isAnalyzed; //analyze force
	float force;

	int near_dir;
	float surface_d;

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

void combine_zome_ztruc(std::vector<std::vector<zomeconn>> &target, std::vector<std::vector<zomeconn>> &source);
void output_struc(std::vector<std::vector<zomeconn>> &target);
void struc_parser(std::vector<std::vector<zomeconn>> &target);
void output_zometool(vec3 &rotation, std::vector<std::vector<zomeconn>> &zome_queue, int piece_id);
void output_zometool(std::vector<std::vector<zomeconn>> &output_connect, std::string &filename);
float point_surface_dist(GLMmodel *model, vec3 &p);

#endif // ZOMESTRUC_H_INCLUDED
