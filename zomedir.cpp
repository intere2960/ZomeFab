#include "zomedir.h"
#include "glui_internal.h"

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
    dir->push_back(vec3(0.5 * golden, 0.5 / golden, 0.5));
	dir->push_back(vec3(0.5 * golden, -0.5 / golden, 0.5));
	dir->push_back(vec3(-0.5 * golden, 0.5 / golden, 0.5));
	dir->push_back(vec3(-0.5 * golden, -0.5 / golden, 0.5));

	dir->push_back(vec3(-0.5 * golden, -0.5 / golden, -0.5));
	dir->push_back(vec3(-0.5 * golden, 0.5 / golden, -0.5));
	dir->push_back(vec3(0.5 * golden, -0.5 / golden, -0.5));
	dir->push_back(vec3(0.5 * golden, 0.5 / golden, -0.5));

    // around y axis (14-21)
    dir->push_back(vec3(0.5, 0.5 * golden, 0.5 / golden));
	dir->push_back(vec3(-0.5, 0.5 * golden, 0.5 / golden));
	dir->push_back(vec3(0.5, -0.5 * golden, 0.5 / golden));
	dir->push_back(vec3(-0.5, -0.5 * golden, 0.5 / golden));

	dir->push_back(vec3(-0.5, -0.5 * golden, -0.5 / golden));
	dir->push_back(vec3(0.5, -0.5 * golden, -0.5 / golden));
	dir->push_back(vec3(-0.5, 0.5 * golden, -0.5 / golden));
	dir->push_back(vec3(0.5, 0.5 * golden, -0.5 / golden));

    // around z axis (22-29)
    dir->push_back(vec3(0.5 / golden, 0.5, 0.5 * golden));
	dir->push_back(vec3(-0.5 / golden, 0.5, 0.5 * golden));
	dir->push_back(vec3(0.5 / golden, -0.5, 0.5 * golden));
	dir->push_back(vec3(-0.5 / golden, -0.5, 0.5 * golden));

	dir->push_back(vec3(-0.5 / golden, -0.5, -0.5 * golden));
	dir->push_back(vec3(0.5 / golden, -0.5, -0.5 * golden));
	dir->push_back(vec3(-0.5 / golden, 0.5, -0.5 * golden));
	dir->push_back(vec3(0.5 / golden, 0.5, -0.5 * golden));

    // reds (diagonals in planes)
    // around x axis (30-33)
    dir->push_back(vec3(golden * sqrt((5 - sqrt(5)) / 10), sqrt((5 - sqrt(5)) / 10), 0.0)); // (5 + sqrt(5)) / 2 = (5 - sqrt(5)) / 10
	dir->push_back(vec3(golden * sqrt((5 - sqrt(5)) / 10), -sqrt((5 - sqrt(5)) / 10), 0.0));
	dir->push_back(vec3(-golden * sqrt((5 - sqrt(5)) / 10), -sqrt((5 - sqrt(5)) / 10), 0.0));
	dir->push_back(vec3(-golden * sqrt((5 - sqrt(5)) / 10), sqrt((5 - sqrt(5)) / 10), 0.0));

    // around y axis (34-37)
    dir->push_back(vec3(0.0, golden * sqrt((5 - sqrt(5)) / 10), sqrt((5 - sqrt(5)) / 10)));
	dir->push_back(vec3(0.0, golden * sqrt((5 - sqrt(5)) / 10), -sqrt((5 - sqrt(5)) / 10)));
	dir->push_back(vec3(0.0, -golden * sqrt((5 - sqrt(5)) / 10), -sqrt((5 - sqrt(5)) / 10)));
	dir->push_back(vec3(0.0, -golden * sqrt((5 - sqrt(5)) / 10), sqrt((5 - sqrt(5)) / 10)));

    // around z axis (38-41)
    dir->push_back(vec3(sqrt((5 - sqrt(5)) / 10), 0.0, golden * sqrt((5 - sqrt(5)) / 10)));
	dir->push_back(vec3(-sqrt((5 - sqrt(5)) / 10), 0.0, golden * sqrt((5 - sqrt(5)) / 10)));
	dir->push_back(vec3(-sqrt((5 - sqrt(5)) / 10), 0.0, -golden * sqrt((5 - sqrt(5)) / 10)));
	dir->push_back(vec3(sqrt((5 - sqrt(5)) / 10), 0.0, -golden * sqrt((5 - sqrt(5)) / 10)));

    // yellows (diagonals in planes)
    // around x axis (42-35)
    dir->push_back(vec3(golden / sqrt(3), 0.0,  1.0 / golden / sqrt(3)));
	dir->push_back(vec3(golden / sqrt(3), 0.0, -1.0 / golden / sqrt(3)));
	dir->push_back(vec3(-golden / sqrt(3), 0.0, -1.0 / golden / sqrt(3)));
	dir->push_back(vec3(-golden / sqrt(3), 0.0, 1.0 / golden / sqrt(3)));

    // around y axis (46-49)
    dir->push_back(vec3(1.0 / golden / sqrt(3), golden / sqrt(3), 0.0));
	dir->push_back(vec3(-1.0 / golden / sqrt(3), golden / sqrt(3), 0.0));
	dir->push_back(vec3(-1.0 / golden / sqrt(3), -golden / sqrt(3), 0.0));
	dir->push_back(vec3(1.0 / golden / sqrt(3), -golden / sqrt(3), 0.0));

    // around z axis (50-53)
    dir->push_back(vec3(0.0, 1.0 / golden / sqrt(3), golden / sqrt(3)));
	dir->push_back(vec3(0.0, -1.0 / golden / sqrt(3), golden / sqrt(3)));
	dir->push_back(vec3(0.0, -1.0 / golden / sqrt(3), -golden / sqrt(3)));
	dir->push_back(vec3(0.0, 1.0 / golden / sqrt(3), -golden / sqrt(3)));

    // 54-61 (diagonals)
    dir->push_back(vec3(sqrt(3) / 3 , sqrt(3) / 3 , sqrt(3) / 3));
	dir->push_back(vec3(-sqrt(3) / 3 , sqrt(3) / 3 , sqrt(3) / 3));
	dir->push_back(vec3(sqrt(3) / 3 , -sqrt(3) / 3 , sqrt(3) / 3));
	dir->push_back(vec3(sqrt(3) / 3 , sqrt(3) / 3 , -sqrt(3) / 3));
	dir->push_back(vec3(sqrt(3) / 3 , -sqrt(3) / 3 , -sqrt(3) / 3));
	dir->push_back(vec3(-sqrt(3) / 3 , sqrt(3) / 3 , -sqrt(3) / 3));
	dir->push_back(vec3(-sqrt(3) / 3 , -sqrt(3) / 3 , sqrt(3) / 3));
	dir->push_back(vec3(-sqrt(3) / 3 , -sqrt(3) / 3 , -sqrt(3) / 3));

    for(unsigned int i = 0; i < 62; i += 1)
        dir->at(i) = dir->at(i).normalize();
}

