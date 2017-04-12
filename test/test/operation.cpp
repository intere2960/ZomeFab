#include "operation.h"
#include "global.h"
#include <omp.h>

void split(std::vector<std::vector<zomeconn>> &test_connect, int s_index, GLMmodel *model, zometable &splite_table)
{
	zomedir t;

	int c_index;
	float dist = 100000000000000000.0f;

	std::vector<int> use_index;
	for (unsigned int i = 0; i < 62; i += 1){
		if (test_connect.at(COLOR_WHITE).at(s_index).connect_stick[i] != vec2(-1.0f, -1.0f)){
			//vec2 test = test_connect.at(COLOR_WHITE).at(s_index).connect_stick[i];
			////cout << i << " : " << test[0] << " " << test[1] << endl;
			//if (test_connect.at(test[0]).at(test[1]).surface_d < dist){
			//	dist = test_connect.at(test[0]).at(test[1]).surface_d;
			//	c_index = i;
			//}
			use_index.push_back(i);
		}
	}
	int vector_index = rand() % use_index.size();
	c_index = use_index.at(vector_index);

	vec2 test = test_connect.at(COLOR_WHITE).at(s_index).connect_stick[c_index];
	//cout << "test : " << test[0] << " " << test[1] << endl;
	int from_face = test_connect.at((int)test[0]).at((int)test[1]).fromface;
	int from_size = test_connect.at((int)test[0]).at((int)test[1]).size;

	//cout << "from_face : " << from_face << endl;
	//cout << "from_size : " << from_size << endl;

	vec3 use_ball_p = test_connect.at(COLOR_WHITE).at(s_index).position;

	//cout << "use_ball_p : " << use_ball_p[0] << " " << use_ball_p[1] << " " << use_ball_p[2] << endl;

	vec3 use_stick_p = test_connect.at((int)test[0]).at((int)test[1]).position;
	vec3 judge = use_stick_p + 0.5f * t.dir->at(from_face) * t.color_length((int)test[0], from_size) - use_ball_p;

	int toward_ball_index = (int)test_connect.at((int)test[0]).at((int)test[1]).towardindex[1];

	//cout << "judge.length() : " << judge.length() << endl;

	if (from_face != c_index)
		from_face = c_index;

	//if (judge.length() < 0.001){
	if (toward_ball_index == s_index){
		//cout << "in" << endl;
		toward_ball_index = (int)test_connect.at((int)test[0]).at((int)test[1]).fromindex[1];
	}

	vec3 toward_p = test_connect.at(COLOR_WHITE).at((int)test_connect.at((int)test[0]).at((int)test[1]).towardindex[1]).position;

	//cout << "fuck you : " << s_index << " " << toward_ball_index << endl;

	//cout << "from_face : " << from_face << endl;

	std::vector<zomerecord> temp;

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

	int choose_split = 0;
	float split_dist = 10000000000000000.0f;
	for (unsigned int i = 0; i < temp.size(); i += 1){
		vec3 test_d = use_ball_p + t.dir->at((int)temp.at(i).travel_1[0]) * t.face_length((int)temp.at(i).travel_1[0], (int)temp.at(i).travel_1[1]);
		bool dist = (ball_surface_dist(model, test_d) < 100000000000000.0f);
		bool insect = !check_stick_intersect(model, test_d, use_ball_p) && !check_stick_intersect(model, (test_d + use_ball_p) / 2.0f, use_ball_p) && !check_stick_intersect(model, test_d, (test_d + use_ball_p) / 2.0f) && !check_stick_intersect(model, test_d, toward_p) && !check_stick_intersect(model, (test_d + use_ball_p) / 2.0f, toward_p) && !check_stick_intersect(model, test_d, (test_d + use_ball_p) / 2.0f);
		bool not_near = (ball_surface_dist(model, (test_d + use_ball_p) / 2.0f) < 100000000000000.0f) && (ball_surface_dist(model, (test_d + toward_p) / 2.0f) < 100000000000000.0f);
		if (!(dist && insect && not_near)){
			temp.erase(temp.begin() + i);
			i -= 1;
		}
	}

	if (temp.size() > 0){
		choose_split = rand() % temp.size();
		//cout << "choose_split : " << choose_split << endl;

		//cout << "o : " << temp.at(choose_split).origin[0] << " " << temp.at(choose_split).origin[1] << " " << temp.at(choose_split).origin[2] << endl;
		//cout << "t1 : " << temp.at(choose_split).travel_1[0] << " " << temp.at(choose_split).travel_1[1] << " " << temp.at(choose_split).travel_1[2] << endl;
		//cout << "t2 : " << temp.at(choose_split).travel_2[0] << " " << temp.at(choose_split).travel_2[1] << " " << temp.at(choose_split).travel_2[2] << endl;

		zomeconn new_ball;
		zomeconn new_stick_f, new_stick_t;

		new_ball.position = test_connect.at(COLOR_WHITE).at(s_index).position + t.dir->at((int)temp.at(choose_split).travel_1[0]) * t.face_length((int)temp.at(choose_split).travel_1[0], (int)temp.at(choose_split).travel_1[1]);
		new_ball.color = COLOR_WHITE;

		new_stick_f.position = test_connect.at(COLOR_WHITE).at(s_index).position + t.dir->at((int)temp.at(choose_split).travel_1[0]) * t.face_length((int)temp.at(choose_split).travel_1[0], (int)temp.at(choose_split).travel_1[1]) / 2;
		new_stick_f.color = t.face_color((int)temp.at(choose_split).travel_1[0]);
		new_stick_f.size = (int)temp.at(choose_split).travel_1[1];
		new_stick_f.fromface = (int)temp.at(choose_split).travel_1[0];
		new_stick_f.towardface = (int)temp.at(choose_split).travel_1[2];

		//cout << "f:" << endl;
		//cout << new_stick_f.color << endl;
		//cout << new_stick_f.size << endl;
		//cout << new_stick_f.fromface << endl;
		//cout << new_stick_f.towardface << endl;

		new_stick_t.position = test_connect.at(COLOR_WHITE).at(toward_ball_index).position + t.dir->at((int)temp.at(choose_split).travel_2[0]) * t.face_length((int)temp.at(choose_split).travel_2[0], (int)temp.at(choose_split).travel_2[1]) / 2;
		new_stick_t.color = t.face_color((int)temp.at(choose_split).travel_2[0]);
		new_stick_t.size = (int)temp.at(choose_split).travel_2[1];
		new_stick_t.fromface = (int)temp.at(choose_split).travel_2[0];
		new_stick_t.towardface = (int)temp.at(choose_split).travel_2[2];

		//cout << "t:" << endl;
		//cout << new_stick_t.color << endl;
		//cout << new_stick_t.size << endl;
		//cout << new_stick_t.fromface << endl;
		//cout << new_stick_t.towardface << endl;

		//cout << toward_ball_index << endl;
		//cout << new_stick_t.fromface << endl;
		//cout << new_stick_t.towardface << endl;

		test_connect.at(COLOR_WHITE).at(s_index).connect_stick[new_stick_f.fromface] = vec2((float)new_stick_f.color, (float)test_connect.at(new_stick_f.color).size());
		new_ball.connect_stick[new_stick_f.towardface] = vec2((float)new_stick_f.color, (float)test_connect.at(new_stick_f.color).size());

		new_stick_f.fromindex = vec2((float)COLOR_WHITE, (float)s_index);
		new_stick_f.towardindex = vec2((float)COLOR_WHITE, (float)test_connect.at(COLOR_WHITE).size());
		test_connect.at(new_stick_f.color).push_back(new_stick_f);

		test_connect.at(COLOR_WHITE).at(toward_ball_index).connect_stick[new_stick_t.fromface] = vec2((float)new_stick_t.color, (float)test_connect.at(new_stick_t.color).size());
		new_ball.connect_stick[new_stick_t.towardface] = vec2((float)new_stick_t.color, (float)test_connect.at(new_stick_t.color).size());

		new_stick_t.fromindex = vec2((float)COLOR_WHITE, (float)toward_ball_index);
		new_stick_t.towardindex = vec2((float)COLOR_WHITE, (float)test_connect.at(COLOR_WHITE).size());
		test_connect.at(new_stick_t.color).push_back(new_stick_t);

		test_connect.at(COLOR_WHITE).push_back(new_ball);
	}
}

