
#define GLEW_STATIC

#include <Windows.h>    // for solving the Code::Blocks errors
#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>

#include <GL/glew.h>
#include <GL/glut.h>

#include "textfile.h"
#include "glm.h"
#include "algebra3.h"
#include "glui_internal.h"

#include "shell.h"
#include "bisect.h"

using namespace std;

GLMmodel *myObj = NULL;
GLMmodel *myObj_inner = NULL;

char model_source[] = "test_model/cube.obj";
//cube bunny alduin TestBall kitten dolphin Column4 ateneav0525 sphere
char model_out[] = "test_model/out/out.obj";

int width, height;
int start_x, start_y;
GLdouble theta = -M_PI/2, phi = M_PI / 2;

vec3 eye_pos(0.0, 0.0, 0.0);
vec3 center(eye_pos[0] + sin(phi) * cos(theta), eye_pos[1] + cos(phi), 4 * sin(phi) * sin(theta));
vec3 up(0.0, 1.0, 0.0);

GLfloat vertices[][3] =
    {{-0.5,-0.5,-0.5},
    {0.5,-0.5,-0.5},
    {0.5,0.5,-0.5},
    {-0.5,0.5,-0.5},
    {-0.5,-0.5,0.5},
    {0.5,-0.5,0.5},
    {0.5,0.5,0.5},
    {-0.5,0.5,0.5}};

char vertex_shader[] = "myshader.vert",
     fragment_shader[] = "myshader.frag";

GLdouble lightTheta = 10.0;
GLfloat light0_ambient[] = {0.9, 0.9, 0.9, 1.0};
GLfloat light0_diffuse[] = {0.7, 0.7, 0.7, 1.0};
GLfloat light0_specular[] = {0.7, 0.7, 0.7, 1.0};
GLfloat light0_pos[] = {0.0, 0.0, 0.0, 1.0};
GLfloat light0_shininess = 50;

vec3 bounding_max;
vec3 bounding_min;
vec3 bound_size;
vec3 bound_center;

bool show = true;

//plane test_plane1(1.0, 2.0, 0.0, 1.0, -1); // ax+by+cz=d  e->cut dir
plane test_plane1(0.0, 1.0, 0.0, 0.5, -1); // ax+by+cz=d  e->cut dir
plane test_plane2(0.0, 1.0, 0.0, -0.5 , 1);
plane test_plane3(1.0, 0.0, 0.0, 0.5, -1);
plane test_plane4(1.0, 0.0, 0.0, -0.5, 1);

plane test_plane5(0.0, 0.0, 1.0, 0.0, 1);

vector<plane> planes;

vector<edge> all_edge;
vector<bool> is_face_split;
vector<int> face_split_by_plane;


//bounding box

void polygon(int a, int b, int c , int d)
{
/* draw a polygon via list of vertices */
    glBegin(GL_POLYGON);
        glVertex3fv(vertices[a]);
        glVertex3fv(vertices[b]);
        glVertex3fv(vertices[c]);
        glVertex3fv(vertices[d]);
    glEnd();
}

void cube(void)
{
/* map vertices to faces */

	polygon(0,3,2,1);
	polygon(2,3,7,6);
	polygon(0,4,7,3);
	polygon(1,2,6,5);
	polygon(4,5,6,7);
	polygon(0,1,5,4);
}

void draw_bounding_box()
{
    glScalef(bound_size[0],bound_size[1],bound_size[2]);
    glPolygonMode(GL_FRONT, GL_LINE);
    glPolygonMode(GL_BACK, GL_LINE);
    cube();
    glPolygonMode(GL_FRONT, GL_FILL);
    glPolygonMode(GL_BACK, GL_FILL);
    glScalef(1/bound_size[0],1/bound_size[1],1/bound_size[2]);
}

