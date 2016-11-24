#include <vector>
#include "glm.h"
#include "algebra3.h"
#include "glui_internal.h"
#include "bisect.h"

edge::edge()
{
    vec3 zero(0.0, 0.0, 0.0);
    point[0] = zero;
    point[1] = zero;
    index[0] = 0;
    index[1] = 0;
    face_id[0] = -1;
    face_id[1] = -1;
    vertex_push_index = -1;
    l = length();
}

edge::edge(int i1, vec3 &p1, int i2, vec3 &p2)
{
    index[0] = i1;
    point[0] = p1;
    index[1] = i2;
    point[1] = p2;
    face_id[0] = -1;
    face_id[1] = -1;
    vertex_push_index = -1;
    l = length();
}

float edge::length()
{
    vec3 temp = point[1] - point[0];
    return temp.length();
}

plane::plane(){
    plane_par[0] = 0.0;
    plane_par[1] = 0.0;
    plane_par[2] = 0.0;
    plane_par[3] = 0.0;
    dir = 0;
}

plane::plane(float a, float b, float c, float d,int e){
    plane_par[0] = a;
    plane_par[1] = b;
    plane_par[2] = c;
    plane_par[3] = d;
    dir = e;
}

float plane_dir_point(vec3 &point, plane plane)
{
    float judge = plane.plane_par[0] * point[0] + plane.plane_par[1] * point[1] + plane.plane_par[2] * point[2] - plane.plane_par[3];

    if(judge > 0)
        judge = 1;
    else if(judge == 0)
        judge = 0;
    else
        judge = -1;

    return judge;
}

void plane_dir_edge(edge &temp, plane plane, int dir[2])
{
    dir[0] = plane_dir_point(temp.point[0], plane);
    dir[1] = plane_dir_point(temp.point[1], plane);
}

void plane_dist_edge(edge &temp, plane plane, float dist[2])
{
    float judge1 = plane.plane_par[0] * temp.point[0][0] + plane.plane_par[1] * temp.point[0][1] + plane.plane_par[2] * temp.point[0][2] - plane.plane_par[3];
    float judge2 = plane.plane_par[0] * temp.point[1][0] + plane.plane_par[1] * temp.point[1][1] + plane.plane_par[2] * temp.point[1][2] - plane.plane_par[3];

    dist[0] = fabs(judge1) / sqrt(pow(plane.plane_par[0],2) + pow(plane.plane_par[1],2) + pow(plane.plane_par[2],2));
    dist[1] = fabs(judge2) / sqrt(pow(plane.plane_par[0],2) + pow(plane.plane_par[1],2) + pow(plane.plane_par[2],2));
}

void split_all_edge(GLMmodel *myObj, std::vector<edge> &all_edge,std::vector<bool> &is_face_split, plane plane)
{
    for(unsigned int i = 0; i < all_edge.size(); i +=1){
        int dir[2];
        float dist[2];
        plane_dir_edge(all_edge.at(i), plane, dir);
        plane_dist_edge(all_edge.at(i), plane, dist);

        if(dir[0] && dir[1] && (dir[0] != dir[1])){
            float edge_ratio = dist[0] / (dist[0] + dist[1]);
            vec3 new_point = all_edge.at(i).point[0] + edge_ratio * (all_edge.at(i).point[1] - all_edge.at(i).point[0]);
            all_edge.at(i).split_point = new_point;

            if(is_face_split.at(all_edge.at(i).face_id[0]))
                is_face_split.at(all_edge.at(i).face_id[0]) = false;
            if(is_face_split.at(all_edge.at(i).face_id[1]))
                is_face_split.at(all_edge.at(i).face_id[1]) = false;
        }
        else if(dir[0] == 0 || dir[1] == 0){

            vec3 face_vertex1[3];
            for(int j = 0; j < 3; j += 1){
                for(int k = 0; k < 3; k += 1)
                    face_vertex1[j][k] = myObj->vertices[3 * (myObj->triangles.at(all_edge.at(i).face_id[0]).vindices[j]) + k];
            }

            int dir_count1[3] = {0, 0, 0};
            dir_count1[(int)plane_dir_point(face_vertex1[0], plane) + 1] += 1;
            dir_count1[(int)plane_dir_point(face_vertex1[1], plane) + 1] += 1;
            dir_count1[(int)plane_dir_point(face_vertex1[2], plane) + 1] += 1;

            if(dir_count1[0] == 1 && dir_count1[1] == 1 && dir_count1[2] == 1)
                is_face_split.at(all_edge.at(i).face_id[0]) = false;

            vec3 face_vertex2[3];
            for(int j = 0; j < 3; j += 1){
                for(int k = 0; k < 3; k += 1)
                    face_vertex2[j][k] = myObj->vertices[3 * (myObj->triangles.at(all_edge.at(i).face_id[1]).vindices[j]) + k];
            }

            int dir_count2[3] = {0, 0, 0};
            dir_count2[(int)plane_dir_point(face_vertex2[0], plane) + 1] += 1;
            dir_count2[(int)plane_dir_point(face_vertex2[1], plane) + 1] += 1;
            dir_count2[(int)plane_dir_point(face_vertex2[2], plane) + 1] += 1;

            if(dir_count2[0] == 1 && dir_count2[1] == 1 && dir_count2[2] == 1){
                is_face_split.at(all_edge.at(i).face_id[1]) = false;

                if(dir[0] == 0){
                    all_edge.at(i).split_point = all_edge.at(i).point[0];
                    all_edge.at(i).vertex_push_index = all_edge.at(i).index[0];
                }
                else if(dir[1] == 0){
                    all_edge.at(i).split_point = all_edge.at(i).point[1];
                    all_edge.at(i).vertex_push_index = all_edge.at(i).index[1];
                }
            }
        }
    }
}

