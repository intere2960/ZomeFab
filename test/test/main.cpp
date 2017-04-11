#include "main.h"
#include <time.h>
#include <fstream>
#include <omp.h>
#include "nanoflann.hpp"

using namespace std;

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

//void findzoom()
//{
//	zometable splite_table(SPLITE);
//	zometable merge_table(MERGE);
//}

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
	start_stick.fromface = t.opposite_face(best_i);
	start_stick.towardface = best_i;
	start_stick.size = best_s;
	test_connect.at(t.face_color(best_i)).push_back(start_stick);

	cout << "best_i : " << best_i << endl;
	test_connect.at(COLOR_WHITE).at(0).connect_stick[t.opposite_face(best_i)] = vec2(t.face_color(best_i), test_connect.at(t.face_color(best_i)).size() - 1);

	for (int i = 0; i < 62; i += 1){
		if (test_connect.at(COLOR_WHITE).at(0).connect_stick[i] != vec2(-1.0f, -1.0f)){
			cout << test_connect.at(COLOR_WHITE).at(0).connect_stick[i][0] << " " << test_connect.at(COLOR_WHITE).at(0).connect_stick[i][1] << endl;
		}
	}

	test_connect.at(t.face_color(best_i)).at(0).fromindex = vec2(COLOR_WHITE, 0);


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
		temp_stick.fromface = t.opposite_face(best_i);
		temp_stick.towardface = best_i;
		temp_stick.size = best_s;
		if (i < record.size() - 1)
			temp_stick.fromindex = vec2(COLOR_WHITE, i);
		temp_stick.towardindex = vec2(COLOR_WHITE, i - 1);
		test_connect.at(t.face_color(best_i)).push_back(temp_stick);

		test_connect.at(COLOR_WHITE).at(i - 1).connect_stick[best_i] = vec2(t.face_color(best_i), test_connect.at(t.face_color(best_i)).size() - 1);
		if (i < record.size() - 1)
			test_connect.at(COLOR_WHITE).at(i).connect_stick[t.opposite_face(best_i)] = vec2(t.face_color(best_i), test_connect.at(t.face_color(best_i)).size() - 1);
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

