#include <algorithm>
#include "voxel.h"

#include <iostream>
using namespace std;

voxel::voxel(int t_color, int t_size, float t_scale)
{
    color = t_color;
    stick_size = t_size;
    scale = t_scale;

    position = vec3(0.0, 0.0, 0.0);
    rotation = vec3(0.0, 0.0, 0.0);

    toward_vector.push_back(vec3(1.0, 0.0, 0.0));
    toward_vector.push_back(vec3(-1.0, 0.0, 0.0));
    toward_vector.push_back(vec3(0.0, 1.0, 0.0));
    toward_vector.push_back(vec3(0.0, -1.0, 0.0));
    toward_vector.push_back(vec3(0.0, 0.0, 1.0));
    toward_vector.push_back(vec3(0.0, 0.0, -1.0));

    for(int i = 0; i < 8; i += 1){
        int dx = 1;
        int dy = 1;
        int dz = 1;

        if(i % 2 == 1)
            dx = -1;
        if((i / 2) % 2 == 1)
            dy = -1;
        if(i / 4 == 1)
            dz = -1;
        vec3 temp = position + scale * vec3(dx, dy, dz);
        vertex_p[i] = temp;
    }

    edge_p[0] = position + scale * vec3(1, 1, 0);
    edge_p[1] = position + scale * vec3(0, 1, 1);
    edge_p[2] = position + scale * vec3(-1, 1, 0);
    edge_p[3] = position + scale * vec3(0, 1, -1);
    edge_p[4] = position + scale * vec3(1, 0, 1);
    edge_p[5] = position + scale * vec3(-1, 0, 1);
    edge_p[6] = position + scale * vec3(-1, 0, -1);
    edge_p[7] = position + scale * vec3(1, 0, -1);
    edge_p[8] = position + scale * vec3(1, -1, 0);
    edge_p[9] = position + scale * vec3(0, -1, 1);
    edge_p[10] = position + scale * vec3(-1, -1, 0);
    edge_p[11] = position + scale * vec3(0, -1, -1);

    plane_d[0] = vertex_p[0] * toward_vector.at(0);
    plane_d[1] = vertex_p[1] * toward_vector.at(1);
    plane_d[2] = vertex_p[0] * toward_vector.at(2);
    plane_d[3] = vertex_p[2] * toward_vector.at(3);
    plane_d[4] = vertex_p[0] * toward_vector.at(4);
    plane_d[5] = vertex_p[4] * toward_vector.at(5);

    for(int i = 0; i < 8; i += 1){
        for(int j = 0; j < 6; j += 1){
            if(fabs(vertex_p[i] * toward_vector.at(j) - plane_d[j]) < 0.0001){
                face_p[i].push_back(j);
            }
        }
    }

    for(int i = 0; i < 12; i += 1){
        for(int j = 0; j < 6; j += 1){
            if(fabs(edge_p[i] * toward_vector.at(j) - plane_d[j]) < 0.0001){
                face_edge[i].push_back(j);
            }
        }
    }

    for(int i = 0; i < 8; i += 1){
        for(int j = i; j < 8; j += 1){
            vec3 temp = (vertex_p[i] + vertex_p[j]) / 2;
            for(int k = 0; k < 12; k += 1){
                if((temp - edge_p[k]).length() < 0.0001){
                    edge_point[k][0] = i;
                    edge_point[k][1] = j;
                    break;
                }
            }
        }
    }
}