void split_face(GLMmodel *myObj, std::vector<edge> &all_edge,std::vector<bool> &is_face_split, plane plane)
{
    split_all_edge(myObj, all_edge, is_face_split, plane);

    unsigned int current_numtri = myObj->numtriangles;
    for(unsigned int i = 0; i < current_numtri; i += 1){
        if(!is_face_split.at(i)){

            vec3 point_dir;
            int dir_count[3] = {0, 0, 0};
            for(int j = 0; j < 3; j += 1){
                vec3 temp(myObj->vertices.at(3 * (myObj->triangles.at(i).vindices[j]) + 0), myObj->vertices.at(3 * (myObj->triangles.at(i).vindices[j]) + 1), myObj->vertices.at(3 * (myObj->triangles.at(i).vindices[j]) + 2));
                point_dir[j] = plane_dir_point(temp,plane);
                dir_count[(int)point_dir[j] + 1] += 1;
            }
            int choose_dir = -1;
            if(dir_count[0] == 1 && dir_count[1] == 1 && dir_count[2] == 1)
                choose_dir = 0;
            else if(dir_count[2] == 1)
                choose_dir = 1;

            int choose_index = 0;
            for(int j = 0; j < 3; j += 1){
                if(point_dir[j] == choose_dir)
                    choose_index = j;
            }

            vec3 choose_edge(-1, -1, -1);
            vec2 choose_vertex_index(-1,-1);
            int one_split = 3;

            for(int j = 0; j < 3; j += 1){
                bool judge1,judge2;
                judge1 = (myObj->triangles.at(i).vindices[choose_index] == (unsigned int)all_edge.at(myObj->triangles.at(i).edge_index[j]).index[0]) && (myObj->triangles.at(i).vindices[(choose_index + 1) % 3] == (unsigned int)all_edge.at(myObj->triangles.at(i).edge_index[j]).index[1]);
                judge2 = (myObj->triangles.at(i).vindices[choose_index] == (unsigned int)all_edge.at(myObj->triangles.at(i).edge_index[j]).index[1]) && (myObj->triangles.at(i).vindices[(choose_index + 1) % 3] == (unsigned int)all_edge.at(myObj->triangles.at(i).edge_index[j]).index[0]);
                if(judge1){
                    choose_edge[0] = myObj->triangles.at(i).edge_index[j];
                    one_split -= j;
                    choose_vertex_index[0] = 1;
                }
                if(judge2){
                    choose_edge[0] = myObj->triangles.at(i).edge_index[j];
                    one_split -= j;
                    choose_vertex_index[0] = 0;
                }

                judge1 = (myObj->triangles.at(i).vindices[choose_index] == (unsigned int)all_edge.at(myObj->triangles.at(i).edge_index[j]).index[0]) && (myObj->triangles.at(i).vindices[(choose_index + 2) % 3] == (unsigned int)all_edge.at(myObj->triangles.at(i).edge_index[j]).index[1]);
                judge2 = (myObj->triangles.at(i).vindices[choose_index] == (unsigned int)all_edge.at(myObj->triangles.at(i).edge_index[j]).index[1]) && (myObj->triangles.at(i).vindices[(choose_index + 2) % 3] == (unsigned int)all_edge.at(myObj->triangles.at(i).edge_index[j]).index[0]);
                if(judge1){
                    choose_edge[1] = myObj->triangles.at(i).edge_index[j];
                    one_split -= j;
                    choose_vertex_index[1] = 1;
                }
                if(judge2){
                    choose_edge[1] = myObj->triangles.at(i).edge_index[j];
                    one_split -= j;
                    choose_vertex_index[1] = 0;
                }
            }

            choose_edge[2] = myObj->triangles.at(i).edge_index[one_split];

            if(all_edge.at(choose_edge[0]).vertex_push_index == -1){
                myObj->vertices.push_back(all_edge.at(choose_edge[0]).split_point[0]);
                myObj->vertices.push_back(all_edge.at(choose_edge[0]).split_point[1]);
                myObj->vertices.push_back(all_edge.at(choose_edge[0]).split_point[2]);
                myObj->numvertices += 1;
                all_edge.at(choose_edge[0]).vertex_push_index = myObj->numvertices;
            }

            if(all_edge.at(choose_edge[1]).vertex_push_index == -1){
                myObj->vertices.push_back(all_edge.at(choose_edge[1]).split_point[0]);
                myObj->vertices.push_back(all_edge.at(choose_edge[1]).split_point[1]);
                myObj->vertices.push_back(all_edge.at(choose_edge[1]).split_point[2]);
                myObj->numvertices += 1;
                all_edge.at(choose_edge[1]).vertex_push_index = myObj->numvertices;
            }

            if(choose_dir != 0){
                GLMtriangle temp1,temp2;

                temp1.vindices[0] = all_edge.at(choose_edge[1]).vertex_push_index;
                temp1.vindices[1] = all_edge.at(choose_edge[0]).vertex_push_index;
                temp1.vindices[2] = all_edge.at(choose_edge[1]).index[(int)choose_vertex_index[1]];
                myObj->triangles.push_back(temp1);
                myObj->numtriangles += 1;

                temp2.vindices[0] = all_edge.at(choose_edge[0]).vertex_push_index;
                temp2.vindices[1] = all_edge.at(choose_edge[0]).index[(int)choose_vertex_index[0]];
                temp2.vindices[2] = all_edge.at(choose_edge[1]).index[(int)choose_vertex_index[1]];
                myObj->triangles.push_back(temp2);
                myObj->numtriangles += 1;

                myObj->triangles.at(i).vindices[0] = myObj->triangles.at(i).vindices[choose_index];
                myObj->triangles.at(i).vindices[1] = all_edge.at(choose_edge[0]).vertex_push_index;
                myObj->triangles.at(i).vindices[2] = all_edge.at(choose_edge[1]).vertex_push_index;
            }
            else{
                if(all_edge.at(choose_edge[2]).vertex_push_index == -1){
                    myObj->vertices.push_back(all_edge.at(choose_edge[2]).split_point[0]);
                    myObj->vertices.push_back(all_edge.at(choose_edge[2]).split_point[1]);
                    myObj->vertices.push_back(all_edge.at(choose_edge[2]).split_point[2]);
                    myObj->numvertices += 1;
                    all_edge.at(choose_edge[2]).vertex_push_index = myObj->numvertices;
                }

                GLMtriangle temp;

                temp.vindices[0] = myObj->triangles.at(i).vindices[choose_index];
                temp.vindices[1] = all_edge.at(choose_edge[0]).index[(int)choose_vertex_index[0]];
                temp.vindices[2] = all_edge.at(choose_edge[2]).vertex_push_index;
                myObj->triangles.push_back(temp);
                myObj->numtriangles += 1;

                myObj->triangles.at(i).vindices[0] = myObj->triangles.at(i).vindices[choose_index];
                myObj->triangles.at(i).vindices[1] = all_edge.at(choose_edge[2]).vertex_push_index;
                myObj->triangles.at(i).vindices[2] = all_edge.at(choose_edge[1]).index[(int)choose_vertex_index[1]];
            }
        }
    }

    glmFacetNormals(myObj);
}
