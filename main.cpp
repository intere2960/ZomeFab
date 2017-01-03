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
        drawObj(&temp_piece);
//        drawObj(myObj);
//        drawObj(myObj_inner);
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
 enum point_type { _concave = -1, _tangential = 0, _convex = 1 };

int span_tri(Loop &loop, int index1, int index2, int index3)
{
    vec3 p1 = loop.three_d_point.at(index1);
    vec3 p2 = loop.three_d_point.at(index2);
    vec3 p3 = loop.three_d_point.at(index3);

    vec3 vector1 = p3 - p2;
    vec3 vector2 = p1 - p2;

    vec3 cross = vector1 ^ vector2;

    float judge = (cross[0] + cross[1] + cross[2]) * loop.sign_flip;
//    cout << "judge : " << judge << endl;
    loop.num_concave += 1;

    if(judge < -0.0001)
        return _concave;
    if(judge > 0.0001){
        loop.num_concave -= 1;
        return _convex;
    }
    return _tangential;
}

void judge_inverse(Loop &loop){
    float start_s = loop.sign.at(0);
    float end_s = loop.sign.at(loop.sign.size() - 1);

    bool judge1 = start_s < 0;
    bool judge2 = end_s < 0;
    if(judge1 && judge2){
        loop.sign_flip = -1;
        loop.num_concave = 0;
        for(unsigned i = 0; i < loop.sign.size(); i += 1){
            loop.sign.at(i) = loop.sign_flip * loop.sign.at(i);
            if(loop.sign.at(i) != _convex){
                loop.num_concave += 1;
            }
        }

//        vector<int> temp_l;
//        for(unsigned i = loop.loop_line.size(); i > 0 ; i -= 1){
//            temp_l.push_back(loop.loop_line.at(i - 1));
//        }
////        temp_l.push_back(loop.loop_line.at(0));
//        loop.loop_line.swap(temp_l);
//
//        vector<int> temp_s;
//        for(unsigned i = loop.sign.size(); i > 0 ; i -= 1){
//            temp_s.push_back(loop.sign.at(i - 1));
//        }
////        temp_s.push_back(loop.sign.at(0));
//        loop.sign.swap(temp_s);
    }
}

void fill_prepare(GLMmodel &temp_piece)
{
    for(unsigned int i = 0; i < temp_piece.loop->size(); i += 1){
//    for(unsigned int i = 1; i < 2; i += 1){
//        convert_2d(temp_piece.loop->at(i));
        for(unsigned int j = 0; j < temp_piece.loop->at(i).loop_line.size(); j += 1){
            vec3 p(temp_piece.vertices->at(3 * temp_piece.loop->at(i).loop_line.at(j) + 0), temp_piece.vertices->at(3 * temp_piece.loop->at(i).loop_line.at(j) + 1), temp_piece.vertices->at(3 * temp_piece.loop->at(i).loop_line.at(j) + 2));
//            cout << temp_piece.loop->at(i).loop_line.at(j) << " : " << p[0] << " " << p[1] << " " << p[2] << endl;
            temp_piece.loop->at(i).three_d_point.push_back(p);
        }

        temp_piece.loop->at(i).sign_flip = 1;
        temp_piece.loop->at(i).num_concave = 0;
        unsigned int line_size = temp_piece.loop->at(i).loop_line.size();
        for(unsigned int j = 0; j < temp_piece.loop->at(i).loop_line.size(); j += 1){
            int prev = (j - 1 + line_size) % line_size;
            int next = (j + 1) % line_size;
//            cout << prev << " " << j << " " << next << endl;
            temp_piece.loop->at(i).sign.push_back(span_tri(temp_piece.loop->at(i), prev, j, next));
        }
        if(temp_piece.loop->at(i).sign.size() > 0)
            judge_inverse(temp_piece.loop->at(i));
    }
}

