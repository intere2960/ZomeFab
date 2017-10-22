#include <cmath>
#include <fstream>
#include "zomedir.h"
#include "glui_internal.h"
#include "global.h"

zomedir::zomedir()
{
	dir = new std::vector<vec3>();
	//blue
	//0~5
	dir->push_back(vec3(1.0, 0.0, 0.0));
	dir->push_back(vec3(-1.0, 0.0, 0.0));
	dir->push_back(vec3(0.0, 1.0, 0.0));
	dir->push_back(vec3(0.0, -1.0, 0.0));
	dir->push_back(vec3(0.0, 0.0, 1.0));
	dir->push_back(vec3(0.0, 0.0, -1.0));
	// diagonals
    // around x axis (6-13)
    dir->push_back(vec3(0.5f * golden, 0.5f / golden, 0.5f));
	dir->push_back(vec3(0.5f * golden, -0.5f / golden, 0.5f));
	dir->push_back(vec3(-0.5f * golden, 0.5f / golden, 0.5f));
	dir->push_back(vec3(-0.5f * golden, -0.5f / golden, 0.5f));

	dir->push_back(vec3(-0.5f * golden, -0.5f / golden, -0.5f));
	dir->push_back(vec3(-0.5f * golden, 0.5f / golden, -0.5f));
	dir->push_back(vec3(0.5f * golden, -0.5f / golden, -0.5f));
	dir->push_back(vec3(0.5f * golden, 0.5f / golden, -0.5f));

    // around y axis (14-21)
    dir->push_back(vec3(0.5f, 0.5f * golden, 0.5f / golden));
	dir->push_back(vec3(-0.5f, 0.5f * golden, 0.5f / golden));
	dir->push_back(vec3(0.5f, -0.5f * golden, 0.5f / golden));
	dir->push_back(vec3(-0.5f, -0.5f * golden, 0.5f / golden));

	dir->push_back(vec3(-0.5f, -0.5f * golden, -0.5f / golden));
	dir->push_back(vec3(0.5f, -0.5f * golden, -0.5f / golden));
	dir->push_back(vec3(-0.5f, 0.5f * golden, -0.5f / golden));
	dir->push_back(vec3(0.5f, 0.5f * golden, -0.5f / golden));

    // around z axis (22-29)
    dir->push_back(vec3(0.5f / golden, 0.5f, 0.5f * golden));
	dir->push_back(vec3(-0.5f / golden, 0.5f, 0.5f * golden));
	dir->push_back(vec3(0.5f / golden, -0.5f, 0.5f * golden));
	dir->push_back(vec3(-0.5f / golden, -0.5f, 0.5f * golden));

	dir->push_back(vec3(-0.5f / golden, -0.5f, -0.5f * golden));
	dir->push_back(vec3(0.5f / golden, -0.5f, -0.5f * golden));
	dir->push_back(vec3(-0.5f / golden, 0.5f, -0.5f * golden));
	dir->push_back(vec3(0.5f / golden, 0.5f, -0.5f * golden));

    // reds (diagonals in planes)
    // around x axis (30-33)
    dir->push_back(vec3(golden * sqrt((5 - sqrt(5.0f)) / 10), sqrt((5 - sqrt(5.0f)) / 10), 0.0)); // (5 + sqrt(5.0f)) / 2 = (5 - sqrt(5.0f)) / 10
	dir->push_back(vec3(golden * sqrt((5 - sqrt(5.0f)) / 10), -sqrt((5 - sqrt(5.0f)) / 10), 0.0));
	dir->push_back(vec3(-golden * sqrt((5 - sqrt(5.0f)) / 10), -sqrt((5 - sqrt(5.0f)) / 10), 0.0));
	dir->push_back(vec3(-golden * sqrt((5 - sqrt(5.0f)) / 10), sqrt((5 - sqrt(5.0f)) / 10), 0.0));

    // around y axis (34-37)
    dir->push_back(vec3(0.0, golden * sqrt((5 - sqrt(5.0f)) / 10), sqrt((5 - sqrt(5.0f)) / 10)));
	dir->push_back(vec3(0.0, golden * sqrt((5 - sqrt(5.0f)) / 10), -sqrt((5 - sqrt(5.0f)) / 10)));
	dir->push_back(vec3(0.0, -golden * sqrt((5 - sqrt(5.0f)) / 10), -sqrt((5 - sqrt(5.0f)) / 10)));
	dir->push_back(vec3(0.0, -golden * sqrt((5 - sqrt(5.0f)) / 10), sqrt((5 - sqrt(5.0f)) / 10)));

    // around z axis (38-41)
    dir->push_back(vec3(sqrt((5 - sqrt(5.0f)) / 10), 0.0f, golden * sqrt((5 - sqrt(5.0f)) / 10)));
	dir->push_back(vec3(-sqrt((5 - sqrt(5.0f)) / 10), 0.0f, golden * sqrt((5 - sqrt(5.0f)) / 10)));
	dir->push_back(vec3(-sqrt((5 - sqrt(5.0f)) / 10), 0.0f, -golden * sqrt((5 - sqrt(5.0f)) / 10)));
	dir->push_back(vec3(sqrt((5 - sqrt(5.0f)) / 10), 0.0f, -golden * sqrt((5 - sqrt(5.0f)) / 10)));

    // yellows (diagonals in planes)
    // around x axis (42-35)
    dir->push_back(vec3(golden / sqrt(3.0f), 0.0f,  1.0f / golden / sqrt(3.0f)));
	dir->push_back(vec3(golden / sqrt(3.0f), 0.0f, -1.0f / golden / sqrt(3.0f)));
	dir->push_back(vec3(-golden / sqrt(3.0f), 0.0f, -1.0f / golden / sqrt(3.0f)));
	dir->push_back(vec3(-golden / sqrt(3.0f), 0.0f, 1.0f / golden / sqrt(3.0f)));

    // around y axis (46-49)
    dir->push_back(vec3(1.0f / golden / sqrt(3.0f), golden / sqrt(3.0f), 0.0f));
	dir->push_back(vec3(-1.0f / golden / sqrt(3.0f), golden / sqrt(3.0f), 0.0f));
	dir->push_back(vec3(-1.0f / golden / sqrt(3.0f), -golden / sqrt(3.0f), 0.0f));
	dir->push_back(vec3(1.0f / golden / sqrt(3.0f), -golden / sqrt(3.0f), 0.0f));

    // around z axis (50-53)
    dir->push_back(vec3(0.0f, 1.0f / golden / sqrt(3.0f), golden / sqrt(3.0f)));
	dir->push_back(vec3(0.0f, -1.0f / golden / sqrt(3.0f), golden / sqrt(3.0f)));
	dir->push_back(vec3(0.0f, -1.0f / golden / sqrt(3.0f), -golden / sqrt(3.0f)));
	dir->push_back(vec3(0.0f, 1.0f / golden / sqrt(3.0f), -golden / sqrt(3.0f)));

    // 54-61 (diagonals)
    dir->push_back(vec3(sqrt(3.0f) / 3 , sqrt(3.0f) / 3 , sqrt(3.0f) / 3));
	dir->push_back(vec3(-sqrt(3.0f) / 3 , sqrt(3.0f) / 3 , sqrt(3.0f) / 3));
	dir->push_back(vec3(sqrt(3.0f) / 3 , -sqrt(3.0f) / 3 , sqrt(3.0f) / 3));
	dir->push_back(vec3(sqrt(3.0f) / 3 , sqrt(3.0f) / 3 , -sqrt(3.0f) / 3));
	dir->push_back(vec3(sqrt(3.0f) / 3 , -sqrt(3.0f) / 3 , -sqrt(3.0f) / 3));
	dir->push_back(vec3(-sqrt(3.0f) / 3 , sqrt(3.0f) / 3 , -sqrt(3.0f) / 3));
	dir->push_back(vec3(-sqrt(3.0f) / 3 , -sqrt(3.0f) / 3 , sqrt(3.0f) / 3));
	dir->push_back(vec3(-sqrt(3.0f) / 3 , -sqrt(3.0f) / 3 , -sqrt(3.0f) / 3));

    for(unsigned int i = 0; i < 62; i += 1)
        dir->at(i) = dir->at(i).normalize();

	double dir_angles[62][62];
	for (int i = 0; i< 62; i += 1)
	{
		for (int j = 0; j < 62; j += 1)
		{
			double tmpDot = (dir->at(i) * dir->at(j));
			if (tmpDot < -1.0)
				tmpDot = -1.0;
			if (tmpDot > 1.0)
				tmpDot = 1.0;
			dir_angles[i][j] = acos(tmpDot);
		}
	}
	for (int i = 0; i < 62; i += 1)
		dir_angles[i][i] = 100.0;


	for (int i = 0; i < 62; i += 1)
	{
		std::vector<std::vector<int>> dirs_of_dir;
		for (int m = 0; m < 2; m += 1)
		{
			std::vector<int> dirs_in_layer;
			if (m == 0)
				dirs_in_layer.push_back(i);
			double tmpMin = 1e100;
			for (int j = 0; j < 62; j += 1)
			{
				if (dir_angles[i][j] < tmpMin)
					tmpMin = dir_angles[i][j];
			}

			for (int j = 0; j < 62; j += 1)
			{
				if (fabs(dir_angles[i][j] - tmpMin) < 0.3)
				{
					dirs_in_layer.push_back(j);
					dir_angles[i][j] = 100.0;
				}
			}
			dirs_of_dir.push_back(dirs_in_layer);
			dirs_in_layer.clear();
		}
		near_dir.push_back(dirs_of_dir);
		dirs_of_dir.clear();
	}

	for (int i = 0; i < 62; i += 1)
	{
		std::vector<float> angle;
		for (int j = 0; j < 62; j += 1)
		{
			vec3 from = dir->at(i);
			vec3 toward = dir->at(j);

			float dot = from * toward;
			float new_angle;
			if (j == i){
				new_angle = 0.0f;
			}
			else if (j == opposite_face(i)){
				new_angle = M_PI;

			}
			else{
				new_angle = acosf(dot);
			}
			angle.push_back(new_angle);
		}
		near_angle.push_back(angle);
	}
}

