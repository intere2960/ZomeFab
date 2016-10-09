/* Rotating cube with color interpolation */

/* Demonstration of use of homogeneous coordinate
transformations and simple data structure for representing
cube from Chapter 4 */

/*Both normals and colors are assigned to the vertices */
/*Cube is centered at origin so (unnormalized) normals
are the same as the vertex values */

#define GLEW_STATIC

#include <Windows.h>    // for solving the Code::Blocks errors
#include <stdlib.h>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <vector>

#include <GL/glew.h>
#include <GL/glut.h>

#include "glm.h"

#define PI 3.1415926535897

using namespace std;

GLMmodel *myObj = NULL;
GLMmodel *myObj_inner = NULL;
GLMmodel *cube = NULL;

int width, height;
int start_x, start_y;
GLdouble theta = -PI/2, phi = PI / 2;
GLdouble eye_x = 0.0, eye_y = 0.0, eye_z = 10.0,
         center_x = eye_x + sin(phi) * cos(theta), center_y = eye_y + cos(phi), center_z = 4*sin(phi) * sin(theta),
         up_x = 0.0, up_y = 1.0, up_z = 0.0;

bool ridingMode = false;
float delta_x = 0.0, delta_y = 0.0, delta_z = 0.0;
float angle=0;

GLfloat vertices[][3] =
    {{-0.5,-0.5,-0.5},
    {0.5,-0.5,-0.5},
    {0.5,0.5,-0.5},
    {-0.5,0.5,-0.5},
    {-0.5,-0.5,0.5},
    {0.5,-0.5,0.5},
    {0.5,0.5,0.5},
    {-0.5,0.5,0.5}};

GLfloat colors[][3] =
    {{1.0,1.0,1.0},
    {1.0,1.0,1.0},
	{1.0,1.0,1.0},
	{1.0,1.0,1.0},
	{1.0,1.0,1.0},
	{1.0,1.0,1.0},
	{1.0,1.0,1.0},
	{1.0,1.0,1.0}};

	GLfloat normals[][3] = {{-1.0,-1.0,-1.0},{1.0,-1.0,-1.0},
	{1.0,1.0,-1.0}, {-1.0,1.0,-1.0}, {-1.0,-1.0,1.0},
	{1.0,-1.0,1.0}, {1.0,1.0,1.0}, {-1.0,1.0,1.0}};

GLdouble lightTheta = 10.0;
//GLfloat light0_pos[]={100.0, 100.0, 100.0, 1.0};
//GLfloat light0_ambient[] = {0.9, 0.9, 0.9, 1.0};
//GLfloat light0_diffuse[] = {0.7, 0.7, 0.7, 1.0};
//GLfloat light0_specular[] = {0.7, 0.7, 0.7, 1.0};
GLfloat light0_diffuse[]={1.0, 1.0, 1.0, 1.0};
GLfloat light0_ambient[]={1.0, 1.0, 1.0, 1.0};
GLfloat light0_specular[]={1.0, 1.0, 1.0, 1.0};
GLfloat light0_pos[]={0.0, 0.0, 10.0, 1.0};

GLfloat min_x, max_x, min_y, max_y, min_z, max_z;
GLfloat bound_size[3];
GLfloat bound_center[3];

vector<int> *point_tri = NULL;

bool show = true;

//GLfloat model_center[3] = {0.0 , 0.0 , 0.0};

void polygon(int a, int b, int c , int d)
{
/* draw a polygon via list of vertices */
    glBegin(GL_POLYGON);
        //glColor3fv(vertices[a]);
        glVertex3fv(vertices[a]);
        //glColor3fv(vertices[b]);
        glVertex3fv(vertices[b]);
        //glColor3fv(vertices[c]);
        glVertex3fv(vertices[c]);
        //glColor3fv(vertices[d]);
        glVertex3fv(vertices[d]);
    glEnd();
}

void colorcube(void)
{
/* map vertices to faces */

	polygon(0,3,2,1);
	polygon(2,3,7,6);
	polygon(0,4,7,3);
	polygon(1,2,6,5);
	polygon(4,5,6,7);
	polygon(0,1,5,4);
}