voxel::voxel(int t_color, int t_size, float t_scale, vec3 t_position, vec3 t_rotation)
{
    color = t_color;
    stick_size = t_size;
    scale = t_scale;

    position = t_position;
    rotation = t_rotation;

    mat4 R = rotation3D(vec3(1.0, 0.0, 0.0), rotation[0]) * rotation3D(vec3(0.0, 1.0, 0.0), rotation[1]) * rotation3D(vec3(0.0, 0.0, 1.0), rotation[2]);

    toward_vector.push_back(R * vec3(1.0, 0.0, 0.0));
    toward_vector.push_back(R * vec3(-1.0, 0.0, 0.0));
    toward_vector.push_back(R * vec3(0.0, 1.0, 0.0));
    toward_vector.push_back(R * vec3(0.0, -1.0, 0.0));
    toward_vector.push_back(R * vec3(0.0, 0.0, 1.0));
    toward_vector.push_back(R * vec3(0.0, 0.0, -1.0));

    for(int i = 0; i < 8; i += 1){
        int dx = 1;
        int dy = 1;
        int dz = 1;

        if(i % 2 == 1)
            dx = -1;
        if((i / 2) % 2 == 1)
            dy = -1;
        if(i / 4 == 1)
            dz = -1;
        vec3 temp = position + scale * vec3(dx, dy, dz);
        vertex_p[i] = temp;
    }

    edge_p[0] = position + scale * vec3(1, 1, 0);
    edge_p[1] = position + scale * vec3(0, 1, 1);
    edge_p[2] = position + scale * vec3(-1, 1, 0);
    edge_p[3] = position + scale * vec3(0, 1, -1);
    edge_p[4] = position + scale * vec3(1, 0, 1);
    edge_p[5] = position + scale * vec3(-1, 0, 1);
    edge_p[6] = position + scale * vec3(-1, 0, -1);
    edge_p[7] = position + scale * vec3(1, 0, -1);
    edge_p[8] = position + scale * vec3(1, -1, 0);
    edge_p[9] = position + scale * vec3(0, -1, 1);
    edge_p[10] = position + scale * vec3(-1, -1, 0);
    edge_p[11] = position + scale * vec3(0, -1, -1);

    plane_d[0] = vertex_p[0] * toward_vector.at(0);
    plane_d[1] = vertex_p[1] * toward_vector.at(1);
    plane_d[2] = vertex_p[0] * toward_vector.at(2);
    plane_d[3] = vertex_p[2] * toward_vector.at(3);
    plane_d[4] = vertex_p[0] * toward_vector.at(4);
    plane_d[5] = vertex_p[4] * toward_vector.at(5);

    for(int i = 0; i < 8; i += 1){
        for(int j = 0; j < 6; j += 1){
            if(fabs(vertex_p[i] * toward_vector.at(j) - plane_d[j]) < 0.0001){
                face_p[i].push_back(j);
            }
        }
    }

    for(int i = 0; i < 12; i += 1){
        for(int j = 0; j < 6; j += 1){
            if(fabs(edge_p[i] * toward_vector.at(j) - plane_d[j]) < 0.0001){
                face_edge[i].push_back(j);
            }
        }
    }

    for(int i = 0; i < 8; i += 1){
        for(int j = i; j < 8; j += 1){
            vec3 temp = (vertex_p[i] + vertex_p[j]) / 2;
            for(int k = 0; k < 12; k += 1){
                if((temp - edge_p[k]).length() < 0.0001){
                    edge_point[k][0] = i;
                    edge_point[k][1] = j;
                    break;
                }
            }
        }
    }
}