void check_merge(std::vector<std::vector<zomeconn>> &test_connect, std::vector<vec4> &can_merge, GLMmodel *model, zometable &merge_table)
{
	zomedir t;

	//#pragma omp parallel for
	for (int i = 0; i < test_connect.at(COLOR_WHITE).size(); i += 1){
		if (test_connect.at(COLOR_WHITE).at(i).exist){
			std::vector<int> use_slot;
			for (int j = 0; j < 62; j += 1){
				if (test_connect.at(COLOR_WHITE).at(i).connect_stick[j] != vec2(-1.0f, -1.0f)){
					use_slot.push_back(j);
				}
			}

			//#pragma omp parallel for
			for (int j = 0; j < use_slot.size() - 1; j += 1){

				int use_index = (int)test_connect.at((int)test_connect.at(COLOR_WHITE).at(i).connect_stick[use_slot.at(j)][0]).at((int)test_connect.at(COLOR_WHITE).at(i).connect_stick[use_slot.at(j)][1]).towardindex[1];
				if (use_index == i)
					use_index = (int)test_connect.at((int)test_connect.at(COLOR_WHITE).at(i).connect_stick[use_slot.at(j)][0]).at((int)test_connect.at(COLOR_WHITE).at(i).connect_stick[use_slot.at(j)][1]).fromindex[1];
				vec3 use_p = test_connect.at(COLOR_WHITE).at(use_index).position;

				//#pragma omp parallel for
				for (int k = j + 1; k < use_slot.size(); k += 1){
					int opp_face = t.opposite_face(use_slot.at(j));

					#pragma omp parallel for
					for (int a = 0; a < merge_table.table.at(opp_face).at(use_slot.at(k)).size(); a += 1){
						bool judge1 = (merge_table.table.at(opp_face).at(use_slot.at(k)).at(a).origin[1] == test_connect.at((int)test_connect.at(COLOR_WHITE).at(i).connect_stick[use_slot.at(j)][0]).at((int)test_connect.at(COLOR_WHITE).at(i).connect_stick[use_slot.at(j)][1]).size);
						bool judge2 = (merge_table.table.at(opp_face).at(use_slot.at(k)).at(a).travel_1[1] == test_connect.at((int)test_connect.at(COLOR_WHITE).at(i).connect_stick[use_slot.at(k)][0]).at((int)test_connect.at(COLOR_WHITE).at(i).connect_stick[use_slot.at(k)][1]).size);
						if (judge1 && judge2){

							int toward_index = (int)test_connect.at((int)test_connect.at(COLOR_WHITE).at(i).connect_stick[use_slot.at(k)][0]).at((int)test_connect.at(COLOR_WHITE).at(i).connect_stick[use_slot.at(k)][1]).towardindex[1];
							if (toward_index == i)
								toward_index = (int)test_connect.at((int)test_connect.at(COLOR_WHITE).at(i).connect_stick[use_slot.at(k)][0]).at((int)test_connect.at(COLOR_WHITE).at(i).connect_stick[use_slot.at(k)][1]).fromindex[1];
							vec3 toward_p = test_connect.at(COLOR_WHITE).at(toward_index).position;


							bool add = true;
							for (unsigned int b = 0; b < can_merge.size(); b += 1){
								if ((can_merge.at(b)[0] == toward_index) && (can_merge.at(b)[1] == use_index)){
									add = false;
								}
							}

							vec3 test_dir = (test_connect.at(COLOR_WHITE).at(toward_index).position - test_connect.at(COLOR_WHITE).at(use_index).position).normalize();
							int test_index = t.dir_face(test_dir);

							if (test_connect.at(COLOR_WHITE).at(use_index).connect_stick[test_index] != vec2(-1.0f, -1.0f)){
								add = false;
							}

							if (add){
								vec3 test_p = (test_connect.at(COLOR_WHITE).at(use_index).position + test_connect.at(COLOR_WHITE).at(toward_index).position) / 2.0f;

								bool judge = !check_stick_intersect(model, use_p, toward_p) && !check_stick_intersect(model, test_p, use_p) && !check_stick_intersect(model, test_p, toward_p);
								if (judge){
									vec4 connect((float)use_index, (float)toward_index, (float)t.face_color((int)merge_table.table.at(opp_face).at(use_slot.at(k)).at(a).travel_2[0]), (float)merge_table.table.at(opp_face).at(use_slot.at(k)).at(a).travel_2[1]);
									can_merge.push_back(connect);
								}
							}
						}
					}
				}
			}
		}
	}
}