void fake_case(int index)
{
	zomedir t;

	vec2 test = test_connect.at(COLOR_WHITE).at(index).connect_stick[5];
		
	int from_face = test_connect.at(test[0]).at(test[1]).fromface;
	int from_size = test_connect.at(test[0]).at(test[1]).size;
	
	zomeconn new_ball;
	zomeconn new_stick_f, new_stick_t;
	
	vec3 use_ball_p = test_connect.at(COLOR_WHITE).at(index).position;

	vec3 use_stick_p = test_connect.at(test[0]).at(test[1]).position;
	vec3 judge = use_stick_p + 0.5f * t.dir->at(from_face) * t.color_length(test[0], from_size) - use_ball_p;

	int toward_ball_index = test_connect.at(test[0]).at(test[1]).towardindex[1];

	if (from_face != 5)
		from_face = 5;
	
	if (toward_ball_index == 0){
		toward_ball_index = test_connect.at(test[0]).at(test[1]).fromindex[1];
	}

	vector<zomerecord> temp;
	for (unsigned int i = 0; i < merge_table.table.at(from_face).size(); i += 1){
		for (unsigned int j = 0; j < merge_table.table.at(from_face).at(i).size(); j += 1){
			if (merge_table.table.at(from_face).at(i).at(j).origin[1] == from_size){
				temp.push_back(merge_table.table.at(from_face).at(i).at(j));
			}
		}
	}

	new_ball.position = test_connect.at(COLOR_WHITE).at(toward_ball_index).position + t.dir->at(temp.at(2).travel_1[0]) * t.face_length(temp.at(2).travel_1[0], temp.at(2).travel_1[1]);
	new_ball.color = COLOR_WHITE;

	new_stick_f.position = test_connect.at(COLOR_WHITE).at(toward_ball_index).position + t.dir->at(temp.at(2).travel_1[0]) * t.face_length(temp.at(2).travel_1[0], temp.at(2).travel_1[1]) / 2;
	new_stick_f.color = t.face_color(temp.at(2).travel_1[0]);
	new_stick_f.size = temp.at(2).travel_1[1];
	new_stick_f.fromface = temp.at(2).travel_1[0];
	new_stick_f.towardface = temp.at(2).travel_1[2];

	new_stick_t.position = test_connect.at(COLOR_WHITE).at(index).position + t.dir->at(temp.at(2).travel_2[0]) * t.face_length(temp.at(2).travel_2[0], temp.at(2).travel_2[1]) / 2;
	new_stick_t.color = t.face_color(temp.at(2).travel_2[0]);
	new_stick_t.size = temp.at(2).travel_2[1];
	new_stick_t.fromface = temp.at(2).travel_2[0];
	new_stick_t.towardface = temp.at(2).travel_2[2];

	test_connect.at(COLOR_WHITE).at(toward_ball_index).connect_stick[new_stick_f.fromface] = vec2(new_stick_f.color, test_connect.at(new_stick_f.color).size());
	new_ball.connect_stick[new_stick_f.towardface] = vec2(new_stick_f.color, test_connect.at(new_stick_f.color).size());

	new_stick_f.fromindex = vec2(COLOR_WHITE, toward_ball_index);
	new_stick_f.towardindex = vec2(COLOR_WHITE, test_connect.at(COLOR_WHITE).size());
	test_connect.at(new_stick_f.color).push_back(new_stick_f);

	test_connect.at(COLOR_WHITE).at(index).connect_stick[new_stick_t.fromface] = vec2(new_stick_t.color, test_connect.at(new_stick_t.color).size());
	new_ball.connect_stick[new_stick_t.towardface] = vec2(new_stick_t.color, test_connect.at(new_stick_t.color).size());

	new_stick_t.fromindex = vec2(COLOR_WHITE, index);
	new_stick_t.towardindex = vec2(COLOR_WHITE, test_connect.at(COLOR_WHITE).size());
	test_connect.at(new_stick_t.color).push_back(new_stick_t);

	test_connect.at(COLOR_WHITE).push_back(new_ball);
}

float decrease_t(int iteration)
{
	int num = (iteration + 1) / 100;
	return pow(0.99f, (float)num);
}

