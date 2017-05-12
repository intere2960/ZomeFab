#include <algorithm>
#include <vector>
#include <cmath>
#include "shell.h"
#include "global.h"

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

void process_inner(GLMmodel *myObj, GLMmodel *myObj_inner, float shrink_size)
{
    myObj_inner->numnormals = myObj_inner->numvertices;
    myObj_inner->normals = new GLfloat[3 * (myObj_inner->numnormals + 1)];
    for(unsigned int i = 1 ; i <= myObj_inner->numvertices ; i += 1)
    {
		myObj_inner->vertices->at(3 * i + 0) = myObj->vertices->at(3 * i + 0) - shrink_size * myObj->normals[3 * i + 0];
        myObj_inner->normals[3 * i + 0] = myObj->normals[3 * i + 0];

		myObj_inner->vertices->at(3 * i + 1) = myObj->vertices->at(3 * i + 1) - shrink_size * myObj->normals[3 * i + 1];
        myObj_inner->normals[3 * i + 1] = myObj->normals[3 * i + 1];

		myObj_inner->vertices->at(3 * i + 2) = myObj->vertices->at(3 * i + 2) - shrink_size * myObj->normals[3 * i + 2];
        myObj_inner->normals[3 * i + 2] = myObj->normals[3 * i + 2];
    }
	glmFacetNormals(myObj_inner);
}

void combine_inner_outfit(GLMmodel *myObj,GLMmodel *myObj_inner)
{
    for(unsigned int i = 0; i < myObj_inner->numtriangles; i += 1){
        GLMtriangle temp = myObj_inner->triangles->at(i);
        temp.vindices[0] += myObj->numvertices;
        temp.vindices[1] += myObj->numvertices;
        temp.vindices[2] += myObj->numvertices;
        temp.findex += myObj->numfacetnorms;
		temp.tag = INNER;
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
        myObj->vertices->push_back(myObj->vertices->at(3 * i + 0) - 0.05f * myObj->normals[3 * i + 0]);
        myObj->vertices->push_back(myObj->vertices->at(3 * i + 1) - 0.05f * myObj->normals[3 * i + 1]);
        myObj->vertices->push_back(myObj->vertices->at(3 * i + 2) - 0.05f * myObj->normals[3 * i + 2]);
        myObj->numvertices += 1;
    }

    for(unsigned int i = 1; i <= curr_facenorm; i += 1){
        myObj->facetnorms->push_back(myObj->facetnorms->at(3 * i + 0));
        myObj->facetnorms->push_back(myObj->facetnorms->at(3 * i + 1));
        myObj->facetnorms->push_back(myObj->facetnorms->at(3 * i + 2));
        myObj->numfacetnorms += 1;
    }
}