void merge(std::vector<std::vector<zomeconn>> &test_connect, vec4 &merge_vector)
{
	zomedir t;

	vec3 p1 = test_connect.at(COLOR_WHITE).at((int)merge_vector[0]).position;
	vec3 p2 = test_connect.at(COLOR_WHITE).at((int)merge_vector[1]).position;
	vec3 v = (p2 - p1).normalize();

	vec3 stick_p = (p1 + p2) / 2.0f;
	int from_face = t.dir_face(v);
	int toward_face = t.opposite_face(from_face);

	zomeconn new_stick;
	new_stick.position = stick_p;
	new_stick.color = (int)merge_vector[2];
	new_stick.size = (int)merge_vector[3];
	new_stick.fromface = from_face;
	new_stick.fromindex = vec2(COLOR_WHITE, merge_vector[0]);
	new_stick.towardface = toward_face;
	new_stick.towardindex = vec2(COLOR_WHITE, merge_vector[1]);

	test_connect.at(COLOR_WHITE).at((int)merge_vector[0]).connect_stick[from_face] = vec2((float)new_stick.color, (float)test_connect.at(new_stick.color).size());
	test_connect.at(COLOR_WHITE).at((int)merge_vector[1]).connect_stick[toward_face] = vec2((float)new_stick.color, (float)test_connect.at(new_stick.color).size());

	test_connect.at(new_stick.color).push_back(new_stick);
}

