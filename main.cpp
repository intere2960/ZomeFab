#include "main.h"

void drawObj(GLMmodel *myObj)
{
    glBegin(GL_TRIANGLES);
    for (unsigned int i=0; i<myObj->numtriangles; i+=1) {
        for (int v=0; v<3; v+=1) {
//		    glColor3fv( & myObj->vertices[ myObj->triangles[i].vindices[v]*3 ] );
//		    glNormal3fv( & myObj->normals[ myObj->triangles[i].nindices[v]*3 ] );
//		    glColor3f(& myObj->vertices[ myObj->triangles[i].vindices[v]*3 ]);
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
//        drawObj(&temp_piece);
        drawObj(myObj);
//        drawObj(myObj_inner);
        glPolygonMode(GL_FRONT, GL_FILL);
        glPolygonMode(GL_BACK, GL_FILL);
    }

//    drawObj(myObj_inner);
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

void init()
{
//    glmRT(myObj, vec3(0.0, 90.0, 0.0), vec3(0.0, 0.0, 0.0));

    bounding_box();

    eye_pos[2] = eye_pos[2] + 2.0 * bound_size[2];

//    recount_normal(myObj);
//    process_inner(myObj, myObj_inner);
//
////    glmRT(myObj_inner, vec3(0.0, 90.0, 0.0), vec3(0.0, 0.0, 500.0));
//
//    combine_inner_outfit(myObj, myObj_inner);
//
//    collect_edge(myObj, all_edge);
//
//    planes.push_back(test_plane1);
//    planes.push_back(test_plane2);
//    planes.push_back(test_plane3);
//    planes.push_back(test_plane4);
//    planes.push_back(test_plane5); //dir_plane
////    planes.push_back(test_plane6); //dir_plane
////    planes.push_back(test_plane7); //dir_plane
////    planes.push_back(test_plane8); //dir_plane
//
//    cut_intersection(myObj, planes, face_split_by_plane, false);
//
//    split_face(myObj, all_edge, face_split_by_plane, planes);
//
//    find_loop(myObj, all_edge, planes);
//
//    process_piece(temp_piece, myObj, face_split_by_plane);
//
//    fill_hole(temp_piece);
}

void findzoom()
{
    zometable splite_table(SPLITE);
    zometable merage_table(MERAGE);
}

vector<voxel> all_voxel;
vec2 ans;

vec2 check_bound(vector<voxel> &all_voxel, vec3 bounding_max, vec3 bounding_min)
{
    vec2 t_ans(-1,-1);
    for(unsigned int i = 0; i < all_voxel.size(); i += 1){
        for(int j = 0; j < 6; j += 1){
            if(all_voxel.at(i).face_toward[j] == -1){
                vec3 temp_p = all_voxel.at(i).position + all_voxel.at(i).toward_vector.at(j) * 3 * all_voxel.at(i).scale;
                cout << temp_p[0] << " " << temp_p[1] << " " << temp_p[2] << endl;
                cout << j % 3 << " " << bounding_min[j % 3] << " " << bounding_max[j % 3] << endl;
                if((temp_p[j % 3] > bounding_min[j % 3]) && (temp_p[j % 3] < bounding_max[j % 3])){
                    t_ans[0] = i;
                    t_ans[1] = j;
                    return t_ans;
                }
            }
        }
    }
    return t_ans;
}

void voxelization()
{
    zomedir t;
    voxel start(BLUE, SIZE_M, t.color_length(BLUE, SIZE_M) / 2.0);
//    GLMmodel *x = glmReadOBJ("test_model/cube.obj");

    all_voxel.push_back(start);

    ans = check_bound(all_voxel, bounding_max, bounding_min);
    cout << ans[0] << " " << ans[1] << endl;

    int asdf = 1;
    while(ans[0] != -1 && ans[1] != -1 && asdf < 6){
        all_voxel.at(ans[0]).face_toward[(int)ans[1]] = ans[0] + 1;
        vec3 new_p = all_voxel.at(ans[0]).position + all_voxel.at(ans[0]).toward_vector[(int)ans[1]] * all_voxel.at(ans[0]).scale * 2;
        voxel temp(all_voxel.at(ans[0]), new_p, all_voxel.at(ans[0]).rotation);
        int opposite_face = 1;
        if((int)ans[0] % 2 == 1)
            opposite_face = -1;
        temp.face_toward[(int)ans[1] + opposite_face] = ans[1];
        all_voxel.push_back(temp);
        ans = check_bound(all_voxel, bounding_max, bounding_min);
        cout << asdf << endl;
        asdf += 1;
    }

    for(int i = 0; i < all_voxel.size(); i += 1){
        for(int j = 0; j < 6; j += 1){
            cout << all_voxel.at(i).face_toward[j] << " ";
        }
        cout << endl;
    }

//    vec3 xxx = start.position + start.toward_vector[0] * start.scale * 2;
//    voxel test2(start, xxx, start.rotation);
//    GLMmodel *x = glmReadOBJ("test_model/cube.obj");
//    GLMmodel *x1 = glmCopy(x);
//    glmScale(x, start.scale);
//    glmScale(x1, test2.scale);
//    glmRT(x1, test2.rotation, test2.position);
//    glmCombine(x, x1);
//    glmWriteOBJ(x, "123.obj", GLM_NONE);
}

int main(int argc, char **argv)
{
//    findzoom();

    myObj = glmReadOBJ(model_source);
//    myObj_inner = glmCopy(myObj);
//
    init();

    voxelization();

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

