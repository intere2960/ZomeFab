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

#include "textfile.h"
#include "glm.h"
#include "algebra3.h"
#include "glui_internal.h"

#define PI 3.1415926535897

using namespace std;

GLMmodel *myObj = NULL;
GLMmodel *myObj_inner = NULL;

int width, height;
int start_x, start_y;
GLdouble theta = -PI/2, phi = PI / 2;
GLdouble eye_x = 0.0, eye_y = 0.0, eye_z = 0.0,
         center_x = eye_x + sin(phi) * cos(theta), center_y = eye_y + cos(phi), center_z = 4*sin(phi) * sin(theta),
         up_x = 0.0, up_y = 1.0, up_z = 0.0;

bool ridingMode = false;
float delta_x = 0.0, delta_y = 0.0, delta_z = 0.0;
GLfloat angle=0;

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
//GLfloat light0_pos[]={100.0, 100.0, 100.0, 1.0};
GLfloat light0_ambient[] = {0.9, 0.9, 0.9, 1.0};
GLfloat light0_diffuse[] = {0.7, 0.7, 0.7, 1.0};
GLfloat light0_specular[] = {0.7, 0.7, 0.7, 1.0};
//GLfloat light0_diffuse[]={1.0, 1.0, 1.0, 1.0};
//GLfloat light0_ambient[]={1.0, 1.0, 1.0, 1.0};
//GLfloat light0_specular[]={1.0, 1.0, 1.0, 1.0};
GLfloat light0_pos[]={0.0, 0.0, 0.0, 1.0};
GLfloat light0_shininess = 50;

GLfloat min_x, max_x, min_y, max_y, min_z, max_z;
GLfloat bound_size[3];
GLfloat bound_center[3];

//GLfloat angle = 0.0;

vector<int> *point_tri = NULL;
//vec3 *all_point;

bool show = true;

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

void drawObj(GLMmodel *myObj)
{
    if (! myObj) return;

    GLMgroup *groups = myObj->groups;

    glBegin(GL_TRIANGLES);
    while(groups)
    {
        //glBindTexture(GL_TEXTURE_2D, textureid[j]);
        for(int i=0;i<groups->numtriangles;i+=1)
        {
            for (int v=0; v<3; v+=1)
            {
                //glColor3fv(& myObj->vertices[myObj->triangles[groups->triangles[i]].vindices[v]*3 ]);
                glNormal3fv(& myObj->normals[myObj->triangles[groups->triangles[i]].nindices[v]*3 ]);
                //glTexCoord2fv(& myObj->texcoords[myObj->triangles[groups->triangles[i]].tindices[v]*2 ]);
                glVertex3fv(& myObj->vertices[myObj->triangles[groups->triangles[i]].vindices[v]*3 ]);
            }
        }
        groups=groups->next;
    }
    glEnd();

//    glBegin(GL_TRIANGLES);
//    for (int i=0; i<myObj->numtriangles; i+=1) {
//        for (int v=0; v<3; v+=1) {
//		    //glColor3fv( & myObj->vertices[ myObj->triangles[i].vindices[v]*3 ] );
//		    glNormal3fv( & myObj->normals[ myObj->triangles[i].nindices[v]*3 ] );
//		    //glNormal3fv( & myObj->facetnorms[3 * (i+1)]);
//		    glVertex3fv( & myObj->vertices[ myObj->triangles[i].vindices[v]*3 ] );
//        }
//    }
//    glEnd();
}

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

//    setShaders();

    glPushMatrix();

    glTranslatef(-bound_center[0], -bound_center[1], -bound_center[2]);

    if(show){
//        glPolygonMode(GL_FRONT, GL_LINE);
//        glPolygonMode(GL_BACK, GL_LINE);
//        drawObj(myObj);
//        glmDraw(myObj, GLM_FLAT);
        glmDraw(myObj,GLM_SMOOTH);
        //angle += 1.0;
//        glmDraw(myObj,GLM_NONE);
//        glPolygonMode(GL_FRONT, GL_FILL);
//        glPolygonMode(GL_BACK, GL_FILL);
    }


//    drawObj(myObj_inner);
//    glmDraw(myObj_inner, GLM_FLAT);
//    glmDraw(myObj_inner,GLM_SMOOTH);
//    glmDraw(myObj_inner,GLM_NONE);

