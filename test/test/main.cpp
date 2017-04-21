#include "main.h"
#include <ctime>
#include <fstream>
#include <omp.h>

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

	glmFacetNormals(myObj);

	collect_edge(myObj, all_edge);

	find_near_tri(myObj, all_edge);

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

//void test()
//{
//	cout << "sdf segment" << endl;
//	sdf_segment(seg, myObj, model_source);
//
//	all_voxel.resize(seg.size());
//	zome_queue.resize(seg.size());
//
//	obb_center.resize(seg.size());
//	obb_max.resize(seg.size());
//	obb_min.resize(seg.size());
//	obb_size.resize(seg.size());
//	obb_angle.resize(seg.size());
//
//	cout << "generate piece" << endl;
//	#pragma omp parallel for
//	for (int i = 0; i < seg.size(); i += 1){
//		if (seg.size() > 1){
//			vector<edge> fill_edge;
//			collect_edge(&seg.at(i), fill_edge);
//			inform_vertex(&seg.at(i), fill_edge);
//
//			std::vector<int> single_use;
//			for (unsigned int j = 0; j < fill_edge.size(); j += 1){
//				if (fill_edge.at(j).face_id[1] == -1){
//					if ((unsigned)(find(single_use.begin(), single_use.end(), fill_edge.at(j).index[0]) - single_use.begin()) >= single_use.size()){
//						single_use.push_back(fill_edge.at(j).index[0]);
//					}
//					if ((unsigned)(find(single_use.begin(), single_use.end(), fill_edge.at(j).index[1]) - single_use.begin()) >= single_use.size()){
//						single_use.push_back(fill_edge.at(j).index[1]);
//					}
//				}
//			}
//
//			find_easy_loop(&seg.at(i), fill_edge, single_use);
//			fill_hole(seg.at(i), false);
//		}
//
//		std::string s = "test";
//		std::string piece = s + std::to_string(i) + ".obj";
//		glmWriteOBJ(&seg.at(i), my_strdup(piece.c_str()), GLM_NONE);
//
//
//		cout << "piece " << i + 1 << " :" << endl;
//		zome_queue.at(i).resize(4);
//		//computeBestFitOBB(seg.at(i).numvertices, seg.at(i).vertices, obb_size.at(i), obb_max.at(i), obb_min.at(i), obb_center.at(i), obb_angle.at(i), start_m);
//		computeSimpleBB(seg.at(i).numvertices, seg.at(i).vertices, obb_size.at(i), obb_max.at(i), obb_min.at(i), obb_center.at(i));
//		//voxelization(&seg.at(i), all_voxel.at(i), zome_queue.at(i), obb_max.at(i), obb_min.at(i), obb_center.at(i), obb_angle.at(i), COLOR_BLUE, SIZE_S);
//		voxelization(&seg.at(i), all_voxel.at(i), zome_queue.at(i), obb_max.at(i), obb_min.at(i), obb_center.at(i), vec3(0.0, 0.0, 0.0), COLOR_BLUE, SIZE_S);
//		//voxelization(&seg.at(i), all_voxel.at(i), zome_queue.at(i), obb_max.at(i), obb_min.at(i), bound_center, vec3(0.0, 0.0, 0.0), COLOR_BLUE, SIZE_S);
//		cout << "output piece " << i + 1 << endl;
//		output_voxel(all_voxel.at(i), i);
//		output_zometool(all_voxel.at(i).at(0).rotation, zome_queue.at(i), i);
//
//		//for (int j = 0; j < 4; j += 1){
//		//	cout << j << " : " << endl;
//		//	if (j != 3){
//		//		/*for (int k = 0; k < zome_queue.at(i).at(j).size(); k += 1){
//		//			cout << "\t " << k << " : (" << zome_queue.at(i).at(j).at(k).fromindex[0] << " , " << zome_queue.at(i).at(j).at(k).fromindex[1]
//		//			<< ") (" << zome_queue.at(i).at(j).at(k).towardindex[0] << " , " << zome_queue.at(i).at(j).at(k).towardindex[1] << ")" << endl;
//		//			}*/
//		//	}
//		//	else{
//		//		for (int k = 0; k < zome_queue.at(i).at(3).size(); k += 1){
//		//			cout << "\t " << k << " : ";
//		//			for (int a = 0; a < 62; a += 1){
//		//				if (zome_queue.at(i).at(3).at(k).connect_stick[a] != vec2(-1.0f, -1.0f))
//		//					cout << " (" << zome_queue.at(i).at(3).at(k).connect_stick[a][0] << " , " << zome_queue.at(i).at(3).at(k).connect_stick[a][1] << ")";
//		//			}
//		//			cout << endl;
//		//			cout << "\t " << zome_queue.at(i).at(3).at(k).position[0] << " " << zome_queue.at(i).at(3).at(k).position[1] << " " << zome_queue.at(i).at(3).at(k).position[2] << endl;
//		//		}
//		//	}
//		//}
//	}
//}