void voxel_shell(std::vector<voxel> &all_voxel)
{
	std::vector<int> exist_index;
	int edge_length = pow(all_voxel.size(), 1.0 / 3);
	std::vector<std::vector<std::vector<int>>> convert_vertex(edge_length + 1);
	for (int i = 0; i < edge_length + 1; i += 1){
		convert_vertex.at(i).resize(edge_length + 1);

		for (int j = 0; j < edge_length + 1; j += 1){
			convert_vertex.at(i).at(j).resize(edge_length + 1);

			for (int k = 0; k < edge_length + 1; k += 1){
				convert_vertex.at(i).at(j).at(k) = -1;
			}
		}
	}

	std::vector<vec3> search_index;
	for (int i = 0; i < all_voxel.size(); i += 1){
		if (all_voxel.at(i).show){
			for (int j = 0; j < 6; j += 1){
				if (all_voxel.at(i).face_toward[j] != -1){
					if (!all_voxel.at(all_voxel.at(i).face_toward[j]).show){
						exist_index.push_back(i);
						break;
					}
				}
				else{
					exist_index.push_back(i);
					break;
				}
			}
		}

		vec3 index;
		for (int j = 0; j < all_voxel.at(i).coord.size(); j += 1){

			if ((all_voxel.at(i).coord.at(j) % 2) == 1){
				index[0] += pow(2.0f, all_voxel.at(i).coord.size() - 1 - j);
			}

			if ((all_voxel.at(i).coord.at(j) / 2) % 2 == 1){
				index[1] += pow(2.0f, all_voxel.at(i).coord.size() - 1 - j);
			}

			if (all_voxel.at(i).coord.at(j) > 3){
				index[2] += pow(2.0f, all_voxel.at(i).coord.size() - 1 - j);
			}
		}

		search_index.push_back(index);
	}

	GLMmodel surface;
	empty_model(&surface);
	int numvertex = 0;

	for (int i = 0; i < exist_index.size(); i += 1){
		voxel now = all_voxel.at(exist_index.at(i));

		if (now.face_toward[0] == -1 || !all_voxel.at(now.face_toward[0]).show){
			vec3 index[4];
			int use_point[4] = { 0, 2, 6, 4 };
			index[0] = search_index.at(exist_index.at(i)) + vec3(0.0f, 0.0f, 0.0f);
			index[1] = search_index.at(exist_index.at(i)) + vec3(0.0f, 1.0f, 0.0f);
			index[2] = search_index.at(exist_index.at(i)) + vec3(0.0f, 1.0f, 1.0f);
			index[3] = search_index.at(exist_index.at(i)) + vec3(0.0f, 0.0f, 1.0f);
			int model_index[4];

			for (int j = 0; j < 4; j += 1){
				if (convert_vertex.at(index[j][0]).at(index[j][1]).at(index[j][2]) == -1){
					numvertex += 1;
					convert_vertex.at(index[j][0]).at(index[j][1]).at(index[j][2]) = numvertex;
					model_index[j] = numvertex;
					surface.vertices->push_back(now.vertex_p[use_point[j]][0]);
					surface.vertices->push_back(now.vertex_p[use_point[j]][1]);
					surface.vertices->push_back(now.vertex_p[use_point[j]][2]);
				}
				else{
					model_index[j] = convert_vertex.at(index[j][0]).at(index[j][1]).at(index[j][2]);
				}
			}

			GLMtriangle temp_tri1;
			temp_tri1.vindices[0] = model_index[0];
			temp_tri1.vindices[1] = model_index[1];
			temp_tri1.vindices[2] = model_index[2];

			GLMtriangle temp_tri2;
			temp_tri2.vindices[0] = model_index[2];
			temp_tri2.vindices[1] = model_index[3];
			temp_tri2.vindices[2] = model_index[0];

			surface.numtriangles += 2;
			surface.triangles->push_back(temp_tri1);
			surface.triangles->push_back(temp_tri2);
		}
		if (now.face_toward[1] == -1 || !all_voxel.at(now.face_toward[1]).show){
			vec3 index[4];
			int use_point[4] = { 1, 5, 7, 3 };
			index[0] = search_index.at(exist_index.at(i)) + vec3(1.0f, 0.0f, 0.0f);
			index[1] = search_index.at(exist_index.at(i)) + vec3(1.0f, 0.0f, 1.0f);
			index[2] = search_index.at(exist_index.at(i)) + vec3(1.0f, 1.0f, 1.0f);
			index[3] = search_index.at(exist_index.at(i)) + vec3(1.0f, 1.0f, 0.0f);
			int model_index[4];

			for (int j = 0; j < 4; j += 1){
				if (convert_vertex.at(index[j][0]).at(index[j][1]).at(index[j][2]) == -1){
					numvertex += 1;
					convert_vertex.at(index[j][0]).at(index[j][1]).at(index[j][2]) = numvertex;
					model_index[j] = numvertex;
					surface.vertices->push_back(now.vertex_p[use_point[j]][0]);
					surface.vertices->push_back(now.vertex_p[use_point[j]][1]);
					surface.vertices->push_back(now.vertex_p[use_point[j]][2]);
				}
				else{
					model_index[j] = convert_vertex.at(index[j][0]).at(index[j][1]).at(index[j][2]);
				}
			}

			GLMtriangle temp_tri1;
			temp_tri1.vindices[0] = model_index[0];
			temp_tri1.vindices[1] = model_index[1];
			temp_tri1.vindices[2] = model_index[2];

			GLMtriangle temp_tri2;
			temp_tri2.vindices[0] = model_index[2];
			temp_tri2.vindices[1] = model_index[3];
			temp_tri2.vindices[2] = model_index[0];

			surface.numtriangles += 2;
			surface.triangles->push_back(temp_tri1);
			surface.triangles->push_back(temp_tri2);
		}
		if (now.face_toward[2] == -1 || !all_voxel.at(now.face_toward[2]).show){
			vec3 index[4];
			int use_point[4] = { 0, 4, 5, 1 };
			index[0] = search_index.at(exist_index.at(i)) + vec3(0.0f, 0.0f, 0.0f);
			index[1] = search_index.at(exist_index.at(i)) + vec3(0.0f, 0.0f, 1.0f);
			index[2] = search_index.at(exist_index.at(i)) + vec3(1.0f, 0.0f, 1.0f);
			index[3] = search_index.at(exist_index.at(i)) + vec3(1.0f, 0.0f, 0.0f);
			int model_index[4];

			for (int j = 0; j < 4; j += 1){
				if (convert_vertex.at(index[j][0]).at(index[j][1]).at(index[j][2]) == -1){
					numvertex += 1;
					convert_vertex.at(index[j][0]).at(index[j][1]).at(index[j][2]) = numvertex;
					model_index[j] = numvertex;
					surface.vertices->push_back(now.vertex_p[use_point[j]][0]);
					surface.vertices->push_back(now.vertex_p[use_point[j]][1]);
					surface.vertices->push_back(now.vertex_p[use_point[j]][2]);
				}
				else{
					model_index[j] = convert_vertex.at(index[j][0]).at(index[j][1]).at(index[j][2]);
				}
			}

			GLMtriangle temp_tri1;
			temp_tri1.vindices[0] = model_index[0];
			temp_tri1.vindices[1] = model_index[1];
			temp_tri1.vindices[2] = model_index[2];

			GLMtriangle temp_tri2;
			temp_tri2.vindices[0] = model_index[2];
			temp_tri2.vindices[1] = model_index[3];
			temp_tri2.vindices[2] = model_index[0];

			surface.numtriangles += 2;
			surface.triangles->push_back(temp_tri1);
			surface.triangles->push_back(temp_tri2);
		}
		if (now.face_toward[3] == -1 || !all_voxel.at(now.face_toward[3]).show){
			vec3 index[4];
			int use_point[4] = { 2, 3, 7, 6 };
			index[0] = search_index.at(exist_index.at(i)) + vec3(0.0f, 1.0f, 0.0f);
			index[1] = search_index.at(exist_index.at(i)) + vec3(1.0f, 1.0f, 0.0f);
			index[2] = search_index.at(exist_index.at(i)) + vec3(1.0f, 1.0f, 1.0f);
			index[3] = search_index.at(exist_index.at(i)) + vec3(0.0f, 1.0f, 1.0f);
			int model_index[4];

			for (int j = 0; j < 4; j += 1){
				if (convert_vertex.at(index[j][0]).at(index[j][1]).at(index[j][2]) == -1){
					numvertex += 1;
					convert_vertex.at(index[j][0]).at(index[j][1]).at(index[j][2]) = numvertex;
					model_index[j] = numvertex;
					surface.vertices->push_back(now.vertex_p[use_point[j]][0]);
					surface.vertices->push_back(now.vertex_p[use_point[j]][1]);
					surface.vertices->push_back(now.vertex_p[use_point[j]][2]);
				}
				else{
					model_index[j] = convert_vertex.at(index[j][0]).at(index[j][1]).at(index[j][2]);
				}
			}

			GLMtriangle temp_tri1;
			temp_tri1.vindices[0] = model_index[0];
			temp_tri1.vindices[1] = model_index[1];
			temp_tri1.vindices[2] = model_index[2];

			GLMtriangle temp_tri2;
			temp_tri2.vindices[0] = model_index[2];
			temp_tri2.vindices[1] = model_index[3];
			temp_tri2.vindices[2] = model_index[0];

			surface.numtriangles += 2;
			surface.triangles->push_back(temp_tri1);
			surface.triangles->push_back(temp_tri2);
		}
		if (now.face_toward[4] == -1 || !all_voxel.at(now.face_toward[4]).show){
			vec3 index[4];
			int use_point[4] = { 0, 1, 3, 2 };
			index[0] = search_index.at(exist_index.at(i)) + vec3(0.0f, 0.0f, 0.0f);
			index[1] = search_index.at(exist_index.at(i)) + vec3(1.0f, 0.0f, 0.0f);
			index[2] = search_index.at(exist_index.at(i)) + vec3(1.0f, 1.0f, 0.0f);
			index[3] = search_index.at(exist_index.at(i)) + vec3(0.0f, 1.0f, 0.0f);
			int model_index[4];

			for (int j = 0; j < 4; j += 1){
				if (convert_vertex.at(index[j][0]).at(index[j][1]).at(index[j][2]) == -1){
					numvertex += 1;
					convert_vertex.at(index[j][0]).at(index[j][1]).at(index[j][2]) = numvertex;
					model_index[j] = numvertex;
					surface.vertices->push_back(now.vertex_p[use_point[j]][0]);
					surface.vertices->push_back(now.vertex_p[use_point[j]][1]);
					surface.vertices->push_back(now.vertex_p[use_point[j]][2]);
				}
				else{
					model_index[j] = convert_vertex.at(index[j][0]).at(index[j][1]).at(index[j][2]);
				}
			}

			GLMtriangle temp_tri1;
			temp_tri1.vindices[0] = model_index[0];
			temp_tri1.vindices[1] = model_index[1];
			temp_tri1.vindices[2] = model_index[2];

			GLMtriangle temp_tri2;
			temp_tri2.vindices[0] = model_index[2];
			temp_tri2.vindices[1] = model_index[3];
			temp_tri2.vindices[2] = model_index[0];

			surface.numtriangles += 2;
			surface.triangles->push_back(temp_tri1);
			surface.triangles->push_back(temp_tri2);
		}
		if (now.face_toward[5] == -1 || !all_voxel.at(now.face_toward[5]).show){
			vec3 index[4];
			int use_point[4] = { 4, 6, 7, 5 };
			index[0] = search_index.at(exist_index.at(i)) + vec3(0.0f, 0.0f, 1.0f);
			index[1] = search_index.at(exist_index.at(i)) + vec3(0.0f, 1.0f, 1.0f);
			index[2] = search_index.at(exist_index.at(i)) + vec3(1.0f, 1.0f, 1.0f);
			index[3] = search_index.at(exist_index.at(i)) + vec3(1.0f, 0.0f, 1.0f);
			int model_index[4];

			for (int j = 0; j < 4; j += 1){
				if (convert_vertex.at(index[j][0]).at(index[j][1]).at(index[j][2]) == -1){
					numvertex += 1;
					convert_vertex.at(index[j][0]).at(index[j][1]).at(index[j][2]) = numvertex;
					model_index[j] = numvertex;
					surface.vertices->push_back(now.vertex_p[use_point[j]][0]);
					surface.vertices->push_back(now.vertex_p[use_point[j]][1]);
					surface.vertices->push_back(now.vertex_p[use_point[j]][2]);
				}
				else{
					model_index[j] = convert_vertex.at(index[j][0]).at(index[j][1]).at(index[j][2]);
				}
			}

			GLMtriangle temp_tri1;
			temp_tri1.vindices[0] = model_index[0];
			temp_tri1.vindices[1] = model_index[1];
			temp_tri1.vindices[2] = model_index[2];

			GLMtriangle temp_tri2;
			temp_tri2.vindices[0] = model_index[2];
			temp_tri2.vindices[1] = model_index[3];
			temp_tri2.vindices[2] = model_index[0];

			surface.numtriangles += 2;
			surface.triangles->push_back(temp_tri1);
			surface.triangles->push_back(temp_tri2);
		}
	}
	surface.numvertices = numvertex;
	glmWriteOBJ(&surface, "voxel_shell.obj", GLM_NONE);
}