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
//    l = length();
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
//    l = length();
}

//float edge::length()
//{
//    vec3 temp = point[1] - point[0];
//    return temp.length();
//}

bool operator==(const edge &lhs,const edge &rhs){
    if((lhs.point[0] != rhs.point[0]) || (lhs.point[1] != rhs.point[1]))
        return false;
    if((lhs.index[0] != rhs.index[0]) || (lhs.index[1] != rhs.index[1]))
        return false;
    if((lhs.face_id[0] != rhs.face_id[0]) || (lhs.face_id[1] != rhs.face_id[1]))
        return false;
    if(lhs.vertex_push_index != rhs.vertex_push_index)
        return false;
//    if(lhs.l != rhs.l)
//        return false;
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

float plane_dir_point(vec3 &point, plane plane) //have problem
{
    float judge = plane.plane_par[0] * point[0] + plane.plane_par[1] * point[1] + plane.plane_par[2] * point[2] - plane.plane_par[3];

    if(judge > 0.000001)
        judge = 1;
    else if(judge < -0.000001)
        judge = -1;
    else
        judge = 0;

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

void cut_intersection(GLMmodel *myObj, std::vector<plane> planes, std::vector<int> &face_split_by_plane, bool have_dir){

    if(!face_split_by_plane.empty()){
        std::vector<int> v;
        face_split_by_plane.swap(v);
    }

    for(unsigned int i = 0; i < myObj->numtriangles; i += 1){
        vec3 point_dir;
        int dir_count[3] = {0, 0, 0};
        for(int j = 0; j < 3; j += 1){
            vec3 temp(myObj->vertices.at(3 * (myObj->triangles.at(i).vindices[j]) + 0), myObj->vertices.at(3 * (myObj->triangles.at(i).vindices[j]) + 1), myObj->vertices.at(3 * (myObj->triangles.at(i).vindices[j]) + 2));
            point_dir[j] = plane_dir_point(temp, planes.at(0));
            dir_count[(int)point_dir[j] + 1] += 1;
        }
        if((dir_count[(planes.at(0).dir + 2) % 3] + dir_count[(planes.at(0).dir + 3) % 3]) != 3){
            face_split_by_plane.push_back(i);
            myObj->triangles.at(i).split_by_process = true;
            if(!myObj->triangles.at(i).split_plane_id.empty()){
                std::vector<int> v;
                myObj->triangles.at(i).split_plane_id.swap(v);
            }
            myObj->triangles.at(i).split_plane_id.push_back(0);
        }
    }

    for(unsigned int i = 1; i < planes.size(); i += 1){
        for(unsigned int j = 0; j < face_split_by_plane.size(); j += 1){
            vec3 point_dir;
            int dir_count[3] = {0, 0, 0};
            for(int k = 0; k < 3; k += 1){
                vec3 temp(myObj->vertices.at(3 * (myObj->triangles.at(face_split_by_plane.at(j)).vindices[k]) + 0), myObj->vertices.at(3 * (myObj->triangles.at(face_split_by_plane.at(j)).vindices[k]) + 1), myObj->vertices.at(3 * (myObj->triangles.at(face_split_by_plane.at(j)).vindices[k]) + 2));
                point_dir[k] = plane_dir_point(temp, planes.at(i));
                dir_count[(int)point_dir[k] + 1] += 1;
            }

            if(dir_count[planes.at(i).dir + 1] == 0 && dir_count[1] == 0){
                if(!myObj->triangles.at(face_split_by_plane.at(j)).split_plane_id.empty()){
                    std::vector<int> v;
                    myObj->triangles.at(face_split_by_plane.at(j)).split_plane_id.swap(v);
                }
                myObj->triangles.at(face_split_by_plane.at(j)).split_by_process = false;
                face_split_by_plane.erase(face_split_by_plane.begin() + j);
                j -= 1;
            }
            else if(have_dir && planes.size() >= 2 && i == planes.size() - 1 && dir_count[planes.at(i).dir + 1] != 3){
                if(!myObj->triangles.at(face_split_by_plane.at(j)).split_plane_id.empty()){
                    std::vector<int> v;
                    myObj->triangles.at(face_split_by_plane.at(j)).split_plane_id.swap(v);
                }
                myObj->triangles.at(face_split_by_plane.at(j)).split_by_process = false;
                face_split_by_plane.erase(face_split_by_plane.begin() + j);
                j -= 1;
            }
            else{
                if(i != planes.size() - 1){
                    myObj->triangles.at(face_split_by_plane.at(j)).split_plane_id.push_back(i);
                }
                else{
                    if(!have_dir)
                        myObj->triangles.at(face_split_by_plane.at(j)).split_plane_id.push_back(i);
                }
            }
        }
    }
}

bool split_edge(GLMmodel *myObj, std::vector<edge> &all_edge,int split_tri_id, plane plane)
{
    bool judge = false;
    for(unsigned int i = 0; i < 3; i +=1){
        int dir[2];
        float dist[2];
        plane_dir_edge(all_edge.at(myObj->triangles.at(split_tri_id).edge_index[i]), plane, dir);
        plane_dist_edge(all_edge.at(myObj->triangles.at(split_tri_id).edge_index[i]), plane, dist);

        if(dir[0] && dir[1] && (dir[0] != dir[1])){
            if(all_edge.at(myObj->triangles.at(split_tri_id).edge_index[i]).vertex_push_index == -1){
                float edge_ratio = dist[0] / (dist[0] + dist[1]);
                vec3 new_point = all_edge.at(myObj->triangles.at(split_tri_id).edge_index[i]).point[0] + edge_ratio * (all_edge.at(myObj->triangles.at(split_tri_id).edge_index[i]).point[1] - all_edge.at(myObj->triangles.at(split_tri_id).edge_index[i]).point[0]);
                all_edge.at(myObj->triangles.at(split_tri_id).edge_index[i]).split_point = new_point;
            }
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

void split_face(GLMmodel *myObj, std::vector<edge> &all_edge,std::vector<int> &face_split_by_plane, std::vector<plane> &planes)
{
    unsigned int current = face_split_by_plane.size();
//    for(unsigned int i = 0; i < current; i += 1){
    for(unsigned int i = 0; i < 1; i += 1){
        if(myObj->triangles.at(face_split_by_plane.at(i)).split_plane_id.size() != 0){
            plane test_plane = planes.at(myObj->triangles.at(face_split_by_plane.at(i)).split_plane_id.at(0));
            bool test = split_edge(myObj, all_edge, face_split_by_plane.at(i), test_plane);
            myObj->triangles.at(face_split_by_plane.at(i)).split_plane_id.erase(myObj->triangles.at(face_split_by_plane.at(i)).split_plane_id.begin() + 0);

            if(!test){
                if(myObj->triangles.at(face_split_by_plane.at(i)).split_plane_id.size() != 0){
                    face_split_by_plane.push_back(face_split_by_plane.at(i));
                }
            }
            else{
                cout << myObj->triangles.at(face_split_by_plane.at(i)).vindices[0] << " " << myObj->triangles.at(face_split_by_plane.at(i)).vindices[1] << " " << myObj->triangles.at(face_split_by_plane.at(i)).vindices[2] << endl;

                vec3 point_dir;
                int dir_count[3] = {0, 0, 0};
                for(int j = 0; j < 3; j += 1){
                    vec3 temp(myObj->vertices.at(3 * (myObj->triangles.at(face_split_by_plane.at(i)).vindices[j]) + 0), myObj->vertices.at(3 * (myObj->triangles.at(face_split_by_plane.at(i)).vindices[j]) + 1), myObj->vertices.at(3 * (myObj->triangles.at(face_split_by_plane.at(i)).vindices[j]) + 2));
                    point_dir[j] = plane_dir_point(temp, test_plane);
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
                    judge1 = (myObj->triangles.at(face_split_by_plane.at(i)).vindices[choose_index] == (unsigned int)all_edge.at(myObj->triangles.at(face_split_by_plane.at(i)).edge_index[j]).index[0]) && (myObj->triangles.at(face_split_by_plane.at(i)).vindices[(choose_index + 1) % 3] == (unsigned int)all_edge.at(myObj->triangles.at(face_split_by_plane.at(i)).edge_index[j]).index[1]);
                    judge2 = (myObj->triangles.at(face_split_by_plane.at(i)).vindices[choose_index] == (unsigned int)all_edge.at(myObj->triangles.at(face_split_by_plane.at(i)).edge_index[j]).index[1]) && (myObj->triangles.at(face_split_by_plane.at(i)).vindices[(choose_index + 1) % 3] == (unsigned int)all_edge.at(myObj->triangles.at(face_split_by_plane.at(i)).edge_index[j]).index[0]);
                    if(judge1){
                        choose_edge[0] = myObj->triangles.at(face_split_by_plane.at(i)).edge_index[j];
                        one_split -= j;
                        choose_vertex_index[0] = 1;
                    }
                    if(judge2){
                        choose_edge[0] = myObj->triangles.at(face_split_by_plane.at(i)).edge_index[j];
                        one_split -= j;
                        choose_vertex_index[0] = 0;
                    }

                    judge1 = (myObj->triangles.at(face_split_by_plane.at(i)).vindices[choose_index] == (unsigned int)all_edge.at(myObj->triangles.at(face_split_by_plane.at(i)).edge_index[j]).index[0]) && (myObj->triangles.at(face_split_by_plane.at(i)).vindices[(choose_index + 2) % 3] == (unsigned int)all_edge.at(myObj->triangles.at(face_split_by_plane.at(i)).edge_index[j]).index[1]);
                    judge2 = (myObj->triangles.at(face_split_by_plane.at(i)).vindices[choose_index] == (unsigned int)all_edge.at(myObj->triangles.at(face_split_by_plane.at(i)).edge_index[j]).index[1]) && (myObj->triangles.at(face_split_by_plane.at(i)).vindices[(choose_index + 2) % 3] == (unsigned int)all_edge.at(myObj->triangles.at(face_split_by_plane.at(i)).edge_index[j]).index[0]);
                    if(judge1){
                        choose_edge[1] = myObj->triangles.at(face_split_by_plane.at(i)).edge_index[j];
                        one_split -= j;
                        choose_vertex_index[1] = 1;
                    }
                    if(judge2){
                        choose_edge[1] = myObj->triangles.at(face_split_by_plane.at(i)).edge_index[j];
                        one_split -= j;
                        choose_vertex_index[1] = 0;
                    }
                }

                choose_edge[2] = myObj->triangles.at(face_split_by_plane.at(i)).edge_index[one_split];

                if(choose_dir != 0){
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

                    GLMtriangle temp1,temp2;
                    edge inner_e1,inner_e2;
                    edge outer_e1,outer_e2;
                    int inner_edge_index[2];
                    int outer_edge_index[2];

                    temp1.vindices[0] = all_edge.at(choose_edge[1]).vertex_push_index;
                    temp1.vindices[1] = all_edge.at(choose_edge[0]).vertex_push_index;
                    temp1.vindices[2] = all_edge.at(choose_edge[1]).index[(int)choose_vertex_index[1]];
                    temp1.split_plane_id = myObj->triangles.at(face_split_by_plane.at(i)).split_plane_id;
                    temp1.split_by_process = true;

                    if(all_edge.at(choose_edge[0]).vertex_push_index < all_edge.at(choose_edge[1]).vertex_push_index){
                        inner_e1.index[0] = all_edge.at(choose_edge[0]).vertex_push_index;
                        inner_e1.point[0][0] = myObj->vertices.at(3 * all_edge.at(choose_edge[0]).vertex_push_index + 0);
                        inner_e1.point[0][1] = myObj->vertices.at(3 * all_edge.at(choose_edge[0]).vertex_push_index + 1);
                        inner_e1.point[0][2] = myObj->vertices.at(3 * all_edge.at(choose_edge[0]).vertex_push_index + 2);

                        inner_e1.index[1] = all_edge.at(choose_edge[1]).vertex_push_index;
                        inner_e1.point[1][0] = myObj->vertices.at(3 * all_edge.at(choose_edge[1]).vertex_push_index + 0);
                        inner_e1.point[1][1] = myObj->vertices.at(3 * all_edge.at(choose_edge[1]).vertex_push_index + 1);
                        inner_e1.point[1][2] = myObj->vertices.at(3 * all_edge.at(choose_edge[1]).vertex_push_index + 2);
                    }
                    else{
                        inner_e1.index[1] = all_edge.at(choose_edge[0]).vertex_push_index;
                        inner_e1.point[1][0] = myObj->vertices.at(3 * all_edge.at(choose_edge[0]).vertex_push_index + 0);
                        inner_e1.point[1][1] = myObj->vertices.at(3 * all_edge.at(choose_edge[0]).vertex_push_index + 1);
                        inner_e1.point[1][2] = myObj->vertices.at(3 * all_edge.at(choose_edge[0]).vertex_push_index + 2);

                        inner_e1.index[0] = all_edge.at(choose_edge[1]).vertex_push_index;
                        inner_e1.point[0][0] = myObj->vertices.at(3 * all_edge.at(choose_edge[1]).vertex_push_index + 0);
                        inner_e1.point[0][1] = myObj->vertices.at(3 * all_edge.at(choose_edge[1]).vertex_push_index + 1);
                        inner_e1.point[0][2] = myObj->vertices.at(3 * all_edge.at(choose_edge[1]).vertex_push_index + 2);
                    }
                    inner_e1.face_id[0] = face_split_by_plane.at(i);
                    inner_e1.face_id[1] = myObj->numtriangles + 1;
//                    cout << inner_e1.index[0] << " " << inner_e1.index[1] << endl;
                    all_edge.push_back(inner_e1);
                    inner_edge_index[0] = all_edge.size() - 1;

                    inner_e2.index[0] = all_edge.at(choose_edge[1]).index[(int)choose_vertex_index[1]];
                    inner_e2.point[0][0] = myObj->vertices.at(3 * all_edge.at(choose_edge[1]).index[(int)choose_vertex_index[1]] + 0);
                    inner_e2.point[0][1] = myObj->vertices.at(3 * all_edge.at(choose_edge[1]).index[(int)choose_vertex_index[1]] + 1);
                    inner_e2.point[0][2] = myObj->vertices.at(3 * all_edge.at(choose_edge[1]).index[(int)choose_vertex_index[1]] + 2);

                    inner_e2.index[1] = all_edge.at(choose_edge[0]).vertex_push_index;
                    inner_e2.point[1][0] = myObj->vertices.at(3 * all_edge.at(choose_edge[0]).vertex_push_index + 0);
                    inner_e2.point[1][1] = myObj->vertices.at(3 * all_edge.at(choose_edge[0]).vertex_push_index + 1);
                    inner_e2.point[1][2] = myObj->vertices.at(3 * all_edge.at(choose_edge[0]).vertex_push_index + 2);
//                    cout << e2.index[0] << " " << e2.index[1] << endl;
                    inner_e2.face_id[0] = myObj->numtriangles + 1;
                    inner_e2.face_id[1] = myObj->numtriangles + 2;
                    all_edge.push_back(inner_e2);
                    inner_edge_index[1] = all_edge.size() - 1;

                    outer_e2.index[0] = all_edge.at(choose_edge[1]).index[(int)choose_vertex_index[1]];
                    outer_e2.point[0][0] = myObj->vertices.at(3 * all_edge.at(choose_edge[1]).index[(int)choose_vertex_index[1]] + 0);
                    outer_e2.point[0][1] = myObj->vertices.at(3 * all_edge.at(choose_edge[1]).index[(int)choose_vertex_index[1]] + 1);
                    outer_e2.point[0][2] = myObj->vertices.at(3 * all_edge.at(choose_edge[1]).index[(int)choose_vertex_index[1]] + 2);

                    outer_e2.index[1] = all_edge.at(choose_edge[1]).vertex_push_index;
                    outer_e2.point[1][0] = myObj->vertices.at(3 * all_edge.at(choose_edge[1]).vertex_push_index + 0);
                    outer_e2.point[1][1] = myObj->vertices.at(3 * all_edge.at(choose_edge[1]).vertex_push_index + 1);
                    outer_e2.point[1][2] = myObj->vertices.at(3 * all_edge.at(choose_edge[1]).vertex_push_index + 2);
                    outer_e2.connect_index = choose_edge[1];
                    outer_e2.vertex_push_index = all_edge.at(choose_edge[1]).vertex_push_index;
                    all_edge.push_back(outer_e2);
                    outer_edge_index[1] = all_edge.size() - 1;

                    temp1.edge_index[0] = inner_edge_index[0];
                    temp1.edge_index[1] = inner_edge_index[1];
                    temp1.edge_index[2] = outer_edge_index[1];

                    myObj->triangles.push_back(temp1);
                    glmOneFacetNormals(myObj, myObj->numtriangles);
                    myObj->numtriangles += 1;

                    dir_count[0] = 0;
                    dir_count[1] = 0;
                    dir_count[2] = 0;
                    for(int j = 0; j < 3; j += 1){
                        vec3 temp(myObj->vertices.at(3 * (temp1.vindices[j]) + 0), myObj->vertices.at(3 * (temp1.vindices[j]) + 1), myObj->vertices.at(3 * (temp1.vindices[j]) + 2));
                        point_dir[j] = plane_dir_point(temp, test_plane);
                        dir_count[(int)point_dir[j] + 1] += 1;
                    }
                    if(dir_count[test_plane.dir * -1 + 1] == 0){
                        face_split_by_plane.push_back(myObj->numtriangles - 1);
                    }

                    temp2.vindices[0] = all_edge.at(choose_edge[0]).vertex_push_index;
                    temp2.vindices[1] = all_edge.at(choose_edge[0]).index[(int)choose_vertex_index[0]];
                    temp2.vindices[2] = all_edge.at(choose_edge[1]).index[(int)choose_vertex_index[1]];
                    temp2.split_plane_id = myObj->triangles.at(face_split_by_plane.at(i)).split_plane_id;
                    temp2.split_by_process = true;

                    outer_e1.index[0] = all_edge.at(choose_edge[0]).index[(int)choose_vertex_index[0]];
                    outer_e1.point[0][0] = myObj->vertices.at(3 * all_edge.at(choose_edge[0]).index[(int)choose_vertex_index[0]] + 0);
                    outer_e1.point[0][1] = myObj->vertices.at(3 * all_edge.at(choose_edge[0]).index[(int)choose_vertex_index[0]] + 1);
                    outer_e1.point[0][2] = myObj->vertices.at(3 * all_edge.at(choose_edge[0]).index[(int)choose_vertex_index[0]] + 2);

                    outer_e1.index[1] = all_edge.at(choose_edge[0]).vertex_push_index;
                    outer_e1.point[1][0] = myObj->vertices.at(3 * all_edge.at(choose_edge[0]).vertex_push_index + 0);
                    outer_e1.point[1][1] = myObj->vertices.at(3 * all_edge.at(choose_edge[0]).vertex_push_index + 1);
                    outer_e1.point[1][2] = myObj->vertices.at(3 * all_edge.at(choose_edge[0]).vertex_push_index + 2);
                    outer_e1.connect_index = choose_edge[0];
                    outer_e1.vertex_push_index = all_edge.at(choose_edge[0]).vertex_push_index;
                    all_edge.push_back(outer_e1);
                    outer_edge_index[0] = all_edge.size() - 1;

                    temp2.edge_index[0] = outer_edge_index[0];
                    temp2.edge_index[1] = choose_edge[2];
                    temp2.edge_index[2] = inner_edge_index[1];

                    myObj->triangles.push_back(temp2);
                    glmOneFacetNormals(myObj, myObj->numtriangles);
                    myObj->numtriangles += 1;

                    dir_count[0] = 0;
                    dir_count[1] = 0;
                    dir_count[2] = 0;
                    for(int j = 0; j < 3; j += 1){
                        vec3 temp(myObj->vertices.at(3 * (temp2.vindices[j]) + 0), myObj->vertices.at(3 * (temp2.vindices[j]) + 1), myObj->vertices.at(3 * (temp2.vindices[j]) + 2));
                        point_dir[j] = plane_dir_point(temp, test_plane);
                        dir_count[(int)point_dir[j] + 1] += 1;
                    }
                    if(dir_count[test_plane.dir * -1 + 1] == 0){
                        face_split_by_plane.push_back(myObj->numtriangles - 1);
                    }

//                    if(!myObj->triangles.at(all_edge.at(choose_edge[0]).face_id[0]).split_by_process || !myObj->triangles.at(all_edge.at(choose_edge[0]).face_id[1]).split_by_process){
//                        if(!myObj->triangles.at(all_edge.at(choose_edge[0]).face_id[0]).split_by_process)
//                            tri_poly(myObj, all_edge, all_edge.at(choose_edge[0]).face_id[0], all_edge.at(choose_edge[0]));
//                        if(!myObj->triangles.at(all_edge.at(choose_edge[0]).face_id[1]).split_by_process)
//                            tri_poly(myObj, all_edge, all_edge.at(choose_edge[0]).face_id[1], all_edge.at(choose_edge[0]));
//                    }
//                    if(!myObj->triangles.at(all_edge.at(choose_edge[1]).face_id[0]).split_by_process || !myObj->triangles.at(all_edge.at(choose_edge[1]).face_id[1]).split_by_process){
//                        if(!myObj->triangles.at(all_edge.at(choose_edge[1]).face_id[0]).split_by_process)
//                            tri_poly(myObj, all_edge, all_edge.at(choose_edge[1]).face_id[0], all_edge.at(choose_edge[1]));
//                        if(!myObj->triangles.at(all_edge.at(choose_edge[1]).face_id[1]).split_by_process)
//                            tri_poly(myObj, all_edge, all_edge.at(choose_edge[1]).face_id[1], all_edge.at(choose_edge[1]));
//                    }

                    myObj->triangles.at(face_split_by_plane.at(i)).vindices[0] = myObj->triangles.at(face_split_by_plane.at(i)).vindices[choose_index];
                    myObj->triangles.at(face_split_by_plane.at(i)).vindices[1] = all_edge.at(choose_edge[0]).vertex_push_index;
                    myObj->triangles.at(face_split_by_plane.at(i)).vindices[2] = all_edge.at(choose_edge[1]).vertex_push_index;
                    myObj->triangles.at(face_split_by_plane.at(i)).split_by_process = true;
                    glmOneFacetNormals(myObj, face_split_by_plane.at(i));

//                    all_edge.at(choose_edge[0]).index[(int)choose_vertex_index[0]] = all_edge.at(choose_edge[0]).vertex_push_index;
//                    all_edge.at(choose_edge[0]).point[1][0] = myObj->vertices.at(3 * all_edge.at(choose_edge[0]).vertex_push_index + 0);
//                    all_edge.at(choose_edge[0]).point[1][1] = myObj->vertices.at(3 * all_edge.at(choose_edge[0]).vertex_push_index + 1);
//                    all_edge.at(choose_edge[0]).point[1][2] = myObj->vertices.at(3 * all_edge.at(choose_edge[0]).vertex_push_index + 2);
//                    all_edge.at(choose_edge[0]).connect_index = outer_edge_index[0];
//
//                    all_edge.at(choose_edge[1]).index[(int)choose_vertex_index[1]] = all_edge.at(choose_edge[1]).vertex_push_index;
//                    all_edge.at(choose_edge[1]).point[1][0] = myObj->vertices.at(3 * all_edge.at(choose_edge[1]).vertex_push_index + 0);
//                    all_edge.at(choose_edge[1]).point[1][1] = myObj->vertices.at(3 * all_edge.at(choose_edge[1]).vertex_push_index + 1);
//                    all_edge.at(choose_edge[1]).point[1][2] = myObj->vertices.at(3 * all_edge.at(choose_edge[1]).vertex_push_index + 2);
//                    all_edge.at(choose_edge[1]).connect_index = outer_edge_index[1]; //最後才能改

                    dir_count[0] = 0;
                    dir_count[1] = 0;
                    dir_count[2] = 0;
                    for(int j = 0; j < 3; j += 1){
                        vec3 temp(myObj->vertices.at(3 * (myObj->triangles.at(face_split_by_plane.at(i)).vindices[j]) + 0), myObj->vertices.at(3 * (myObj->triangles.at(face_split_by_plane.at(i)).vindices[j]) + 1), myObj->vertices.at(3 * (myObj->triangles.at(face_split_by_plane.at(i)).vindices[j]) + 2));
                        point_dir[j] = plane_dir_point(temp, test_plane);
                        dir_count[(int)point_dir[j] + 1] += 1;
                    }
                    if(dir_count[test_plane.dir * -1 + 1] == 0){
                        face_split_by_plane.push_back(face_split_by_plane.at(i));
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
                    edge inner_e;
                    edge outer_e;
                    int inner_edge_index;
                    int outer_edge_index;

                    temp.vindices[0] = myObj->triangles.at(face_split_by_plane.at(i)).vindices[choose_index];
                    temp.vindices[1] = all_edge.at(choose_edge[0]).index[(int)choose_vertex_index[0]];
                    temp.vindices[2] = all_edge.at(choose_edge[2]).vertex_push_index;
                    temp.split_plane_id = myObj->triangles.at(face_split_by_plane.at(i)).split_plane_id;
                    temp.split_by_process = true;
//                    cout << temp.vindices[0] << " " << temp.vindices[1] << " " << temp.vindices[2] << endl;

                    inner_e.index[0] = myObj->triangles.at(face_split_by_plane.at(i)).vindices[choose_index];
                    inner_e.index[1] = all_edge.at(choose_edge[2]).vertex_push_index;
                    inner_e.point[0][0] = myObj->vertices.at(3 * myObj->triangles.at(face_split_by_plane.at(i)).vindices[choose_index] + 0);
                    inner_e.point[0][1] = myObj->vertices.at(3 * myObj->triangles.at(face_split_by_plane.at(i)).vindices[choose_index] + 1);
                    inner_e.point[0][2] = myObj->vertices.at(3 * myObj->triangles.at(face_split_by_plane.at(i)).vindices[choose_index] + 2);
                    inner_e.point[1][0] = myObj->vertices.at(3 * all_edge.at(choose_edge[2]).vertex_push_index + 0);
                    inner_e.point[1][1] = myObj->vertices.at(3 * all_edge.at(choose_edge[2]).vertex_push_index + 1);
                    inner_e.point[1][2] = myObj->vertices.at(3 * all_edge.at(choose_edge[2]).vertex_push_index + 2);
                    inner_e.face_id[0] = face_split_by_plane.at(i);
                    inner_e.face_id[1] = myObj->numtriangles + 1;
                    all_edge.push_back(inner_e);
                    inner_edge_index = all_edge.size() - 1;

                    outer_e.index[0] = all_edge.at(choose_edge[0]).index[(int)choose_vertex_index[0]];
                    outer_e.index[1] = all_edge.at(choose_edge[2]).vertex_push_index;
                    outer_e.point[0][0] = myObj->vertices.at(3 * all_edge.at(choose_edge[0]).index[(int)choose_vertex_index[0]] + 0);
                    outer_e.point[0][1] = myObj->vertices.at(3 * all_edge.at(choose_edge[0]).index[(int)choose_vertex_index[0]] + 1);
                    outer_e.point[0][2] = myObj->vertices.at(3 * all_edge.at(choose_edge[0]).index[(int)choose_vertex_index[0]] + 2);
                    outer_e.point[1][0] = myObj->vertices.at(3 * all_edge.at(choose_edge[2]).vertex_push_index + 0);
                    outer_e.point[1][1] = myObj->vertices.at(3 * all_edge.at(choose_edge[2]).vertex_push_index + 1);
                    outer_e.point[1][2] = myObj->vertices.at(3 * all_edge.at(choose_edge[2]).vertex_push_index + 2);
                    outer_e.connect_index = choose_edge[2];
                    outer_e.vertex_push_index = all_edge.at(choose_edge[2]).vertex_push_index;
//                    outer_e.face_id[0] = face_split_by_plane.at(i);
//                    outer_e.face_id[1] = myObj->numtriangles + 1;
                    all_edge.push_back(outer_e);
                    outer_edge_index = all_edge.size() - 1;

                    myObj->triangles.push_back(temp);
                    glmOneFacetNormals(myObj, myObj->numtriangles);
                    myObj->numtriangles += 1;


                    dir_count[0] = 0;
                    dir_count[1] = 0;
                    dir_count[2] = 0;
                    for(int j = 0; j < 3; j += 1){
                        vec3 temp2(myObj->vertices.at(3 * (temp.vindices[j]) + 0), myObj->vertices.at(3 * (temp.vindices[j]) + 1), myObj->vertices.at(3 * (temp.vindices[j]) + 2));
                        point_dir[j] = plane_dir_point(temp2, test_plane);
                        dir_count[(int)point_dir[j] + 1] += 1;
                    }
                    if(dir_count[test_plane.dir * -1 + 1] == 0){
                        face_split_by_plane.push_back(myObj->numtriangles - 1);
                    }

                    myObj->triangles.at(face_split_by_plane.at(i)).vindices[0] = myObj->triangles.at(face_split_by_plane.at(i)).vindices[choose_index];
                    myObj->triangles.at(face_split_by_plane.at(i)).vindices[1] = all_edge.at(choose_edge[2]).vertex_push_index;
                    myObj->triangles.at(face_split_by_plane.at(i)).vindices[2] = all_edge.at(choose_edge[1]).index[(int)choose_vertex_index[1]];
                    myObj->triangles.at(face_split_by_plane.at(i)).split_by_process = true;
                    glmOneFacetNormals(myObj, face_split_by_plane.at(i));
//                    cout << myObj->triangles.at(face_split_by_plane.at(i)).vindices[0] << " " << myObj->triangles.at(face_split_by_plane.at(i)).vindices[1] << " " << myObj->triangles.at(face_split_by_plane.at(i)).vindices[2] << endl;

                    all_edge.at(choose_edge[2]).index[0] = all_edge.at(choose_edge[1]).index[(int)choose_vertex_index[1]];
                    all_edge.at(choose_edge[2]).point[1][0] = myObj->vertices.at(3 * all_edge.at(choose_edge[1]).index[(int)choose_vertex_index[1]] + 0);
                    all_edge.at(choose_edge[2]).point[1][1] = myObj->vertices.at(3 * all_edge.at(choose_edge[1]).index[(int)choose_vertex_index[1]] + 1);
                    all_edge.at(choose_edge[2]).point[1][2] = myObj->vertices.at(3 * all_edge.at(choose_edge[1]).index[(int)choose_vertex_index[1]] + 2);

                    all_edge.at(choose_edge[2]).index[1] = all_edge.at(choose_edge[2]).vertex_push_index;
                    all_edge.at(choose_edge[2]).point[1][0] = myObj->vertices.at(3 * all_edge.at(choose_edge[2]).vertex_push_index + 0);
                    all_edge.at(choose_edge[2]).point[1][1] = myObj->vertices.at(3 * all_edge.at(choose_edge[2]).vertex_push_index + 1);
                    all_edge.at(choose_edge[2]).point[1][2] = myObj->vertices.at(3 * all_edge.at(choose_edge[2]).vertex_push_index + 2);
                    all_edge.at(choose_edge[2]).connect_index = outer_edge_index;

                    dir_count[0] = 0;
                    dir_count[1] = 0;
                    dir_count[2] = 0;
                    for(int j = 0; j < 3; j += 1){
                        vec3 temp(myObj->vertices.at(3 * (myObj->triangles.at(face_split_by_plane.at(i)).vindices[j]) + 0), myObj->vertices.at(3 * (myObj->triangles.at(face_split_by_plane.at(i)).vindices[j]) + 1), myObj->vertices.at(3 * (myObj->triangles.at(face_split_by_plane.at(i)).vindices[j]) + 2));
                        point_dir[j] = plane_dir_point(temp, test_plane);
                        dir_count[(int)point_dir[j] + 1] += 1;
                    }
                    if(dir_count[test_plane.dir * -1 + 1] == 0){
                        face_split_by_plane.push_back(face_split_by_plane.at(i));
                    }

//                    if(!myObj->triangles.at(all_edge.at(choose_edge[2]).face_id[0]).split_by_process || !myObj->triangles.at(all_edge.at(choose_edge[2]).face_id[1]).split_by_process){
//                        if(!myObj->triangles.at(all_edge.at(choose_edge[2]).face_id[0]).split_by_process)
//                            tri_poly(myObj, all_edge, all_edge.at(choose_edge[2]).face_id[0], all_edge.at(choose_edge[2]));
//                        if(!myObj->triangles.at(all_edge.at(choose_edge[2]).face_id[1]).split_by_process)
//                            tri_poly(myObj, all_edge, all_edge.at(choose_edge[2]).face_id[1], all_edge.at(choose_edge[2]));
//                    }
                }
            }
        }
        if(i == current -1){
//            collect_edge(myObj, all_edge);
            face_split_by_plane.erase(face_split_by_plane.begin(), face_split_by_plane.begin() + current);

            for(unsigned int j = 0; j < face_split_by_plane.size(); j += 1){
                myObj->triangles.at(face_split_by_plane.at(j)).split_by_process = true;
            }
            i = -1;
            current = face_split_by_plane.size();
        }
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
    temp.split_by_process = false;
    myObj->triangles.push_back(temp);
    glmOneFacetNormals(myObj, myObj->numtriangles);
    myObj->numtriangles += 1;

    myObj->triangles.at(face_id).vindices[0] = all_edge.at(choose_index[0]).index[top_index];
    myObj->triangles.at(face_id).vindices[1] = splited_edge.vertex_push_index;
    myObj->triangles.at(face_id).vindices[2] = all_edge.at(choose_index[1]).index[choose_vertex_index[1]];
    myObj->triangles.at(face_id).split_by_process = false;
    glmOneFacetNormals(myObj, face_id);
}

void collect_edge(GLMmodel *myObj, std::vector<edge> &all_edge)
{
    int count = 0;
    std::vector<vec2> *temp_edge = new std::vector<vec2>[myObj->numvertices + 1];
    std::vector<int> *temp_point_tri = new std::vector<int>[myObj->numvertices + 1];


    for(unsigned int i = 0; i < myObj->numtriangles ; i += 1){

        int min_index = myObj->triangles.at(i).vindices[0], temp_index = 0;
        myObj->triangles.at(i).split_by_process = false;

        if((unsigned int)std::min(min_index, (int)myObj->triangles.at(i).vindices[1]) == myObj->triangles.at(i).vindices[1]){
            min_index = myObj->triangles.at(i).vindices[1];
            temp_index = 1;
        }
        if((unsigned int)std::min(min_index, (int)myObj->triangles.at(i).vindices[2]) == myObj->triangles.at(i).vindices[2]){
            min_index = myObj->triangles.at(i).vindices[2];
            temp_index = 2;
        }

        unsigned int mid_index = myObj->triangles.at(i).vindices[(temp_index + 1) % 3], temp_mid_index = (temp_index + 1) % 3;
        if(mid_index > myObj->triangles.at(i).vindices[(temp_index + 2) % 3]){
            mid_index = myObj->triangles.at(i).vindices[(temp_index + 2) % 3];
            temp_mid_index = (temp_index + 2) % 3;
        }

        vec2 push_index1(min_index, mid_index), push_index2(min_index, myObj->triangles.at(i).vindices[(3 - temp_index - temp_mid_index) % 3]), push_index3(mid_index, myObj->triangles.at(i).vindices[(3 - temp_index - temp_mid_index) % 3]);

        bool add1 = true, add2 = true, add3 = true;

        for(unsigned int j = 0; j < temp_edge[min_index].size(); j += 1){
            if(push_index1[1] == temp_edge[min_index].at(j)[1]){
                add1 = false;
                break;
            }
        }
        for(unsigned int j = 0; j < temp_edge[min_index].size(); j += 1){
            if(push_index2[1] == temp_edge[min_index].at(j)[1]){
                add2 = false;
                break;
            }
        }
        for(unsigned int j = 0; j < temp_edge[mid_index].size(); j += 1){
            if(push_index3[1] == temp_edge[mid_index].at(j)[1]){
                add3 = false;
                break;
            }
        }

        if(add1){
            temp_edge[min_index].push_back(push_index1);
            count += 1;
        }

        if(add2){
            temp_edge[min_index].push_back(push_index2);
            count += 1;
        }

        if(add3){
            temp_edge[mid_index].push_back(push_index3);
            count += 1;
        }

        for(int j = 0 ; j < 3 ; j += 1)
        {
            temp_point_tri[myObj->triangles.at(i).vindices[j]].push_back(i);
            myObj->triangles.at(i).edge_index[j] = -1;
        }
    }

    if(!all_edge.empty()){
        std::vector<edge> v;
        all_edge.swap(v);
    }

    for(unsigned int i = 0; i < myObj->numvertices + 1; i += 1){
        for(unsigned int j = 0; j < temp_edge[i].size(); j += 1){
            vec3 p1(myObj->vertices.at((int)temp_edge[i].at(j)[0] * 3 + 0), myObj->vertices.at((int)temp_edge[i].at(j)[0] * 3 + 1), myObj->vertices.at((int)temp_edge[i].at(j)[0] * 3 + 2));
            vec3 p2(myObj->vertices.at((int)temp_edge[i][j][1] * 3 + 0), myObj->vertices.at((int)temp_edge[i].at(j)[1] * 3 + 1), myObj->vertices.at((int)temp_edge[i].at(j)[1] * 3 + 2));
            edge temp((int)temp_edge[i].at(j)[0], p1, (int)temp_edge[i].at(j)[1], p2);
            temp.connect_index = -1;
            all_edge.push_back(temp);
        }
    }

    for(unsigned int i = 1; i <= myObj->numvertices; i += 1)
        sort(temp_point_tri[i].begin(), temp_point_tri[i].begin() + temp_point_tri[i].size());

    for(unsigned int i = 0; i < all_edge.size(); i += 1){
        std::vector<int> temp_vector(temp_point_tri[all_edge.at(i).index[0]].size() + temp_point_tri[all_edge.at(i).index[1]].size());
        std::vector<int>::iterator it;

        it = set_intersection(temp_point_tri[all_edge.at(i).index[0]].begin(), temp_point_tri[all_edge.at(i).index[0]].begin() + temp_point_tri[all_edge.at(i).index[0]].size(), temp_point_tri[all_edge.at(i).index[1]].begin(), temp_point_tri[all_edge.at(i).index[1]].begin() + temp_point_tri[all_edge[i].index[1]].size(), temp_vector.begin());
        temp_vector.resize(it - temp_vector.begin());

        all_edge.at(i).face_id[0] = temp_vector[0];
        all_edge.at(i).face_id[1] = temp_vector[1];

        for(int j = 0; j < 3; j += 1){
            if(myObj->triangles.at(temp_vector[0]).edge_index[j] == -1){
                myObj->triangles.at(temp_vector[0]).edge_index[j] = i;
                break;
            }
        }

        for(int j = 0; j < 3; j += 1){
            if(myObj->triangles.at(temp_vector[1]).edge_index[j] == -1){
                myObj->triangles.at(temp_vector[1]).edge_index[j] = i;
                break;
            }
        }
    }

    for(unsigned int i = 0; i < myObj->numtriangles; i += 1){
        float temp_edge_index[3];
        temp_edge_index[0] = myObj->triangles.at(i).edge_index[0];
        temp_edge_index[1] = myObj->triangles.at(i).edge_index[1];
        temp_edge_index[2] = myObj->triangles.at(i).edge_index[2];

        for(int j = 0; j < 3; j += 1){
            bool judge1,judge2;
            judge1 = (myObj->triangles.at(i).vindices[0] == (unsigned int)all_edge.at(temp_edge_index[j]).index[0]) && (myObj->triangles.at(i).vindices[1] == (unsigned int)all_edge.at(temp_edge_index[j]).index[1]);
            judge2 = (myObj->triangles.at(i).vindices[0] == (unsigned int)all_edge.at(temp_edge_index[j]).index[1]) && (myObj->triangles.at(i).vindices[1] == (unsigned int)all_edge.at(temp_edge_index[j]).index[0]);
            if(judge1 || judge2){
                myObj->triangles.at(i).edge_index[0] = temp_edge_index[j];
            }

            judge1 = (myObj->triangles.at(i).vindices[1] == (unsigned int)all_edge.at(temp_edge_index[j]).index[0]) && (myObj->triangles.at(i).vindices[2] == (unsigned int)all_edge.at(temp_edge_index[j]).index[1]);
            judge2 = (myObj->triangles.at(i).vindices[1] == (unsigned int)all_edge.at(temp_edge_index[j]).index[1]) && (myObj->triangles.at(i).vindices[2] == (unsigned int)all_edge.at(temp_edge_index[j]).index[0]);
            if(judge1 || judge2){
                myObj->triangles.at(i).edge_index[1] = temp_edge_index[j];
            }

            judge1 = (myObj->triangles.at(i).vindices[2] == (unsigned int)all_edge.at(temp_edge_index[j]).index[0]) && (myObj->triangles.at(i).vindices[0] == (unsigned int)all_edge.at(temp_edge_index[j]).index[1]);
            judge2 = (myObj->triangles.at(i).vindices[2] == (unsigned int)all_edge.at(temp_edge_index[j]).index[1]) && (myObj->triangles.at(i).vindices[0] == (unsigned int)all_edge.at(temp_edge_index[j]).index[0]);
            if(judge1 || judge2){
                myObj->triangles.at(i).edge_index[2] = temp_edge_index[j];
            }
        }
    }

    for(unsigned int i = 0; i < myObj->numvertices + 1; i += 1){
        if(!temp_edge[i].empty()){
            std::vector<vec2> v;
            temp_edge[i].swap(v);
        }
        if(!temp_point_tri[i].empty()){
            std::vector<int> v;
            temp_point_tri[i].swap(v);
        }
    }

    delete temp_edge;
    delete temp_point_tri;
}