void simple_test()
{
	all_voxel.resize(1);
	zome_queue.resize(1);

	obb_center.resize(1);
	obb_max.resize(1);
	obb_min.resize(1);
	obb_size.resize(1);
	obb_angle.resize(1);

	std::string s = "test";
	std::string piece = s + std::to_string(0) + ".obj";
	glmWriteOBJ(myObj, my_strdup(piece.c_str()), GLM_NONE);


	cout << "piece " << 0 + 1 << " :" << endl;
	zome_queue.at(0).resize(4);
	computeSimpleBB(myObj->numvertices, myObj->vertices, obb_size.at(0), obb_max.at(0), obb_min.at(0), obb_center.at(0));
	voxelization(myObj, all_voxel.at(0), zome_queue.at(0), obb_max.at(0), obb_min.at(0), obb_center.at(0), vec3(0.0, 0.0, 0.0), COLOR_BLUE, SIZE_S);
	cout << "output piece " << 0 + 1 << endl;
	output_voxel(all_voxel.at(0), 0);
	output_zometool(all_voxel.at(0).at(0).rotation, zome_queue.at(0), 0);
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

#include "MRFOptimization.h"
#include "MRFProblems.h"
#include "MRFGraphCut.h"

double AverageRadius(GLMmodel *model, vec3 &centroid)
{
	float area = 0.0f;
	float distance = 0;
	for (int i = 0; i < model->numtriangles; i += 1){
	
		vec3 p1 = vec3(model->vertices->at(3 * model->triangles->at(i).vindices[0] + 0), model->vertices->at(3 * model->triangles->at(i).vindices[0] + 1), model->vertices->at(3 * model->triangles->at(i).vindices[0] + 2));
		vec3 p2 = vec3(model->vertices->at(3 * model->triangles->at(i).vindices[1] + 0), model->vertices->at(3 * model->triangles->at(i).vindices[1] + 1), model->vertices->at(3 * model->triangles->at(i).vindices[1] + 2));
		vec3 p3 = vec3(model->vertices->at(3 * model->triangles->at(i).vindices[2] + 0), model->vertices->at(3 * model->triangles->at(i).vindices[2] + 1), model->vertices->at(3 * model->triangles->at(i).vindices[2] + 2));
		vec3 g = (p1 + p2 + p3) / 3.0f;

		vec3 v1 = p1 - p2;
		vec3 v2 = p3 - p2;

		vec3 test_normal = v2 ^ v1;
		float face_area = test_normal.length();
		distance += face_area *	(g - centroid).length();
		area += face_area;

	}
	
	if (area == 0) 
		return 0;
	else 
		return distance / area; 
}

void test_graph_cut(GLMmodel *model, std::vector<std::vector<zomeconn>> &test_connect, vector<simple_material> &materials, float color_material[20][3])
{
	int num_labels = 0;

	std::vector<std::vector<int>> index(test_connect.at(COLOR_WHITE).size());
	std::vector<int> use;
	for (int i = 0; i < model->numtriangles; i += 1){
		index.at(model->triangles->at(i).near_node).push_back(i);
		int test_index = find(use.begin(), use.end(), model->triangles->at(i).near_node) - use.begin();
		if (test_index == use.size()){
			use.push_back(model->triangles->at(i).near_node);
		}
	}

	std::vector<int> convert_index(test_connect.at(COLOR_WHITE).size());
	for (unsigned int i = 0; i < test_connect.at(COLOR_WHITE).size(); i += 1){
		if (index.at(i).size() != 0){
			convert_index.at(i) = num_labels;
			num_labels += 1;
		}
	}

	VKGraph * nhd = new VKGraph(model->numtriangles);

	double radius = AverageRadius(model, glmCentroid(model));
	cout << "radius : " << radius << endl;

	float max_data = 0.0f;
	float max_smooth = 0.0f;
	vector<double> dataterm(model->numtriangles * num_labels);
	for (int i = 0; i < model->numtriangles; i += 1){

		vec3 now_n = vec3(model->facetnorms->at(3 * model->triangles->at(i).findex + 0), model->facetnorms->at(3 * model->triangles->at(i).findex + 1), model->facetnorms->at(3 * model->triangles->at(i).findex + 2));
		vec3 p1 = vec3(model->vertices->at(3 * model->triangles->at(i).vindices[0] + 0), model->vertices->at(3 * model->triangles->at(i).vindices[0] + 1), model->vertices->at(3 * model->triangles->at(i).vindices[0] + 2));
		vec3 p2 = vec3(model->vertices->at(3 * model->triangles->at(i).vindices[1] + 0), model->vertices->at(3 * model->triangles->at(i).vindices[1] + 1), model->vertices->at(3 * model->triangles->at(i).vindices[1] + 2));
		vec3 p3 = vec3(model->vertices->at(3 * model->triangles->at(i).vindices[2] + 0), model->vertices->at(3 * model->triangles->at(i).vindices[2] + 1), model->vertices->at(3 * model->triangles->at(i).vindices[2] + 2));
		vec3 now_g = (p1 + p2 + p3) / 3.0f;

		for (int j = 0; j < 3; j += 1){
			if (!nhd->Adjacent(model->triangles->at(i).near_tri[j], i))
			{
				vec3 judge_n = vec3(model->facetnorms->at(3 * model->triangles->at(model->triangles->at(i).near_tri[j]).findex + 0), model->facetnorms->at(3 * model->triangles->at(model->triangles->at(i).near_tri[j]).findex + 1), model->facetnorms->at(3 * model->triangles->at(model->triangles->at(i).near_tri[j]).findex + 2));
				vec3 judge_p1 = vec3(model->vertices->at(3 * model->triangles->at(model->triangles->at(i).near_tri[j]).vindices[0] + 0), model->vertices->at(3 * model->triangles->at(model->triangles->at(i).near_tri[j]).vindices[0] + 1), model->vertices->at(3 * model->triangles->at(model->triangles->at(i).near_tri[j]).vindices[0] + 2));
				vec3 judge_p2 = vec3(model->vertices->at(3 * model->triangles->at(model->triangles->at(i).near_tri[j]).vindices[1] + 0), model->vertices->at(3 * model->triangles->at(model->triangles->at(i).near_tri[j]).vindices[1] + 1), model->vertices->at(3 * model->triangles->at(model->triangles->at(i).near_tri[j]).vindices[1] + 2));
				vec3 judge_p3 = vec3(model->vertices->at(3 * model->triangles->at(model->triangles->at(i).near_tri[j]).vindices[2] + 0), model->vertices->at(3 * model->triangles->at(model->triangles->at(i).near_tri[j]).vindices[2] + 1), model->vertices->at(3 * model->triangles->at(model->triangles->at(i).near_tri[j]).vindices[2] + 2));
				vec3 judge_g = (judge_p1 + judge_p2 + judge_p3) / 3.0f;

				float dist = (now_g - judge_g).length();
				float edist = 10 * dist / radius;
				float angle;
				if (fabs(now_n * judge_n - 1) < 0.001f){
					angle = 0.0f + numeric_limits<float>::epsilon();
				}
				else{
					angle = acos(now_n * judge_n);
				}
				float smooth = -log(angle / M_PI) * edist;

				//cout << i << " : " << now_n * judge_n << " " << angle << " " << dist << " " << -log(angle / M_PI) << " " << -log(angle / M_PI) * dist << endl;
				//cout << i << " : " << model->triangles->at(i).near_tri[j] << " : " << -log(angle / M_PI) * dist << endl;

				nhd->AddEdge(i, model->triangles->at(i).near_tri[j], smooth);

				if (max_smooth < smooth){
					max_smooth = smooth;
				}
			}
		}

		for (int j = 0; j < use.size(); j += 1){
			vec3 judge_p = test_connect.at(COLOR_WHITE).at(use.at(j)).position;
			float dist = (now_g - judge_p).length();
			//cout << dist << endl;
			dataterm.at(i * num_labels + convert_index.at(use.at(j))) = dist;
			if (max_data < dist){
				max_data = dist;
			}
		}
	}

	for (int i = 0; i < model->numtriangles; i += 1){
		for (int j = 0; j < nhd->Node(i)->edges.size(); j += 1){
			nhd->Node(i)->edges.at(j)->weight /= max_smooth;
		}
	}

	for (int i = 0; i < dataterm.size(); i += 1){
		dataterm.at(i) /= max_data;
	}

	std::cout << "NEdges = " << nhd->NumEdges() << std::endl;
	
	MRFProblemSmoothness problem(model->numtriangles, num_labels, nhd, dataterm);

	MRFGraphCut mrfSolver(&problem);

	mrfSolver.Optimize();

	double Edata, Esmooth, Elabel;
	double E = mrfSolver.Energy(&Edata, &Esmooth, &Elabel);
	std::cout << "E1 = " << E << " = " << Edata << " + " << Esmooth << " + " << Elabel << std::endl;

	cout << "max data : " << max_data << endl;
	cout << "max smooth : " << max_smooth << endl;

	vector<vector<int>> mat(model->numtriangles);
		
	for (int i = 0; i < model->numtriangles; i += 1){
		mat.at(mrfSolver.Label(i)).push_back(i);
		//cout << i << " : " << mrfSolver.Label(i) << endl;
	}
	
	int material_class = 0;
	for (int i = 0; i < model->numtriangles; i += 1){
		if (mat.at(i).size() > 0){
			for (unsigned int j = 0; j < mat.at(i).size(); j += 1){
				model->triangles->at(mat.at(i).at(j)).material_id_graph_cut = material_class;
			}
			
			simple_material temp_m;
			temp_m.name = std::to_string(materials.size());
			temp_m.diffuse[0] = (float)rand() / (float)RAND_MAX;
			temp_m.diffuse[1] = (float)rand() / (float)RAND_MAX;
			temp_m.diffuse[2] = (float)rand() / (float)RAND_MAX;
			/*temp_m.diffuse[0] = color_material[i % 20][0];
			temp_m.diffuse[1] = color_material[i % 20][1];
			temp_m.diffuse[2] = color_material[i % 20][2];*/
			materials.push_back(temp_m);
			
			material_class += 1;
		}
	}

	/*vec3 deleta = vec3(0.0f, 1.0f, -1.0f) / material_class;
	for (int i = 0; i < material_class; i += 1){
		simple_material temp_m;
		temp_m.name = std::to_string(materials.size());
		temp_m.diffuse[0] = 0.8f + deleta[0] * i;
		temp_m.diffuse[1] = 0.0f + deleta[1] * i;
		temp_m.diffuse[2] = 1.0f + deleta[2] * i;
		materials.push_back(temp_m);
	}*/

	
	//
	
	//	for (int i = 0; i < model->numtriangles; i += 1){
	//		if (mat.at(i).size() > 0){
	//			for (unsigned int j = 0; j < mat.at(i).size(); j += 1){
	//				model->triangles->at(mat.at(i).at(j)).material_id_graph_cut = material_class;
	//			}
	//
	//			simple_material temp_m;
	//			temp_m.name = std::to_string(materials.size());
	//			temp_m.diffuse[0] = (float)rand() / (float)RAND_MAX;
	//			temp_m.diffuse[1] = (float)rand() / (float)RAND_MAX;
	//			temp_m.diffuse[2] = (float)rand() / (float)RAND_MAX;
	//			materials.push_back(temp_m);
	//
	//			material_class += 1;
	//		}
	//	}
}

int main(int argc, char **argv)
{
	//    findzoom();

	myObj = glmReadOBJ(model_source);
	//myObj_inner = glmCopy(myObj);

	init();
	
	////test();
	//simple_test();

	////fake();
	//	
	///*for (int j = 0; j < 4; j += 1){
	//	for (int k = 0; k < zome_queue.at(2).at(j).size(); k += 1){
	//		zome_queue.at(2).at(j).at(k).position += delta;
	//	}
	//}*/

	//vector<vector<zomeconn>> output_ans(4);
	//for (unsigned int i = 0; i < zome_queue.size(); i += 1){
	//	combine_zome_ztruc(output_ans, zome_queue.at(i));
	//}
	////combine_zome_ztruc(zome_queue.at(1), zome_queue.at(2));
	////combine_zome_ztruc(zome_queue.at(1), test_connect);

	//output_zometool(output_ans, string("fake_head_out.obj"));
	//output_struc(output_ans, string("fake_head_out.txt"));

	clock_t total_start, total_finish;
	total_start = clock();

	clock_t start, finish;
	float duration;

	srand((unsigned)time(NULL));
	struc_parser(test_connect, string("2000_20000.txt"));

	nearest_point_parser(myObj, string("head_2times_nearest_point.txt"));

	////start = clock();

	//PointCloud<float> cloud;
	//// Generate points:
	//generatePointCloud(cloud, myObj);

	//judge_outter(test_connect);

	//cout << myObj->numtriangles << endl;

	//float origin_term[4];
	//float origin_e = compute_energy(test_connect, myObj, cloud, origin_term);

	//output_nearest_point(myObj, string("head_2times_nearest_point.txt"));
	//
	//int num_iteration = 0;

	//finish = clock();
	//duration = (float)(finish - start) / CLOCKS_PER_SEC;
	//cout << duration << " s" << endl;

	//ofstream os("energy_2000_0.txt");

	//os << "origin energy : " << origin_e << endl;
	//os << "origin energy(dist) : " << origin_term[0] << endl;
	//os << "origin energy(angle) : " << origin_term[1] << endl;
	//os << "origin energy(total_number) : " << origin_term[2] << endl;
	//os << "origin energy(use_stick) : " << origin_term[3] << endl;
	//os << endl;

	////vector<simple_material> materials_color, material_energy_dist, material_energy_angle;
	////energy_angle_material(test_connect, material_energy_angle);

	//os << "start" << endl;
	//int collision = 0;

	//vec3 give_up;
	//int num_split = 0, num_merge = 0, num_bridge = 0, num_kill = 0;

	//float inital_t = 1.0f;

	//int energy_bigger_accept = 0;
	//int energy_smaller_accept = 0;
	//int energy_bigger_reject = 0;
	//int energy_smaller_reject = 0;

	//for (int i = 0; i < num_iteration; i += 1){
	//	float now_t = inital_t * decrease_t(i);

	//	os << i << " :" << endl;

	//	vector<vector<zomeconn>> temp_connect(4);
	//	temp_connect = test_connect;

	//	start = clock();

	//	int choose_op = rand() % 4;
	//	if (choose_op == 0){
	//		os << "split" << endl;
	//		int result;
	//		do{
	//			result = rand() % test_connect.at(COLOR_WHITE).size();
	//		} while (!test_connect.at(COLOR_WHITE).at(result).exist || !test_connect.at(COLOR_WHITE).at(result).outter);
	//		os << result << endl;
	//		split(temp_connect, result, myObj, cloud, splite_table);
	//		num_split += 1;
	//	}
	//	else if (choose_op == 1){
	//		os << "merge" << endl;
	//		vector<vec4> can_merge;
	//		check_merge(temp_connect, can_merge, myObj, merge_table);
	//		if (can_merge.size() > 0){
	//			int merge_index = rand() % can_merge.size();
	//			os << merge_index << endl;
	//			merge(temp_connect, can_merge.at(merge_index));
	//		}
	//		num_merge += 1;
	//	}
	//	else if (choose_op == 2){
	//		os << "bridge" << endl;
	//		vector<vec4> can_bridge;
	//		check_bridge(temp_connect, can_bridge, myObj, merge_table);
	//		if (can_bridge.size() > 0){
	//			int bridge_index = rand() % can_bridge.size();
	//			bridge(temp_connect, can_bridge.at(bridge_index));
	//		}
	//		num_bridge += 1;
	//	}
	//	else {
	//		os << "kill" << endl;
	//		int result;
	//		do{
	//			result = rand() % test_connect.at(COLOR_WHITE).size();
	//		} while (!test_connect.at(COLOR_WHITE).at(result).exist);
	//		//			} while (!test_connect.at(COLOR_WHITE).at(result).exist || test_connect.at(COLOR_WHITE).at(result).outter);
	//		os << result << endl;
	//		kill(temp_connect, result);
	//		num_kill += 1;
	//	}

	//	finish = clock();
	//	duration = (float)(finish - start) / CLOCKS_PER_SEC;
	//	os << "op : " << duration << " s" << endl;

	//	judge_outter(temp_connect);

	//	start = clock();
	//	float term[4];
	//	float temp_e = compute_energy(temp_connect, myObj, cloud, term);
	//	finish = clock();
	//	duration = (float)(finish - start) / CLOCKS_PER_SEC;
	//	os << "energy : " << duration << " s" << endl;

	//	float p = (float)rand() / (float)RAND_MAX;
	//	os << p << " " << exp((origin_e - temp_e) / now_t) << endl;

	//	if (p < exp((origin_e - temp_e) / now_t)){
	//		if (collision_test(temp_connect, give_up)){

	//			//start = clock();
	//			test_connect = temp_connect;
	//			finish = clock();
	//			//duration = (float)(finish - start) / CLOCKS_PER_SEC;
	//			//cout << "copy : " << duration << " s" << endl;

	//			os << "accept energy : " << temp_e << endl;
	//			os << "energy(dist) : " << term[0] << endl;
	//			os << "energy(angle) : " << term[1] << endl;
	//			os << "energy(total_number) : " << term[2] << endl;
	//			os << "energy(use_stick) : " << term[3] << endl;

	//			if (temp_e < origin_e){
	//				energy_smaller_accept += 1;
	//			}
	//			else{
	//				energy_bigger_accept += 1;
	//			}

	//			origin_e = temp_e;

	//			//output_zometool(temp_connect, string(to_string(i) + ".obj"));
	//		}
	//		else{
	//			collision += 1;
	//			os << "reject energy : " << temp_e << endl;
	//			os << "energy(dist) : " << term[0] << endl;
	//			os << "energy(angle) : " << term[1] << endl;
	//			os << "energy(total_number) : " << term[2] << endl;
	//			os << "energy(use_stick) : " << term[3] << endl;

	//			if (temp_e < origin_e){
	//				energy_smaller_reject += 1;
	//			}
	//			else{
	//				energy_bigger_reject += 1;
	//			}
	//		}
	//	}
	//	else{
	//		os << "reject energy : " << temp_e << endl;
	//		os << "energy(dist) : " << term[0] << endl;
	//		os << "energy(angle) : " << term[1] << endl;
	//		os << "energy(total_number) : " << term[2] << endl;
	//		os << "energy(use_stick) : " << term[3] << endl;

	//		if (temp_e < origin_e){
	//			energy_smaller_reject += 1;
	//		}
	//		else{
	//			energy_bigger_reject += 1;
	//		}
	//	}
	//	os << "T : " << now_t << endl;;
	//	os << endl;
	//}

	/*float final_term[4];
	float final_e = compute_energy(test_connect, myObj, cloud, final_term);

	os << "final energy : " << final_e << endl;
	os << "final energy(dist) : " << final_term[0] << endl;
	os << "final energy(angle) : " << final_term[1] << endl;
	os << "final energy(total_number) : " << final_term[2] << endl;
	os << "final energy(use_stick) : " << final_term[3] << endl;
	os << endl;

	os << "collision : " << collision << " " << num_iteration << endl;
	os << "split : " << num_split << " merge : " << num_merge << " bridge : " << num_bridge << " kill : " << num_kill << endl;
	os << "ball-to-ball : " << give_up[0] << " ball-to-rod :  " << give_up[1] << " rod-to-rod :  " << give_up[2] << endl;

	os << "Z' < Z and accept : " << energy_smaller_accept << endl;
	os << "Z' > Z and accept : " << energy_bigger_accept << endl;
	os << "Z' < Z and reject : " << energy_smaller_reject << endl;
	os << "Z' > Z and reject : " << energy_bigger_reject << endl;
	os << endl;

	vec3 count[4];
	count_struct(test_connect, count);
	os << "BLUE : S => " << count[0][0] << ", M => " << count[0][1] << ", L => " << count[0][2] << endl;
	os << "Red : S => " << count[1][0] << ", M => " << count[1][1] << ", L => " << count[1][2] << endl;
	os << "Yellow : S => " << count[2][0] << ", M => " << count[2][1] << ", L => " << count[2][2] << endl;
	os << "Ball : " << count[3][0] << endl;*/

	/*total_finish = clock();
	duration = (float)(total_finish - total_start) / CLOCKS_PER_SEC;
	os << endl << "totoal time : " << duration << " s" << endl;

	os.close();*/

	/*vector<simple_material> materials_color, material_energy_dist, material_energy_angle, material_use_stick;
	kdtree_near_node_colorful(myObj, test_connect, materials_color);

	output_zometool(test_connect, string("2000_0.obj"));
	output_struc(test_connect, string("2000_0.txt"));

	output_material(materials_color, std::string("2000_0_colorful.mtl"));
	output_zometool_exp(test_connect, string("fake_2000_0(colorful).obj"), materials_color, std::string("2000_0_colorful.mtl"), COLORFUL);
	glmWriteOBJ_EXP(myObj, "fake_model_2000_0(colorful).obj", materials_color, std::string("2000_0_colorful.mtl"), COLORFUL);

	kdtree_near_node_energy_dist(myObj, test_connect, material_energy_dist);
	output_material(material_energy_dist, std::string("2000_0_energy_dist.mtl"));
	glmWriteOBJ_EXP(myObj, "fake_model_2000_0(dist).obj", material_energy_dist, std::string("2000_0_energy_dist.mtl"), ENERGY_DIST);

	energy_material(test_connect, material_energy_angle, ENERGY_ANGLE);
	output_material(material_energy_angle, std::string("2000_0_energy_angle.mtl"));
	output_zometool_exp(test_connect, string("fake_energy_2000_0(angle).obj"), material_energy_angle, std::string("2000_0_energy_angle.mtl"), ENERGY_ANGLE);

	energy_material(test_connect, material_use_stick, ENERGY_USE_STICK);
	output_material(material_use_stick, std::string("2000_0_energy_use_stick.mtl"));
	output_zometool_exp(test_connect, string("fake_energy_2000_0(use_stick).obj"), material_use_stick, std::string("2000_0_energy_use_stick.mtl"), ENERGY_USE_STICK);*/
	
	vector<simple_material> materials_graph_cut;
	test_graph_cut(myObj, test_connect, materials_graph_cut, color_material);
	output_material(materials_graph_cut, std::string("graph_cut10000_normal(data_smooth).mtl"));
	glmWriteOBJ_EXP(myObj, "graph_cut10000_normal(data_smooth).obj", materials_graph_cut, std::string("graph_cut10000_normal(data_smooth).mtl"), GRAPH_CUT);
	
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
	system("pause");
	return 0;
}