void bounding_box()
{
    bounding_min[0] = bounding_max[0] = myObj->vertices.at(3 * 1 + 0);
    bounding_min[1] = bounding_max[1] = myObj->vertices.at(3 * 1 + 1);
    bounding_min[2] = bounding_max[2] = myObj->vertices.at(3 * 1 + 2);

    for(unsigned int i = 1 ; i <= myObj->numvertices ; i += 1)
    {
        if (myObj->vertices.at(3 * i + 0) < bounding_min[0]) bounding_min[0] = myObj->vertices.at(3 * i + 0);
        if (myObj->vertices.at(3 * i + 0) > bounding_max[0]) bounding_max[0] = myObj->vertices.at(3 * i + 0);
        if (myObj->vertices.at(3 * i + 1) < bounding_min[1]) bounding_min[1] = myObj->vertices.at(3 * i + 1);
        if (myObj->vertices.at(3 * i + 1) > bounding_max[1]) bounding_max[1] = myObj->vertices.at(3 * i + 1);
        if (myObj->vertices.at(3 * i + 2) < bounding_min[2]) bounding_min[2] = myObj->vertices.at(3 * i + 2);
        if (myObj->vertices.at(3 * i + 2) > bounding_max[2]) bounding_max[2] = myObj->vertices.at(3 * i + 2);
    }

    bound_size[0] = bounding_max[0] - bounding_min[0];
    bound_size[1] = bounding_max[1] - bounding_min[1];
    bound_size[2] = bounding_max[2] - bounding_min[2];

    bound_center[0] = (bounding_max[0] + bounding_min[0])/2.0;
    bound_center[1] = (bounding_max[1] + bounding_min[1])/2.0;
    bound_center[2] = (bounding_max[2] + bounding_min[2])/2.0;
}


