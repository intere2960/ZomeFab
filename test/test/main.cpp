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
		output_zometool(all_voxel.at(i).at(0).rotation, zome_queue.at(i), i);

		//for (int j = 0; j < 4; j += 1){
		//	cout << j << " : " << endl;
		//	if (j != 3){
		//		/*for (int k = 0; k < zome_queue.at(i).at(j).size(); k += 1){
		//			cout << "\t " << k << " : (" << zome_queue.at(i).at(j).at(k).fromindex[0] << " , " << zome_queue.at(i).at(j).at(k).fromindex[1]
		//			<< ") (" << zome_queue.at(i).at(j).at(k).towardindex[0] << " , " << zome_queue.at(i).at(j).at(k).towardindex[1] << ")" << endl;
		//			}*/
		//	}
		//	else{
		//		for (int k = 0; k < zome_queue.at(i).at(3).size(); k += 1){
		//			cout << "\t " << k << " : ";
		//			for (int a = 0; a < 62; a += 1){
		//				if (zome_queue.at(i).at(3).at(k).connect_stick[a] != vec2(-1.0f, -1.0f))
		//					cout << " (" << zome_queue.at(i).at(3).at(k).connect_stick[a][0] << " , " << zome_queue.at(i).at(3).at(k).connect_stick[a][1] << ")";
		//			}
		//			cout << endl;
		//			cout << "\t " << zome_queue.at(i).at(3).at(k).position[0] << " " << zome_queue.at(i).at(3).at(k).position[1] << " " << zome_queue.at(i).at(3).at(k).position[2] << endl;
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


	test_connect.at(COLOR_WHITE).at(0).connect_stick[best_i] = vec2(t.face_color(best_i), test_connect.at(t.face_color(best_i)).size() - 1);
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

		test_connect.at(COLOR_WHITE).at(i - 1).connect_stick[best_i] = vec2(t.face_color(best_i), test_connect.at(t.face_color(best_i)).size() - 1);
		if (i < record.size() - 1)
			test_connect.at(COLOR_WHITE).at(i).connect_stick[best_i] = vec2(t.face_color(best_i), test_connect.at(t.face_color(best_i)).size() - 1);
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

	output_zometool(test_connect, string("test123.obj"));
}

float forbidden_energy(float dist)
{
	zomedir t;
	vec2 p1(t.color_length(COLOR_BLUE, SIZE_S) / 3.0f, 0.0f);
	vec2 p2(t.color_length(COLOR_BLUE, SIZE_S) * 1.0f, 35.0f);
	float a = p2[0] / pow((p2[0] - p1[0]), 2);

	if (dist > p2[0]){
		return p2[1];
	}
	if (dist < p1[0]){
		return 0.0f;
	}

	return a * pow((dist - p1[0]), 2);
}

