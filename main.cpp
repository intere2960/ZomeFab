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
		    glVertex3fv( & myObj->vertices->at( myObj->triangles->at(i).vindices[v]*3 ) );
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
        drawObj(myObj_inner);
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

//int convert_2d(Loop &loop){
//    mat3 axis_mat;
//
//    axis_mat[2] = -loop.plane_normal;
//
//    float d = 1.0f / axis_mat[2].length();
//
//    axis_mat[0][0] = axis_mat[2][1] * d;
//    axis_mat[0][1] = -axis_mat[2][0] * d;
//    axis_mat[0][2] = 0.0;
//
//    axis_mat[1][0] = -axis_mat[2][2] * axis_mat[0][1];
//    axis_mat[1][1] = axis_mat[2][2] * axis_mat[0][0];
//    axis_mat[1][2] = axis_mat[2][0] * axis_mat[0][1] - axis_mat[2][1] * axis_mat[0][0];
//
//    axis_mat = axis_mat.transpose();
//
//    for(unsigned int i = 0; i < loop.loop_line.size(); i += 1){
//        vec3 p(temp_piece.vertices->at(3 * loop.loop_line.at(i) + 0), temp_piece.vertices->at(3 * loop.loop_line.at(i) + 1), temp_piece.vertices->at(3 * loop.loop_line.at(i) + 2));
//        float temp_x = axis_mat[0][0] * p[0] + axis_mat[1][0] * p[1] + axis_mat[2][0] * p[2];
//        float temp_y = axis_mat[0][1] * p[0] + axis_mat[1][1] * p[1] + axis_mat[2][1] * p[2];
//        vec2 new_point(temp_x, temp_y);
//        loop.two_d_point.push_back(new_point);
//    }
//}

int span_tri(GLMmodel &temp_piece, int index1, int index2, int index3, int sign_flip)
{
    vec3 p1(temp_piece.vertices->at(3 * index1 + 0), temp_piece.vertices->at(3 * index1 + 1), temp_piece.vertices->at(3 * index1 + 2));
    vec3 p2(temp_piece.vertices->at(3 * index2 + 0), temp_piece.vertices->at(3 * index2 + 1), temp_piece.vertices->at(3 * index2 + 2));
    vec3 p3(temp_piece.vertices->at(3 * index3 + 0), temp_piece.vertices->at(3 * index3 + 1), temp_piece.vertices->at(3 * index3 + 2));

    vec3 vector1 = p3 - p2;
    vec3 vector2 = p1 - p2;

    vec3 cross = vector1 ^ vector2;

    float judge = cross[0] + cross[1] + cross[2];
//    cout << "judge : " << judge << endl;

    if(judge < -0.0001)
        return -1 * sign_flip;
    if(judge > 0.0001)
        return 1 * sign_flip;
    return 0;
}

void judge_inverse(Loop &loop){
    float start_s = loop.sign.at(0);
    float end_s = loop.sign.at(loop.sign.size() - 1);

    bool judge1 = start_s < 0;
    bool judge2 = end_s < 0;
    if(judge1 && judge2){
        loop.sign_flip = -1;
        for(unsigned i = 0; i < loop.sign.size(); i += 1){
            loop.sign.at(i)= loop.sign_flip * loop.sign.at(i);
        }
    }
}

void sign_calc()
{
    for(unsigned int i = 0; i < temp_piece.loop->size(); i += 1){
//    for(unsigned int i = 1; i < 2; i += 1){
//        convert_2d(temp_piece.loop->at(i));
        temp_piece.loop->at(i).sign_flip = 1;
        unsigned int line_size = temp_piece.loop->at(i).loop_line.size();
        for(unsigned int j = 0; j < temp_piece.loop->at(i).loop_line.size(); j += 1){
            int prev = (j - 1 + line_size) % line_size;
            int next = (j + 1 + line_size) % line_size;
            temp_piece.loop->at(i).sign.push_back(span_tri(temp_piece, temp_piece.loop->at(i).loop_line.at(prev), temp_piece.loop->at(i).loop_line.at(j), temp_piece.loop->at(i).loop_line.at(next), temp_piece.loop->at(i).sign_flip));
        }
        if(temp_piece.loop->at(i).sign.size() > 0)
            judge_inverse(temp_piece.loop->at(i));
    }
}

void fill_test()
{
    sign_calc();
}

void init()
{
    bounding_box();

    eye_pos[2] = eye_pos[2] + 2.0 * bound_size[2];

    recount_normal(myObj);
    process_inner(myObj, myObj_inner);
    combine_inner_outfit(myObj, myObj_inner);

    collect_edge(myObj, all_edge);

    planes.push_back(test_plane1);
    planes.push_back(test_plane2);
    planes.push_back(test_plane3);
    planes.push_back(test_plane4);
    planes.push_back(test_plane5); //dir_plane
//    planes.push_back(test_plane6); //dir_plane

    cut_intersection(myObj, planes, face_split_by_plane, false);
    split_face(myObj, all_edge, face_split_by_plane, planes);

    for(unsigned int i = 1; i < myObj->cut_loop->size(); i += 1){
        if(myObj->cut_loop->at(i).align_plane.size() > 1){
            sort(myObj->cut_loop->at(i).align_plane.begin(), myObj->cut_loop->at(i).align_plane.begin() + myObj->cut_loop->at(i).align_plane.size());
            myObj->multi_vertex->push_back(i);
        }
    }

    find_loop(myObj, all_edge, planes);

    process_piece(temp_piece, myObj, face_split_by_plane);
    fill_test();

    for(unsigned int i = 0; i < temp_piece.loop->size(); i += 1){
        cout << i << " : " << endl;
//        cout << "normal " << temp_piece.loop->at(i).plane_normal[0] << " " << temp_piece.loop->at(i).plane_normal[1] << " " << temp_piece.loop->at(i).plane_normal[2] << endl;
        for(unsigned int j = 0; j < temp_piece.loop->at(i).loop_line.size(); j += 1){
            cout << temp_piece.loop->at(i).loop_line.at(j) << "(" << temp_piece.loop->at(i).sign.at(j) << ") ";
        }
        cout << endl;
    }
}

int main(int argc, char **argv)
{
    myObj = glmReadOBJ(model_source);
    myObj_inner = glmCopy(myObj);

//    myObj_inner = glmReadOBJ(model_source) ;

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

