#include <queue>
#include "pathdata.h"
#include "zomedir.h"
#include "global.h"

#include <iostream>
using namespace std;

pathdata::pathdata(vec3 start_point, vec3 end_point)
{
	road_path.push_back(start_point);
	dis_to_end = (end_point - road_path.back()).length();
	line_weight = dis_to_end;
}

pathdata::pathdata(pathdata pre_path, vec3 next_vec, vec3 end_point)
{
	road_path = pre_path.road_path;
	road_path.push_back(road_path.back() + next_vec);
	dis_to_end = (end_point - road_path.back()).length();
	line_weight = pre_path.line_weight - (end_point - pre_path.road_path.back()).length() + next_vec.length() + dis_to_end;
}

pathdata::~pathdata()
{
	road_path.clear();
}

bool pathdata::over_the_end_point(vec3 end_point)
{
	vec3 start_point = road_path[road_path.size() - 2];
	vec3 mainVec = (end_point - start_point);
	if (mainVec.length() < 0.001)
		return false;
	mainVec.normalize();
	return ((mainVec * road_path.back()) * (mainVec * start_point) < 0.0);
}

bool pathdata::operator < (pathdata a)const
{
	return line_weight > a.line_weight;
}

pathdata pathdata::operator = (pathdata a)
{
	road_path = a.road_path;
	dis_to_end = a.dis_to_end;
	line_weight = a.line_weight;
	return *this;
}

void connect_points(vec3 start, vec3 end, std::vector<vec3> &path_points)
{
	zomedir t;
	vec3 main_v = end - start;
	float main_dis = main_v.length();
	int main_d = t.find_near_dir(main_v);

	int best_s, best_d;
	t.find_best_zome(start, end, main_d, best_s, best_d);

	vec3 new_end_point = start + t.face_length(best_d, best_s) * t.dir->at(best_d);
	path_points.push_back(new_end_point);

	if ((new_end_point - end).length() > t.color_length(COLOR_BLUE, SIZE_L))
		connect_points(new_end_point, end, path_points); 
}

void connect_points_optimize(vec3 start, vec3 end, std::vector<vec3> &path_points)
{
	zomedir t;
	vec3 main_v = end - start;

	if (main_v.length() > t.color_length(COLOR_BLUE, SIZE_L)){
		cout << "in" << endl;
		std::vector<vec3> mid_point;
		connect_points(start, end - main_v * t.color_length(COLOR_BLUE, SIZE_L) / main_v.length(), mid_point);
		path_points = mid_point;
		start = mid_point.back();
		mid_point.clear();
	}

	std::priority_queue<pathdata> paths;
	paths.push(pathdata(start, end));
	pathdata best_pd(paths.top());
	bool get_ans = false;

	do{
		pathdata temp_pd(paths.top());
		paths.pop();
		int tmp_dir = t.find_near_dir(end - temp_pd.road_path.back());
		std::vector<int> &close_dirs = (t.near_dir.at(tmp_dir).at(0));

		for (int i = 1; i >= 0; i -= 1){
			for (unsigned int j = 0; j < close_dirs.size(); j += 1){
				pathdata new_pd = pathdata(temp_pd, t.face_length(close_dirs.at(j), i) * t.dir->at(close_dirs.at(j)), end);
				if (!new_pd.over_the_end_point(end)){
					paths.push(new_pd);
					//cout << "test " << close_dirs.at(j) << " : " << new_pd.road_path.back()[0] << " " << new_pd.road_path.back()[1] << " " << new_pd.road_path.back()[2] << endl;
				}

				if (new_pd.dis_to_end < best_pd.dis_to_end){
					best_pd = new_pd;
					cout << "dte : " << new_pd.dis_to_end << endl;
					
					/*int near_d  = t.find_near_dir(start - end);
					int temp_size, temp_dir;
					t.find_best_zome(start, new_pd.road_path.at(0), near_d, temp_size, temp_dir);
					
					cout << "( " << temp_dir << " , " << temp_size << " ) ";
					for (int k = 0; k < new_pd.road_path.size(); k += 1){
						if (k > 0){
							t.find_best_zome(new_pd.road_path.at(k - 1), new_pd.road_path.at(k), near_d, temp_size, temp_dir);
							cout << "( " << temp_dir << " , " << temp_size << " ) ";
						}
					}
					cout << endl;*/
					int temp_dir;
					/*temp_dir = t.dir_face((new_pd.road_path.at(0) - start).normalize());
					cout << new_pd.road_path.at(0)[0] << " " << new_pd.road_path.at(0)[1] << " " << new_pd.road_path.at(0)[2] << endl;
					cout << start[0] << " " << start[1] << " " << start[2] << endl;
					cout << (new_pd.road_path.at(0) - start).normalize()[0] << " " << (new_pd.road_path.at(0) - start).normalize()[1] << " " << (new_pd.road_path.at(0) - start).normalize()[2] << endl;
					cout << " ( " << temp_dir << " ) ";*/
					for (int k = 0; k < new_pd.road_path.size(); k += 1){
						if (k > 0){
							temp_dir = t.dir_face((new_pd.road_path.at(k) - new_pd.road_path.at(k - 1)).normalize());
							//cout << "( " << temp_dir << " , " << (new_pd.road_path.at(k) - new_pd.road_path.at(k - 1)).length() << " ) ";
						}
					}
					//cout << endl;

					if (new_pd.dis_to_end < 0.3f)
						get_ans = true;	
				}
			}
		}
	} while (!get_ans && paths.size() > 0);

	for (unsigned int i = 1; i< best_pd.road_path.size(); i += 1)
	{
		path_points.push_back(best_pd.road_path.at(i));
	}
}