
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

vector<int> *point_tri = NULL;

bool show = true;

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
    //cout << myObj->vertices[3 * 1 + 0] << " " << myObj->vertices[3 * 1 + 1] << " " << myObj->vertices[3 * 1 + 2] << endl;
    bounding_min[0] = bounding_max[0] = myObj->vertices[3 * 1 + 0];
    bounding_min[1] = bounding_max[1] = myObj->vertices[3 * 1 + 1];
    bounding_min[2] = bounding_max[2] = myObj->vertices[3 * 1 + 2];

    for(int i = 1 ; i <= myObj->numvertices ; i += 1)
    {
        if (myObj->vertices[3 * i + 0] < bounding_min[0]) bounding_min[0] = myObj->vertices[3 * i + 0];
        if (myObj->vertices[3 * i + 0] > bounding_max[0]) bounding_max[0] = myObj->vertices[3 * i + 0];
        if (myObj->vertices[3 * i + 1] < bounding_min[1]) bounding_min[1] = myObj->vertices[3 * i + 1];
        if (myObj->vertices[3 * i + 1] > bounding_max[1]) bounding_max[1] = myObj->vertices[3 * i + 1];
        if (myObj->vertices[3 * i + 2] < bounding_min[2]) bounding_min[2] = myObj->vertices[3 * i + 2];
        if (myObj->vertices[3 * i + 2] > bounding_max[2]) bounding_max[2] = myObj->vertices[3 * i + 2];
    }

    bound_size[0] = bounding_max[0] - bounding_min[0];
    bound_size[1] = bounding_max[1] - bounding_min[1];
    bound_size[2] = bounding_max[2] - bounding_min[2];

    bound_center[0] = (bounding_max[0] + bounding_min[0])/2.0;
    bound_center[1] = (bounding_max[1] + bounding_min[1])/2.0;
    bound_center[2] = (bounding_max[2] + bounding_min[2])/2.0;
}

//shader

void setShaders()
{
    GLhandleARB v = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB),
                f = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB),
                p;

	char *vs = NULL, *fs = NULL;

	vs = textFileRead("myshader.vert");
    fs = textFileRead("myshader.frag");

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
    if(key == 'q' || key == 'Q') //quit
    {
        glmDelete(myObj);
        glmDelete(myObj_inner);
        delete point_tri;
        exit(0);
    }
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
        glmDelete(myObj);
        glmDelete(myObj_inner);
        delete point_tri;
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

