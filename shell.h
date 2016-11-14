#ifndef SHELL_H_INCLUDED
#define SHELL_H_INCLUDED

void recount_normal(GLMmodel *myObj,std::vector<int> *point_tri);
void process_inner(GLMmodel *myObj,GLMmodel *myObj_inner);
void combine_inner_outfit(GLMmodel *myObj,GLMmodel *myObj_inner);

#endif // SHELL_H_INCLUDED