zomedir::~zomedir()
{
    if (!dir->empty()){
        std::vector<vec3> v;
        dir->swap(v);
    }
    delete dir;
}

float zomedir::phi(int index)
{
	float sinPhi = dir->at(index)[2] / sqrt(pow(dir->at(index)[0], 2) + pow(dir->at(index)[2], 2) );
	float phi = asin(sinPhi);
	if(dir->at(index)[0] < 0)
	{
		phi = M_PI - phi;
	}
	if(sqrt(pow(dir->at(index)[0], 2) + pow(dir->at(index)[2], 2)) < 0.0001)
	{
		phi = 0;
	}
	return -phi;
}

float zomedir::theta(int index)
{
	float sinTheta = dir->at(index)[1] / sqrt(pow(dir->at(index)[0] , 2) + pow(dir->at(index)[1] , 2) + pow(dir->at(index)[2] , 2));
	float theta = acos(sinTheta);
	return -theta;
}

float zomedir::roll(int index)
{
	float roll;
	if(index < 30)
	{
		float sinroll = dir->at(index)[2] / sqrt(pow(dir->at(index)[0], 2) + pow(dir->at(index)[2], 2));
		roll = asin(sinroll);
		if(index == 0) roll = M_PI * 0.5f;
		if(index == 1) roll = M_PI * 0.5f;
		if(index == 2) roll = 0.0;
		if(index == 3) roll = 0.0;
		if(index == 4) roll = 0.0;
		if(index == 5) roll = 0.0;

		if(index == 6) roll = M_PI - roll;

		if(index == 9) roll = M_PI - roll;
		if(index == 10) roll = M_PI - roll;
		if(index == 11);
		if(index == 12) roll = -(M_PI - roll);
		if(index == 13) roll = M_PI - roll;
		if(index == 14);
		if(index == 15) roll = M_PI - roll;
		if(index == 16) roll = -roll;
		if(index == 17) roll = -(M_PI - roll);
		if(index == 18);
		if(index == 19) roll = -roll;
		if(index == 20) roll = M_PI - roll;
		if(index == 22);
		if(index == 23) roll = M_PI - roll;
		if(index == 24) roll = M_PI - roll;
		if(index == 25) roll = -(M_PI - roll);
		if(index == 26);
		if(index == 27) roll = M_PI - roll;
		if(index == 28) roll = M_PI - roll;

		if ( 0 && sqrt(pow(dir->at(index)[0], 2) + pow(dir->at(index)[2], 2)) < 0.0001)
		{
			roll = 0;
		}
	}
	else if(index < 42)
	{
		if(index == 30) roll = 0;
		if(index == 31) roll = M_PI;
		if(index == 32) roll = M_PI;
		if(index == 33) roll = 0;
		if(index == 34) roll = M_PI;
		if(index == 35) roll = M_PI;
		if(index == 36) roll = 0;
		if(index == 37) roll = 0;
		if(index == 38) roll = M_PI * 0.5f;
		if(index == 39) roll = M_PI * 1.5f;
		if(index == 40) roll = M_PI * 0.5f;
		if(index == 41) roll = M_PI * 1.5f;
	}
	else
	{
		if(index == 42) roll = M_PI * 0.5f;
		if(index == 43) roll = M_PI * 1.5f;
		if(index == 44) roll = M_PI * 0.5f;
		if(index == 45) roll = M_PI * 1.5f;
		if(index == 46) roll = 0;
		if(index == 47) roll = 0;
		if(index == 48) roll = M_PI;
		if(index == 49) roll = M_PI;
		if(index == 50) roll = M_PI;
		if(index == 51) roll = 0;
		if(index == 52) roll = 0;
		if(index == 53) roll = M_PI;

		if(index == 54) roll = 22.2369f / 180.0f * M_PI;
		if(index == 55) roll = -22.2369f / 180.0f * M_PI;
		if(index == 56) roll = -22.2369f / 180.0f * M_PI + M_PI;
		if(index == 57) roll = -22.2369f / 180.0f * M_PI;
		if(index == 58) roll = 22.2369f / 180.0f * M_PI + M_PI;
		if(index == 59) roll = 22.2369f / 180.0f * M_PI;
		if(index == 60) roll = 22.2369f / 180.0f * M_PI + M_PI;
		if(index == 61) roll = -22.2369f / 180.0f * M_PI + M_PI;
	}
	return -roll;
}