void drawObj(GLMmodel *myObj)
{
    if (! myObj) return;

//    GLMgroup *groups = myObj->groups;
//    //int j=0;
//    while(groups)
//    {
//        //glBindTexture(GL_TEXTURE_2D, textureid[j]);
//        glBegin(GL_TRIANGLES);
//        //glBegin(GL_POINTS);
//        for(int i=0;i<groups->numtriangles;i+=1)
//        {
//            for (int v=0; v<3; v+=1)
//            {
//                //glColor3fv(& myObj->vertices[myObj->triangles[groups->triangles[i]].vindices[v]*3 ]);
//                glNormal3fv(& myObj->vertices[myObj->triangles[groups->triangles[i]].nindices[v]*3 ]);
//                //glTexCoord2fv(& myObj->texcoords[myObj->triangles[groups->triangles[i]].tindices[v]*2 ]);
//                glVertex3fv(& myObj->vertices[myObj->triangles[groups->triangles[i]].vindices[v]*3 ]);
//            }
//        }
//        groups=groups->next;
//        j+=1;
//        glEnd();
//    }
    glBegin(GL_TRIANGLES);
    for (int i=0; i<myObj->numtriangles; i+=1) {
        for (int v=0; v<3; v+=1) {
		    glColor3fv( & myObj->vertices[ myObj->triangles[i].vindices[v]*3 ] );
		    //glNormal3fv( & myObj->normals[ myObj->triangles[i].nindices[v]*3 ] );
		    glNormal3fv( & myObj->facetnorms[3 * (i+1)]);
		    glVertex3fv( & myObj->vertices[ myObj->triangles[i].vindices[v]*3 ] );
        }
    }
    glEnd();
}

//void setShaders()
//{
//    GLhandleARB v = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB),
//                f = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB),
//                p;
//
//	char *vs = NULL, *fs = NULL;
//
//	vs = textFileRead("myshader.v");
//    fs = textFileRead("myshader.f");
//
//	const char * vv = vs;
//	const char * ff = fs;
//
//	glShaderSourceARB(v, 1, &vv, NULL);
//	glShaderSourceARB(f, 1, &ff, NULL);
//
//	free(vs);free(fs);
//
//	glCompileShaderARB(v);
//	glCompileShaderARB(f);
//
//    p = glCreateProgramObjectARB();
//	glAttachObjectARB(p,v);
//	glAttachObjectARB(p,f);
//
//	glLinkProgramARB(p);
//	glUseProgramObjectARB(p);
//
//    glUniform1iARB(glGetUniformLocationARB(p, "texture"), 0);
//    glUniform3fARB(glGetUniformLocationARB(p, "light"), light0_pos[0], light0_pos[1], light0_pos[2]);
//
//    glUniform4fARB(glGetUniformLocationARB(p, "l_ambient"), 1.4, 1.4, 1.4, 1.0 );
//    glUniform4fARB(glGetUniformLocationARB(p, "l_diffuse"), 0.7, 0.7, 0.7, 1.0 );
//    glUniform4fARB(glGetUniformLocationARB(p, "l_specular"), 0.7, 0.7, 0.7, 1.0 );
//}

void display(void)
{
    /* display callback, clear frame buffer and z buffer,
        rotate cube and draw, swap buffers */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eye_x, eye_y, eye_z, center_x, center_y, center_z, up_x, up_y, up_z);

    //lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