float compute_energy(vector<vector<zomeconn>> &test_connect, GLMmodel *model)
{
	float energy = 0.0f;
	zomedir t;

	for (unsigned int a = 0; a < test_connect.size(); a += 1){
		for (unsigned int i = 0; i < test_connect.at(a).size(); i += 1){
			vec3 ball_n;
			bool use[62] = { false };
			if (a == 3){
				for (unsigned int k = 0; k < 62; k += 1){
					if (test_connect.at(a).at(i).connect_stick[k] != vec2(-1.0f, -1.0f))
						use[k] = true;
				}
			}			

			if (test_connect.at(a).at(i).near_dir == -1 || use[test_connect.at(a).at(i).near_dir]){

				for (unsigned int j = 0; j < model->triangles->size(); j += 1){
					vec3 p1(model->vertices->at(3 * model->triangles->at(j).vindices[0] + 0), model->vertices->at(3 * model->triangles->at(j).vindices[0] + 1), model->vertices->at(3 * model->triangles->at(j).vindices[0] + 2));
					vec3 p2(model->vertices->at(3 * model->triangles->at(j).vindices[1] + 0), model->vertices->at(3 * model->triangles->at(j).vindices[1] + 1), model->vertices->at(3 * model->triangles->at(j).vindices[1] + 2));
					vec3 p3(model->vertices->at(3 * model->triangles->at(j).vindices[2] + 0), model->vertices->at(3 * model->triangles->at(j).vindices[2] + 1), model->vertices->at(3 * model->triangles->at(j).vindices[2] + 2));
					vec3 v1 = p1 - p2;
					vec3 v2 = p3 - p2;
					vec3 n = v2 ^ v1;
					float d = n * p1;

					int near_index = -1;
					for (unsigned int k = 0; k < t.dir->size(); k += 1){
						if (!use[k]){
							ball_n = t.dir->at(k);
							vec3 edge1 = p2 - p1;
							vec3 edge2 = p3 - p2;
							vec3 edge3 = p1 - p3;
							vec3 insect_p;
							vec3 judge1;
							vec3 judge2;
							vec3 judge3;

							float t = (d - (test_connect.at(a).at(i).position * n)) / (n * ball_n);
							if (t > 0){
								insect_p = test_connect.at(a).at(i).position + ball_n * t;

								judge1 = insect_p - p1;
								judge2 = insect_p - p2;
								judge3 = insect_p - p3;

								if (((edge1 ^ judge1) * n > 0) && ((edge2 ^ judge2) * n > 0) && ((edge3 ^ judge3) * n > 0)){
									if ((insect_p - test_connect.at(a).at(i).position).length() < test_connect.at(a).at(i).surface_d){
										test_connect.at(a).at(i).surface_d = (insect_p - test_connect.at(a).at(i).position).length();
										near_index = k;
									}
								}
							}
						}
					}
					if (near_index != -1)
						test_connect.at(a).at(i).near_dir = near_index;
				}
			}
		}
	}

	float energy_dist = 0.0f;
	for (unsigned int a = 0; a < test_connect.size(); a += 1){
		for (unsigned int i = 0; i < test_connect.at(a).size(); i += 1){
			energy_dist += pow(test_connect.at(a).at(i).surface_d, 2) * (1.0 + forbidden_energy(test_connect.at(a).at(i).surface_d));
		}
	}
	energy_dist /= (pow(t.color_length(COLOR_BLUE, SIZE_S), 2) * (test_connect.at(COLOR_BLUE).size() + test_connect.at(COLOR_RED).size() + test_connect.at(COLOR_YELLOW).size() + test_connect.at(COLOR_WHITE).size()));

	float energy_fair = 0.0f;
	for (unsigned int i = 0; i < test_connect.at(COLOR_WHITE).size(); i += 1){
		vec3 temp_ring_p;
		int use_stick = 0;
		for (unsigned int j = 0; j < 62; j += 1){
			if (test_connect.at(COLOR_WHITE).at(i).connect_stick[j] != vec2(-1.0f, -1.0f)){
				//cout << "abcd : " << test_connect.at(COLOR_WHITE).at(i).connect_stick[j][0] << " " << test_connect.at(COLOR_WHITE).at(i).connect_stick[j][1] << endl;
				temp_ring_p += test_connect.at(test_connect.at(COLOR_WHITE).at(i).connect_stick[j][0]).at(test_connect.at(COLOR_WHITE).at(i).connect_stick[j][1]).position;
				use_stick += 1;
			}
		}
		temp_ring_p /= use_stick;
		energy_fair += (test_connect.at(COLOR_WHITE).at(i).position - temp_ring_p).length2();
	}
	energy_fair /= (pow(t.color_length(COLOR_BLUE, SIZE_S), 2) * test_connect.at(COLOR_WHITE).size());

	energy = energy_dist + energy_fair;
	cout << energy << endl;

	return energy;
}