float zomedir::face_length(int index, int size)
{
	if(index == -1 || size < 0)
	{
		//error
	}
	//1:46.3mm
	float length;
	if(index < 30)
	{
		if(size == SIZE_S) length = 1 * SCALE;//46.3
		if(size == SIZE_M) length = GOLDEN * SCALE;//74.9
		if(size == SIZE_L) length = GOLDEN * GOLDEN * SCALE;//121.2
	}
	else if(index < 42)
	{
		if(size == SIZE_S) length = sqrt( 2 + GOLDEN ) / 2 * SCALE;//43.8
		if(size == SIZE_M) length = sqrt( 2 + GOLDEN ) / 2 * GOLDEN * SCALE;//71
		if(size == SIZE_L) length = sqrt( 2 + GOLDEN ) / 2 * GOLDEN * GOLDEN * SCALE;//115
	}
	else
	{
		if(size == SIZE_S) length = sqrt(3.0f) / 2 * SCALE;//40.1
		if(size == SIZE_M) length = sqrt(3.0f) / 2 * GOLDEN * SCALE;//64.8
		if(size == SIZE_L) length = sqrt(3.0f) / 2 * GOLDEN * GOLDEN * SCALE;//104.9
	}
	return length;
}

float zomedir::color_length(int color, int size)
{
	//1:46.3mm
	float length;
	if(color == COLOR_BLUE)
	{
		if(size == SIZE_S) length = 1 * SCALE;//46.3
		if(size == SIZE_M) length = GOLDEN * SCALE;//74.9 76.533
		if(size == SIZE_L) length = GOLDEN * GOLDEN * SCALE;//121.2
	}
	else if(color == COLOR_RED)
	{
		if(size == SIZE_S) length = sqrt( 2 + GOLDEN ) / 2 * SCALE;//43.8
		if(size == SIZE_M) length = sqrt( 2 + GOLDEN ) / 2 * GOLDEN * SCALE;//71
		if(size == SIZE_L) length = sqrt( 2 + GOLDEN ) / 2 * GOLDEN * GOLDEN * SCALE;//115
	}
	else if(color == COLOR_YELLOW)
	{
		if(size == SIZE_S) length = sqrt(3.0f) / 2 * SCALE;//40.1
		if(size == SIZE_M) length = sqrt(3.0f) / 2 * GOLDEN * SCALE;//64.8
		if(size == SIZE_L) length = sqrt(3.0f) / 2 * GOLDEN * GOLDEN * SCALE;//104.9
	}

	return length;
}

