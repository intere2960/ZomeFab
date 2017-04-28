#ifndef SHELL_H_INCLUDED
#define SHELL_H_INCLUDED

#include "glm.h"

void recount_normal(GLMmodel *myObj);
void process_inner(GLMmodel *myObj, GLMmodel *myObj_inner, float shrink_size);
void combine_inner_outfit(GLMmodel *myObj,GLMmodel *myObj_inner);
void combine_inner_outfit2(GLMmodel *myObj);

#endif // SHELL_H_INCLUDED
