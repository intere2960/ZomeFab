#include "main.h"
#include <omp.h>

void drawObj(GLMmodel *myObj)
{
	glBegin(GL_TRIANGLES);
	for (unsigned int i = 0; i<myObj->numtriangles; i += 1) {
		for (int v = 0; v<3; v += 1) {
			//		    glColor3fv( & myObj->vertices[ myObj->triangles[i].vindices[v]*3 ] );
			//		    glNormal3fv( & myObj->normals[ myObj->triangles[i].nindices[v]*3 ] );
			//		    glColor3f(& myObj->vertices[ myObj->triangles[i].vindices[v]*3 ]);
			//		    glNormal3fv( & myObj->facetnorms.at(3 * (i + 1)));
			glVertex3fv(&myObj->vertices->at(myObj->triangles->at(i).vindices[v] * 3));
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

	light0_pos[0] = bound_size[0] * cos(lightTheta*M_PI / 180.0f);
	light0_pos[1] = bound_size[1] * 1.0f / sin(M_PI / 4.0f) * sin(lightTheta*M_PI / 180.0f);
	light0_pos[2] = bound_size[2] * cos(lightTheta*M_PI / 180.0f);

	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);

	//    setShaders();

	glPushMatrix();

	glTranslatef(-bound_center[0], -bound_center[1], -bound_center[2]);

	if (show){
		glPolygonMode(GL_FRONT, GL_LINE);
		glPolygonMode(GL_BACK, GL_LINE);
		//        drawObj(&temp_piece);
		drawObj(myObj);
		//        drawObj(myObj_inner);
		glPolygonMode(GL_FRONT, GL_FILL);
		glPolygonMode(GL_BACK, GL_FILL);
	}

	//    drawObj(myObj_inner);
	if (show_piece)
		drawObj(&temp_piece);

	glPopMatrix();

	//draw_bounding_box();
	//draw_best_bounding_box();

	glutSwapBuffers();
	glutPostRedisplay();
}

void myReshape(int w, int h)
{
	width = w;
	height = h;
	float ratio = w * 1.0f / h;
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

	eye_pos[2] = eye_pos[2] + 2.0f * bound_size[2];

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
	//    fill_hole(temp_piece, true);
}

void findzoom()
{
	zometable splite_table(SPLITE);
	zometable merge_table(MERGE);
}

void test()
{
	cout << "sdf segment" << endl;
	sdf_segment(seg, myObj, model_source);

	all_voxel.resize(seg.size());
	zome_queue.resize(seg.size());

	obb_center.resize(seg.size());
	obb_max.resize(seg.size());
	obb_min.resize(seg.size());
	obb_size.resize(seg.size());
	obb_angle.resize(seg.size());

	cout << "generate piece" << endl;
	#pragma omp parallel for
	for (int i = 0; i < seg.size(); i += 1){
		if (seg.size() > 1){
			vector<edge> fill_edge;
			collect_edge(&seg.at(i), fill_edge);
			inform_vertex(&seg.at(i), fill_edge);

			std::vector<int> single_use;
			for (unsigned int j = 0; j < fill_edge.size(); j += 1){
				if (fill_edge.at(j).face_id[1] == -1){
					if ((unsigned)(find(single_use.begin(), single_use.end(), fill_edge.at(j).index[0]) - single_use.begin()) >= single_use.size()){
						single_use.push_back(fill_edge.at(j).index[0]);
					}
					if ((unsigned)(find(single_use.begin(), single_use.end(), fill_edge.at(j).index[1]) - single_use.begin()) >= single_use.size()){
						single_use.push_back(fill_edge.at(j).index[1]);
					}
				}
			}

			find_easy_loop(&seg.at(i), fill_edge, single_use);
			fill_hole(seg.at(i), false);
		}

		std::string s = "test";
		std::string piece = s + std::to_string(i) + ".obj";
		glmWriteOBJ(&seg.at(i), my_strdup(piece.c_str()), GLM_NONE);


		cout << "piece " << i + 1 << " :" << endl;
		zome_queue.at(i).resize(4);
		//computeBestFitOBB(seg.at(i).numvertices, seg.at(i).vertices, obb_size.at(i), obb_max.at(i), obb_min.at(i), obb_center.at(i), obb_angle.at(i), start_m);
		computeSimpleBB(seg.at(i).numvertices, seg.at(i).vertices, obb_size.at(i), obb_max.at(i), obb_min.at(i), obb_center.at(i));
		//voxelization(&seg.at(i), all_voxel.at(i), zome_queue.at(i), obb_max.at(i), obb_min.at(i), obb_center.at(i), obb_angle.at(i), COLOR_BLUE, SIZE_S);
		voxelization(&seg.at(i), all_voxel.at(i), zome_queue.at(i), obb_max.at(i), obb_min.at(i), obb_center.at(i), vec3(0.0, 0.0, 0.0), COLOR_BLUE, SIZE_S);
		//voxelization(&seg.at(i), all_voxel.at(i), zome_queue.at(i), obb_max.at(i), obb_min.at(i), bound_center, vec3(0.0, 0.0, 0.0), COLOR_BLUE, SIZE_S);
		cout << "output piece " << i + 1 << endl;
		output_voxel(all_voxel.at(i), i);
		output_zometool(all_voxel.at(i), zome_queue.at(i), i);

		//for (int j = 0; j < 4; j += 1){
		//	cout << j << " : " << endl;
		//	if (j != 3){
		//		/*for (int k = 0; k < zome_queue.at(i).at(j).size(); k += 1){
		//			cout << "\t " << k << " : (" << zome_queue.at(i).at(j).at(k).fromindex[0] << " , " << zome_queue.at(i).at(j).at(k).fromindex[1]
		//			<< ") (" << zome_queue.at(i).at(j).at(k).towardindex[0] << " , " << zome_queue.at(i).at(j).at(k).towardindex[1] << ")" << endl;
		//		}*/
		//	}
		//	else{
		//		for (int k = 0; k < zome_queue.at(i).at(3).size(); k += 1){
		//			if (zome_queue.at(i).at(3).at(k).connect_stick.size() != 6){
		//				cout << "\t " << k << " : ";
		//				for (int a = 0; a < zome_queue.at(i).at(3).at(k).connect_stick.size(); a += 1){
		//					cout << " (" << zome_queue.at(i).at(3).at(k).connect_stick.at(a)[0] << " , " << zome_queue.at(i).at(3).at(k).connect_stick.at(a)[1] << ")";
		//				}
		//				cout << endl;
		//				cout << "\t " << zome_queue.at(i).at(3).at(k).position[0] << " " << zome_queue.at(i).at(3).at(k).position[1] << " " << zome_queue.at(i).at(3).at(k).position[2] << endl;
		//			}
		//		}
		//	}
		//}
	}
}

vector<vector<zomeconn>> test_connect(4);
vec3 delta;

void fake()
{
	zomedir t;

	vec3 p1(26.576f, 120.994f, 75.2333f);
	//vec3 p2 = p1 + t.dir->at(2) * t.color_length(COLOR_BLUE, SIZE_M) + t.dir->at(30) * t.color_length(COLOR_RED, SIZE_S) + t.dir->at(34) * t.color_length(COLOR_RED, SIZE_S) + t.dir->at(54) * t.color_length(COLOR_YELLOW, SIZE_S) + t.dir->at(35) * t.color_length(COLOR_RED, SIZE_M);
	//vec3 p2 = p1 + t.dir->at(2) * t.color_length(COLOR_BLUE, SIZE_S); 
	//cout << p2[0] << " " << p2[1] << " " << p2[2] << endl;
	vec3 temp1(61.6096f, 285.876f, 69.4521f);
	vec3 temp2(64.8425f, 288.677f, 69.6503f);
	delta = temp2 - temp1;
	vec3 p2(61.6096f, 285.876f, -25.1479f);
	//vec3 p2(61.6096f, 238.576f, -25.1479f);
	cout << delta[0] << " " << delta[1] << " " << delta[2] << endl;
	p2 += delta;

	vec3 asd = p2 - p1;
	int near_d = t.find_near_dir(asd);
	int best_s, best_i;
	cout << asd.length() << endl;
	cout << near_d << endl;
	t.find_best_zome(p1, p2, near_d, best_s, best_i);
	cout << best_s << " " << best_i << endl;

	vector<vec3> record;
	record.push_back(p1);
	connect_points_optimize(p1, p2, record);

	t.find_best_zome(p1, record.at(0), near_d, best_s, best_i);
	
	cout << record.at(0)[0] << " " << record.at(0)[1] << " " << record.at(0)[2] << endl;
	cout << best_i << " " << best_s << endl;

	zomeconn start_ball;
	start_ball.color = COLOR_WHITE;
	start_ball.position = record.at(0);
	test_connect.at(COLOR_WHITE).push_back(start_ball);

	zomeconn start_stick;
	start_stick.color = t.face_color(best_i);
	start_stick.position = (record.at(0) + p1) / 2;
	start_stick.fromface = best_i;
	start_stick.towardface = t.opposite_face(best_i);
	start_stick.size = best_s;
	test_connect.at(t.face_color(best_i)).push_back(start_stick);


	test_connect.at(COLOR_WHITE).at(0).connect_stick.push_back(vec2(t.face_color(best_i), test_connect.at(t.face_color(best_i)).size() - 1));
	test_connect.at(t.face_color(best_i)).at(0).towardindex = vec2(COLOR_WHITE, 0);


	for (unsigned int i = 1; i < record.size(); i += 1){
		if (i < record.size() - 1){
			zomeconn temp_ball;
			temp_ball.color = COLOR_WHITE;
			temp_ball.position = record.at(i);
			test_connect.at(COLOR_WHITE).push_back(temp_ball);
		}

		cout << record.at(i)[0] << " " << record.at(i)[1] << " " << record.at(i)[2] << endl;
		t.find_best_zome(record.at(i - 1), record.at(i), near_d, best_s, best_i);
		cout << best_i << " " << best_s << endl;

		zomeconn temp_stick;
		temp_stick.color = t.face_color(best_i);
		temp_stick.position = (record.at(i) + record.at(i - 1)) / 2;
		temp_stick.fromface = best_i;
		temp_stick.towardface = t.opposite_face(best_i);
		temp_stick.size = best_s;
		if (i < record.size() - 1)
			temp_stick.towardindex = vec2(COLOR_WHITE, i);
		temp_stick.fromindex = vec2(COLOR_WHITE, i - 1);
		test_connect.at(t.face_color(best_i)).push_back(temp_stick);

		test_connect.at(COLOR_WHITE).at(i - 1).connect_stick.push_back(vec2(t.face_color(best_i), test_connect.at(t.face_color(best_i)).size() - 1));
		if (i < record.size() - 1)
			test_connect.at(COLOR_WHITE).at(i).connect_stick.push_back(vec2(t.face_color(best_i), test_connect.at(t.face_color(best_i)).size() - 1));
	}

	//for (int j = 0; j < 4; j += 1){
	//	cout << j << " : " << endl;
	//	if (j != 3){
	//		for (int k = 0; k < test_connect.at(j).size(); k += 1){
	//			cout << "\t " << k << " : (" << test_connect.at(j).at(k).fromindex[0] << " , " << test_connect.at(j).at(k).fromindex[1]
	//				<< ") (" << test_connect.at(j).at(k).towardindex[0] << " , " << test_connect.at(j).at(k).towardindex[1] << ")" << endl;
	//		}
	//	}
	//	else{
	//		for (int k = 0; k < test_connect.at(3).size(); k += 1){
	//			if (test_connect.at(3).at(k).connect_stick.size() != 6){
	//				cout << "\t " << k << " : ";
	//				for (int a = 0; a < test_connect.at(3).at(k).connect_stick.size(); a += 1){
	//					cout << " (" << test_connect.at(3).at(k).connect_stick.at(a)[0] << " , " << test_connect.at(3).at(k).connect_stick.at(a)[1] << ")";
	//				}
	//				cout << endl;
	//				//cout << "\t " << test_connect.at(3).at(k).position[0] << " " << test_connect.at(3).at(k).position[1] << " " << test_connect.at(3).at(k).position[2] << endl;
	//			}
	//		}
	//	}
	//}
	
	GLMmodel *zome = glmReadOBJ("test_model/zometool/zomeball.obj");
	for (unsigned int i = 0; i < test_connect.at(3).size(); i += 1){
		if (i == 0){
			glmR(zome, vec3(0.0, 0.0, 0.0));
			glmRT(zome, vec3(0.0, 0.0, 0.0), test_connect.at(3).at(i).position);
		}
		else{
			GLMmodel *new_ball = glmReadOBJ("test_model/zometool/zomeball.obj");
			glmR(new_ball, vec3(0.0, 0.0, 0.0));
			glmRT(new_ball, vec3(0.0, 0.0, 0.0), test_connect.at(3).at(i).position);
			glmCombine(zome, new_ball);
		}
	}

	for (unsigned int i = 0; i < test_connect.size() - 1; i += 1){
		for (unsigned int j = 0; j < test_connect.at(i).size(); j += 1){
			GLMmodel *new_stick = NULL;
			if (i == COLOR_BLUE){
				if (test_connect.at(i).at(j).size == SIZE_S)
					new_stick = glmReadOBJ("test_model/zometool/BlueS.obj");
				if (test_connect.at(i).at(j).size == SIZE_M)
					new_stick = glmReadOBJ("test_model/zometool/BlueM.obj");
				if (test_connect.at(i).at(j).size == SIZE_L)
					new_stick = glmReadOBJ("test_model/zometool/BlueL.obj");
			}
			else if (i == COLOR_RED){
				if (test_connect.at(i).at(j).size == SIZE_S)
					new_stick = glmReadOBJ("test_model/zometool/RedS.obj");
				if (test_connect.at(i).at(j).size == SIZE_M)
					new_stick = glmReadOBJ("test_model/zometool/RedM.obj");
				if (test_connect.at(i).at(j).size == SIZE_L)
					new_stick = glmReadOBJ("test_model/zometool/RedL.obj");
			}
			else if (i == COLOR_YELLOW){
				if (test_connect.at(i).at(j).size == SIZE_S)
					new_stick = glmReadOBJ("test_model/zometool/YellowS.obj");
				if (test_connect.at(i).at(j).size == SIZE_M)
					new_stick = glmReadOBJ("test_model/zometool/YellowM.obj");
				if (test_connect.at(i).at(j).size == SIZE_L)
					new_stick = glmReadOBJ("test_model/zometool/YellowL.obj");
			}
			glmRT(new_stick, vec3(0.0, t.roll(test_connect.at(i).at(j).fromface), 0.0), vec3(0.0, 0.0, 0.0));
			glmRT(new_stick, vec3(0.0, t.phi(test_connect.at(i).at(j).fromface), t.theta(test_connect.at(i).at(j).fromface)), vec3(0.0, 0.0, 0.0));
			glmR(new_stick, vec3(0.0, 0.0, 0.0));
			glmRT(new_stick, vec3(0.0, 0.0, 0.0), test_connect.at(i).at(j).position);
			glmCombine(zome, new_stick);
		}
	}

	glmWriteOBJ(zome, "test123.obj", GLM_NONE);
}

void inital_energy()
{
	zomedir t;

	for (unsigned int i = 0; i < myObj->triangles->size(); i += 1){
		vec3 p1(myObj->vertices->at(3 * myObj->triangles->at(i).vindices[0] + 0), myObj->vertices->at(3 * myObj->triangles->at(i).vindices[0] + 1), myObj->vertices->at(3 * myObj->triangles->at(i).vindices[0] + 2));
		vec3 p2(myObj->vertices->at(3 * myObj->triangles->at(i).vindices[1] + 0), myObj->vertices->at(3 * myObj->triangles->at(i).vindices[1] + 1), myObj->vertices->at(3 * myObj->triangles->at(i).vindices[1] + 2));
		vec3 p3(myObj->vertices->at(3 * myObj->triangles->at(i).vindices[2] + 0), myObj->vertices->at(3 * myObj->triangles->at(i).vindices[2] + 1), myObj->vertices->at(3 * myObj->triangles->at(i).vindices[2] + 2));
		vec3 v1 = p1 - p2;
		vec3 v2 = p3 - p2;
		vec3 n = v1 ^ v2;
		float d = n * p1;

		for (unsigned int j = 0; j < test_connect.at(COLOR_WHITE).size(); j += 1){
			vec3 ball_n;
			for (unsigned int k = 0; k < test_connect.at(COLOR_WHITE).at(j).connect_stick.size(); k += 1){
				int f_index = test_connect.at(test_connect.at(COLOR_WHITE).at(j).connect_stick.at(k)[0]).at(test_connect.at(COLOR_WHITE).at(j).connect_stick.at(k)[1]).fromface;
				ball_n += t.dir->at(f_index);
			}
			ball_n = ball_n.normalize();

			vec3 edge1 = p2 - p1;
			vec3 edge2 = p3 - p2;
			vec3 edge3 = p1 - p3;

			float t = (d - (test_connect.at(COLOR_WHITE).at(j).position * n)) / (n * ball_n);
			vec3 insect_p = test_connect.at(COLOR_WHITE).at(j).position + ball_n * t;

			vec3 judge1 = insect_p - p1;
			vec3 judge2 = insect_p - p2;
			vec3 judge3 = insect_p - p3;

			if ((edge1 * judge1 > 0) && (edge2 * judge2 > 0) && (edge3 * judge3 > 0)){
				if ((insect_p - test_connect.at(COLOR_WHITE).at(j).position).length() < test_connect.at(COLOR_WHITE).at(j).surface_d){
					test_connect.at(COLOR_WHITE).at(j).surface_d = (insect_p - test_connect.at(COLOR_WHITE).at(j).position).length();
				}
			}

			ball_n *= -1;
			t = (d - (test_connect.at(COLOR_WHITE).at(j).position * n)) / (n * ball_n);
			insect_p = test_connect.at(COLOR_WHITE).at(j).position + ball_n * t;

			judge1 = insect_p - p1;
			judge2 = insect_p - p2;
			judge3 = insect_p - p3;

			if ((edge1 * judge1 > 0) && (edge2 * judge2 > 0) && (edge3 * judge3 > 0)){
				if ((insect_p - test_connect.at(COLOR_WHITE).at(j).position).length() < test_connect.at(COLOR_WHITE).at(j).surface_d){
					test_connect.at(COLOR_WHITE).at(j).surface_d = (insect_p - test_connect.at(COLOR_WHITE).at(j).position).length();
				}
			}
		}
	}

	/*for (unsigned int i = 0; i < test_connect.at(COLOR_WHITE).size(); i += 1){
	cout << test_connect.at(COLOR_WHITE).at(i).surface_d << endl;
	}*/
}

int main(int argc, char **argv)
{
	//    findzoom();

	myObj = glmReadOBJ(model_source);
	//myObj_inner = glmCopy(myObj);

	init();

	/*test();

	fake();
		
	for (int j = 0; j < 4; j += 1){
		for (int k = 0; k < zome_queue.at(2).at(j).size(); k += 1){
			zome_queue.at(2).at(j).at(k).position += delta;
		}
	}

	combine_zome_ztruc(zome_queue.at(1), zome_queue.at(2));
	combine_zome_ztruc(zome_queue.at(1), test_connect);

	for (int j = 0; j < 4; j += 1){
		cout << j << " : " << endl;
		if (j != 3){
			for (int k = 0; k < zome_queue.at(1).at(j).size(); k += 1){
				cout << "\t " << k << " : (" << zome_queue.at(1).at(j).at(k).fromindex[0] << " , " << zome_queue.at(1).at(j).at(k).fromindex[1]
				<< ") (" << zome_queue.at(1).at(j).at(k).towardindex[0] << " , " << zome_queue.at(1).at(j).at(k).towardindex[1] << ")" << endl;
			}
		}
		else{
			for (int k = 0; k < zome_queue.at(1).at(3).size(); k += 1){
				if (zome_queue.at(1).at(3).at(k).connect_stick.size() != 6){
					cout << "\t " << k << " : ";
					for (int a = 0; a < zome_queue.at(1).at(3).at(k).connect_stick.size(); a += 1){
						cout << " (" << zome_queue.at(1).at(3).at(k).connect_stick.at(a)[0] << " , " << zome_queue.at(1).at(3).at(k).connect_stick.at(a)[1] << ")";
					}
					cout << endl;
					cout << "\t " << zome_queue.at(1).at(3).at(k).position[0] << " " << zome_queue.at(1).at(3).at(k).position[1] << " " << zome_queue.at(1).at(3).at(k).position[2] << endl;
				}
			}
		}
	}
	
	zomedir t;
	GLMmodel *zome = glmReadOBJ("test_model/zometool/zomeball.obj");
	for (unsigned int i = 0; i < zome_queue.at(1).at(3).size(); i += 1){
		if (i == 0){
			glmR(zome, vec3(0.0, 0.0, 0.0));
			glmRT(zome, vec3(0.0, 0.0, 0.0), zome_queue.at(1).at(3).at(i).position);
		}
		else{
			GLMmodel *new_ball = glmReadOBJ("test_model/zometool/zomeball.obj");
			glmR(new_ball, vec3(0.0, 0.0, 0.0));
			glmRT(new_ball, vec3(0.0, 0.0, 0.0), zome_queue.at(1).at(3).at(i).position);
			glmCombine(zome, new_ball);
		}
	}

	for (unsigned int i = 0; i < zome_queue.at(1).size() - 1; i += 1){
		for (unsigned int j = 0; j < zome_queue.at(1).at(i).size(); j += 1){
			GLMmodel *new_stick = NULL;
			if (i == COLOR_BLUE){
				if (zome_queue.at(1).at(i).at(j).size == SIZE_S)
					new_stick = glmReadOBJ("test_model/zometool/BlueS.obj");
				if (zome_queue.at(1).at(i).at(j).size == SIZE_M)
					new_stick = glmReadOBJ("test_model/zometool/BlueM.obj");
				if (zome_queue.at(1).at(i).at(j).size == SIZE_L)
					new_stick = glmReadOBJ("test_model/zometool/BlueL.obj");
			}
			else if (i == COLOR_RED){
				if (zome_queue.at(1).at(i).at(j).size == SIZE_S)
					new_stick = glmReadOBJ("test_model/zometool/RedS.obj");
				if (zome_queue.at(1).at(i).at(j).size == SIZE_M)
					new_stick = glmReadOBJ("test_model/zometool/RedM.obj");
				if (zome_queue.at(1).at(i).at(j).size == SIZE_L)
					new_stick = glmReadOBJ("test_model/zometool/RedL.obj");
			}
			else if (i == COLOR_YELLOW){
				if (zome_queue.at(1).at(i).at(j).size == SIZE_S)
					new_stick = glmReadOBJ("test_model/zometool/YellowS.obj");
				if (zome_queue.at(1).at(i).at(j).size == SIZE_M)
					new_stick = glmReadOBJ("test_model/zometool/YellowM.obj");
				if (zome_queue.at(1).at(i).at(j).size == SIZE_L)
					new_stick = glmReadOBJ("test_model/zometool/YellowL.obj");
			}
			glmRT(new_stick, vec3(0.0, t.roll(zome_queue.at(1).at(i).at(j).fromface), 0.0), vec3(0.0, 0.0, 0.0));
			glmRT(new_stick, vec3(0.0, t.phi(zome_queue.at(1).at(i).at(j).fromface), t.theta(zome_queue.at(1).at(i).at(j).fromface)), vec3(0.0, 0.0, 0.0));
			glmR(new_stick, vec3(0.0, 0.0, 0.0));
			glmRT(new_stick, vec3(0.0, 0.0, 0.0), zome_queue.at(1).at(i).at(j).position);
			glmCombine(zome, new_stick);
		}
	}

	glmWriteOBJ(zome, "test.obj", GLM_NONE);	

	output_struc(zome_queue.at(1));*/

	struc_parser(test_connect);
	
	inital_energy();

	/*zomedir t;
	GLMmodel *zome = glmReadOBJ("test_model/zometool/zomeball.obj");
	for (unsigned int i = 0; i < test_connect.at(3).size(); i += 1){
		if (i == 0){
			glmR(zome, vec3(0.0, 0.0, 0.0));
			glmRT(zome, vec3(0.0, 0.0, 0.0), test_connect.at(3).at(i).position);
		}
		else{
			GLMmodel *new_ball = glmReadOBJ("test_model/zometool/zomeball.obj");
			glmR(new_ball, vec3(0.0, 0.0, 0.0));
			glmRT(new_ball, vec3(0.0, 0.0, 0.0), test_connect.at(3).at(i).position);
			glmCombine(zome, new_ball);
		}
	}

	for (unsigned int i = 0; i < test_connect.size() - 1; i += 1){
		for (unsigned int j = 0; j < test_connect.at(i).size(); j += 1){
			GLMmodel *new_stick = NULL;
			if (i == COLOR_BLUE){
				if (test_connect.at(i).at(j).size == SIZE_S)
					new_stick = glmReadOBJ("test_model/zometool/BlueS.obj");
				if (test_connect.at(i).at(j).size == SIZE_M)
					new_stick = glmReadOBJ("test_model/zometool/BlueM.obj");
				if (test_connect.at(i).at(j).size == SIZE_L)
					new_stick = glmReadOBJ("test_model/zometool/BlueL.obj");
			}
			else if (i == COLOR_RED){
				if (test_connect.at(i).at(j).size == SIZE_S)
					new_stick = glmReadOBJ("test_model/zometool/RedS.obj");
				if (test_connect.at(i).at(j).size == SIZE_M)
					new_stick = glmReadOBJ("test_model/zometool/RedM.obj");
				if (test_connect.at(i).at(j).size == SIZE_L)
					new_stick = glmReadOBJ("test_model/zometool/RedL.obj");
			}
			else if (i == COLOR_YELLOW){
				if (test_connect.at(i).at(j).size == SIZE_S)
					new_stick = glmReadOBJ("test_model/zometool/YellowS.obj");
				if (test_connect.at(i).at(j).size == SIZE_M)
					new_stick = glmReadOBJ("test_model/zometool/YellowM.obj");
				if (test_connect.at(i).at(j).size == SIZE_L)
					new_stick = glmReadOBJ("test_model/zometool/YellowL.obj");
			}
			glmRT(new_stick, vec3(0.0, t.roll(test_connect.at(i).at(j).fromface), 0.0), vec3(0.0, 0.0, 0.0));
			glmRT(new_stick, vec3(0.0, t.phi(test_connect.at(i).at(j).fromface), t.theta(test_connect.at(i).at(j).fromface)), vec3(0.0, 0.0, 0.0));
			glmR(new_stick, vec3(0.0, 0.0, 0.0));
			glmRT(new_stick, vec3(0.0, 0.0, 0.0), test_connect.at(i).at(j).position);
			glmCombine(zome, new_stick);
		}
	}

	glmWriteOBJ(zome, "fake.obj", GLM_NONE);*/
	
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
	//
	system("pause");
	return 0;
}