int zomedir::face_color(int index)
{
	int color;
	if(index < 30)
	{
		color = COLOR_BLUE;
	}
	else if(index < 42)
	{
		color = COLOR_RED;
	}
	else
	{
		color = COLOR_YELLOW;
	}
	return color;
}

int zomedir::opposite_face(int index)
{
	vec3 fdir;
	fdir = dir->at(index);
    fdir *= -1.0;
    fdir = fdir.normalize();
	return dir_face(fdir);
}

int zomedir::dir_face(vec3 &fdir)
{
	for(int i = 0; i < 62; i += 1)
	{
		if(fdir == dir->at(i))
		{
			return i;
		}

		if((fdir - dir->at(i)).length() < 0.0001 )
		{

			return i;
		}
	}
	return -1;
}

int zomedir::dir_face(vec3 &vecInitial, vec3 &vecEnd, int size)
{
	vec3 tempdir;
	tempdir = (vecEnd - vecInitial).normalize();

	float d = tempdir.length();

	for(int i = 0; i < 62; i += 1)
	{
		if(tempdir == dir->at(i) && d == face_length(i, size))
		{
			return i;
		}

		if((tempdir - dir->at(i)).length() < 0.0001)
		{
			if(abs(d - face_length(i, size)) < 0.0001)
			{
				return i;
			}
		}
	}
	return -1;
}