voxel::voxel(voxel t, vec3 t_position, vec3 t_rotation)
{
    color = t.color;
    stick_size = t.stick_size;
    scale = t.scale;

    position = t_position;
    rotation = t_rotation;

    mat4 R = rotation3D(vec3(1.0, 0.0, 0.0), rotation[0]) * rotation3D(vec3(0.0, 1.0, 0.0), rotation[1]) * rotation3D(vec3(0.0, 0.0, 1.0), rotation[2]);

    toward_vector.push_back(R * vec3(1.0, 0.0, 0.0));
    toward_vector.push_back(R * vec3(-1.0, 0.0, 0.0));
    toward_vector.push_back(R * vec3(0.0, 1.0, 0.0));
    toward_vector.push_back(R * vec3(0.0, -1.0, 0.0));
    toward_vector.push_back(R * vec3(0.0, 0.0, 1.0));
    toward_vector.push_back(R * vec3(0.0, 0.0, -1.0));

    for(int i = 0; i < 8; i += 1){
        int dx = 1;
        int dy = 1;
        int dz = 1;

        if(i % 2 == 1)
            dx = -1;
        if((i / 2) % 2 == 1)
            dy = -1;
        if(i / 4 == 1)
            dz = -1;
        vec3 temp = position + scale * vec3(dx, dy, dz);
        vertex_p[i] = temp;
    }

    edge_p[0] = position + scale * vec3(1, 1, 0);
    edge_p[1] = position + scale * vec3(0, 1, 1);
    edge_p[2] = position + scale * vec3(-1, 1, 0);
    edge_p[3] = position + scale * vec3(0, 1, -1);
    edge_p[4] = position + scale * vec3(1, 0, 1);
    edge_p[5] = position + scale * vec3(-1, 0, 1);
    edge_p[6] = position + scale * vec3(-1, 0, -1);
    edge_p[7] = position + scale * vec3(1, 0, -1);
    edge_p[8] = position + scale * vec3(1, -1, 0);
    edge_p[9] = position + scale * vec3(0, -1, 1);
    edge_p[10] = position + scale * vec3(-1, -1, 0);
    edge_p[11] = position + scale * vec3(0, -1, -1);

    plane_d[0] = vertex_p[0] * toward_vector.at(0);
    plane_d[1] = vertex_p[1] * toward_vector.at(1);
    plane_d[2] = vertex_p[0] * toward_vector.at(2);
    plane_d[3] = vertex_p[2] * toward_vector.at(3);
    plane_d[4] = vertex_p[0] * toward_vector.at(4);
    plane_d[5] = vertex_p[4] * toward_vector.at(5);

    for(int i = 0; i < 8; i += 1){
        for(int j = 0; j < 6; j += 1){
            if(fabs(vertex_p[i] * toward_vector.at(j) - plane_d[j]) < 0.0001){
                face_p[i].push_back(j);
            }
        }
    }

    for(int i = 0; i < 12; i += 1){
        for(int j = 0; j < 6; j += 1){
            if(fabs(edge_p[i] * toward_vector.at(j) - plane_d[j]) < 0.0001){
                face_edge[i].push_back(j);
            }
        }
    }

    for(int i = 0; i < 8; i += 1){
        for(int j = i; j < 8; j += 1){
            vec3 temp = (vertex_p[i] + vertex_p[j]) / 2;
            for(int k = 0; k < 12; k += 1){
                if((temp - edge_p[k]).length() < 0.0001){
                    edge_point[k][0] = i;
                    edge_point[k][1] = j;
                    break;
                }
            }
        }
    }
}

void assign_coord(voxel &judge, vec3 &origin){
    int id = 0;
    if(judge.position[0] <= origin[0]){
        id += 1;
    }
    if(judge.position[1] <= origin[1]){
        id += 2;
    }
    if(judge.position[2] <= origin[2]){
        id += 4;
    }
    judge.coord.push_back(id);
    judge.coord_origin.push_back(origin);
}

vec2 check_bound(std::vector<voxel> &all_voxel, int max_d)
{
    vec2 t_ans(-1,-1);
    for(unsigned int i = 0; i < all_voxel.size(); i += 1){
        for(int j = 0; j < 6; j += 1){
            if(all_voxel.at(i).face_toward[j] == -1){
                int weight = 0;
                if(j % 2 == 1)
                    weight = 1;
                vec3 bound = all_voxel.at(0).position + vec3(all_voxel.at(0).scale, all_voxel.at(0).scale, all_voxel.at(0).scale) + all_voxel.at(i).toward_vector.at(j) * all_voxel.at(i).scale * 2 * max_d;
                vec3 now = all_voxel.at(i).position + vec3(all_voxel.at(i).scale, all_voxel.at(i).scale, all_voxel.at(i).scale) + weight * all_voxel.at(i).toward_vector.at(j) * all_voxel.at(i).scale * 2;

                bool judge = !(fabs(now[j / 2] - bound[j / 2]) < 0.0001) && (now[j / 2] < bound[j / 2]);
                if(j % 2 == 1)
                    judge = !(fabs(now[j / 2] - bound[j / 2]) < 0.0001) && (now[j / 2] > bound[j / 2]);

                if(judge){
                    t_ans[0] = i;
                    t_ans[1] = j;
                    return t_ans;
                }
            }
        }
    }
    return t_ans;
}

void addexist_toward(std::vector<voxel> &all_voxel, voxel &check){
    for(int i = 0; i < 6; i += 1){
        if(check.face_toward[i] == -1){
            for(unsigned int j = 0; j < all_voxel.size(); j += 1){
                if(((check.position + check.toward_vector[i] * check.scale * 2) - all_voxel.at(j).position).length() < 0.0001){
                    check.face_toward[i] = j;

                    int opposite_face = 1;
                    if(i % 2 == 1)
                        opposite_face = -1;
                    all_voxel.at(j).face_toward[i + opposite_face] = all_voxel.size();
                    break;
                }
            }
        }
    }
}