void collect_edge()
{
    int count = 0;
    vector<vec2> *temp_edge = new vector<vec2>[myObj->numvertices + 1];
    vector<int> *temp_point_tri = new std::vector<int>[myObj->numvertices + 1];

    if(!is_face_split.empty()){
        vector<bool> v;
        is_face_split.swap(v);
    }

    for(unsigned int i = 0; i < myObj->numtriangles ; i += 1){

        int min_index = myObj->triangles.at(i).vindices[0], temp_index = 0;
        is_face_split.push_back(true);

        if((unsigned int)min(min_index, (int)myObj->triangles.at(i).vindices[1]) == myObj->triangles.at(i).vindices[1]){
            min_index = myObj->triangles.at(i).vindices[1];
            temp_index = 1;
        }
        if((unsigned int)min(min_index, (int)myObj->triangles.at(i).vindices[2]) == myObj->triangles.at(i).vindices[2]){
            min_index = myObj->triangles.at(i).vindices[2];
            temp_index = 2;
        }

        unsigned int mid_index = myObj->triangles.at(i).vindices[(temp_index + 1) % 3], temp_mid_index = (temp_index + 1) % 3;
        if(mid_index > myObj->triangles.at(i).vindices[(temp_index + 2) % 3]){
            mid_index = myObj->triangles.at(i).vindices[(temp_index + 2) % 3];
            temp_mid_index = (temp_index + 2) % 3;
        }

        vec2 push_index1(min_index, mid_index), push_index2(min_index, myObj->triangles.at(i).vindices[(3 - temp_index - temp_mid_index) % 3]), push_index3(mid_index, myObj->triangles.at(i).vindices[(3 - temp_index - temp_mid_index) % 3]);

        bool add1 = true, add2 = true, add3 = true;

        for(unsigned int j = 0; j < temp_edge[min_index].size(); j += 1){
            if(push_index1[1] == temp_edge[min_index].at(j)[1]){
                add1 = false;
                break;
            }
        }
        for(unsigned int j = 0; j < temp_edge[min_index].size(); j += 1){
            if(push_index2[1] == temp_edge[min_index].at(j)[1]){
                add2 = false;
                break;
            }
        }
        for(unsigned int j = 0; j < temp_edge[mid_index].size(); j += 1){
            if(push_index3[1] == temp_edge[mid_index].at(j)[1]){
                add3 = false;
                break;
            }
        }

        if(add1){
            temp_edge[min_index].push_back(push_index1);
            count += 1;
        }

        if(add2){
            temp_edge[min_index].push_back(push_index2);
            count += 1;
        }

        if(add3){
            temp_edge[mid_index].push_back(push_index3);
            count += 1;
        }

        for(int j = 0 ; j < 3 ; j += 1)
        {
            temp_point_tri[myObj->triangles.at(i).vindices[j]].push_back(i);
            myObj->triangles.at(i).edge_index[j] = -1;
        }
    }

    if(!all_edge.empty()){
        vector<edge> v;
        all_edge.swap(v);
    }

    for(unsigned int i = 0; i < myObj->numvertices + 1; i += 1){
        for(unsigned int j = 0; j < temp_edge[i].size(); j += 1){
            vec3 p1(myObj->vertices.at((int)temp_edge[i].at(j)[0] * 3 + 0), myObj->vertices.at((int)temp_edge[i].at(j)[0] * 3 + 1), myObj->vertices.at((int)temp_edge[i].at(j)[0] * 3 + 2));
            vec3 p2(myObj->vertices.at((int)temp_edge[i][j][1] * 3 + 0), myObj->vertices.at((int)temp_edge[i].at(j)[1] * 3 + 1), myObj->vertices.at((int)temp_edge[i].at(j)[1] * 3 + 2));
            edge temp((int)temp_edge[i].at(j)[0], p1, (int)temp_edge[i].at(j)[1], p2);
            all_edge.push_back(temp);
        }
    }

    for(unsigned int i = 1; i <= myObj->numvertices; i += 1)
        sort(temp_point_tri[i].begin(), temp_point_tri[i].begin() + temp_point_tri[i].size());

    for(unsigned int i = 0; i < all_edge.size(); i += 1){
        vector<int> temp_vector(temp_point_tri[all_edge.at(i).index[0]].size() + temp_point_tri[all_edge.at(i).index[1]].size());
        vector<int>::iterator it;

        it = set_intersection(temp_point_tri[all_edge.at(i).index[0]].begin(), temp_point_tri[all_edge.at(i).index[0]].begin() + temp_point_tri[all_edge.at(i).index[0]].size(), temp_point_tri[all_edge.at(i).index[1]].begin(), temp_point_tri[all_edge.at(i).index[1]].begin() + temp_point_tri[all_edge[i].index[1]].size(), temp_vector.begin());
        temp_vector.resize(it - temp_vector.begin());

        all_edge.at(i).face_id[0] = temp_vector[0];
        all_edge.at(i).face_id[1] = temp_vector[1];

        for(int j = 0; j < 3; j += 1){
            if(myObj->triangles.at(temp_vector[0]).edge_index[j] == -1){
                myObj->triangles.at(temp_vector[0]).edge_index[j] = i;
                break;
            }
        }

        for(int j = 0; j < 3; j += 1){
            if(myObj->triangles.at(temp_vector[1]).edge_index[j] == -1){
                myObj->triangles.at(temp_vector[1]).edge_index[j] = i;
                break;
            }
        }
    }

    for(unsigned int i = 0; i < myObj->numtriangles; i += 1){
        float temp_edge_index[3];
        temp_edge_index[0] = myObj->triangles.at(i).edge_index[0];
        temp_edge_index[1] = myObj->triangles.at(i).edge_index[1];
        temp_edge_index[2] = myObj->triangles.at(i).edge_index[2];

//        std::cout << myObj->triangles.at(i).edge_index[0] << " " << myObj->triangles.at(i).edge_index[1] << " " << myObj->triangles.at(i).edge_index[2] << std::endl;
//
//        std::cout << "index : " << i << std::endl;

        for(int j = 0; j < 3; j += 1){
            bool judge1,judge2;
            judge1 = (myObj->triangles.at(i).vindices[0] == (unsigned int)all_edge.at(temp_edge_index[j]).index[0]) && (myObj->triangles.at(i).vindices[1] == (unsigned int)all_edge.at(temp_edge_index[j]).index[1]);
            judge2 = (myObj->triangles.at(i).vindices[0] == (unsigned int)all_edge.at(temp_edge_index[j]).index[1]) && (myObj->triangles.at(i).vindices[1] == (unsigned int)all_edge.at(temp_edge_index[j]).index[0]);
            if(judge1 || judge2){
                myObj->triangles.at(i).edge_index[0] = temp_edge_index[j];
            }

            judge1 = (myObj->triangles.at(i).vindices[1] == (unsigned int)all_edge.at(temp_edge_index[j]).index[0]) && (myObj->triangles.at(i).vindices[2] == (unsigned int)all_edge.at(temp_edge_index[j]).index[1]);
            judge2 = (myObj->triangles.at(i).vindices[1] == (unsigned int)all_edge.at(temp_edge_index[j]).index[1]) && (myObj->triangles.at(i).vindices[2] == (unsigned int)all_edge.at(temp_edge_index[j]).index[0]);
            if(judge1 || judge2){
                myObj->triangles.at(i).edge_index[1] = temp_edge_index[j];
            }

            judge1 = (myObj->triangles.at(i).vindices[2] == (unsigned int)all_edge.at(temp_edge_index[j]).index[0]) && (myObj->triangles.at(i).vindices[0] == (unsigned int)all_edge.at(temp_edge_index[j]).index[1]);
            judge2 = (myObj->triangles.at(i).vindices[2] == (unsigned int)all_edge.at(temp_edge_index[j]).index[1]) && (myObj->triangles.at(i).vindices[0] == (unsigned int)all_edge.at(temp_edge_index[j]).index[0]);
            if(judge1 || judge2){
                myObj->triangles.at(i).edge_index[2] = temp_edge_index[j];
            }
        }
    }

    for(unsigned int i = 0; i < myObj->numvertices + 1; i += 1){
        if(!temp_edge[i].empty()){
            vector<vec2> v;
            temp_edge[i].swap(v);
        }
        if(!temp_point_tri[i].empty()){
            vector<int> v;
            temp_point_tri[i].swap(v);
        }
    }

    delete temp_edge;
    delete temp_point_tri;
}