void split(vector<vector<zomeconn>> &test_connect, int s_index, GLMmodel *model)
{
	zomedir t;

	int c_index;
	float dist = 100000000000000000.0f;
	
	for (unsigned int i = 0; i < 62; i += 1){
		if (test_connect.at(COLOR_WHITE).at(s_index).connect_stick[i] != vec2(-1.0f, -1.0f)){
			vec2 test = test_connect.at(COLOR_WHITE).at(s_index).connect_stick[i];
			if (test_connect.at(test[0]).at(test[1]).surface_d < dist){
				dist = test_connect.at(test[0]).at(test[1]).surface_d;
				c_index = i;
			}
		}
	}

	vec2 test = test_connect.at(COLOR_WHITE).at(s_index).connect_stick[c_index];
	int from_face = test_connect.at(test[0]).at(test[1]).fromface;
	int from_size = test_connect.at(test[0]).at(test[1]).size;

	//cout << "from_size : " << from_size << endl;

	vec3 use_ball_p = test_connect.at(COLOR_WHITE).at(s_index).position;

	//cout << "use_ball_p : " << use_ball_p[0] << " " << use_ball_p[1] << " " << use_ball_p[2] << endl;

	vec3 use_stick_p = test_connect.at(test[0]).at(test[1]).position;
	vec3 judge = use_stick_p + 0.5f * t.dir->at(from_face) * t.color_length(test[0], from_size) - use_ball_p;
	
	int toward_ball_index = test_connect.at(test[0]).at(test[1]).towardindex[1];
	
	if (judge.length() < 0.001){
		//cout << "in" << endl;
		from_face = t.opposite_face(from_face);
		toward_ball_index = test_connect.at(test[0]).at(test[1]).fromindex[1];
	}

	//cout << "from_face" << endl;
	//cout << from_face << endl;

	vector<zomerecord> temp;
	for (unsigned int i = 0; i < splite_table.table.at(from_face).size(); i += 1){
		for (unsigned int j = 0; j < splite_table.table.at(from_face).at(i).size(); j += 1){
			if (splite_table.table.at(from_face).at(i).at(j).origin[1] == from_size){
				temp.push_back(splite_table.table.at(from_face).at(i).at(j));
			}
		}
	}

	//cout << temp.size() << endl;

	for (unsigned int i = 0; i < temp.size(); i += 1){
		if (test_connect.at(COLOR_WHITE).at(s_index).connect_stick[(int)temp.at(i).travel_1[0]] != vec2(-1.0f, -1.0f)){
			temp.erase(temp.begin() + i);
			i -= 1;
			continue;
		}

		if (test_connect.at(COLOR_WHITE).at(toward_ball_index).connect_stick[(int)temp.at(i).travel_2[0]] != vec2(-1.0f, -1.0f)){
			temp.erase(temp.begin() + i);
			i -= 1;
			continue;
		}
	}

	//cout << temp.size() << endl;

	float choose_split = 0;
	float split_dist = 10000000000000000.0f;
	for (unsigned int i = 0; i < temp.size(); i += 1){
		vec3 test_d = use_ball_p + t.dir->at(temp.at(i).travel_1[0]) * t.face_length(temp.at(i).travel_1[0], temp.at(i).travel_1[1]);
		float judge_d = point_surface_dist(myObj , test_d);
		if (judge_d < split_dist){
			choose_split = i;
			split_dist = judge_d;
		}
	}
	cout << "choose_split : " << choose_split << endl;
	
	//cout << "o : " << temp.at(2).origin[0] << " " << temp.at(2).origin[1] << " " << temp.at(2).origin[2] << endl;
	//cout << "t1 : " << temp.at(2).travel_1[0] << " " << temp.at(2).travel_1[1] << " " << temp.at(2).travel_1[2] << endl;
	//cout << "t2 : " << temp.at(2).travel_2[0] << " " << temp.at(2).travel_2[1] << " " << temp.at(2).travel_2[2] << endl;

	zomeconn new_ball;
	zomeconn new_stick_f, new_stick_t;

	new_ball.position = test_connect.at(COLOR_WHITE).at(s_index).position + t.dir->at(temp.at(choose_split).travel_1[0]) * t.face_length(temp.at(choose_split).travel_1[0], temp.at(choose_split).travel_1[1]);
	new_ball.color = COLOR_WHITE;

	new_stick_f.position = test_connect.at(COLOR_WHITE).at(s_index).position + t.dir->at(temp.at(choose_split).travel_1[0]) * t.face_length(temp.at(choose_split).travel_1[0], temp.at(choose_split).travel_1[1]) / 2;
	new_stick_f.color = t.face_color(temp.at(choose_split).travel_1[0]);
	new_stick_f.size = temp.at(choose_split).travel_1[1];
	new_stick_f.fromface = temp.at(choose_split).travel_1[0];
	new_stick_f.towardface = temp.at(choose_split).travel_1[2];

	/*cout << "f:" << endl;
	cout << new_stick_f.color << endl;
	cout << new_stick_f.size << endl;
	cout << new_stick_f.fromface << endl;
	cout << new_stick_f.towardface << endl;*/

	new_stick_t.position = test_connect.at(COLOR_WHITE).at(toward_ball_index).position + t.dir->at(temp.at(choose_split).travel_2[0]) * t.face_length(temp.at(choose_split).travel_2[0], temp.at(choose_split).travel_2[1]) / 2;
	new_stick_t.color = t.face_color(temp.at(choose_split).travel_2[0]);
	new_stick_t.size = temp.at(choose_split).travel_2[1];
	new_stick_t.fromface = temp.at(choose_split).travel_2[0];
	new_stick_t.towardface = temp.at(choose_split).travel_2[2];

	/*cout << "t:" << endl;
	cout << new_stick_t.color << endl;
	cout << new_stick_t.size << endl;
	cout << new_stick_t.fromface << endl;
	cout << new_stick_t.towardface << endl;
	
	cout << toward_ball_index << endl;
	cout << new_stick_t.fromface << endl;
	cout << new_stick_t.towardface << endl;*/

	test_connect.at(COLOR_WHITE).at(s_index).connect_stick[new_stick_f.fromface] = vec2(new_stick_f.color, test_connect.at(new_stick_f.color).size());
	new_ball.connect_stick[new_stick_f.towardface] = vec2(new_stick_f.color, test_connect.at(new_stick_f.color).size());
	
	new_stick_f.fromindex = vec2(COLOR_WHITE, s_index);
	new_stick_f.towardindex = vec2(COLOR_WHITE, test_connect.at(COLOR_WHITE).size());
	test_connect.at(new_stick_f.color).push_back(new_stick_f);

	test_connect.at(COLOR_WHITE).at(toward_ball_index).connect_stick[new_stick_t.fromface] = vec2(new_stick_t.color, test_connect.at(new_stick_t.color).size());
	new_ball.connect_stick[new_stick_t.towardface] = vec2(new_stick_t.color, test_connect.at(new_stick_t.color).size());
	
	new_stick_t.fromindex = vec2(COLOR_WHITE, toward_ball_index);
	new_stick_t.towardindex = vec2(COLOR_WHITE, test_connect.at(COLOR_WHITE).size());
	test_connect.at(new_stick_t.color).push_back(new_stick_t);
	
	test_connect.at(COLOR_WHITE).push_back(new_ball);	
}