int zomedir::find_near_dir(vec3 input)
{
	vec3 n_input = input.normalize();
	float temp_max = -1.0;
	int ans;

	for (unsigned int i = 0; i < dir->size(); i += 1){
		float temp = n_input * dir->at(i);
		if (temp > temp_max){
			temp_max = temp;
			ans = i;
		}
	}
	return ans;
}

void zomedir::find_best_zome(vec3 start, vec3 end, int near_index, int &choose_size, int &choose_dir)
{
	double tmpMin = 1e100;

	std::vector<int> close_dirs = near_dir.at(near_index).at(0);
	close_dirs.insert(close_dirs.end(), near_dir.at(near_index).at(1).begin(), near_dir.at(near_index).at(1).end());

	for (unsigned int j = 0; j < close_dirs.size(); j += 1)
	{
		int tmpDir = close_dirs[j];
		int tmpColor = face_color(tmpDir);
		for (int i = 0; i < 3; i += 1)
		{
			vec3 tmp_end_point = start + color_length(tmpColor, i) * dir->at(tmpDir);
			double tmpDis = (tmp_end_point - end).length();
			if (tmpDis < tmpMin)
			{
				tmpMin = tmpDis;
				choose_size = i;
				choose_dir = tmpDir;
			}
		}
	}
}

float zomedir::dir_parameter(int i, int d)
{
	return -dir->at(i)[d];
}

float zomedir::weight(int color, int size)
{
	float weight[3][3] =
	{{WEIGHT_BLUE_S, WEIGHT_BLUE_M, WEIGHT_BLUE_L},
	  {WEIGHT_RED_S, WEIGHT_RED_M, WEIGHT_RED_L},
	  {WEIGHT_YELLOW_S, WEIGHT_YELLOW_M, WEIGHT_YELLOW_L}};

	if(color < 3)
	{
		return weight[color][size];
	}
	else if(color == 3)
	{
		return WEIGHT_WHITE;
	}
	else
	{
		return 0;
	}
}

zometable::zometable(int type){
    table.resize(62);
    for(unsigned int i = 0; i < table.size(); i += 1){
        table.at(i).resize(62);
    }

    if(type == SPLITE){
        std::ifstream is("zome_splite.txt");
        if (is){
            vector_parser(SPLITE, is);
            is.close();
        }
        else{
            find_zomevector(SPLITE);
        }
    }
    else if(type == MERGE){
        std::ifstream is("zome_merge.txt");
        if (is){
            vector_parser(MERGE, is);
            is.close();
        }
        else{
            find_zomevector(MERGE);
        }
    }
}

