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

    //draw_bounding_box();
	draw_best_bounding_box();

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

	computeSimpleBB(myObj->numvertices, myObj->vertices, bound_size, bounding_max, bounding_min, bound_center);

    eye_pos[2] = eye_pos[2] + 2.0 * bound_size[2];

    recount_normal(myObj);
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
    zometable merge_table(MERGE);
}

void test()
{
	//sdf_segment(seg, myObj, model_source);
	
	computeBestFitOBB(myObj->numvertices, myObj->vertices, obb_size, obb_max, obb_min, obb_angle, start_m);
}

int main(int argc, char **argv)
{
//    findzoom();

    myObj = glmReadOBJ(model_source);
//    myObj_inner = glmCopy(myObj);

    init();

	test();

    /*zomedir t;
	voxel start(COLOR_BLUE, SIZE_M, t.color_length(COLOR_BLUE, SIZE_M) / 2.0, bound_center, vec3(obb_angle[0], obb_angle[1], obb_angle[2]));
	for (int i = 0; i < 8; i += 1){
		cout << start.vertex_p[i][0] << " " << start.vertex_p[i][1] << " " << start.vertex_p[i][2] << endl;
	}
	cout << endl;
	for (int i = 0; i < 12; i += 1){
		cout << start.edge_p[i][0] << " " << start.edge_p[i][1] << " " << start.edge_p[i][2] << endl;
	}*/
//    for(int i = 0; i < 6; i += 1)
//        cout << start.plane_d[i] << endl;
//    cout << endl;
//    for(int i = 0; i < 8; i += 1){
//        for(int j = 0; j < start.face_p[i].size(); j += 1)
//            cout << start.face_p[i].at(j) << " ";
//        cout << endl;
//    }
//    cout << endl;
//    for(int i = 0; i < 12; i += 1){
////        for(int j = 0; j < start.face_edge[i].size(); j += 1)
////            cout << start.face_edge[i].at(j) << " ";
//        cout << start.edge_point[i][0] << " " << start.edge_point[i][1];
//        cout << endl;
//    }

	voxelization(myObj, all_voxel, zome_queue, obb_max, obb_min, bound_center, COLOR_BLUE, SIZE_S);

	//glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	//glutInitWindowSize(1000,1000);

 //   glutCreateWindow("Zometool");
	//glutDisplayFunc(display);
	//glutReshapeFunc(myReshape);
	//glutMouseFunc(mouse);
 //   glutMotionFunc(mouseMotion);
 //   glutKeyboardFunc(keyboard);
 //   glutSpecialFunc(special);
	//glEnable(GL_DEPTH_TEST); /* Enable hidden--surface--removal */

	//glewInit();

	//setShaders();

	//glutMainLoop();
	
	system("pause");
    return 0;
}