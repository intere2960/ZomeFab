#ifndef SHELL_H_INCLUDED
#define SHELL_H_INCLUDED

#include <vector>
#include "glm.h"
#include "voxel.h"

void recount_normal(GLMmodel *myObj);
void process_inner(GLMmodel *myObj, GLMmodel *myObj_inner, float shrink_size);
void combine_inner_outfit(GLMmodel *myObj,GLMmodel *myObj_inner);
void combine_inner_outfit2(GLMmodel *myObj);
void voxel_shell(std::vector<voxel> &all_voxel);

#endif // SHELL_H_INCLUDED
