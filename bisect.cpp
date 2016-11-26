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

bool operator==(const edge &lhs,const edge &rhs){
    if((lhs.point[0] != rhs.point[0]) || (lhs.point[1] != rhs.point[1]))
        return false;
    if((lhs.index[0] != rhs.index[0]) || (lhs.index[1] != rhs.index[1]))
        return false;
    if((lhs.face_id[0] != rhs.face_id[0]) || (lhs.face_id[1] != rhs.face_id[1]))
        return false;
    if(lhs.vertex_push_index != rhs.vertex_push_index)
        return false;
    if(lhs.l != rhs.l)
        return false;
    return true;
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

bool split_edge_test(GLMmodel *myObj, std::vector<edge> &all_edge,std::vector<bool> &is_face_split,int split_tri_id, plane plane)
{
    bool judge = false;
    for(unsigned int i = 0; i < 3; i +=1){
        int dir[2];
        float dist[2];
        plane_dir_edge(all_edge.at(myObj->triangles.at(split_tri_id).edge_index[i]), plane, dir);
        plane_dist_edge(all_edge.at(myObj->triangles.at(split_tri_id).edge_index[i]), plane, dist);

        if(dir[0] && dir[1] && (dir[0] != dir[1])){
            float edge_ratio = dist[0] / (dist[0] + dist[1]);
            vec3 new_point = all_edge.at(myObj->triangles.at(split_tri_id).edge_index[i]).point[0] + edge_ratio * (all_edge.at(myObj->triangles.at(split_tri_id).edge_index[i]).point[1] - all_edge.at(myObj->triangles.at(split_tri_id).edge_index[i]).point[0]);
            all_edge.at(myObj->triangles.at(split_tri_id).edge_index[i]).split_point = new_point;
            if(!judge)
                judge = true;
        }
        else if(dir[0] == 0 || dir[1] == 0){

            vec3 face_vertex1[3];
            for(int j = 0; j < 3; j += 1){
                for(int k = 0; k < 3; k += 1)
                    face_vertex1[j][k] = myObj->vertices[3 * myObj->triangles.at(split_tri_id).vindices[j] + k];
            }

            int dir_count1[3] = {0, 0, 0};
            dir_count1[(int)plane_dir_point(face_vertex1[0], plane) + 1] += 1;
            dir_count1[(int)plane_dir_point(face_vertex1[1], plane) + 1] += 1;
            dir_count1[(int)plane_dir_point(face_vertex1[2], plane) + 1] += 1;

            if(dir_count1[0] == 1 && dir_count1[1] == 1 && dir_count1[2] == 1){
                if(dir[0] == 0){
                    all_edge.at(i).split_point = all_edge.at(i).point[0];
                    all_edge.at(i).vertex_push_index = all_edge.at(i).index[0];
                }
                else if(dir[1] == 0){
                    all_edge.at(i).split_point = all_edge.at(i).point[1];
                    all_edge.at(i).vertex_push_index = all_edge.at(i).index[1];
                }
                if(!judge)
                    judge = true;
            }
        }
    }
    return judge;
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
                glmOneFacetNormals(myObj, myObj->numtriangles);
                myObj->numtriangles += 1;

                temp2.vindices[0] = all_edge.at(choose_edge[0]).vertex_push_index;
                temp2.vindices[1] = all_edge.at(choose_edge[0]).index[(int)choose_vertex_index[0]];
                temp2.vindices[2] = all_edge.at(choose_edge[1]).index[(int)choose_vertex_index[1]];
                myObj->triangles.push_back(temp2);
                glmOneFacetNormals(myObj, myObj->numtriangles);
                myObj->numtriangles += 1;

                myObj->triangles.at(i).vindices[0] = myObj->triangles.at(i).vindices[choose_index];
                myObj->triangles.at(i).vindices[1] = all_edge.at(choose_edge[0]).vertex_push_index;
                myObj->triangles.at(i).vindices[2] = all_edge.at(choose_edge[1]).vertex_push_index;
                glmOneFacetNormals(myObj, i);
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
                glmOneFacetNormals(myObj, myObj->numtriangles);
                myObj->numtriangles += 1;

                myObj->triangles.at(i).vindices[0] = myObj->triangles.at(i).vindices[choose_index];
                myObj->triangles.at(i).vindices[1] = all_edge.at(choose_edge[2]).vertex_push_index;
                myObj->triangles.at(i).vindices[2] = all_edge.at(choose_edge[1]).index[(int)choose_vertex_index[1]];
                glmOneFacetNormals(myObj, i);
            }
        }
    }
}