bool check_ear_index(Loop &loop, int current)
{
    if(loop.num_concave == 0)
        return true;

    if(loop.sign.at(current) == _concave)
        return false;

    int prev_index = (current - 1 + loop.loop_line.size()) % loop.loop_line.size();
    int next_index = (current + 1) % loop.loop_line.size();
    int concave_check = 0;

    for(unsigned int i = 0; i < loop.sign.size(); i += 1){
        int index = (current + i + 2) % loop.loop_line.size();
        if(index == prev_index)
            break;

        if(loop.sign.at(index) != _convex){
            if(span_tri(loop, next_index, prev_index, index) != _concave &&
               span_tri(loop, prev_index, current, index) != _concave &&
               span_tri(loop, current, next_index, index) != _concave ){

//                cout << "current F: " << loop.loop_line.at(current) << endl;
                return false;
            }

            concave_check += 1;
            if (concave_check == loop.num_concave) {
                break;
            }
        }
    }

//    cout << "current T: " << loop.loop_line.at(current) << endl;
    return true;
}

int find_ear_index(Loop &loop, int current)
{
    int index = current;
    int total_size = loop.sign.size();
    for(unsigned int i = 0; i < loop.sign.size(); i += 1){
//        cout << "test : " << index << endl;
        if(check_ear_index(loop, index)){
//            cout << "check_ear_index : " << loop.loop_line.at(index) << endl;
            return index;
        }
        index = (index + 1) % total_size;
    }

    index = current;
//    cout << "current : " << loop.loop_line.at(current) << endl;
    for(unsigned int i = 0; i < loop.sign.size(); i += 1){
//        cout << "test : " << loop.sign.at(index) << endl;
        if(loop.sign.at(index) != _concave){
//            cout << "loop.sign.at(index : " << loop.loop_line.at(index) << endl;
            return index;
        }
        index = (index + 1) % total_size;
    }
    return index;
}

void triangulate(Loop &loop)
{
//    cout << "size : " << loop.loop_line.size() << endl;
    int travel_index = 0;
    while(loop.loop_line.size() > 3){
        int ear_index = find_ear_index(loop, travel_index);
//        cout << ear_index << endl;
        int prev_index = (ear_index - 1 + loop.loop_line.size()) % loop.loop_line.size();
        int next_index = (ear_index + 1) % loop.loop_line.size();

        if(loop.sign.at(ear_index) != _convex){
            loop.num_concave -= 1;
        }

        vec3 tri_vindex;
        tri_vindex[0] = loop.loop_line.at(prev_index);
        tri_vindex[1] = loop.loop_line.at(ear_index);
        tri_vindex[2] = loop.loop_line.at(next_index);
        cout << tri_vindex[0] << " " << tri_vindex[1] << " " << tri_vindex[2] << endl;
//        cout << loop.three_d_point.at(prev_index)[0] << " " << loop.three_d_point.at(prev_index)[1] << " " << loop.three_d_point.at(prev_index)[2] << endl;
//        cout << loop.three_d_point.at(ear_index)[0] << " " << loop.three_d_point.at(ear_index)[1] << " " << loop.three_d_point.at(ear_index)[2] << endl;
//        cout << loop.three_d_point.at(next_index)[0] << " " << loop.three_d_point.at(next_index)[1] << " " << loop.three_d_point.at(next_index)[2] << endl;
//        cout << endl;
        loop.tri.push_back(tri_vindex);

        if(loop.sign.at(prev_index) != _convex){
            int prev_prev_index = (ear_index - 2 + loop.loop_line.size()) % loop.loop_line.size();
            loop.sign.at(prev_index) = span_tri(loop, prev_prev_index, prev_index, next_index);
            if(loop.sign.at(prev_index) == _convex){
                loop.num_concave -= 1;
            }
        }

        if(loop.sign.at(next_index) != _convex){
            int next_next_index = (ear_index + 2) % loop.loop_line.size();
            loop.sign.at(next_index) = span_tri(loop, prev_index, next_index, next_next_index);
            if(loop.sign.at(next_index) == _convex){
                loop.num_concave -= 1;
            }
        }
//        cout << "next : " << loop.sign.at(next_index) << endl;

        loop.loop_line.erase(loop.loop_line.begin() + ear_index);
        loop.sign.erase(loop.sign.begin() + ear_index);
        loop.three_d_point.erase(loop.three_d_point.begin() + ear_index);

        travel_index = next_index % loop.loop_line.size();
//        cout << "travel_index : " << loop.loop_line.at(travel_index) << endl;
//        cout << travel_index << endl;
//        cout << endl;
    }
//    cout << "size : " << loop.loop_line.size() << endl;

    if(loop.loop_line.size() == 3) {
        vec3 tri_vindex;
        tri_vindex[0] = loop.loop_line.at(0);
        tri_vindex[1] = loop.loop_line.at(1);
        tri_vindex[2] = loop.loop_line.at(2);
        cout << tri_vindex[0] << " " << tri_vindex[1] << " " << tri_vindex[2] << endl;
//        cout << loop.three_d_point.at(0)[0] << " " << loop.three_d_point.at(0)[0] << " " << loop.three_d_point.at(0)[0] << endl;
//        cout << loop.three_d_point.at(1)[0] << " " << loop.three_d_point.at(1)[1] << " " << loop.three_d_point.at(1)[1] << endl;
//        cout << loop.three_d_point.at(2)[0] << " " << loop.three_d_point.at(2)[2] << " " << loop.three_d_point.at(2)[2] << endl;
        cout << endl;
        loop.tri.push_back(tri_vindex);
//        cout << endl;
	}
}

