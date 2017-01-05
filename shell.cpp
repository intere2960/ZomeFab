#include <algorithm>
#include <vector>
#include "glm.h"
#include <cmath>
#include <vector>

void recount_normal(GLMmodel *myObj)
{
    glmFacetNormals(myObj);

    std::vector<int> *point_tri = new std::vector<int>[myObj->numvertices + 1];

    for(unsigned int i = 0 ; i < myObj->numtriangles ; i += 1)
    {
        for(int j = 0 ; j < 3 ; j += 1)
        {
            bool add = true;
            for(unsigned int k = 0 ; k < point_tri[myObj->triangles->at(i).vindices[j]].size() ; k += 1)
            {
                GLfloat *temp = &myObj->facetnorms->at(3 * point_tri[myObj->triangles->at(i).vindices[j]][k]);
                if(*temp == myObj->facetnorms->at(3 * (i + 1) + 0) && *(temp + 1) == myObj->facetnorms->at(3 * (i + 1) + 1) && *(temp + 2) == myObj->facetnorms->at(3 * (i + 1) + 2))
                {
                    add = false;
                    break;
                }
            }
            if(add)
                point_tri[myObj->triangles->at(i).vindices[j]].push_back(i + 1);
        }
    }

    for(unsigned int i = 1 ; i <= myObj->numvertices ; i += 1)
    {
        sort(point_tri[i].begin(),point_tri[i].begin() + point_tri[i].size());
    }

    myObj->numnormals = myObj->numvertices;
    myObj->normals = new GLfloat[3 * (myObj->numnormals + 1)];
    for(unsigned int i = 1 ; i <= myObj->numnormals ; i += 1)
    {
        GLfloat temp[3] = {0.0 , 0.0 , 0.0};
        for(unsigned int j = 0 ; j < point_tri[i].size() ; j += 1)
        {
            temp[0] += myObj->facetnorms->at(3 * point_tri[i].at(j) + 0);
            temp[1] += myObj->facetnorms->at(3 * point_tri[i].at(j) + 1);
            temp[2] += myObj->facetnorms->at(3 * point_tri[i].at(j) + 2);
        }
        GLfloat normal_length = sqrt(pow(temp[0],2) + pow(temp[1],2) + pow(temp[2],2));
        temp[0] /= normal_length;
        temp[1] /= normal_length;
        temp[2] /= normal_length;

        myObj->normals[3 * i + 0] = temp[0];
        myObj->normals[3 * i + 1] = temp[1];
        myObj->normals[3 * i + 2] = temp[2];
    }
}

void process_inner(GLMmodel *myObj,GLMmodel *myObj_inner)
{
    myObj_inner->numnormals = myObj_inner->numvertices;
    myObj_inner->normals = new GLfloat[3 * (myObj_inner->numnormals + 1)];
    for(unsigned int i = 1 ; i <= myObj_inner->numvertices ; i += 1)
    {
        myObj_inner->vertices->at(3 * i + 0) = myObj->vertices->at(3 * i + 0) - 10.05 * myObj->normals[3 * i + 0];
        myObj_inner->normals[3 * i + 0] = myObj->normals[3 * i + 0];

        myObj_inner->vertices->at(3 * i + 1) = myObj->vertices->at(3 * i + 1) - 10.05 * myObj->normals[3 * i + 1];
        myObj_inner->normals[3 * i + 1] = myObj->normals[3 * i + 1];

        myObj_inner->vertices->at(3 * i + 2) = myObj->vertices->at(3 * i + 2) - 10.05 * myObj->normals[3 * i + 2];
        myObj_inner->normals[3 * i + 2] = myObj->normals[3 * i + 2];
    }

    myObj_inner->numfacetnorms = myObj->numfacetnorms;
    myObj_inner->facetnorms = new std::vector<GLfloat>(3 * (myObj_inner->numfacetnorms + 1));

    for(unsigned int i = 1 ; i <= myObj->numfacetnorms ; i += 1)
    {
        myObj_inner->facetnorms->at(3 * i + 0) =  myObj->facetnorms->at(3 * i + 0);
        myObj_inner->facetnorms->at(3 * i + 1) =  myObj->facetnorms->at(3 * i + 1);
        myObj_inner->facetnorms->at(3 * i + 2) =  myObj->facetnorms->at(3 * i + 2);
    }
}

void combine_inner_outfit(GLMmodel *myObj,GLMmodel *myObj_inner)
{
    for(unsigned int i = 0; i < myObj_inner->numtriangles; i += 1){
        GLMtriangle temp = myObj_inner->triangles->at(i);
        temp.vindices[0] += myObj->numvertices;
        temp.vindices[1] += myObj->numvertices;
        temp.vindices[2] += myObj->numvertices;
        temp.findex += myObj->numfacetnorms;
        myObj->triangles->push_back(temp);
        myObj->numtriangles += 1;
    }

    for(unsigned int i = 1; i <= myObj_inner->numvertices; i += 1){
        myObj->vertices->push_back(myObj_inner->vertices->at(3 * i + 0));
        myObj->vertices->push_back(myObj_inner->vertices->at(3 * i + 1));
        myObj->vertices->push_back(myObj_inner->vertices->at(3 * i + 2));
        myObj->numvertices += 1;

        vertex new_vertex;
        myObj->cut_loop->push_back(new_vertex);
    }

    for(unsigned int i = 1; i <= myObj_inner->numfacetnorms; i += 1){
        myObj->facetnorms->push_back(myObj_inner->facetnorms->at(3 * i + 0));
        myObj->facetnorms->push_back(myObj_inner->facetnorms->at(3 * i + 1));
        myObj->facetnorms->push_back(myObj_inner->facetnorms->at(3 * i + 2));
        myObj->numfacetnorms += 1;
    }
}

void combine_inner_outfit2(GLMmodel *myObj)
{
    unsigned int curr_tri = myObj->numtriangles, curr_vertex = myObj->numvertices, curr_facenorm = myObj->numfacetnorms;
    for(unsigned int i = 0; i < curr_tri; i += 1){
        GLMtriangle temp = myObj->triangles->at(i);
        temp.vindices[0] += curr_vertex;
        temp.vindices[1] += curr_vertex;
        temp.vindices[2] += curr_vertex;
        temp.findex += myObj->numfacetnorms;
        myObj->triangles->push_back(temp);
        myObj->numtriangles += 1;
    }

    for(unsigned int i = 1; i <= curr_vertex; i += 1){
        myObj->vertices->push_back(myObj->vertices->at(3 * i + 0) - 0.05 * myObj->normals[3 * i + 0]);
        myObj->vertices->push_back(myObj->vertices->at(3 * i + 1) - 0.05 * myObj->normals[3 * i + 1]);
        myObj->vertices->push_back(myObj->vertices->at(3 * i + 2) - 0.05 * myObj->normals[3 * i + 2]);
        myObj->numvertices += 1;
    }

    for(unsigned int i = 1; i <= curr_facenorm; i += 1){
        myObj->facetnorms->push_back(myObj->facetnorms->at(3 * i + 0));
        myObj->facetnorms->push_back(myObj->facetnorms->at(3 * i + 1));
        myObj->facetnorms->push_back(myObj->facetnorms->at(3 * i + 2));
        myObj->numfacetnorms += 1;
    }
}
