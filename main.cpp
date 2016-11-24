#include "main.h"

void drawObj(GLMmodel *myObj)
{
//    if (! myObj) return;
//
//    GLMgroup *groups = myObj->groups;
//
//    glBegin(GL_TRIANGLES);
//    while(groups)
//    {
//        //glBindTexture(GL_TEXTURE_2D, textureid[j]);
//        for(int i=0;i<groups->numtriangles;i+=1)
//        {
//            for (int v=0; v<3; v+=1)
//            {
//                //glColor3fv(& myObj->vertices[myObj->triangles[groups->triangles[i]].vindices[v]*3 ]);
//                glNormal3fv(& myObj->normals[myObj->triangles[groups->triangles[i]].nindices[v]*3 ]);
//                //glTexCoord2fv(& myObj->texcoords[myObj->triangles[groups->triangles[i]].tindices[v]*2 ]);
//                glVertex3fv(& myObj->vertices[myObj->triangles[groups->triangles[i]].vindices[v]*3 ]);
//            }
//        }
//        groups=groups->next;
//    }
//    glEnd();

    glBegin(GL_TRIANGLES);
    for (unsigned int i=0; i<myObj->numtriangles; i+=1) {
        for (int v=0; v<3; v+=1) {
		    //glColor3fv( & myObj->vertices[ myObj->triangles[i].vindices[v]*3 ] );
		    //glNormal3fv( & myObj->normals[ myObj->triangles[i].nindices[v]*3 ] );
		    //glColor3f(& myObj->vertices[ myObj->triangles[i].vindices[v]*3 ]);
		    glNormal3fv( & myObj->facetnorms.at(3 * (i + 1)));
		    glVertex3fv( & myObj->vertices.at( myObj->triangles.at(i).vindices[v]*3 ) );
        }
    }
    glEnd();
}

void display(void)
{
    /* display callback, clear frame buffer and z buffer,
        rotate cube and draw, swap buffers */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eye_pos[0], eye_pos[1], eye_pos[2], center[0], center[1], center[2], up[0], up[1], up[2]);

    //lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    light0_pos[0] = bound_size[0] * cos(lightTheta*M_PI/180.0);
    light0_pos[1] = bound_size[1] * 1/sin(M_PI/4) * sin(lightTheta*M_PI/180.0);
    light0_pos[2] = bound_size[2] * cos(lightTheta*M_PI/180.0);

    glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);

//    setShaders();

    glPushMatrix();

    glTranslatef(-bound_center[0], -bound_center[1], -bound_center[2]);

    if(show){
        glPolygonMode(GL_FRONT, GL_LINE);
        glPolygonMode(GL_BACK, GL_LINE);
        drawObj(myObj);
//        glmDraw(myObj, GLM_FLAT);
//        glmDraw(myObj,GLM_SMOOTH);
        //angle += 1.0;
//        glmDraw(myObj,GLM_NONE);
        glPolygonMode(GL_FRONT, GL_FILL);
        glPolygonMode(GL_BACK, GL_FILL);
    }


//    drawObj(myObj_inner);
//    glmDraw(myObj_inner, GLM_FLAT);
//    glmDraw(myObj_inner,GLM_SMOOTH);
//    glmDraw(myObj_inner,GLM_NONE);

//    glTranslatef(bound_center[0], bound_center[1], bound_center[2]);

    glPopMatrix();

//    draw_bounding_box();

    glutSwapBuffers();
    glutPostRedisplay();
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

void test(){
    for(unsigned int i = 0; i < myObj->numtriangles; i += 1){
        vec3 point_dir;
        int dir_count[3] = {0, 0, 0};
        for(int j = 0; j < 3; j += 1){
            vec3 temp(myObj->vertices.at(3 * (myObj->triangles.at(i).vindices[j]) + 0), myObj->vertices.at(3 * (myObj->triangles.at(i).vindices[j]) + 1), myObj->vertices.at(3 * (myObj->triangles.at(i).vindices[j]) + 2));
            point_dir[j] = plane_dir_point(temp, planes.at(0));
            dir_count[(int)point_dir[j] + 1] += 1;
        }
        if(dir_count[(planes.at(0).dir * -1) + 1] != 3){
            face_split_by_plane.push_back(i);
            myObj->triangles.at(i).splite_face_id.push_back(0);
        }
    }

    for(unsigned int i = 1; i < planes.size(); i += 1){
        for(unsigned int j = 0; j < face_split_by_plane.size(); j += 1){
            vec3 point_dir;
            int dir_count[3] = {0, 0, 0};
            for(int k = 0; k < 3; k += 1){
                vec3 temp(myObj->vertices.at(3 * (myObj->triangles.at(face_split_by_plane.at(j)).vindices[k]) + 0), myObj->vertices.at(3 * (myObj->triangles.at(face_split_by_plane.at(j)).vindices[k]) + 1), myObj->vertices.at(3 * (myObj->triangles.at(face_split_by_plane.at(j)).vindices[k]) + 2));
                point_dir[k] = plane_dir_point(temp, planes.at(i));
                dir_count[(int)point_dir[k] + 1] += 1;
            }

            if(dir_count[planes.at(i).dir + 1] == 0 && dir_count[1] == 0){
                if(!myObj->triangles.at(face_split_by_plane.at(j)).splite_face_id.empty()){
                    vector<int> v;
                    myObj->triangles.at(face_split_by_plane.at(j)).splite_face_id.swap(v);
                }
                face_split_by_plane.erase(face_split_by_plane.begin() + j);
                j -= 1;
            }
            else{
                if(i != planes.size() - 1)
                    myObj->triangles.at(face_split_by_plane.at(j)).splite_face_id.push_back(i);
            }
        }
    }
}

void init()
{
    bounding_box();

    eye_pos[2] = eye_pos[2] + 2.0 * bound_size[2];

//    planes.push_back(test_plane1);
    recount_normal(myObj);
    process_inner(myObj, myObj_inner);
    combine_inner_outfit(myObj, myObj_inner);
//    combine_inner_outfit2(myObj);

    collect_edge();

    planes.push_back(test_plane1);
    planes.push_back(test_plane2);
    planes.push_back(test_plane3);
    planes.push_back(test_plane4);
    planes.push_back(test_plane5);

    test();
//    split_face(myObj, all_edge, is_face_split, planes.at(0));
//    collect_edge();
//    split_face(myObj, all_edge, is_face_split, planes.at(1));
//    collect_edge();
//    split_face(myObj, all_edge, is_face_split, planes.at(2));
//    collect_edge();
//    split_face(myObj, all_edge, is_face_split, planes.at(3));
//    collect_edge();
}

int main(int argc, char **argv)
{
    myObj = glmReadOBJ(model_source);
    GLMmodel temp = *myObj;
    myObj_inner = &temp;

//    myObj_inner = glmReadOBJ(model_source);

    init();
//
//	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
//	glutInitWindowSize(1000,1000);
//
//    glutCreateWindow("Zometool");
//	glutDisplayFunc(display);
//	glutReshapeFunc(myReshape);
//	glutMouseFunc(mouse);
//    glutMotionFunc(mouseMotion);
//    glutKeyboardFunc(keyboard);
//    glutSpecialFunc(special);
//	glEnable(GL_DEPTH_TEST); /* Enable hidden--surface--removal */
//
//	glewInit();
//
//	setShaders();
//
//	glutMainLoop();

  return 0;
}