void fill_test()
{
////    cout << "ear : " << find_ear_index(temp_piece.loop->at(0)) << endl;
    for(unsigned int i = 0; i < temp_piece.loop->size(); i += 1){
//    for(unsigned int i = 3; i < temp_piece.loop->size(); i += 1){
//    for(unsigned int i = 0; i < 1; i += 1){
        cout << "plane " << i << endl;
        triangulate(temp_piece.loop->at(i));
        for(unsigned int j = 0; j < temp_piece.loop->at(i).tri.size(); j += 1){
            vec3 tri_vindex = temp_piece.loop->at(i).tri.at(j);
            GLMtriangle temp;
            temp.vindices[0] = tri_vindex[0];
            temp.vindices[1] = tri_vindex[1];
            temp.vindices[2] = tri_vindex[2];
            temp_piece.triangles->push_back(temp);
            temp_piece.numtriangles += 1;
        }
    }
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

    fill_prepare(temp_piece);

    for(unsigned int i = 0; i < temp_piece.loop->size(); i += 1){
//        cout << i << " : " << endl;
        cout << i << " : " << temp_piece.loop->at(i).num_concave << endl;
//        cout << "normal " << temp_piece.loop->at(i).plane_normal[0] << " " << temp_piece.loop->at(i).plane_normal[1] << " " << temp_piece.loop->at(i).plane_normal[2] << endl;
        for(unsigned int j = 0; j < temp_piece.loop->at(i).loop_line.size(); j += 1){
            cout << temp_piece.loop->at(i).loop_line.at(j) << "(" << temp_piece.loop->at(i).sign.at(j) << ") ";
        }
        cout << endl;
    }

    fill_test();

//    for(unsigned int i = 0; i < temp_piece.loop->size(); i += 1){
//        cout << i << " : " << endl;
////        cout << "normal " << temp_piece.loop->at(i).plane_normal[0] << " " << temp_piece.loop->at(i).plane_normal[1] << " " << temp_piece.loop->at(i).plane_normal[2] << endl;
//        for(unsigned int j = 0; j < temp_piece.loop->at(i).loop_line.size(); j += 1){
//            cout << temp_piece.loop->at(i).loop_line.at(j) << "(" << temp_piece.loop->at(i).sign.at(j) << ") ";
//        }
//        cout << endl;
//    }
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

