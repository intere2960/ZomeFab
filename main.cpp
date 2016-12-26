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
//		    glNormal3fv( & myObj->facetnorms.at(3 * (i + 1)));
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

    if(show_piece)
        drawObj(&temp_piece);

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

vector<edge> all_piece_edge;
void fill_test()
{
//    for(unsigned int i = 0; i < planes.size(); i += 1){
//        for(unsigned int j = 0; j < temp_piece.numvertices; j += 1){
//            vec3 temp(temp_piece.vertices.at(3 * (j + 1) + 0), temp_piece.vertices.at(3 * (j + 1) + 1), temp_piece.vertices.at(3 * (j + 1) + 2));
//    //        cout << plane_dir_point(temp, planes.at(i)) << endl;
//            if(plane_dir_point(temp, planes.at(i)) == 0){
//                cout << j << " : " << temp[0] << " " << temp[1] << " " << temp[2] << endl;
//            }
//        }
//        cout << endl;
//    }

//    collect_edge(&temp_piece, all_piece_edge);
//
//    bool use_vertice[temp_piece.numvertices], use_face[temp_piece.numtriangles];
//
//    for(unsigned int i = 1; i <= temp_piece.numvertices; i += 1){
//        vec3 temp(temp_piece.vertices.at(3 * i + 0), temp_piece.vertices.at(3 * i + 1), temp_piece.vertices.at(3 * i + 2));
//        cout << i << " : " << temp[0] << " " << temp[1] << " " << temp[2] << endl;
//        use_vertice[i] = false;
//    }
//    cout << endl;
//
//    for(unsigned int i = 0; i < temp_piece.numtriangles; i += 1){
//        if(split_edge(&temp_piece, all_piece_edge, i, planes.at(0)) == _align)
//            cout << "tri " << i << " : " << temp_piece.triangles.at(i).vindices[0] << " " << temp_piece.triangles.at(i).vindices[1] << " " << temp_piece.triangles.at(i).vindices[2] << endl;
//        use_face[i] = false;
//    }
//    cout << endl;
//
//    for(unsigned int i = 0; i < all_piece_edge.size(); i += 1){
//        cout << all_piece_edge.at(i).index[0] << " " << all_piece_edge.at(i).index[1] << " face : " << all_piece_edge.at(i).face_id[0] << " " << all_piece_edge.at(i).face_id[1] << endl;
//    }
//    cout << endl;
//    int start_index, start_edge;
//    for(unsigned int i = 0; i < all_piece_edge.size(); i += 1){
//        if(all_piece_edge.at(i).face_id[1] == -1){
////            cout << "test : " << all_piece_edge.at(i).index[0] << " " << all_piece_edge.at(i).index[1] << endl;
//            vec3 temp1(temp_piece.vertices.at(3 * all_piece_edge.at(i).index[0] + 0), temp_piece.vertices.at(3 * all_piece_edge.at(i).index[0] + 1), temp_piece.vertices.at(3 * all_piece_edge.at(i).index[0] + 2));
//            vec3 temp2(temp_piece.vertices.at(3 * all_piece_edge.at(i).index[1] + 0), temp_piece.vertices.at(3 * all_piece_edge.at(i).index[1] + 1), temp_piece.vertices.at(3 * all_piece_edge.at(i).index[1] + 2));
//            int judge1 = plane_dir_point(temp1, planes.at(0));
//            int judge2 = plane_dir_point(temp2, planes.at(0));
//            if(judge1 ^ judge2){
////                cout << all_piece_edge.at(i).index[0] << " " << all_piece_edge.at(i).index[1] << " face : " << all_piece_edge.at(i).face_id[0] << " " << all_piece_edge.at(i).face_id[1] << endl;
//
//                int plane_id = 0 - 1;
//                if(plane_id < 0)
//                    plane_id = plane_id + planes.size() - 1;
//
//                if(judge1 == 0){
//                    if(plane_dir_point(temp1, planes.at(plane_id)) == 0){
////                        cout << all_piece_edge.at(i).index[0] << endl;
//                        start_index = all_piece_edge.at(i).index[0];
//                        start_edge = i;
//                        break;
//                    }
//                }
//                if(judge2 == 0){
//                    if(plane_dir_point(temp2, planes.at(plane_id)) == 0){
////                        cout << all_piece_edge.at(i).index[1] << endl;
//                        start_index = all_piece_edge.at(i).index[1];
//                        start_edge = i;
//                        break;
//                    }
//                }
//            }
//        }
//    }
//    cout << endl;
//    cout << start_index << " , edge: " << start_edge << endl;
//
//    int current_face = all_piece_edge.at(start_edge).face_id[0];
//    use_face[current_face] = true;
////    cout << current_face;
//    vector<int> face_loop;
//    face_loop.push_back(start_index);
}

void init()
{
    bounding_box();

    eye_pos[2] = eye_pos[2] + 2.0 * bound_size[2];

    recount_normal(myObj);
//    process_inner(myObj, myObj_inner);
//    combine_inner_outfit(myObj, myObj_inner);

    collect_edge(myObj, all_edge);

    planes.push_back(test_plane1);
    planes.push_back(test_plane2);
    planes.push_back(test_plane3);
    planes.push_back(test_plane4);
    planes.push_back(test_plane5); //dir_plane
//    planes.push_back(test_plane6); //dir_plane

    cut_intersection(myObj,planes, face_split_by_plane, true);
    split_face(myObj, all_edge, face_split_by_plane, planes);

    for(unsigned int i = 1; i < myObj->cut_loop.size(); i += 1){
 //        cout << myObj->cut_loop.at(i).size() << endl;
         if(myObj->cut_loop.at(i).align_plane.size() > 1){
             cout << i << " : ";
             for(unsigned int j = 0; j < myObj->cut_loop.at(i).connect_edge.size(); j += 1){
                 cout << myObj->cut_loop.at(i).connect_edge.at(j) << " ";
             }
             cout << endl;
             cout << "\tcut plane : ";
             for(unsigned int j = 0; j < myObj->cut_loop.at(i).align_plane.size(); j += 1){
                 cout << myObj->cut_loop.at(i).align_plane.at(j) << " ";
             }
             cout << endl;
         }
     }

    process_piece(temp_piece, myObj, face_split_by_plane);
//    fill_test();
}

int main(int argc, char **argv)
{
    myObj = glmReadOBJ(model_source);
    GLMmodel temp = *myObj;
    myObj_inner = &temp;

//    myObj_inner = glmReadOBJ(model_source);

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