zomedir::~zomedir()
{
    if (!dir->empty()){
        std::vector<vec3> v;
        dir->swap(v);
    }
    delete dir;
}

GLfloat zomedir::phi(int index)
{
	GLfloat sinPhi = dir->at(index)[2] / sqrt(pow(dir->at(index)[0], 2) + pow(dir->at(index)[2], 2) );
	GLfloat phi = asin(sinPhi);
	if(dir->at(index)[0] < 0)
	{
		phi = M_PI - phi;
	}
	if(sqrt(pow(dir->at(index)[0], 2) + pow(dir->at(index)[2], 2)) < SMALL_VALUE)
	{
		phi = 0;
	}
	return -phi;
}

GLfloat zomedir::theta(int index)
{
	GLfloat sinTheta = dir->at(index)[1] / sqrt(pow(dir->at(index)[0] , 2) + pow(dir->at(index)[1] , 2) + pow(dir->at(index)[2] , 2));
	GLfloat theta = acos(sinTheta);
	return -theta;
}

GLfloat zomedir::roll(int index)
{
	GLfloat roll;
	if(index < 30)
	{
		GLfloat sinroll = dir->at(index)[2] / sqrt(pow(dir->at(index)[0], 2) + pow(dir->at(index)[2], 2));
		roll = asin(sinroll);
		if(index == 0) roll = M_PI * 0.5;
		if(index == 1) roll = M_PI * 0.5;
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

		if ( 0 && sqrt(pow(dir->at(index)[0], 2) + pow(dir->at(index)[2], 2)) < SMALL_VALUE)
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
		if(index == 38) roll = M_PI * 0.5;
		if(index == 39) roll = M_PI * 1.5;
		if(index == 40) roll = M_PI * 0.5;
		if(index == 41) roll = M_PI * 1.5;
	}
	else
	{
		if(index == 42) roll = M_PI * 0.5;
		if(index == 43) roll = M_PI * 1.5;
		if(index == 44) roll = M_PI * 0.5;
		if(index == 45) roll = M_PI * 1.5;
		if(index == 46) roll = 0;
		if(index == 47) roll = 0;
		if(index == 48) roll = M_PI;
		if(index == 49) roll = M_PI;
		if(index == 50) roll = M_PI;
		if(index == 51) roll = 0;
		if(index == 52) roll = 0;
		if(index == 53) roll = M_PI;

		if(index == 54) roll = 22.2369 / 180.0 * M_PI;
		if(index == 55) roll = -22.2369 / 180.0 * M_PI;
		if(index == 56) roll = -22.2369 / 180.0 * M_PI + M_PI;
		if(index == 57) roll = -22.2369 / 180.0 * M_PI;
		if(index == 58) roll = 22.2369 / 180.0 * M_PI + M_PI;
		if(index == 59) roll = 22.2369 / 180.0 * M_PI;
		if(index == 60) roll = 22.2369 / 180.0 * M_PI + M_PI;
		if(index == 61) roll = -22.2369 / 180.0 * M_PI + M_PI;
	}
	return -roll;
}