//void merge()
//{
//	zomedir t;
//
//	vec2 test = test_connect.at(COLOR_WHITE).at(test_connect.at(COLOR_WHITE).size() - 1).connect_stick.at(0);
//	int from_face = test_connect.at(test[0]).at(test[1]).fromface;
//	int from_size = test_connect.at(test[0]).at(test[1]).size;
//	vec3 use_ball_p = test_connect.at(COLOR_WHITE).at(test_connect.at(COLOR_WHITE).size() - 1).position;
//	vec3 use_stick_p = test_connect.at(test[0]).at(test[1]).position;
//	vec3 judge = use_stick_p + t.dir->at(from_face) * t.color_length(test[0], from_size) / 2 - use_ball_p;
//	if (judge.length() < 0.001){
//		from_face = t.opposite_face(from_face);
//	}
//
//	vector<zomerecord> temp;
//	for (unsigned int i = 0; i < merge_table.table.at(from_face).size(); i += 1){
//		for (unsigned int j = 0; j < merge_table.table.at(from_face).at(i).size(); j += 1){
//			if (merge_table.table.at(from_face).at(i).at(j).origin[1] == from_size){
//				temp.push_back(merge_table.table.at(from_face).at(i).at(j));
//			}
//		}
//	}
//
//	zomeconn new_stick;
//	new_stick.position = test_connect.at(COLOR_WHITE).at(test_connect.at(COLOR_WHITE).size() - 1).position + t.dir->at(temp.at(3).travel_2[0]) * t.face_length(temp.at(3).travel_2[0], temp.at(3).travel_2[1]) / 2;
//	new_stick.color = t.face_color(temp.at(3).travel_2[0]);
//	new_stick.size = temp.at(3).travel_2[1];
//	new_stick.fromface = temp.at(3).travel_2[0];
//	new_stick.towardface = temp.at(3).travel_2[2];
//
//	new_stick.fromindex = vec2(COLOR_WHITE, test_connect.at(COLOR_WHITE).size() - 1);
//	vec3 toward_position = test_connect.at(COLOR_WHITE).at(test_connect.at(COLOR_WHITE).size() - 1).position + t.dir->at(temp.at(3).travel_2[0]) * t.face_length(temp.at(3).travel_2[0], temp.at(3).travel_2[1]);
//	for (unsigned int i = 0; i < test_connect.at(COLOR_WHITE).size(); i += 1){
//		if ((test_connect.at(COLOR_WHITE).at(i).position - toward_position).length() < 0.0001f){
//			new_stick.towardindex = vec2(COLOR_WHITE, i);
//			break;
//		}
//	}
//	
//	test_connect.at(new_stick.color).push_back(new_stick);
//
//	cout << new_stick.fromindex[1] << " " << new_stick.towardindex[1] << endl;
//}

