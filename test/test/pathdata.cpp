#include "pathdata.h"

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