int main(int argc, char **argv)
{
	//    findzoom();

	myObj = glmReadOBJ(model_source);
	//myObj_inner = glmCopy(myObj);

	//cout << "size : " << myObj->numtriangles << endl;
	init();

	///////*vec3 p1(174.059, 312.327, 107.917);
	//////vec3 p2(112.142, 288.677, 69.6504);*/
	///////*vec3 p1(-77.0575, 430.577, 69.6504);
	//////vec3 p2(-177.241, 392.31, 7.7339);*/
	///////*vec3 p1(-138.974, 321.361, -57.633);
	//////vec3 p2(-138.974, 359.627, -33.983);*/
	/////*vec3 p1(17.5425, 241.377, -24.9497);
	////vec3 p2(-44.374, 141.194, 13.3168);*/
	////vec3 p1(-138.974, 359.627, 13.317);
	////vec3 p2(-100.7075, 359.627, 75.233);
	////bool test = check_stick_intersect(myObj, p1, p2);
	////cout << "test : " << test << endl;

	////test();

	////fake();
	////	
	////for (int j = 0; j < 4; j += 1){
	////	for (int k = 0; k < zome_queue.at(2).at(j).size(); k += 1){
	////		zome_queue.at(2).at(j).at(k).position += delta;
	////	}
	////}

	////combine_zome_ztruc(zome_queue.at(1), zome_queue.at(2));
	////combine_zome_ztruc(zome_queue.at(1), test_connect);

	//////for (int j = 0; j < 4; j += 1){
	//////	cout << j << " : " << endl;
	//////	if (j != 3){
	//////		/*for (int k = 0; k < zome_queue.at(i).at(j).size(); k += 1){
	//////		cout << "\t " << k << " : (" << zome_queue.at(i).at(j).at(k).fromindex[0] << " , " << zome_queue.at(i).at(j).at(k).fromindex[1]
	//////		<< ") (" << zome_queue.at(i).at(j).at(k).towardindex[0] << " , " << zome_queue.at(i).at(j).at(k).towardindex[1] << ")" << endl;
	//////		}*/
	//////	}
	//////	else{
	//////		for (int k = 0; k < zome_queue.at(1).at(3).size(); k += 1){
	//////			cout << "\t " << k << " : ";
	//////			for (int a = 0; a < 62; a += 1){
	//////				if (zome_queue.at(1).at(3).at(k).connect_stick[a] != vec2(-1.0f, -1.0f))
	//////					cout << " (" << zome_queue.at(1).at(3).at(k).connect_stick[a][0] << " , " << zome_queue.at(1).at(3).at(k).connect_stick[a][1] << ")";
	//////			}
	//////			cout << endl;
	//////			cout << "\t " << zome_queue.at(1).at(3).at(k).position[0] << " " << zome_queue.at(1).at(3).at(k).position[1] << " " << zome_queue.at(1).at(3).at(k).position[2] << endl;
	//////		}
	//////	}
	//////}

	////output_zometool(zome_queue.at(1), string("test.obj"));
	////output_struc(zome_queue.at(1), string("fake.txt"));
	clock_t total_start, total_finish;
	total_start = clock();

	clock_t start, finish;
	float duration;
	
	srand((unsigned)time(NULL));
	struc_parser(test_connect, string("fake.txt"));
		
	//struc_parser(test_connect, string("fake123.txt"));
	//start = clock();
	
	float origin_term[2];
	float origin_e = compute_energy(test_connect, myObj, origin_term);

	//finish = clock();
	//duration = (float)(finish - start) / CLOCKS_PER_SEC;

	//cout << duration << " s" << endl;

	ofstream os("test.txt");

	os << "origin energy : " << origin_e << endl;
	os << "origin energy(dist) : " << origin_term[0] << endl;
	os << "origin energy(angle) : " << origin_term[1] << endl;
	os << endl;

	///*for (int j = 0; j < 4; j += 1){
	//	cout << j << " : " << endl;
	//	if (j != 3){
	//		for (int k = 0; k < test_connect.at(j).size(); k += 1){
	//			cout << "\t " << k << " : (" << test_connect.at(j).at(k).fromindex[0] << " , " << test_connect.at(j).at(k).fromindex[1]
	//				<< ") (" << test_connect.at(j).at(k).towardindex[0] << " , " << test_connect.at(j).at(k).towardindex[1] << ")" << endl;
	//		}
	//	}
	//	else{
	//		for (int k = 0; k < test_connect.at(3).size(); k += 1){
	//			cout << "\t " << k << " : ";
	//			for (int a = 0; a < 62; a += 1){
	//				if (test_connect.at(3).at(k).connect_stick[a] != vec2(-1.0f, -1.0f))
	//					cout << a << ": (" << test_connect.at(3).at(k).connect_stick[a][0] << " , " << test_connect.at(3).at(k).connect_stick[a][1] << ") ";
	//			}
	//			cout << endl;
	//			cout << "\t " << test_connect.at(3).at(k).position[0] << " " << test_connect.at(3).at(k).position[1] << " " << test_connect.at(3).at(k).position[2] << endl;
	//		}
	//	}
	//}*/

	os << "start" << endl;
	int collision = 0;

	vec3 give_up;
	int num_split = 0, num_merge = 0, num_bridge = 0;

	float inital_t = 1.0f;

	int energy_bigger_accept = 0;
	int energy_smaller_accept = 0;
	int energy_bigger_reject = 0;
	int energy_smaller_reject = 0;

	for (int i = 0; i < 1000; i += 1){
		float now_t = inital_t * decrease_t(i);

		os << i << " :" << endl;
		int choose_op = rand() % 3;
				
		vector<vector<zomeconn>> temp_connect(4);
		temp_connect = test_connect;

		start = clock();
		if (choose_op == 0){
			os << "split" << endl;
			int result = rand() % test_connect.at(COLOR_WHITE).size();
			os << result << endl;
			split(temp_connect, result, myObj, splite_table);
			num_split += 1;
		}
		else if(choose_op == 1){
			os << "merge" << endl;
			vector<vec4> can_merge;
			check_merge(temp_connect, can_merge, myObj, merge_table);
			if (can_merge.size() > 0){
				int merge_index = rand() % can_merge.size();
				merge(temp_connect, can_merge.at(merge_index));
			}
			num_merge += 1;
		}
		else{
			os << "bridge" << endl;
			vector<vec4> can_bridge;
			check_bridge(temp_connect, can_bridge, myObj, merge_table);
			if (can_bridge.size() > 0){
				int bridge_index = rand() % can_bridge.size();
				bridge(temp_connect, can_bridge.at(bridge_index));
			}
			num_bridge += 1;
		}
		finish = clock();
		duration = (float)(finish - start) / CLOCKS_PER_SEC;
		os << "op : " << duration << " s" << endl;

		start = clock();
		float term[2];
		float temp_e = compute_energy(temp_connect, myObj, term);
		finish = clock();
		duration = (float)(finish - start) / CLOCKS_PER_SEC;
		os << "energy : " << duration << " s" << endl;

		float p = (float)rand() / (float)RAND_MAX;
		//cout << p << " " << exp((origin_e - temp_e) / now_t) << endl;
		
		//if (temp_e < origin_e){
		if (p < exp((origin_e - temp_e) / now_t)){
			if (collision_test(temp_connect, give_up)){
				
				//start = clock();
				test_connect = temp_connect;				
				//finish = clock();
				//duration = (float)(finish - start) / CLOCKS_PER_SEC;
				//cout << "copy : " << duration << " s" << endl;

				os << "accept energy : " << temp_e << endl;
				os << "energy(dist) : " << term[0] << endl;
				os << "energy(angle) : " << term[1] << endl;

				if (temp_e < origin_e){
					energy_smaller_accept += 1;
				}
				else{
					energy_bigger_accept += 1;
				}

				origin_e = temp_e;
			}
			else{
				collision += 1;
				os << "reject energy : " << temp_e << endl;
				os << "energy(dist) : " << term[0] << endl;
				os << "energy(angle) : " << term[1] << endl;

				if (temp_e < origin_e){
					energy_smaller_reject += 1;
				}
				else{
					energy_bigger_reject += 1;
				}
			}
		}
		else{
			os << "reject energy : " << temp_e << endl;
			os << "energy(dist) : " << term[0] << endl;
			os << "energy(angle) : " << term[1] << endl;

			if (temp_e < origin_e){
				energy_smaller_reject += 1;
			}
			else{
				energy_bigger_reject += 1;
			}
		}
		os << "T : " << now_t << endl;;
		os << endl;
	}

	os << "collision : " << collision << " " << 10000 << endl;
	os << "split : " << num_split << " merge : " << num_merge << " bridge : " << num_bridge << endl;
	os << "ball-to-ball : " << give_up[0] << " ball-to-rod :  " << give_up[1] << " rod-to-rod :  " << give_up[2] << endl;

	os << "Z' < Z and accept : " << energy_smaller_accept << endl;
	os << "Z' > Z and accept : " << energy_bigger_accept << endl;
	os << "Z' < Z and reject : " << energy_smaller_reject << endl;
	os << "Z' > Z and reject : " << energy_bigger_reject << endl;

	/////*vector<vec4> can_bridge;
	////check_bridge(can_bridge);
	////bridge(test_connect, can_bridge.at(can_bridge.size() - 1));*/

	/////*cout << can_bridge.size() << endl;

	////for (unsigned int i = 0; i < can_bridge.size(); i += 1){
	////	cout << can_bridge.at(i)[0] << " " << can_bridge.at(i)[1] << " " << can_bridge.at(i)[2] << " " << can_bridge.at(i)[3] << endl;
	////}*/
	////
	////

	total_finish = clock();
	duration = (float)(total_finish - total_start) / CLOCKS_PER_SEC;
	os << endl << "totoal time : " << duration << " s" << endl;

	os.close();
	output_zometool(test_connect, string("fake123aaa.obj"));
	///*output_struc(test_connect, string("fake123.txt"));*/
		
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
	//system("pause");
	return 0;
}