void zometable::find_zomevector(int type){
    zomedir t;
    vec3 origin_p(0.0, 0.0, 0.0);
    vec3 end_p;
    vec3 travel;
    std::ofstream os;
    bool judge;

    if(type == SPLITE)
        os.open("zome_splite.txt");
    else if(type == MERGE)
        os.open("zome_merge.txt");

    for(unsigned int hole = 0; hole < t.dir->size(); hole +=1){
        for(int i = 0; i < 3; i += 1){
            if(type == SPLITE)
                end_p = origin_p + t.dir->at(hole) * t.color_length(t.face_color(hole), i);
            else if(type == MERGE)
                travel = origin_p + t.dir->at(hole) * t.color_length(t.face_color(hole), i);

            for(unsigned int j = 0; j < t.dir->size(); j += 1){
                if(type == SPLITE)
                    judge = !(j == hole && i < 2) && j != (unsigned)t.opposite_face(hole);
                else if(type == MERGE)
                    judge = j != (unsigned)t.opposite_face(hole);
                if(judge){
                    for(int b = 0; b < 3; b += 1){ //size
                        vec3 temp;
                        vec3 l;
                        if(type == SPLITE){
                            temp = origin_p + t.color_length(t.face_color(j), b) * t.dir->at(j);
                            l = temp - end_p;
                        }
                        else if(type == MERGE){
                            temp = travel + t.color_length(t.face_color(j), b) * t.dir->at(j);
                            l = temp - origin_p;
                        }

                        vec3 n_l = l;
                        int x = t.dir_face(n_l.normalize());
                        if(x != -1){
                            if((ABS(t.face_length(x, 0) - l.length()) < 0.0001) || (ABS(t.face_length(x, 1) - l.length()) < 0.0001) || (ABS(t.face_length(x, 2) - l.length()) < 0.0001)){

                                int s_l = -1;
                                s_l = (ABS(t.face_length(x, 0) - l.length()) < 0.0001)? 0:s_l;
                                s_l = (ABS(t.face_length(x, 1) - l.length()) < 0.0001)? 1:s_l;
                                s_l = (ABS(t.face_length(x, 2) - l.length()) < 0.0001)? 2:s_l;

                                zomerecord temp_r;
								temp_r.origin[0] = (float)hole;
								temp_r.origin[1] = (float)i;
								temp_r.origin[2] = (float)t.opposite_face(hole);

								temp_r.travel_1[0] = (float)j;
								temp_r.travel_1[1] = (float)b;
								temp_r.travel_1[2] = (float)t.opposite_face(j);

								temp_r.travel_2[0] = (float)x;
								temp_r.travel_2[1] = (float)s_l;
								temp_r.travel_2[2] = (float)t.opposite_face(x);


                                if(type == SPLITE)
                                    table.at(hole).at(j).push_back(temp_r);
                                else if(type == MERGE)
                                    table.at(hole).at(hole).push_back(temp_r);

                                os << hole << " " << i << " " << t.opposite_face(hole) << " " << j << " " << b << " " << t.opposite_face(j) << " " << x << " " << s_l << " " << t.opposite_face(x) << std::endl;
                            }
                        }
                    }
                }
            }
        }
    }
    os.close();
}

void zometable::vector_parser(int type, std::ifstream &is)
{
    zomerecord temp_r;
    while(is >> temp_r.origin[0] >> temp_r.origin[1] >> temp_r.origin[2] >> temp_r.travel_1[0] >> temp_r.travel_1[1] >> temp_r.travel_1[2] >> temp_r.travel_2[0] >> temp_r.travel_2[1] >> temp_r.travel_2[2]){
        if(type == SPLITE)
			table.at((int)temp_r.origin[0]).at((int)temp_r.travel_1[0]).push_back(temp_r);
        else if(type == MERGE)
			table.at((int)temp_r.origin[0]).at((int)temp_r.travel_1[0]).push_back(temp_r);
    }
}