void oct_tree(std::vector<voxel> &all_voxel, int start, int end, int depth, vec3 origin, int coord_id){
    if(end - start != 1){
        if(depth == 0){
            std::sort(all_voxel.begin(),
                      all_voxel.end(),
                      [](voxel a, voxel b){
                        return b.coord.at(b.coord.size() - 1) > a.coord.at(a.coord.size() - 1);
                      });
        }
        else{
            int dx = 1;
            int dy = 1;
            int dz = 1;
            if(coord_id % 2 == 1)
                dx = -1;
            if((coord_id / 2) % 2 == 1)
                dy = -1;
            if(coord_id / 4 == 1)
                dz = -1;
            origin += all_voxel.at(start).scale * pow((all_voxel.size() / pow(8, depth)), 1.0 / 3.0) * vec3(dx, dy, dz);

            for(int i = start; i < end; i += 1){
                assign_coord(all_voxel.at(i), origin);
            }
            std::sort(all_voxel.begin() + start,
                      all_voxel.begin() + end,
                      [](voxel a, voxel b){
                        return b.coord.at(b.coord.size() - 1) > a.coord.at(a.coord.size() - 1);
                      });
        }

        int d = (end - start) / 8;
        oct_tree(all_voxel, start + 0 * d, start + 1 * d, depth + 1, origin, 0);
        oct_tree(all_voxel, start + 1 * d, start + 2 * d, depth + 1, origin, 1);
        oct_tree(all_voxel, start + 2 * d, start + 3 * d, depth + 1, origin, 2);
        oct_tree(all_voxel, start + 3 * d, start + 4 * d, depth + 1, origin, 3);
        oct_tree(all_voxel, start + 4 * d, start + 5 * d, depth + 1, origin, 4);
        oct_tree(all_voxel, start + 5 * d, start + 6 * d, depth + 1, origin, 5);
        oct_tree(all_voxel, start + 6 * d, start + 7 * d, depth + 1, origin, 6);
        oct_tree(all_voxel, start + 7 * d, start + 8 * d, depth + 1, origin, 7);
    }
}

int search_coord(std::vector<voxel> &all_voxel, int start, int end, vec3 p, int depth)
{
    if(end - start != 1){
        int id = 0;
        if(p[0] <= all_voxel.at(start).coord_origin.at(depth)[0]){
            id += 1;
        }
        if(p[1] <= all_voxel.at(start).coord_origin.at(depth)[1]){
            id += 2;
        }
        if(p[2] <= all_voxel.at(start).coord_origin.at(depth)[2]){
            id += 4;
        }

        depth += 1;
        int d = all_voxel.size() / pow(8, depth);

        return search_coord(all_voxel, start + d * id, start + d * (id + 1), p, depth);
    }
    return start;
}

void rebuild_facetoward(std::vector<voxel> &all_voxel)
{
    for(unsigned int i = 0; i < all_voxel.size(); i += 1){
        for(int j = 0; j < 6; j += 1){
            all_voxel.at(i).face_toward[j] = -1;
        }
    }

    for(unsigned int i = 0; i < all_voxel.size(); i += 1){
        for(int j = 0; j < 6; j += 1){
            if(all_voxel.at(i).face_toward[j] == -1){
                for(unsigned int k = 0; k < all_voxel.size(); k += 1){
                    if(((all_voxel.at(i).position + all_voxel.at(i).toward_vector[j] * all_voxel.at(i).scale * 2) - all_voxel.at(k).position).length() < 0.0001){
                        all_voxel.at(i).face_toward[j] = k;

                        int opposite_face = 1;
                        if(j % 2 == 1)
                            opposite_face = -1;
                        all_voxel.at(k).face_toward[j + opposite_face] = i;
                        break;
                    }
                }
            }
        }
    }
}

void cross_edge(std::vector<voxel> &all_voxel, vec3 p1, vec3 p2, int index1, int index2)
{
    if(index1 != index2){
        bool judge = false;
        for(int i = 0; i < 6; i += 1){
            if(all_voxel.at(index1).face_toward[i] == index2)
                judge = true;
        }
        if(!judge){
            vec3 new_p = (p1 + p2) / 2;
            int new_index = search_coord(all_voxel, 0, all_voxel.size(), new_p, 0);
            cross_edge(all_voxel, p1, new_p, index1, new_index);
            cross_edge(all_voxel, p2, new_p, index2, new_index);
        }

        if(all_voxel.at(index1).show)
            all_voxel.at(index1).show = false;
        if(all_voxel.at(index2).show)
            all_voxel.at(index2).show = false;
    }
}

