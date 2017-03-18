#include <vector>
#include "algebra3.h"

class pathdata
{
public:
	pathdata(vec3 start_point, vec3 end_point);
	pathdata(pathdata pre_path, vec3 next_vec, vec3 end_point);
	~pathdata();
	bool over_the_end_point(vec3 end_point);
	bool operator < (pathdata a)const;
	pathdata operator = (pathdata a);

	std::vector<vec3> road_path;
	double dis_to_end;
	double line_weight;

};