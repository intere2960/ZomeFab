#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cstdlib>

#include "glm.h"
#include "bisect.h"
#include "voxel.h"
#include "operation.h"
#include "graphcut.h"
#include "zomestruc.h"
#include "global.h"

GLMmodel *myObj = NULL;

std::string model_source;
std::string shell_source;

using namespace std;

int main(int argc, char **argv)
{
	string model_file;
	int total_num;
	int iterations;
	int mode;
	float wedge;
	float wnode;
	float label;
	float salient;
	int plane_tune;
	float inner_shell_edge;

	ifstream is("ZomeFab input.txt");
	is >> model_file;
	is >> total_num;
	is >> iterations;
	is >> wedge;
	is >> wnode;
	is >> label;
	is >> salient;
	is >> plane_tune;
	is >> inner_shell_edge;
	is >> mode;
	is.close();

	model_source = "test_model/" + model_file + ".obj";
	shell_source = "test_model/" + model_file + "_voxel_shell.obj";

	myObj = glmReadOBJ(my_strdup(model_source.c_str()));

	switch (mode){
		case VOXELIZATION:
			cout << "Voxelizaton" << endl;
			struct_voxelization(myObj, model_file);
			break;
		case SA:
			cout << "Simulated annealing" << endl;
			simulated_annealing(myObj, model_file, (float)total_num, iterations);
			break;
		case FAKE_SALIENCY:
			cout << "Fake saliency" << endl;
			fake_saliency(myObj, model_file);
			//saliency_texture(myObj, model_file);
			break;
		case GRAPH_CUT_PROCESS:
			cout << "Graph cut" << endl;
			graph_cut(myObj, model_file, string(model_file + "_" + std::to_string(total_num) + "_" + std::to_string(iterations) + ".txt"), (float)total_num, wedge, wnode, label, salient);
			break;
		case INNER_SHELL:
			cout << "Inner shell" << endl;
			voxel_inner_shell(myObj, model_file, inner_shell_edge);
			break;
		case CUT_MESH:
			cout << "Cut mesh" << endl;
			cut_mesh(myObj, model_file, shell_source, string(model_file + "_" + std::to_string(total_num) + "_" + std::to_string(iterations) + ".txt"), plane_tune);
			break;
		case OUTPUT_ZOME:
			cout << "Output zometool" << endl;
			output_color_zome(model_file, string(model_file + "_" + std::to_string(total_num) + "_" + std::to_string(iterations) + ".txt"));
			break;
	}

	cout << "done" << endl;
	system("pause");
	return 0;
}