void check_bridge(std::vector<std::vector<zomeconn>> &test_connect, std::vector<vec4> &can_bridge, GLMmodel *model, zometable &merge_table)
{
	zomedir t;

	//#pragma omp parallel for
	for (int i = 0; i < test_connect.at(COLOR_WHITE).size(); i += 1){
		if (test_connect.at(COLOR_WHITE).at(i).exist){
			//#pragma omp parallel for
			for (int j = 0; j < test_connect.at(COLOR_WHITE).size(); j += 1){
				if (i != j && test_connect.at(COLOR_WHITE).at(j).exist){
					vec3 test_l = test_connect.at(COLOR_WHITE).at(j).position - test_connect.at(COLOR_WHITE).at(i).position;
					vec3 test_n = (test_connect.at(COLOR_WHITE).at(j).position - test_connect.at(COLOR_WHITE).at(i).position).normalize();
					int test_index = t.dir_face(test_n);
					if (test_index != -1){
						if (test_connect.at(COLOR_WHITE).at(i).connect_stick[test_index] == vec2(-1.0f, -1.0f)){
							float l = test_l.length();

							#pragma omp parallel for
							for (int size = 0; size < 3; size += 1){
								if (fabs(t.face_length(test_index, size) - l) < 0.001f){
									bool add = true;
									for (unsigned int k = 0; k < can_bridge.size(); k += 1){
										if ((can_bridge.at(k)[0] == j) && (can_bridge.at(k)[1] == i)){
											add = false;
										}
									}

									if (add){
										vec3 test_p = (test_connect.at(COLOR_WHITE).at(i).position + test_connect.at(COLOR_WHITE).at(j).position) / 2.0f;
										vec3 from_p = test_connect.at(COLOR_WHITE).at(i).position;
										vec3 toward_p = test_connect.at(COLOR_WHITE).at(j).position;
										bool judge = !check_stick_intersect(model, from_p, toward_p) && !check_stick_intersect(model, test_p, from_p) && !check_stick_intersect(model, test_p, toward_p);
										if (judge){
											vec4 connect((float)i, (float)j, (float)t.face_color(test_index), (float)size);
											can_bridge.push_back(connect);
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void bridge(std::vector<std::vector<zomeconn>> &test_connect, vec4 &bridge_vector)
{
	zomedir t;

	vec3 p1 = test_connect.at(COLOR_WHITE).at((int)bridge_vector[0]).position;
	vec3 p2 = test_connect.at(COLOR_WHITE).at((int)bridge_vector[1]).position;
	vec3 v = (p2 - p1).normalize();

	vec3 stick_p = (p1 + p2) / 2.0f;
	int from_face = t.dir_face(v);
	int toward_face = t.opposite_face(from_face);

	zomeconn new_stick;
	new_stick.position = stick_p;
	new_stick.color = (int)bridge_vector[2];
	new_stick.size = (int)bridge_vector[3];
	new_stick.fromface = from_face;
	new_stick.fromindex = vec2(COLOR_WHITE, bridge_vector[0]);
	new_stick.towardface = toward_face;
	new_stick.towardindex = vec2(COLOR_WHITE, bridge_vector[1]);

	test_connect.at(COLOR_WHITE).at((int)bridge_vector[0]).connect_stick[from_face] = vec2((float)new_stick.color, (float)test_connect.at(new_stick.color).size());
	test_connect.at(COLOR_WHITE).at((int)bridge_vector[1]).connect_stick[toward_face] = vec2((float)new_stick.color, (float)test_connect.at(new_stick.color).size());

	test_connect.at(new_stick.color).push_back(new_stick);
}

void kill(std::vector<std::vector<zomeconn>> &test_connect, int index)
{
	std::vector<vec2> use_stick;
	for (int i = 0; i < 62; i += 1){
		if (test_connect.at(COLOR_WHITE).at(index).connect_stick[i] != vec2(-1.0f, -1.0f))
			use_stick.push_back(test_connect.at(COLOR_WHITE).at(index).connect_stick[i]);
	}

	test_connect.at(COLOR_WHITE).at(index).exist = false;

	for (unsigned int i = 0; i < use_stick.size(); i += 1){
		int fromface = test_connect.at(use_stick.at(i)[0]).at(use_stick.at(i)[1]).fromface;
		vec2 fromindex = test_connect.at(use_stick.at(i)[0]).at(use_stick.at(i)[1]).fromindex;
		int towardface = test_connect.at(use_stick.at(i)[0]).at(use_stick.at(i)[1]).towardface;
		vec2 towardindex = test_connect.at(use_stick.at(i)[0]).at(use_stick.at(i)[1]).towardindex;

		test_connect.at(fromindex[0]).at(fromindex[1]).connect_stick[fromface] = vec2(-1.0f, -1.0f);
		bool vanish = true;
		for (int j = 0; j < 62; j += 1){
			if (test_connect.at(fromindex[0]).at(fromindex[1]).connect_stick[j] != vec2(-1.0f, -1.0f)){
				vanish = false;
				break;
			}
		}
		if (vanish)
			test_connect.at(fromindex[0]).at(fromindex[1]).exist = false;

		test_connect.at(towardindex[0]).at(towardindex[1]).connect_stick[towardface] = vec2(-1.0f, -1.0f);
		vanish = true;
		for (int j = 0; j < 62; j += 1){
			if (test_connect.at(towardindex[0]).at(towardindex[1]).connect_stick[j] != vec2(-1.0f, -1.0f)){
				vanish = false;
				break;
			}
		}
		if (vanish)
			test_connect.at(towardindex[0]).at(towardindex[1]).exist = false;

		test_connect.at(use_stick.at(i)[0]).at(use_stick.at(i)[1]).exist = false;
	}

}

float forbidden_energy(float dist)
{
	zomedir t;
	vec2 p1(t.color_length(COLOR_BLUE, SIZE_S) / 3.0f, 0.0f);
	vec2 p2(t.color_length(COLOR_BLUE, SIZE_S) * 1.0f, 70.0f);
	float a = p2[0] / pow((p2[0] - p1[0]), 2);

	if (dist > p2[0]){
		return p2[1];
	}
	if (dist < p1[0]){
		return 0.0f;
	}

	return a * pow((dist - p1[0]), 2);
}

float compute_energy(std::vector<std::vector<zomeconn>> &test_connect, GLMmodel *model, float *term)
{
	float energy = 0.0f;
	zomedir t;

	float energy_dist = 0.0f;

	time_t start, finish;
	float duration;

	//#pragma omp parallel for
	for (int a = 0; a < test_connect.size(); a += 1){
		//#pragma omp parallel for
		for (int i = 0; i < test_connect.at(a).size(); i += 1){
			if (test_connect.at(a).at(i).exist){
				//cout << a << " " << i << endl;
				//cout << test_connect.at(a).at(i).position[0] << " " << test_connect.at(a).at(i).position[1] << " " << test_connect.at(a).at(i).position[2] << endl;
				if (test_connect.at(a).at(i).surface_d == 100000000000000.0f){
					//start = clock();
					test_connect.at(a).at(i).surface_d = point_surface_dist(model, test_connect.at(a).at(i).position);
					test_connect.at(a).at(i).energy_d = pow(test_connect.at(a).at(i).surface_d, 2) * (1.0f + forbidden_energy(test_connect.at(a).at(i).surface_d));
					//finish = clock();
					//duration = (float)(finish - start) / CLOCKS_PER_SEC;
					//cout << "point_surface_dist : " << duration << " s" << endl;
				}

				//start = clock();

				//energy_dist += pow(test_connect.at(a).at(i).surface_d, 2) * (1.0 + forbidden_energy(test_connect.at(a).at(i).surface_d));
				energy_dist += test_connect.at(a).at(i).energy_d;

				//finish = clock();
				//duration = (float)(finish - start) / CLOCKS_PER_SEC;
				//cout << "point_surface_dist : " << duration << " s" << endl;
			}
		}
	}
	//cout << "reuse : " << reuse << endl;

	energy_dist /= (pow(t.color_length(COLOR_BLUE, SIZE_S), 2) * (test_connect.at(COLOR_BLUE).size() + test_connect.at(COLOR_RED).size() + test_connect.at(COLOR_YELLOW).size() + test_connect.at(COLOR_WHITE).size()));

	float energy_angle = 0.0f;
	#pragma omp parallel for
	for (int i = 0; i < test_connect.at(COLOR_WHITE).size(); i += 1){
		if (test_connect.at(COLOR_WHITE).at(i).exist){
			std::vector<int> use_stick;
			for (int j = 0; j < 62; j += 1){
				if (test_connect.at(COLOR_WHITE).at(i).connect_stick[j] != vec2(-1.0f, -1.0f)){
					use_stick.push_back(j);
				}
			}

			float sum_angle = 0.0f;
			#pragma omp parallel for
			for (int j = 0; j < use_stick.size() - 1; j += 1){
				#pragma omp parallel for
				for (int k = j; k < use_stick.size(); k += 1){
					sum_angle += fabs(t.near_angle.at(use_stick.at(j)).at(use_stick.at(k)) - M_PI / 2.0f);
				}
			}

			energy_angle += sum_angle;
		}
	}

	energy_angle /= test_connect.at(COLOR_WHITE).size();

	float energy_number = 0.0f;
	float target_number = 1000.0f;

	int number = 0;
	for (int i = 0; i < test_connect.at(COLOR_WHITE).size(); i += 1){
		if (test_connect.at(COLOR_WHITE).at(i).exist){
			number += 1;
		}
	}

	energy_number = pow((number - target_number), 2.0f) / target_number;

	//float energy_fair = 0.0f;
	//for (unsigned int i = 0; i < test_connect.at(COLOR_WHITE).size(); i += 1){
	//	vec3 temp_ring_p;
	//	int use_stick = 0;
	//	for (unsigned int j = 0; j < 62; j += 1){
	//		if (test_connect.at(COLOR_WHITE).at(i).connect_stick[j] != vec2(-1.0f, -1.0f)){
	//			//cout << "abcd : " << test_connect.at(COLOR_WHITE).at(i).connect_stick[j][0] << " " << test_connect.at(COLOR_WHITE).at(i).connect_stick[j][1] << endl;
	//			temp_ring_p += test_connect.at(test_connect.at(COLOR_WHITE).at(i).connect_stick[j][0]).at(test_connect.at(COLOR_WHITE).at(i).connect_stick[j][1]).position;
	//			use_stick += 1;
	//		}
	//	}
	//	temp_ring_p /= use_stick;
	//	energy_fair += (test_connect.at(COLOR_WHITE).at(i).position - temp_ring_p).length2();
	//}
	//energy_fair /= (pow(t.color_length(COLOR_BLUE, SIZE_S), 2) * test_connect.at(COLOR_WHITE).size());

	//energy = energy_dist + energy_fair;
	//energy = energy_dist + 0.1 * energy_angle;
	energy = energy_dist + 0.1 * energy_angle + energy_number;
	/*cout << "energy : " << energy << endl;
	cout << "energy_dist : " << energy_dist << endl;
	cout << "energy_angle : " << energy_angle << endl;*/
	term[0] = energy_dist;
	term[1] = energy_angle;
	term[2] = energy_number;

	return energy;
}