int main(int argc, char **argv)
{
	//    findzoom();

	myObj = glmReadOBJ(model_source);
	//myObj_inner = glmCopy(myObj);

	init();

	//test();

	//fake();
	//	
	//for (int j = 0; j < 4; j += 1){
	//	for (int k = 0; k < zome_queue.at(2).at(j).size(); k += 1){
	//		zome_queue.at(2).at(j).at(k).position += delta;
	//	}
	//}

	//combine_zome_ztruc(zome_queue.at(1), zome_queue.at(2));
	//combine_zome_ztruc(zome_queue.at(1), test_connect);

	////for (int j = 0; j < 4; j += 1){
	////	cout << j << " : " << endl;
	////	if (j != 3){
	////		/*for (int k = 0; k < zome_queue.at(i).at(j).size(); k += 1){
	////		cout << "\t " << k << " : (" << zome_queue.at(i).at(j).at(k).fromindex[0] << " , " << zome_queue.at(i).at(j).at(k).fromindex[1]
	////		<< ") (" << zome_queue.at(i).at(j).at(k).towardindex[0] << " , " << zome_queue.at(i).at(j).at(k).towardindex[1] << ")" << endl;
	////		}*/
	////	}
	////	else{
	////		for (int k = 0; k < zome_queue.at(1).at(3).size(); k += 1){
	////			cout << "\t " << k << " : ";
	////			for (int a = 0; a < 62; a += 1){
	////				if (zome_queue.at(1).at(3).at(k).connect_stick[a] != vec2(-1.0f, -1.0f))
	////					cout << " (" << zome_queue.at(1).at(3).at(k).connect_stick[a][0] << " , " << zome_queue.at(1).at(3).at(k).connect_stick[a][1] << ")";
	////			}
	////			cout << endl;
	////			cout << "\t " << zome_queue.at(1).at(3).at(k).position[0] << " " << zome_queue.at(1).at(3).at(k).position[1] << " " << zome_queue.at(1).at(3).at(k).position[2] << endl;
	////		}
	////	}
	////}

	//output_zometool(zome_queue.at(1), string("test.obj"));
	//output_struc(zome_queue.at(1));
	
	srand((unsigned)time(NULL));

	struc_parser(test_connect);

	float origin_e = compute_energy(test_connect, myObj);

	//split(test_connect, 0, myObj);
		
	/////*for (unsigned int i = 0; i < 6; i += 1){
	////	int result = rand() % test_connect.at(COLOR_WHITE).size();
	////	cout << result << endl;
	////}*/

	cout << "start" << endl;
	int result;
	for (int i = 0; i < 100; i += 1){
		result = rand() % test_connect.at(COLOR_WHITE).size();
		cout << result << endl;
		
		vector<vector<zomeconn>> temp_connect(4);
		temp_connect = test_connect;

		split(temp_connect, result, myObj);

		float temp_e = compute_energy(temp_connect, myObj);

		if (temp_e < origin_e){
			test_connect = temp_connect;
			origin_e = temp_e;
		}
	}

	output_zometool(test_connect, string("fake123.obj"));
		
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