void split_face_test(GLMmodel *myObj, std::vector<edge> &all_edge,std::vector<bool> &is_face_split,std::vector<int> &face_split_by_plane, std::vector<plane> &planes) // split face not in range have to erace
{
//    for(unsigned int i = 0; i < face_split_by_plane.size(); i += 1){
    unsigned int current = face_split_by_plane.size();
    for(unsigned int i = 0; i < current; i += 1){
//        for(unsigned int j = 0; j < myObj->triangles.at(face_split_by_plane.at(i)).splite_plane_id.size(); j += 1){
            plane test_plane = planes.at(myObj->triangles.at(face_split_by_plane.at(i)).splite_plane_id.at(0));
            bool test = split_edge_test(myObj, all_edge, is_face_split, face_split_by_plane.at(i), test_plane);
//            std::cout << myObj->triangles.at(face_split_by_plane.at(i)).splite_plane_id.at(0) << std::endl;
            myObj->triangles.at(face_split_by_plane.at(i)).splite_plane_id.erase(myObj->triangles.at(face_split_by_plane.at(i)).splite_plane_id.begin() + 0);

//            myObj->triangles.at(face_split_by_plane.at(i)).splite_plane_id.at(0)
            if(test){
//            if(myObj->triangles.at(face_split_by_plane.at(i)).splite_plane_id.size() > 0){
//                myObj->triangles.at(face_split_by_plane.at(i)).splite_plane_id.erase(myObj->triangles.at(face_split_by_plane.at(i)).splite_plane_id.begin());

                vec3 point_dir;
                int dir_count[3] = {0, 0, 0};
                for(int k = 0; k < 3; k += 1){
                    vec3 temp(myObj->vertices.at(3 * (myObj->triangles.at(face_split_by_plane.at(i)).vindices[k]) + 0), myObj->vertices.at(3 * (myObj->triangles.at(face_split_by_plane.at(i)).vindices[k]) + 1), myObj->vertices.at(3 * (myObj->triangles.at(face_split_by_plane.at(i)).vindices[k]) + 2));
                    point_dir[k] = plane_dir_point(temp, test_plane);
                    dir_count[(int)point_dir[k] + 1] += 1;
                }

                int choose_dir = -1;
                if(dir_count[0] == 1 && dir_count[1] == 1 && dir_count[2] == 1)
                    choose_dir = 0;
                else if(dir_count[2] == 1)
                    choose_dir = 1;

                int choose_index = 0;
                for(int k = 0; k < 3; k += 1){
                    if(point_dir[k] == choose_dir)
                        choose_index = k;
                }

                vec3 choose_edge(-1, -1, -1);
                vec2 choose_vertex_index(-1,-1);
                int one_split = 3;

                for(int k = 0; k < 3; k += 1){
                    bool judge1,judge2;
                    judge1 = (myObj->triangles.at(face_split_by_plane.at(i)).vindices[choose_index] == (unsigned int)all_edge.at(myObj->triangles.at(face_split_by_plane.at(i)).edge_index[k]).index[0]) && (myObj->triangles.at(face_split_by_plane.at(i)).vindices[(choose_index + 1) % 3] == (unsigned int)all_edge.at(myObj->triangles.at(face_split_by_plane.at(i)).edge_index[k]).index[1]);
                    judge2 = (myObj->triangles.at(face_split_by_plane.at(i)).vindices[choose_index] == (unsigned int)all_edge.at(myObj->triangles.at(face_split_by_plane.at(i)).edge_index[k]).index[1]) && (myObj->triangles.at(face_split_by_plane.at(i)).vindices[(choose_index + 1) % 3] == (unsigned int)all_edge.at(myObj->triangles.at(face_split_by_plane.at(i)).edge_index[k]).index[0]);
                    if(judge1){
                        choose_edge[0] = myObj->triangles.at(face_split_by_plane.at(i)).edge_index[k];
                        one_split -= k;
                        choose_vertex_index[0] = 1;
                    }
                    if(judge2){
                        choose_edge[0] = myObj->triangles.at(face_split_by_plane.at(i)).edge_index[k];
                        one_split -= k;
                        choose_vertex_index[0] = 0;
                    }

                    judge1 = (myObj->triangles.at(face_split_by_plane.at(i)).vindices[choose_index] == (unsigned int)all_edge.at(myObj->triangles.at(face_split_by_plane.at(i)).edge_index[k]).index[0]) && (myObj->triangles.at(face_split_by_plane.at(i)).vindices[(choose_index + 2) % 3] == (unsigned int)all_edge.at(myObj->triangles.at(face_split_by_plane.at(i)).edge_index[k]).index[1]);
                    judge2 = (myObj->triangles.at(face_split_by_plane.at(i)).vindices[choose_index] == (unsigned int)all_edge.at(myObj->triangles.at(face_split_by_plane.at(i)).edge_index[k]).index[1]) && (myObj->triangles.at(face_split_by_plane.at(i)).vindices[(choose_index + 2) % 3] == (unsigned int)all_edge.at(myObj->triangles.at(face_split_by_plane.at(i)).edge_index[k]).index[0]);
                    if(judge1){
                        choose_edge[1] = myObj->triangles.at(face_split_by_plane.at(i)).edge_index[k];
                        one_split -= k;
                        choose_vertex_index[1] = 1;
                    }
                    if(judge2){
                        choose_edge[1] = myObj->triangles.at(face_split_by_plane.at(i)).edge_index[k];
                        one_split -= k;
                        choose_vertex_index[1] = 0;
                    }
                }

                choose_edge[2] = myObj->triangles.at(face_split_by_plane.at(i)).edge_index[one_split];
//                std::cout << choose_edge[0] << " " << choose_edge[1] << " " << choose_edge[2] << std::endl;

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

//                for(int k = 0; k < myObj->triangles.at(face_split_by_plane.at(i)).splite_plane_id.size(); k += 1)
//                    std::cout << "test : " << myObj->triangles.at(face_split_by_plane.at(i)).splite_plane_id.at(k) << " ";
//                std::cout << std::endl;

                if(choose_dir != 0){
//                    std::cout << planes.at(j).dir << std::endl;

                    GLMtriangle temp1,temp2;

                    temp1.vindices[0] = all_edge.at(choose_edge[1]).vertex_push_index;
                    temp1.vindices[1] = all_edge.at(choose_edge[0]).vertex_push_index;
                    temp1.vindices[2] = all_edge.at(choose_edge[1]).index[(int)choose_vertex_index[1]];
                    temp1.splite_plane_id = myObj->triangles.at(face_split_by_plane.at(i)).splite_plane_id;
                    myObj->triangles.push_back(temp1);
                    glmOneFacetNormals(myObj, myObj->numtriangles);
                    myObj->numtriangles += 1;

                    dir_count[0] = 0;
                    dir_count[1] = 0;
                    dir_count[2] = 0;
                    for(int k = 0; k < 3; k += 1){
                        vec3 temp(myObj->vertices.at(3 * (temp1.vindices[k]) + 0), myObj->vertices.at(3 * (temp1.vindices[k]) + 1), myObj->vertices.at(3 * (temp1.vindices[k]) + 2));
                        point_dir[k] = plane_dir_point(temp, test_plane);
                        dir_count[(int)point_dir[k] + 1] += 1;
                    }
                    if(dir_count[test_plane.dir * -1 + 1] == 0){
//                        std::cout << dir_count[0] << " " << dir_count[1] << " " << dir_count[2] << std::endl;
                        face_split_by_plane.push_back(myObj->numtriangles - 1);
                    }

//                    std::cout << 1 << " : " << temp1.vindices[0] << " " << temp1.vindices[1] << " " << temp1.vindices[2] << std::endl;
//                    std::cout << myObj->vertices[3*temp1.vindices[0]+0] << " " << myObj->vertices[3*temp1.vindices[0]+1] << " " << myObj->vertices[3*temp1.vindices[0]+2] << std::endl;
//                    std::cout << myObj->vertices[3*temp1.vindices[1]+0] << " " << myObj->vertices[3*temp1.vindices[1]+1] << " " << myObj->vertices[3*temp1.vindices[1]+2] << std::endl;
//                    std::cout << myObj->vertices[3*temp1.vindices[2]+0] << " " << myObj->vertices[3*temp1.vindices[2]+1] << " " << myObj->vertices[3*temp1.vindices[2]+2] << std::endl;
//                    for(int k = 0; k < myObj->triangles.at(myObj->numtriangles-1).splite_plane_id.size(); k += 1)
//                        std::cout << "test 1: " << myObj->triangles.at(myObj->numtriangles-1).splite_plane_id.at(k) << " ";
//                    std::cout << std::endl;
//                    std::cout << myObj->numtriangles << std::endl;
//                    std::cout << std::endl;

                    temp2.vindices[0] = all_edge.at(choose_edge[0]).vertex_push_index;
                    temp2.vindices[1] = all_edge.at(choose_edge[0]).index[(int)choose_vertex_index[0]];
                    temp2.vindices[2] = all_edge.at(choose_edge[1]).index[(int)choose_vertex_index[1]];
                    temp2.splite_plane_id = myObj->triangles.at(face_split_by_plane.at(i)).splite_plane_id;
                    myObj->triangles.push_back(temp2);
                    glmOneFacetNormals(myObj, myObj->numtriangles);
                    myObj->numtriangles += 1;

                    dir_count[0] = 0;
                    dir_count[1] = 0;
                    dir_count[2] = 0;
                    for(int k = 0; k < 3; k += 1){
                        vec3 temp(myObj->vertices.at(3 * (temp2.vindices[k]) + 0), myObj->vertices.at(3 * (temp2.vindices[k]) + 1), myObj->vertices.at(3 * (temp2.vindices[k]) + 2));
                        point_dir[k] = plane_dir_point(temp, test_plane);
                        dir_count[(int)point_dir[k] + 1] += 1;
                    }
                    if(dir_count[test_plane.dir * -1 + 1] == 0){
//                        std::cout << dir_count[0] << " " << dir_count[1] << " " << dir_count[2] << std::endl;
                        face_split_by_plane.push_back(myObj->numtriangles - 1);
                    }
//                    std::cout << 2 << " : " << temp2.vindices[0] << " " << temp2.vindices[1] << " " << temp2.vindices[2] << std::endl;
//                    std::cout << myObj->vertices[3*temp2.vindices[0]+0] << " " << myObj->vertices[3*temp2.vindices[0]+1] << " " << myObj->vertices[3*temp2.vindices[0]+2] << std::endl;
//                    std::cout << myObj->vertices[3*temp2.vindices[1]+0] << " " << myObj->vertices[3*temp2.vindices[1]+1] << " " << myObj->vertices[3*temp2.vindices[1]+2] << std::endl;
//                    std::cout << myObj->vertices[3*temp2.vindices[2]+0] << " " << myObj->vertices[3*temp2.vindices[2]+1] << " " << myObj->vertices[3*temp2.vindices[2]+2] << std::endl;
//                    for(int k = 0; k < myObj->triangles.at(myObj->numtriangles-1).splite_plane_id.size(); k += 1)
//                        std::cout << "test 2: " << myObj->triangles.at(myObj->numtriangles-1).splite_plane_id.at(k) << " ";
//                    std::cout << std::endl;
//                    std::cout << myObj->numtriangles << std::endl;
//                    std::cout << std::endl;

                    myObj->triangles.at(face_split_by_plane.at(i)).vindices[0] = myObj->triangles.at(face_split_by_plane.at(i)).vindices[choose_index];
                    myObj->triangles.at(face_split_by_plane.at(i)).vindices[1] = all_edge.at(choose_edge[0]).vertex_push_index;
                    myObj->triangles.at(face_split_by_plane.at(i)).vindices[2] = all_edge.at(choose_edge[1]).vertex_push_index;
                    glmOneFacetNormals(myObj, face_split_by_plane.at(i));

                    dir_count[0] = 0;
                    dir_count[1] = 0;
                    dir_count[2] = 0;
                    for(int k = 0; k < 3; k += 1){
                        vec3 temp(myObj->vertices.at(3 * (myObj->triangles.at(face_split_by_plane.at(i)).vindices[k]) + 0), myObj->vertices.at(3 * (myObj->triangles.at(face_split_by_plane.at(i)).vindices[k]) + 1), myObj->vertices.at(3 * (myObj->triangles.at(face_split_by_plane.at(i)).vindices[k]) + 2));
                        point_dir[k] = plane_dir_point(temp, test_plane);
                        dir_count[(int)point_dir[k] + 1] += 1;
                    }
                    if(dir_count[test_plane.dir * -1 + 1] == 0){
//                        std::cout << dir_count[0] << " " << dir_count[1] << " " << dir_count[2] << std::endl;
                        face_split_by_plane.push_back(face_split_by_plane.at(i));
                    }
//                    std::cout << 3 << " : " << myObj->triangles.at(face_split_by_plane.at(i)).vindices[0] << " " << myObj->triangles.at(face_split_by_plane.at(i)).vindices[1] << " " << myObj->triangles.at(face_split_by_plane.at(i)).vindices[2] << std::endl;
//                    std::cout << myObj->vertices[3*myObj->triangles.at(face_split_by_plane.at(i)).vindices[0]+0] << " " << myObj->vertices[3*myObj->triangles.at(face_split_by_plane.at(i)).vindices[0]+1] << " " << myObj->vertices[3*myObj->triangles.at(face_split_by_plane.at(i)).vindices[0]+2] << std::endl;
//                    std::cout << myObj->vertices[3*myObj->triangles.at(face_split_by_plane.at(i)).vindices[1]+0] << " " << myObj->vertices[3*myObj->triangles.at(face_split_by_plane.at(i)).vindices[1]+1] << " " << myObj->vertices[3*myObj->triangles.at(face_split_by_plane.at(i)).vindices[1]+2] << std::endl;
//                    std::cout << myObj->vertices[3*myObj->triangles.at(face_split_by_plane.at(i)).vindices[2]+0] << " " << myObj->vertices[3*myObj->triangles.at(face_split_by_plane.at(i)).vindices[2]+1] << " " << myObj->vertices[3*myObj->triangles.at(face_split_by_plane.at(i)).vindices[2]+2] << std::endl;
//                    for(int k = 0; k < myObj->triangles.at(face_split_by_plane.at(i)).splite_plane_id.size(); k += 1)
//                        std::cout << "test 3: " << myObj->triangles.at(face_split_by_plane.at(i)).splite_plane_id.at(k) << " ";
//                    std::cout << std::endl;
//                    std::cout << face_split_by_plane.at(i) << std::endl;
//                    std::cout << std::endl;
//                    std::cout << std::endl;
                    if(is_face_split[all_edge.at(choose_edge[0]).face_id[0]] || is_face_split[all_edge.at(choose_edge[0]).face_id[1]]){
                        if(is_face_split[all_edge.at(choose_edge[0]).face_id[0]])
                            tri_poly(myObj, all_edge, all_edge.at(choose_edge[0]).face_id[0], all_edge.at(choose_edge[0]));
                        if(is_face_split[all_edge.at(choose_edge[0]).face_id[1]])
                            tri_poly(myObj, all_edge, all_edge.at(choose_edge[0]).face_id[1], all_edge.at(choose_edge[0]));
                    }
                    if(is_face_split[all_edge.at(choose_edge[1]).face_id[0]] || is_face_split[all_edge.at(choose_edge[1]).face_id[1]]){
                        if(is_face_split[all_edge.at(choose_edge[1]).face_id[0]])
                            tri_poly(myObj, all_edge, all_edge.at(choose_edge[1]).face_id[0], all_edge.at(choose_edge[1]));
                        if(is_face_split[all_edge.at(choose_edge[1]).face_id[1]])
                            tri_poly(myObj, all_edge, all_edge.at(choose_edge[1]).face_id[1], all_edge.at(choose_edge[1]));
                    }
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

                    temp.vindices[0] = myObj->triangles.at(face_split_by_plane.at(i)).vindices[choose_index];
                    temp.vindices[1] = all_edge.at(choose_edge[0]).index[(int)choose_vertex_index[0]];
                    temp.vindices[2] = all_edge.at(choose_edge[2]).vertex_push_index;
                    temp.splite_plane_id = myObj->triangles.at(face_split_by_plane.at(i)).splite_plane_id;
                    myObj->triangles.push_back(temp);
                    glmOneFacetNormals(myObj, myObj->numtriangles);
                    myObj->numtriangles += 1;

                    dir_count[0] = 0;
                    dir_count[1] = 0;
                    dir_count[2] = 0;
                    for(int k = 0; k < 3; k += 1){
                        vec3 temp2(myObj->vertices.at(3 * (temp.vindices[k]) + 0), myObj->vertices.at(3 * (temp.vindices[k]) + 1), myObj->vertices.at(3 * (temp.vindices[k]) + 2));
                        point_dir[k] = plane_dir_point(temp2, test_plane);
                        dir_count[(int)point_dir[k] + 1] += 1;
                    }
                    if(dir_count[test_plane.dir * -1 + 1] == 0){
//                        std::cout << dir_count[0] << " " << dir_count[1] << " " << dir_count[2] << std::endl;
                        face_split_by_plane.push_back(myObj->numtriangles - 1);
                    }

                    myObj->triangles.at(face_split_by_plane.at(i)).vindices[0] = myObj->triangles.at(face_split_by_plane.at(i)).vindices[choose_index];
                    myObj->triangles.at(face_split_by_plane.at(i)).vindices[1] = all_edge.at(choose_edge[2]).vertex_push_index;
                    myObj->triangles.at(face_split_by_plane.at(i)).vindices[2] = all_edge.at(choose_edge[1]).index[(int)choose_vertex_index[1]];
                    glmOneFacetNormals(myObj, face_split_by_plane.at(i));

                    dir_count[0] = 0;
                    dir_count[1] = 0;
                    dir_count[2] = 0;
                    for(int k = 0; k < 3; k += 1){
                        vec3 temp(myObj->vertices.at(3 * (myObj->triangles.at(face_split_by_plane.at(i)).vindices[k]) + 0), myObj->vertices.at(3 * (myObj->triangles.at(face_split_by_plane.at(i)).vindices[k]) + 1), myObj->vertices.at(3 * (myObj->triangles.at(face_split_by_plane.at(i)).vindices[k]) + 2));
                        point_dir[k] = plane_dir_point(temp, test_plane);
                        dir_count[(int)point_dir[k] + 1] += 1;
                    }
                    if(dir_count[test_plane.dir * -1 + 1] == 0){
//                        std::cout << dir_count[0] << " " << dir_count[1] << " " << dir_count[2] << std::endl;
                        face_split_by_plane.push_back(face_split_by_plane.at(i));
                    }

                    if(is_face_split[all_edge.at(choose_edge[2]).face_id[0]] || is_face_split[all_edge.at(choose_edge[2]).face_id[1]]){
                        if(is_face_split[all_edge.at(choose_edge[2]).face_id[0]])
                            tri_poly(myObj, all_edge, all_edge.at(choose_edge[2]).face_id[0], all_edge.at(choose_edge[2]));
                        if(is_face_split[all_edge.at(choose_edge[2]).face_id[1]])
                            tri_poly(myObj, all_edge, all_edge.at(choose_edge[2]).face_id[1], all_edge.at(choose_edge[2]));
                    }
                }
            }
//        }
    }
}