//    light0_pos[0] = cos(lightTheta*PI/180.0);
//    light0_pos[1] = 1/sin(PI/4) * sin(lightTheta*PI/180.0);
//    light0_pos[2] = cos(lightTheta*PI/180.0);

    glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);

    if(show){
        //glTranslatef(0.0, 0.0, -10.0);    //Dragon
        //glScalef(4,4,4);
//        glPolygonMode(GL_FRONT, GL_LINE);
//        glPolygonMode(GL_BACK, GL_LINE);

        drawObj(myObj);
        //glmDraw(myObj,GLM_SMOOTH);

//        glPolygonMode(GL_FRONT, GL_FILL);
//        glPolygonMode(GL_BACK, GL_FILL);
        //glScalef(0.25,0.25,0.25);
        //glTranslatef(0.0, 0.0, 10.0);
        //printf("%f\n", myObj->vertices[5]);
    }

    //glTranslatef(0.0, 0.0, -10.0);    //Dragon
    //glScalef(0.5,0.5,0.5);
    drawObj(myObj_inner);
    //glmDraw(myObj_inner,GLM_SMOOTH);
    //printf("%f\n", myObj_inner->vertices[5]);
    //glScalef(2.0,2.0,2.0);
    //glTranslatef(0.0, 0.0, 10.0);

    glTranslatef(bound_center[0], bound_center[1], bound_center[2]);    //Dragon
    glScalef(bound_size[0],bound_size[1],bound_size[2]);
    glPolygonMode(GL_FRONT, GL_LINE);
    glPolygonMode(GL_BACK, GL_LINE);
    colorcube();
    glPolygonMode(GL_FRONT, GL_FILL);
    glPolygonMode(GL_BACK, GL_FILL);
    //glmDraw(myObj_inner,GLM_SMOOTH);
    //printf("%f\n", myObj_inner->vertices[5]);
    glScalef(1/bound_size[0],1/bound_size[1],1/bound_size[2]);
    glTranslatef(-bound_center[0], -bound_center[1], -bound_center[2]);

    glutSwapBuffers();
    glutPostRedisplay();
}

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
    if(theta > 2 * PI) theta -= 2 * PI;
    if(theta < -2 * PI) theta += 2 * PI;
    GLdouble tmp = phi;
    phi += 2 * static_cast<double> (y - start_y) / height;
    if(phi > 0 && phi < PI)
        center_y = eye_y + cos(phi);
    else
        phi = tmp;
    center_x = eye_x + sin(phi) * cos(theta);
    center_z = eye_z + sin(phi) * sin(theta);
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
        if(!ridingMode)
        {
            eye_x += 0.1 * sin(phi) * cos(theta);
            eye_y += 0.1 * cos(phi);
            eye_z += 0.1 * sin(phi) * sin(theta);
            center_x += 0.1 * sin(phi) * cos(theta);
            center_y += 0.1 * cos(phi);
            center_z += 0.1 * sin(phi) * sin(theta);
        }
        else
            delta_z += 0.2;
    }
    if(key == 's' || key == 'S') //move backward
    {
        if(!ridingMode)
        {
            eye_x -= 0.1 * sin(phi) * cos(theta);
            eye_y -= 0.1 * cos(phi);
            eye_z -= 0.1 * sin(phi) * sin(theta);
            center_x -= 0.1 * sin(phi) * cos(theta);
            center_y -= 0.1 * cos(phi);
            center_z -= 0.1 * sin(phi) * sin(theta);
        }
        else
            delta_z -= 0.2;
    }
    if(key == 'a' || key == 'A') //move left
    {
        if(!ridingMode)
        {
            eye_x += 0.1 * sin(phi) * sin(theta);
            eye_z += -0.1 * sin(phi) * cos(theta);
            center_x += 0.1 * sin(phi) * sin(theta);
            center_z += -0.1 * sin(phi) * cos(theta);
        }
        else
            delta_x += 0.2;
    }
    if(key == 'd' || key == 'D') //move right
    {
        if(!ridingMode)
        {
            eye_x += -0.1 * sin(phi) * sin(theta);
            eye_z += 0.1 * sin(phi) * cos(theta);
            center_x += -0.1 * sin(phi) * sin(theta);
            center_z += 0.1 * sin(phi) * cos(theta);
        }
        else
            delta_x -= 0.2;
    }
    if(key == 'r' || key == 'R') // up
    {
        if(!ridingMode)
        {
            eye_y += 0.1;
            center_y += 0.1;
        }
        else
            delta_y -= 0.2;
    }
    if(key == 'f' || key == 'F') // down
    {
        if(!ridingMode)
        {
            eye_y -= 0.1;
            center_y -= 0.1;
        }
        else
            delta_y += 0.2;
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
        exit(0);
    }
}