GLfloat zomedir::face_length(int index, int size)
{
	if(index == -1 || size < 0)
	{
		//error
	}
	//1:46.3mm
	GLfloat length;
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
		if(size == SIZE_S) length = sqrt(3) / 2 * SCALE;//40.1
		if(size == SIZE_M) length = sqrt(3) / 2 * GOLDEN * SCALE;//64.8
		if(size == SIZE_L) length = sqrt(3) / 2 * GOLDEN * GOLDEN * SCALE;//104.9
	}
	return length;
}
GLfloat zomedir::color_length(int index, int size)
{
	//1:46.3mm
	GLfloat length;
	if(index == COLOR_BLUE)
	{
		if(size == SIZE_S) length = 1 * SCALE;//46.3
		if(size == SIZE_M) length = GOLDEN * SCALE;//74.9
		if(size == SIZE_L) length = GOLDEN * GOLDEN * SCALE;//121.2
	}
	else if(index == COLOR_RED)
	{
		if(size == SIZE_S) length = sqrt( 2 + GOLDEN ) / 2 * SCALE;//43.8
		if(size == SIZE_M) length = sqrt( 2 + GOLDEN ) / 2 * GOLDEN * SCALE;//71
		if(size == SIZE_L) length = sqrt( 2 + GOLDEN ) / 2 * GOLDEN * GOLDEN * SCALE;//115
	}
	else if(index == COLOR_YELLOW)
	{
		if(size == SIZE_S) length = sqrt(3) / 2 * SCALE;//40.1
		if(size == SIZE_M) length = sqrt(3) / 2 * GOLDEN * SCALE;//64.8
		if(size == SIZE_L) length = sqrt(3) / 2 * GOLDEN * GOLDEN * SCALE;//104.9
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
	if(index < 0 )
	{
//		std::cout << "zomedir::GetOppositeFace error¡Gface id:-1" << std::endl;
	}
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

		if((fdir - dir->at(i)).length() < SMALL_VALUE )
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

	GLfloat d = tempdir.length();

	for(int i = 0; i < 62; i += 1)
	{
		if(tempdir == dir->at(i) && d == face_length(i, size))
		{
			return i;
		}

		if((tempdir - dir->at(i)).length() < SMALL_VALUE)
		{
			if(abs(d - face_length(i, size)) < SMALL_VALUE)
			{
				return i;
			}
		}
	}
	return -1;
}

float zomedir::dir_parameter(int i, int d)
{
	return -dir->at(i)[d];
}

float zomedir::weight(int color, int size)
{
	GLfloat weight[3][3] =
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
//		std::cout << "zomedir::GetWeight error:color input error" << std::endl;
		return 0;
	}
}