void tri_poly(GLMmodel *myObj, std::vector<edge> &all_edge, int face_id, edge &splited_edge){
    int current_index;
    for(int i = 0; i < 3; i += 1){
        if(all_edge.at(myObj->triangles.at(face_id).edge_index[i]) == splited_edge){
            current_index = i;
            break;
        }
    }

    int choose_index[2] = {myObj->triangles.at(face_id).edge_index[(current_index + 2) % 3], myObj->triangles.at(face_id).edge_index[(current_index + 1) % 3]};
    int top_index;
    int choose_vertex_index[2] = {-1, -1};
    bool judge1,judge2;
    judge1 = all_edge.at(choose_index[0]).index[0] == all_edge.at(choose_index[1]).index[0];
    judge2 = all_edge.at(choose_index[0]).index[0] == all_edge.at(choose_index[1]).index[1];
    if(judge1){
        top_index = 0;
        choose_vertex_index[0] = 1;
        choose_vertex_index[1] = 1;
    }
    if(judge2){
        top_index = 0;
        choose_vertex_index[0] = 1;
        choose_vertex_index[1] = 0;
    }
    if(choose_vertex_index[0] == -1 && choose_vertex_index[1] == -1){
        judge1 = all_edge.at(choose_index[0]).index[1] == all_edge.at(choose_index[1]).index[0];
        judge2 = all_edge.at(choose_index[0]).index[1] == all_edge.at(choose_index[1]).index[1];
        if(judge1){
            top_index = 1;
            choose_vertex_index[0] = 0;
            choose_vertex_index[1] = 1;
        }
        if(judge2){
            top_index = 1;
            choose_vertex_index[0] = 0;
            choose_vertex_index[1] = 0;
        }
    }

    GLMtriangle temp;

    temp.vindices[0] = all_edge.at(choose_index[0]).index[top_index];
    temp.vindices[1] = all_edge.at(choose_index[0]).index[choose_vertex_index[0]];
    temp.vindices[2] = splited_edge.vertex_push_index;
    myObj->triangles.push_back(temp);
    glmOneFacetNormals(myObj, myObj->numtriangles);
    myObj->numtriangles += 1;

    myObj->triangles.at(face_id).vindices[0] = all_edge.at(choose_index[0]).index[top_index];
    myObj->triangles.at(face_id).vindices[1] = splited_edge.vertex_push_index;
    myObj->triangles.at(face_id).vindices[2] = all_edge.at(choose_index[1]).index[choose_vertex_index[1]];
    glmOneFacetNormals(myObj, face_id);
}