void special(int key, int x, int y)
{
    if(key == GLUT_KEY_UP) // look up
        if(phi - 0.02 > 0) phi -= 0.02;
    if(key == GLUT_KEY_DOWN) // look down
        if(phi + 0.02 < PI) phi += 0.02;
    if(key == GLUT_KEY_LEFT) // turn left
    {
        if(!ridingMode)
        {
            theta -= 0.1;
            if(theta <= -2 * PI) theta += 2 * PI;
        }
        else
        {
            angle += 1;
        }
    }
    if(key == GLUT_KEY_RIGHT) // turn right
    {
        if(!ridingMode)
        {
            theta += 0.1;
            if(theta >= 2 * PI) theta -= 2 * PI;
        }
        else
        {
            angle -= 1;
        }
    }
    center_x = eye_x + sin(phi) * cos(theta);
    center_y = eye_y + cos(phi);
    center_z = eye_z + sin(phi) * sin(theta);
}

void myReshape(int w, int h)
{
    width = w;
    height = h;
    float ratio = w * 1.0 / h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, w, h);
    gluPerspective(50.0, ratio, 0.1, 100000.0);
    glMatrixMode(GL_MODELVIEW);
}

void init()
{
    //cout << myObj->vertices[3 * 1 + 0] << " " << myObj->vertices[3 * 1 + 1] << " " << myObj->vertices[3 * 1 + 2] << endl;
    min_x = max_x = myObj->vertices[3 * 1 + 0];
    min_y = max_y = myObj->vertices[3 * 1 + 1];
    min_z = max_z = myObj->vertices[3 * 1 + 2];

    for(int i = 1 ; i <= myObj->numvertices ; i += 1)
    {
        if (myObj->vertices[3 * i + 0] < min_x) min_x = myObj->vertices[3 * i + 0];
        if (myObj->vertices[3 * i + 0] > max_x) max_x = myObj->vertices[3 * i + 0];
        if (myObj->vertices[3 * i + 1] < min_y) min_y = myObj->vertices[3 * i + 1];
        if (myObj->vertices[3 * i + 1] > max_y) max_y = myObj->vertices[3 * i + 1];
        if (myObj->vertices[3 * i + 2] < min_z) min_z = myObj->vertices[3 * i + 2];
        if (myObj->vertices[3 * i + 2] > max_z) max_z = myObj->vertices[3 * i + 2];
//        model_center[0] += myObj->vertices[3 * i + 0];
//        model_center[1] += myObj->vertices[3 * i + 1];
//        model_center[2] += myObj->vertices[3 * i + 2];
    }

//    cout << max_x << " " << max_y << " " << max_z << endl;
//    cout << min_x << " " << min_y << " " << min_z << endl;

    bound_size[0] = max_x - min_x;
    bound_size[1] = max_y - min_y;
    bound_size[2] = max_z - min_z;

    bound_center[0] = (max_x + min_x)/2.0;
    bound_center[1] = (max_y + min_y)/2.0;
    bound_center[2] = (max_z + min_z)/2.0;

    cout << bound_size[0] << " " << bound_size[1] << " " << bound_size[2] << endl;
    cout << bound_center[0] << " " << bound_center[1] << " " << bound_center[2] << endl;

//    model_center[0] = model_center[0]/myObj->numvertices;
//    model_center[1] = model_center[1]/myObj->numvertices;
//    model_center[2] = model_center[2]/myObj->numvertices;

//    cout << model_center[0] << " " << model_center[1] << " " << model_center[2] << endl;

    //if(myObj->numnormals == 0)
    point_tri = new vector<int>[myObj->numvertices + 1];
    //cout << point_tri[0].size() << endl;

    //cout << point_tri[myObj->numvertices-1].size() << endl;

//        for(int i = 0 ; i < myObj->numtriangles ; i += 1)
//        {
//            cout << i + 1 << " : ";
//            for(int j = 0 ; j < 3 ; j += 1)
//            {
//                cout << myObj->triangles[i].vindices[j] << " ";
//            }
//
//            cout << endl << "Normal : ";
//
//            for(int j = 0 ; j < 3 ; j += 1)
//            {
//                cout << myObj->facetnorms[3 * ( i + 1 ) + j] << " ";
//            }
//
//            cout << endl;
//        }
//
//         cout << endl;

    for(int i = 0 ; i < myObj->numtriangles ; i += 1)
    {
        for(int j = 0 ; j < 3 ; j += 1)
        {
            bool add = true;
            //cout << myObj->triangles[i].vindices[j] << " ";
            for(int k = 0 ; k < point_tri[myObj->triangles[i].vindices[j]].size() ; k += 1)
            {
                //cout << point_tri[myObj->triangles[i].vindices[j]][k] << " ";
                GLfloat *temp = &myObj->facetnorms[3 * point_tri[myObj->triangles[i].vindices[j]][k]];
                //cout << myObj->facetnorms[3 * point_tri[myObj->triangles[i].vindices[j]][k]] << endl;

                //cout <<  *temp << " " << *(temp + 1) << " " << *(temp + 2) << endl;
                if(*temp == myObj->facetnorms[3 * (i+1) + 0] && *(temp + 1) == myObj->facetnorms[3 * (i+1) + 1] && *(temp + 2) == myObj->facetnorms[3 * (i+1) + 2])
                {
                    add = false;
                    break;
                    //cout << "in" << endl;
                }
                //point_tri[myObj->triangles[i].vindices[j]].push_back(i + 1);
            }
            //cout << endl;
            if(add)
                point_tri[myObj->triangles[i].vindices[j]].push_back(i + 1);
        }
        //cout << endl;
    }
    //cout << endl;

    for(int i = 1 ; i <= myObj->numvertices ; i += 1)
    {
        sort(point_tri[i].begin(),point_tri[i].begin() + point_tri[i].size());
    }

//        for(int j = 1 ; j <= myObj->numvertices ; j += 1)
//        {
//            cout << j << " : ";
//            for(int i = 0 ; i < point_tri[j].size() ; i += 1)
//            {
//                cout << point_tri[j][i] << " ";
//            }
//            cout << endl;
//        }
    //cout << point_tri[1].size() << endl;

    myObj->numnormals = myObj->numvertices;
    myObj->normals = new GLfloat[3 * (myObj->numnormals + 1)];
    for(int i = 1 ; i <= myObj->numnormals ; i += 1)
    {
        GLfloat temp[3] = {0.0 , 0.0 , 0.0};
        for(int j = 0 ; j < point_tri[i].size() ; j += 1)
        {
            temp[0] += myObj->facetnorms[3 * point_tri[i][j] + 0];
            temp[1] += myObj->facetnorms[3 * point_tri[i][j] + 1];
            temp[2] += myObj->facetnorms[3 * point_tri[i][j] + 2];
        }
        GLfloat normal_length = sqrt(pow(temp[0],2) + pow(temp[1],2) + pow(temp[2],2));
        temp[0] /= normal_length;
        temp[1] /= normal_length;
        temp[2] /= normal_length;

        myObj->normals[3 * i + 0] = temp[0];
        myObj->normals[3 * i + 1] = temp[1];
        myObj->normals[3 * i + 2] = temp[2];

        //cout << temp[0] << " " << temp[1] << " " << temp[2] << endl;
    }

    myObj_inner->numnormals = myObj_inner->numvertices;
    myObj_inner->normals = new GLfloat[3 * (myObj_inner->numnormals + 1)];

    for(int i = 1 ; i <= myObj_inner->numvertices ; i += 1)
    {
        myObj_inner->vertices[3 * i + 0] = myObj->vertices[3 * i + 0] - 0.05 * myObj->normals[3 * i + 0];
        myObj_inner->normals[3 * i + 0] =  -1 * myObj->normals[3 * i + 0];

        myObj_inner->vertices[3 * i + 1] = myObj->vertices[3 * i + 1] - 0.05 * myObj->normals[3 * i + 1];
        myObj_inner->normals[3 * i + 1] =  -1 * myObj->normals[3 * i + 1];

        myObj_inner->vertices[3 * i + 2] = myObj->vertices[3 * i + 2] - 0.05 * myObj->normals[3 * i + 2];
        myObj_inner->normals[3 * i + 2] =  -1 * myObj->normals[3 * i + 2];
    }

    myObj_inner->numfacetnorms = myObj->numfacetnorms;
    myObj_inner->facetnorms = new GLfloat[3 * (myObj_inner->numfacetnorms + 1)];

    for(int i = 1 ; i <= myObj->numfacetnorms ; i += 1)
    {
        myObj_inner->facetnorms[3 * i + 0] =  -1 * myObj->facetnorms[3 * i + 0];

        myObj_inner->facetnorms[3 * i + 1] =  -1 * myObj->facetnorms[3 * i + 1];

        myObj_inner->facetnorms[3 * i + 2] =  -1 * myObj->facetnorms[3 * i + 2];
    }
    //cout << myObj->numvertices << " " << myObj->numnormals << endl;



    //GLfloat model_center2[3] = {0.0 , 0.0 , 0.0};
//    GLfloat model_center2[3];
//    model_center2[0] = 0.0;
//    model_center2[1] = 0.0;
//    model_center2[2] = 0.0;
//    cout << model_center2[0] << " " << model_center2[1] << " " << model_center2[2] << endl;
//    for(int i = 1 ; i <= myObj_inner->numvertices ; i += 1)
//    {
//        model_center2[0] += myObj_inner->vertices[3 * i + 0];
//        model_center2[1] += myObj_inner->vertices[3 * i + 1];
//        model_center2[2] += myObj_inner->vertices[3 * i + 2];
//        //cout << myObj_inner->vertices[3 * i + 0] << " " << myObj_inner->vertices[3 * i + 1] << " " << myObj_inner->vertices[3 * i + 2] << endl;
//    }
//    cout << model_center2[0] << " " << model_center2[1] << " " << model_center2[2] << endl;
//
//    model_center2[0] = model_center2[0]/myObj_inner->numvertices;
//    model_center2[1] = model_center2[1]/myObj_inner->numvertices;
//    model_center2[2] = model_center2[2]/myObj_inner->numvertices;
//
//    cout << model_center2[0] << " " << model_center2[1] << " " << model_center2[2] << endl;
}