//shader

void setShaders()
{
    GLhandleARB v = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB),
                f = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB),
                p;

	char *vs = NULL, *fs = NULL;

	vs = textFileRead(vertex_shader);
    fs = textFileRead(fragment_shader);

	const char * vv = vs;
	const char * ff = fs;

	glShaderSourceARB(v, 1, &vv, NULL);
	glShaderSourceARB(f, 1, &ff, NULL);

	free(vs);free(fs);

	glCompileShaderARB(v);
	glCompileShaderARB(f);

    p = glCreateProgramObjectARB();
	glAttachObjectARB(p,v);
	glAttachObjectARB(p,f);

	glLinkProgramARB(p);
	glUseProgramObjectARB(p);

    glUniform1iARB(glGetUniformLocationARB(p, "texture"), 0);
    glUniform3fARB(glGetUniformLocationARB(p, "light"), light0_pos[0], light0_pos[1], light0_pos[2]);

    glUniform4fARB(glGetUniformLocationARB(p, "l_ambient"), light0_ambient[0], light0_ambient[1], light0_ambient[2], light0_ambient[3] );
    glUniform4fARB(glGetUniformLocationARB(p, "l_diffuse"), light0_diffuse[0], light0_diffuse[1], light0_diffuse[2], light0_diffuse[3] );
    glUniform4fARB(glGetUniformLocationARB(p, "l_specular"), light0_specular[0], light0_specular[1], light0_specular[2], light0_specular[3] );
    glUniform1fARB(glGetUniformLocationARB(p, "l_shininess"), light0_shininess );

//	glEnableClientState(GL_VERTEX_ARRAY);
//	glEnableVertexAttribArray(glGetAttribLocation(p,"position"));
//	glVertexPointer(3,GL_FLOAT,0,all_point);

	//glVertexAttribPointer(glGetAttribLocation(p,"position"),3,GL_FLOAT,0,0,all_point);

//    glUniform1fARB(glGetUniformLocationARB(p, "angle"), angle );

    //glVertexAttrib3fARB(glGetAttribLocationARB(p, "light"), light0_pos[0], light0_pos[1], light0_pos[2]);
}

//control

void mouse(int btn, int state, int x, int y)
{
    if(state == GLUT_DOWN)
    {
        start_x = x;
        start_y = y;
    }
}

void mouseMotion(int x, int y)
{
    theta += 2 * static_cast<double> (x - start_x) / width;
    if(theta > 2 * M_PI) theta -= 2 * M_PI;
    if(theta < -2 * M_PI) theta += 2 * M_PI;
    GLdouble tmp = phi;
    phi += 2 * static_cast<double> (y - start_y) / height;
    if(phi > 0 && phi < M_PI)
        center[1] = eye_pos[1]  + bound_size[1] * cos(phi);
    else
        phi = tmp;
    center[0] = eye_pos[0] + bound_size[0] * sin(phi) * cos(theta);
    center[2] = eye_pos[2] + bound_size[2] * sin(phi) * sin(theta);
    start_x = x;
    start_y = y;
}

