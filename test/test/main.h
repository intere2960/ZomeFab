#define GLEW_STATIC

#include <Windows.h>    // for solving the Code::Blocks errors
#include <stdlib.h>
#include <iostream>
#include <cmath>
#include <vector>

#include <GL/glew.h>
#include <GL/glut.h>

#include "textfile.h"
#include "glm.h"
#include "algebra3.h"

#include "shell.h"
#include "bisect.h"
#include "fill.h"
#include "zomedir.h"
#include "voxel.h"
#include "segment.h"
#include "bestfitobb.h"

using namespace std;

GLMmodel *myObj = NULL;
GLMmodel *myObj_inner = NULL;

GLMmodel temp_piece;

char model_source[] = "test_model/dolphin.obj";
//cube bunny alduin TestBall kitten dolphin Column4 ateneav0525 sphere
char model_out[] = "test_model/out/out_p.obj";

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
bool show_piece = true;

//plane test_plane1(0.0, 1.0, 0.0, 0.5, -1); // ax+by+cz=d  e->cut dir
//plane test_plane2(1.0, 0.0, 0.0, 0.5, -1);
//plane test_plane3(0.0, -1.0, 0.0, 0.5 , -1);
//plane test_plane4(-1.0, 0.0, 0.0, 0.5, -1);
//plane test_plane5(0.0, 0.0, -1.0, 0.0, -1);

//plane test_plane1(1.0, 1.0, 0.0, 0.5, -1); // ax+by+cz=d  e->cut dir
//plane test_plane2(1.0, -0.5, 0.0, 0.5, -1);
//plane test_plane3(0.0, 1.0, 0.0, -0.5 , 1);
//plane test_plane4(1.0, 0.5, 0.0, -0.5, 1);
//plane test_plane5(1.0, -1.0, 0.0, -0.5, 1);
//plane test_plane6(0.0, 0.0, -1.0, 0.0, -1);

//plane test_plane1(-3.41344e-006, 0.302711, -0.953082, -21.0594, -1); // ax+by+cz=d  e->cut dir
//plane test_plane2(0.822947, -0.230986, -0.519041, -18.4975, -1);
////plane test_plane2(2.0, 1.0, 0.0, 1.0, 1);
//plane test_plane3(0.445941, -0.894582, 0.0293094, -61.5363, -1);
//plane test_plane4(-0.148074, -0.973148, 0.176228, -47.3311, -1);
//plane test_plane5(-0.886545, -0.297628, -0.354197, -22.8361, -1);
//plane test_plane6(0.0, 0.0, -1.0, 0.0, -1);

//plane test_plane1(0.657198, 0.388593, -0.645822, 24.1358, -1); // ax+by+cz=d  e->cut dir
//plane test_plane2(0.797712, -0.528882, -0.289723, 11.3956, -1);
//plane test_plane3(-0.190125, -0.869877, 0.455156, 2.80831, -1);
//plane test_plane4(-0.815291, -0.516631, 0.26152, -1.73863, -1);
//plane test_plane5(-0.445947, 0.89458, -0.0293085, 63.5362, -1);
//plane test_plane6(0.0, 0.0, -1.0, 0.0, -1);

//plane test_plane1(-0.148074, -0.973148, 0.176228, -47.3311, 1); // ax+by+cz=d  e->cut dir
//plane test_plane2(-0.815291, -0.516631, 0.26152, -1.73863, 1);
//plane test_plane3(-0.999344, -0.0356894, 0.00622273, -4.31148, 1);
//plane test_plane4(-0.209343, 0.925338, -0.31611, -30.4608, 1);
//plane test_plane5(0.688078, 0.661504, -0.298263, -37.4853, 1);
//plane test_plane6(0.79353, -0.279945, 0.540315, -5.83478, 1);
//plane test_plane7(0.618385, -0.635714, 0.462025, -22.8756, 1);
//plane test_plane8(0.0, 0.0, -1.0, 0.0, -1);

//plane test_plane1(-0.526496, 0.634479, 0.565896, 122.793, 1); // ax+by+cz=d  e->cut dir
//plane test_plane2(0.0971029, 0.848491, 0.520225, 95.435, 1);
//plane test_plane3(-0.759769, -0.35547, -0.544419, 14.6121, -1);
//plane test_plane4(-0.78622, 0.556466, -0.268705, 123.328, -1);
//plane test_plane5(0.268936, 0.931406, -0.245269, 90.5815, -1);
//plane test_plane6(0.690705, 0.662271, -0.290386, 32.8401, -1);

plane test_plane1(0.11906, 0.991704, 0.048449, 122.042, 1); // ax+by+cz=d  e->cut dir
plane test_plane2(0.662443, -0.0535381, -0.747197, -37.4936, 1);
plane test_plane3(-0.626667, 0.745387, 0.227345, 149.35, -1);
plane test_plane4(-0.57429, 0.215177, -0.789867, 55.7267, -1);
plane test_plane5(-0.521073, -0.171785, -0.836046, 4.02722, -1);
//plane test_plane6(0.128078, -0.991759, -0.00310767, 20.1272, -1);
//plane test_plane7(-0.837661, -0.521881, 0.161134, 67.0868, -1);
//plane test_plane8(0.0, 0.0, -1.0, 0.0, -1);

vector<plane> planes;

vector<edge> all_edge;
vector<int> face_split_by_plane;
vector<int> face_inner_split_by_plane;

vector<voxel> all_voxel;
vector<vector<zomeconn>> zome_queue(4);
vector<GLMmodel> seg;

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

    if(key == 'v' || key == 'V') // down
    {
        show_piece = !show_piece;
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
//        glmWriteOBJ(myObj, model_out, GLM_NONE);
//        glmWriteOBJ(myObj, "test_model/zometool/view.obj", GLM_NONE);
//        glmWriteOBJ(&temp_piece, model_out, GLM_NONE);
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

//zome OBJ
//
//    GLMmodel *ball1 = NULL;
//    GLMmodel *ball2 = NULL;
//    GLMmodel *stick = NULL;
//    ball1 = glmReadOBJ("test_model/zometool/zomeball.obj");
//    ball2 = glmReadOBJ("test_model/zometool/zomeball.obj");
//    stick = glmReadOBJ("test_model/zometool/YellowS.obj");
//
//    zomedir t;
//    for(unsigned int i = 0; i < t.dir->size(); i += 1){
//        cout << i << " : " << t.dir->at(i)[0] << " " << t.dir->at(i)[1] << " " << t.dir->at(i)[2] << " => " << t.theta(i) << " " << t.phi(i) << " " << t.roll(i) << endl;
//    }
//
//    glmR(stick, vec3(0.0, 1.0, 0.0), t.roll(51));
//    glmRT(stick, vec3(0.0, t.phi(51), t.theta(51)), vec3(0.0, t.color_length(COLOR_YELLOW, SIZE_S) / 2, 0.0));
//    glmRT(ball2, vec3(0.0, 0.0, 0.0), vec3(SCALE, 0.0, 0.0));
//
//    glmCombine(ball1, stick);