//    glTranslatef(bound_center[0], bound_center[1], bound_center[2]);

    glPopMatrix();

    draw_bounding_box();

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
        center_y = eye_y  + bound_size[1] * cos(phi);
    else
        phi = tmp;
    center_x = eye_x + bound_size[0] * sin(phi) * cos(theta);
    center_z = eye_z + bound_size[2] * sin(phi) * sin(theta);
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
        eye_x += 0.1 * bound_size[0] * sin(phi) * cos(theta);
        eye_y += 0.1 * bound_size[1] * cos(phi);
        eye_z += 0.1 * bound_size[2] * sin(phi) * sin(theta);
        center_x += 0.1 * bound_size[0] * sin(phi) * cos(theta);
        center_y += 0.1 * bound_size[1] * cos(phi);
        center_z += 0.1 * bound_size[2] * sin(phi) * sin(theta);
    }
    if(key == 's' || key == 'S') //move backward
    {
        eye_x -= 0.1 * bound_size[0] * sin(phi) * cos(theta);
        eye_y -= 0.1 * bound_size[1] * cos(phi);
        eye_z -= 0.1 * bound_size[2] * sin(phi) * sin(theta);
        center_x -= 0.1 * bound_size[0] * sin(phi) * cos(theta);
        center_y -= 0.1 * bound_size[1] * cos(phi);
        center_z -= 0.1 * bound_size[2] * sin(phi) * sin(theta);
     }
    if(key == 'a' || key == 'A') //move left
    {
        eye_x += 0.1 * bound_size[0] * sin(phi) * sin(theta);
        eye_z += -0.1 * bound_size[2] * sin(phi) * cos(theta);
        center_x += 0.1 * bound_size[0] * sin(phi) * sin(theta);
        center_z += -0.1 * bound_size[2] * sin(phi) * cos(theta);
    }
    if(key == 'd' || key == 'D') //move right
    {
        eye_x += -0.1 * bound_size[0] * sin(phi) * sin(theta);
        eye_z += 0.1 * bound_size[2] * sin(phi) * cos(theta);
        center_x += -0.1 * bound_size[0] * sin(phi) * sin(theta);
        center_z += 0.1 * bound_size[2] * sin(phi) * cos(theta);
    }
    if(key == 'r' || key == 'R') // up
    {
        eye_y += 0.1 * bound_size[1];
        center_y += 0.1 * bound_size[1];
    }
    if(key == 'f' || key == 'F') // down
    {
        eye_y -= 0.1 * bound_size[1];
        center_y -= 0.1 * bound_size[1];
    }

    if(key == 'z' || key == 'Z')
    {
        theta = -PI/2, phi = PI / 2;
        eye_x = 0.0, eye_y = 0.0, eye_z = 0.0 + 2.0 * bound_size[2],
        center_x = eye_x + sin(phi) * cos(theta), center_y = eye_y + cos(phi), center_z = 4*sin(phi) * sin(theta);
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
        theta -= 0.1;
        if(theta <= -2 * PI) theta += 2 * PI;
    }
    if(key == GLUT_KEY_RIGHT) // turn right
    {
        theta += 0.1;
        if(theta >= 2 * PI) theta -= 2 * PI;
    }
    center_x = eye_x + bound_size[0] * sin(phi) * cos(theta);
    center_y = eye_y + bound_size[1] * cos(phi);
    center_z = eye_z + bound_size[2] * sin(phi) * sin(theta);
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

void bounding_box()
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
    }

    bound_size[0] = max_x - min_x;
    bound_size[1] = max_y - min_y;
    bound_size[2] = max_z - min_z;

    bound_center[0] = (max_x + min_x)/2.0;
    bound_center[1] = (max_y + min_y)/2.0;
    bound_center[2] = (max_z + min_z)/2.0;

    eye_z = eye_z + 2.0 * bound_size[2];

    light0_pos[0] += 1.25 * bound_size[0];
    light0_pos[1] += 1.25 * bound_size[1];
    light0_pos[2] += 1.25 * bound_size[2];
}

void recount_normal()
{
    point_tri = new vector<int>[myObj->numvertices + 1];

    for(int i = 0 ; i < myObj->numtriangles ; i += 1)
    {
        for(int j = 0 ; j < 3 ; j += 1)
        {
            bool add = true;
            for(int k = 0 ; k < point_tri[myObj->triangles[i].vindices[j]].size() ; k += 1)
            {
                GLfloat *temp = &myObj->facetnorms[3 * point_tri[myObj->triangles[i].vindices[j]][k]];
                if(*temp == myObj->facetnorms[3 * (i+1) + 0] && *(temp + 1) == myObj->facetnorms[3 * (i+1) + 1] && *(temp + 2) == myObj->facetnorms[3 * (i+1) + 2])
                {
                    add = false;
                    break;
                }
            }
            if(add)
                point_tri[myObj->triangles[i].vindices[j]].push_back(i + 1);
        }
    }

    for(int i = 1 ; i <= myObj->numvertices ; i += 1)
    {
        sort(point_tri[i].begin(),point_tri[i].begin() + point_tri[i].size());
    }

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

    }

    myObj_inner->numnormals = myObj_inner->numvertices;
    myObj_inner->normals = new GLfloat[3 * (myObj_inner->numnormals + 1)];
}

void process_inner()
{
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
}

void init()
{
    bounding_box();
    recount_normal();
    process_inner();

//    all_point = new vec3[myObj->numvertices];
//
//    for(int i = 0 ; i < myObj->numvertices ; i += 1)
//    {
//        all_point[i] = vec3(myObj->vertices[3 * (i + 1) + 0],myObj->vertices[3 * (i + 1 ) + 1],myObj->vertices[3 * (i + 1) + 2]);
////        cout << all_point[i][0] << " " << all_point[i][1] << " " << all_point[i][2] << endl;
//    }
}

int main(int argc, char **argv)
{
    //myObj = glmReadOBJ("test_model/sponza.obj");
    myObj = glmReadOBJ("test_model/cube.obj");
    //myObj = glmReadOBJ("test_model/alduin.obj");
    myObj_inner = glmReadOBJ("test_model/cube.obj");

    glmFacetNormals(myObj);

    init();

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

	setShaders();

	glutMainLoop();
    return 0;
}