int main(int argc, char **argv)
{
    //myObj = glmReadOBJ("sponza.obj");
    myObj = glmReadOBJ("cube.obj");
    //myObj = glmReadOBJ("alduin.obj");
    myObj_inner = glmReadOBJ("cube.obj");

    cube = glmReadOBJ("cube.obj");

//    glmUnitize(myObj);
//    glmUnitize(myObj_inner);

    glmFacetNormals(myObj);
    //glmFacetNormals(myObj_inner);

    myObj->position[0] = 0.0;
    myObj->position[1] = 0.0;
    myObj->position[2] = 0.0;

    myObj_inner->position[0] = 0.0;
    myObj_inner->position[1] = 0.0;
    myObj_inner->position[2] = 0.0;

//    myObj->position[0] = model_center[0];
//    myObj->position[1] = model_center[1];
//    myObj->position[2] = model_center[2];
//
//    myObj_inner->position[0] = model_center[0];
//    myObj_inner->position[1] = model_center[1];
//    myObj_inner->position[2] = model_center[2];

//    for(int i = 1 ; i <= myObj->numfacetnorms ; i += 1)
//    {
//        cout << myObj->facetnorms[3 * i + 0] << " " << myObj->facetnorms[3 * i + 1] << " " << myObj->facetnorms[3 * i + 2] << endl;
//    }
//    for(int i = 1 ; i <= myObj->numvertices ; i += 1)
//    {
//        cout << myObj->vertices[3 * i + 0] << " " << myObj->vertices[3 * i + 1] << " " << myObj->vertices[3 * i + 2] << endl;
//    }

    init();

//    eye_x = model_center[0];
//    eye_y = model_center[1];
//    eye_z = model_center[2];

    //glmVertexNormals(myObj,150.0);
    //printf("%d %d \n",myObj->numtriangles,myObj->numfacetnorms);
//    cout << endl;
    //cout << myObj->numtriangles << " " << myObj->numfacetnorms << endl;


//    for(int i = 0 ; i < myObj_inner->numfacetnorms ; i += 1)
//    {
//        cout << myObj_inner->facetnorms[3 * i + 0] << " " << myObj_inner->facetnorms[3 * i + 1] << " " << myObj_inner->facetnorms[3 * i + 2] << endl;
//    }

    //glmVertexNormals(myObj_inner,150.0);

    //cout << myObj_inner->numtriangles << " " << myObj_inner->numfacetnorms << endl;

    //point_tri = new

	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(1000,1000);

    glutCreateWindow("Zometool");
	glutDisplayFunc(display);
	glutReshapeFunc(myReshape);
	glutMouseFunc(mouse);
    glutMotionFunc(mouseMotion);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
	glEnable(GL_DEPTH_TEST); /* Enable hidden--surface--removal */

	glewInit();

	glutMainLoop();
    return 0;
}