void keyboard(unsigned char key,int x,int y)
{
    //²¾°Ê
//    if(key == 'q' || key == 'Q') //quit
//    {
//        glmDelete(myObj);
//        glmDelete(myObj_inner);
//        exit(0);
//    }
    if(key == 'w' || key == 'W') //move forward
    {
        eye_pos[0] += 0.1 * bound_size[0] * sin(phi) * cos(theta);
        eye_pos[1] += 0.1 * bound_size[1] * cos(phi);
        eye_pos[2] += 0.1 * bound_size[2] * sin(phi) * sin(theta);
        center[0] += 0.1 * bound_size[0] * sin(phi) * cos(theta);
        center[1] += 0.1 * bound_size[1] * cos(phi);
        center[2] += 0.1 * bound_size[2] * sin(phi) * sin(theta);
    }
    if(key == 's' || key == 'S') //move backward
    {
        eye_pos[0] -= 0.1 * bound_size[0] * sin(phi) * cos(theta);
        eye_pos[1] -= 0.1 * bound_size[1] * cos(phi);
        eye_pos[2] -= 0.1 * bound_size[2] * sin(phi) * sin(theta);
        center[0] -= 0.1 * bound_size[0] * sin(phi) * cos(theta);
        center[1] -= 0.1 * bound_size[1] * cos(phi);
        center[2] -= 0.1 * bound_size[2] * sin(phi) * sin(theta);
     }
    if(key == 'a' || key == 'A') //move left
    {
        eye_pos[0] += 0.1 * bound_size[0] * sin(phi) * sin(theta);
        eye_pos[2] += -0.1 * bound_size[2] * sin(phi) * cos(theta);
        center[0] += 0.1 * bound_size[0] * sin(phi) * sin(theta);
        center[2] += -0.1 * bound_size[2] * sin(phi) * cos(theta);
    }
    if(key == 'd' || key == 'D') //move right
    {
        eye_pos[0] += -0.1 * bound_size[0] * sin(phi) * sin(theta);
        eye_pos[2] += 0.1 * bound_size[2] * sin(phi) * cos(theta);
        center[0] += -0.1 * bound_size[0] * sin(phi) * sin(theta);
        center[2] += 0.1 * bound_size[2] * sin(phi) * cos(theta);
    }
    if(key == 'r' || key == 'R') // up
    {
        eye_pos[1] += 0.1 * bound_size[1];
        center[1] += 0.1 * bound_size[1];
    }
    if(key == 'f' || key == 'F') // down
    {
        eye_pos[1] -= 0.1 * bound_size[1];
        center[1] -= 0.1 * bound_size[1];
    }

    if(key == 'z' || key == 'Z')
    {
        theta = -M_PI/2, phi = M_PI / 2;
        eye_pos[0] = 0.0, eye_pos[1] = 0.0, eye_pos[2] = 0.0 + 2.0 * bound_size[2],
        center[0] = eye_pos[0] + sin(phi) * cos(theta), center[1] = eye_pos[1] + cos(phi), center[2] = 4*sin(phi) * sin(theta);
    }

    if(key == 'b' || key == 'B') // down
    {
        show = !show;
    }

    if(key=='+')
    {
        lightTheta += 10.0;
    }
    else if(key=='-')
    {
        lightTheta -= 10.0;
    }


    if(key==27)
    {
//        glmWriteOBJ(myObj, model_out, GLM_FLAT);
//        glmDelete(myObj);
//        glmDelete(myObj_inner);
        exit(0);
    }
}

void special(int key, int x, int y)
{
    if(key == GLUT_KEY_UP) // look up
        if(phi - 0.02 > 0) phi -= 0.02;
    if(key == GLUT_KEY_DOWN) // look down
        if(phi + 0.02 < M_PI) phi += 0.02;
    if(key == GLUT_KEY_LEFT) // turn left
    {
        theta -= 0.1;
        if(theta <= -2 * M_PI) theta += 2 * M_PI;
    }
    if(key == GLUT_KEY_RIGHT) // turn right
    {
        theta += 0.1;
        if(theta >= 2 * M_PI) theta -= 2 * M_PI;
    }
    center[0] = eye_pos[0] + bound_size[0] * sin(phi) * cos(theta);
    center[1] = eye_pos[1] + bound_size[1] * cos(phi);
    center[2] = eye_pos[2] + bound_size[2] * sin(phi) * sin(theta);
}