void voxelization(GLMmodel *model, std::vector<voxel> &all_voxel, vec3 &bounding_max, vec3 &bounding_min, vec3 &bound_center, int v_color, float v_size)
{
    zomedir t;
    voxel start(v_color, v_size, t.color_length(v_color, v_size) / 2.0, bound_center, vec3(0.0, 0.0, 0.0));
    vec3 origin = vec3(bound_center) + vec3(start.scale, start.scale, start.scale);

    int max_d = -1;
    for(int i = 0; i < 3; i += 1){
        float pos = ceil((bounding_max[i] - origin[i]) / (start.scale * 2)),
              neg = ceil((origin[i] - bounding_min[i]) / (start.scale * 2));
        if(pos > max_d)
            max_d = pos;
        if(neg > max_d)
            max_d = neg;
    }

    int half_edge = 1;
    while(true){
        if(half_edge < max_d)
            half_edge *= 2;
        else{
            max_d = half_edge;
            break;
        }
    }

    assign_coord(start, origin);
    all_voxel.push_back(start);
    vec2 ans = check_bound(all_voxel, max_d);

    while(ans[0] != -1 && ans[1] != -1){

        all_voxel.at(ans[0]).face_toward[(int)ans[1]] = all_voxel.size();

        vec3 new_p = all_voxel.at(ans[0]).position + all_voxel.at(ans[0]).toward_vector[(int)ans[1]] * all_voxel.at(ans[0]).scale * 2;
        voxel temp(all_voxel.at(ans[0]), new_p, all_voxel.at(ans[0]).rotation);

        int opposite_face = 1;
        if((int)ans[1] % 2 == 1)
            opposite_face = -1;
        temp.face_toward[(int)ans[1] + opposite_face] = ans[0];
        addexist_toward(all_voxel, temp);

        assign_coord(temp, origin);
        all_voxel.push_back(temp);

        ans = check_bound(all_voxel, max_d);
    }

    oct_tree(all_voxel, 0, all_voxel.size(), 0, origin, -1);
    rebuild_facetoward(all_voxel);

    for(unsigned int i = 0; i < model->numtriangles; i += 1){
        int test_coord[3];
        vec3 test_p[3];
        for(int j = 0; j < 3; j += 1){
            test_p[j] = vec3(model->vertices->at(3 * model->triangles->at(i).vindices[j] + 0), model->vertices->at(3 * model->triangles->at(i).vindices[j] + 1), model->vertices->at(3 * model->triangles->at(i).vindices[j] + 2));
            test_coord[j] = search_coord(all_voxel, 0, all_voxel.size(), test_p[j], 0);
            if(all_voxel.at(test_coord[j]).show)
                all_voxel.at(test_coord[j]).show = false;
        }
        cross_edge(all_voxel, test_p[0], test_p[1], test_coord[0], test_coord[1]);
        cross_edge(all_voxel, test_p[1], test_p[2], test_coord[1], test_coord[2]);
        cross_edge(all_voxel, test_p[2], test_p[0], test_coord[2], test_coord[0]);
    }

    bool use[all_voxel.size()] = {false};
    for(unsigned int i = 0; i < all_voxel.size(); i += 1){
        if(!all_voxel.at(i).show)
            use[i] = true;
    }

    std::vector<std::vector<int>> region;
    while(true){
        std::vector<int> t_queue;
        for(unsigned int i = 0; i < all_voxel.size(); i += 1){
            if(!use[i]){
                t_queue.push_back(i);
                use[i] = true;
                break;
            }
        }

        for(unsigned int i = 0; i < t_queue.size(); i += 1){
            for(int j = 0; j < 6; j += 1){
                if(all_voxel.at(t_queue.at(i)).face_toward[j] != -1){
                    if(!use[all_voxel.at(t_queue.at(i)).face_toward[j]]){
                        t_queue.push_back(all_voxel.at(t_queue.at(i)).face_toward[j]);
                        use[all_voxel.at(t_queue.at(i)).face_toward[j]] = true;
                    }
                }
            }
        }

        region.push_back(t_queue);

        bool test = true;
        for(unsigned int i = 0; i < all_voxel.size(); i += 1){
            if(!use[i]){
                test = false;
                break;
            }
        }

        if(test)
            break;
    }

    for(unsigned int i = 0; i < region.size(); i += 1){
        bool test = false;
        for(unsigned int j = 0; j < region.at(i).size(); j += 1){
            if(test){
                all_voxel.at(region.at(i).at(j)).show = false;
                region.at(i).erase(region.at(i).begin() + j);
                j -= 1;
            }
            else{
                for(int k = 0 ; k < 6; k += 1){
                    if(all_voxel.at(region.at(i).at(j)).face_toward[k] == -1){
                        j = -1;
                        test = true;
                        break;
                    }
                }
            }
        }
    }

//    for(unsigned int i = 0; i < region.size(); i += 1){
//        cout << region.at(i).size() << endl;
//    }

    std::vector<std::vector<zomeconn>> zome_queue(4);

    for(unsigned int i = 0; i < region.size(); i += 1){
        if(region.at(i).size() > 0){
            std::vector<int> done;
            for(unsigned int j = 0; j < region.at(i).size(); j += 1){
                bool p[8] = {false};
                bool e[12] = {false};
                for(int k = 0 ; k < 6; k += 1){
                    if(all_voxel.at(all_voxel.at(region.at(i).at(j)).face_toward[k]).show){
                    }
                    else{
                        for(int a = 0; a < 8; a += 1){
                            for(int b = 0; b < all_voxel.at(region.at(i).at(j)).face_p[a].size(); b += 2){
                                if((find(all_voxel.at(region.at(i).at(j)).face_p[a].begin(), all_voxel.at(region.at(i).at(j)).face_p[a].end(), k) - all_voxel.at(region.at(i).at(j)).face_p[a].begin()) < all_voxel.at(region.at(i).at(j)).face_p[a].size()){
                                    if(!p[a]){
                                        zomeconn new_point;
                                        new_point.position = all_voxel.at(region.at(i).at(j)).vertex_p[a];
                                        new_point.color = COLOR_WHITE;
                                        zome_queue.at(COLOR_WHITE).push_back(new_point);
                                        p[a] = true;
                                    }
                                }
                            }
                        }

                        for(int a = 0; a < 12; a += 1){
                            for(int b = 0; b < all_voxel.at(region.at(i).at(j)).face_edge[a].size(); b += 2){
                                if((find(all_voxel.at(region.at(i).at(j)).face_edge[a].begin(), all_voxel.at(region.at(i).at(j)).face_edge[a].end(), k) - all_voxel.at(region.at(i).at(j)).face_edge[a].begin()) < all_voxel.at(region.at(i).at(j)).face_edge[a].size()){
                                    if(!e[a]){
                                        zomeconn new_edge;
                                        new_edge.position = all_voxel.at(region.at(i).at(j)).edge_p[a];
                                        new_edge.color = all_voxel.at(region.at(i).at(j)).color;
                                        new_edge.size = all_voxel.at(region.at(i).at(j)).stick_size;

                                        int index[2];
                                        for(int c = 0; c < 2; c += 1){
                                            for(unsigned d = 0; d < zome_queue.at(COLOR_WHITE).size(); d += 1){
                                                if((all_voxel.at(region.at(i).at(j)).vertex_p[(int)all_voxel.at(region.at(i).at(j)).edge_point[a][c]] - zome_queue.at(COLOR_WHITE).at(d).position).length() < 0.0001){
                                                    index[c] = d;
                                                    break;
                                                }
                                            }
                                        }
                                        new_edge.fromindex = vec2(COLOR_WHITE, index[0]);
                                        new_edge.towardindex = vec2(COLOR_WHITE, index[1]);

                                        new_edge.fromface = t.dir_face((new_edge.position - zome_queue.at(COLOR_WHITE).at(index[0]).position).normalize());
                                        new_edge.towardface = t.opposite_face(new_edge.fromface);

                                        zome_queue.at(all_voxel.at(region.at(i).at(j)).color).push_back(new_edge);
                                        e[a] = true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    for(unsigned int  i = 0; i < 4; i += 1){
        cout << i << " : " << endl;
        for(unsigned int j = 0; j < zome_queue.at(i).size(); j += 1){
            cout << zome_queue.at(i).at(j).position[0] << " " << zome_queue.at(i).at(j).position[1] << " " << zome_queue.at(i).at(j).position[2] << endl;
            cout << zome_queue.at(i).at(j).fromindex[0] << " " << zome_queue.at(i).at(j).fromindex[1] << " " << zome_queue.at(i).at(j).towardindex[0] << " " << zome_queue.at(i).at(j).towardindex[1] << endl;
            cout << zome_queue.at(i).at(j).fromface << " " << zome_queue.at(i).at(j).towardface << endl;
        }
        cout << endl;
    }

    GLMmodel *zome = glmReadOBJ("test_model/zometool/zomeball.obj");

    for(unsigned int i = 0; i < zome_queue.at(3).size(); i += 1){
        if(i == 0){
            glmRT(zome, vec3(0.0, 0.0, 0.0), zome_queue.at(3).at(i).position);
        }
        else{
            GLMmodel *new_ball = glmReadOBJ("test_model/zometool/zomeball.obj");
            glmRT(new_ball, vec3(0.0, 0.0, 0.0), zome_queue.at(3).at(i).position);
            glmCombine(zome, new_ball);
        }
    }

    for(unsigned int i = 0; i < zome_queue.size() - 1; i += 1){
        for(unsigned int j = 0; j < zome_queue.at(i).size(); j += 1){
            GLMmodel *new_stick;
            if(i == COLOR_BLUE){
                if(zome_queue.at(i).at(j).size == SIZE_S)
                    new_stick = glmReadOBJ("test_model/zometool/BlueS.obj");
                if(zome_queue.at(i).at(j).size == SIZE_M)
                    new_stick = glmReadOBJ("test_model/zometool/BlueM.obj");
                if(zome_queue.at(i).at(j).size == SIZE_L)
                    new_stick = glmReadOBJ("test_model/zometool/BlueL.obj");
            }
            else if(i == COLOR_RED){
                if(zome_queue.at(i).at(j).size == SIZE_S)
                    new_stick = glmReadOBJ("test_model/zometool/RedS.obj");
                if(zome_queue.at(i).at(j).size == SIZE_M)
                    new_stick = glmReadOBJ("test_model/zometool/RedM.obj");
                if(zome_queue.at(i).at(j).size == SIZE_L)
                    new_stick = glmReadOBJ("test_model/zometool/RedL.obj");
            }
            else if(i == COLOR_YELLOW){
                if(zome_queue.at(i).at(j).size == SIZE_S)
                    new_stick = glmReadOBJ("test_model/zometool/YellowS.obj");
                if(zome_queue.at(i).at(j).size == SIZE_M)
                    new_stick = glmReadOBJ("test_model/zometool/YellowM.obj");
                if(zome_queue.at(i).at(j).size == SIZE_L)
                    new_stick = glmReadOBJ("test_model/zometool/YellowL.obj");
            }
            glmR(new_stick, vec3(0.0, 1.0, 0.0), t.roll(zome_queue.at(i).at(j).fromface));
            glmRT(new_stick, vec3(0.0, t.phi(zome_queue.at(i).at(j).fromface), t.theta(zome_queue.at(i).at(j).fromface)), vec3(0.0, 0.0, 0.0));
            glmRT(new_stick, vec3(0.0, 0.0, 0.0), zome_queue.at(i).at(j).position);
            glmCombine(zome, new_stick);
        }
    }

    glmWriteOBJ(zome, "123zome.obj", GLM_NONE);

//    GLMmodel *cube = glmReadOBJ("test_model/cube.obj");
//    GLMmodel *output = glmCopy(cube);
//    int num = 0;
//    for(unsigned int i = 0; i < all_voxel.size(); i += 1){
//        if(all_voxel.at(i).show){
//            if(num == 0){
//                glmScale(output, all_voxel.at(i).scale);
//                glmRT(output, all_voxel.at(i).rotation, all_voxel.at(i).position);
//            }
//            else{
//                GLMmodel *temp = glmCopy(cube);
//                glmScale(temp, all_voxel.at(i).scale);
//                glmRT(temp, all_voxel.at(i).rotation, all_voxel.at(i).position);
//                glmCombine(output, temp);
//            }
//            num += 1;
//        }
//    }
//    glmWriteOBJ(output, "123.obj", GLM_NONE);
}
