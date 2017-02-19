#include <algorithm>
#include "voxel.h"

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

#include <iostream>

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
    std::vector<std::vector<int>> coord(8);
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
//        std::cout << ans[0] << " " << ans[1] << " " << all_voxel.size() << std::endl;

        all_voxel.at(ans[0]).face_toward[(int)ans[1]] = all_voxel.size();

//        std::cout << "new" << std::endl;
//        for(int i = 0; i < 6; i += 1){
//            std::cout << all_voxel.at(ans[0]).face_toward[i] << " ";
//        }
//        std::cout << std::endl;

        vec3 new_p = all_voxel.at(ans[0]).position + all_voxel.at(ans[0]).toward_vector[(int)ans[1]] * all_voxel.at(ans[0]).scale * 2;
        voxel temp(all_voxel.at(ans[0]), new_p, all_voxel.at(ans[0]).rotation);

        int opposite_face = 1;
        if((int)ans[1] % 2 == 1)
            opposite_face = -1;
        temp.face_toward[(int)ans[1] + opposite_face] = ans[0];
        addexist_toward(all_voxel, temp);

//        std::cout << "new test" << std::endl;
//        for(int i = 0; i < 6; i += 1){
//            std::cout << temp.face_toward[i] << " ";
//        }
//        std::cout << std::endl;

        assign_coord(temp, origin);
        all_voxel.push_back(temp);

        ans = check_bound(all_voxel, max_d);
    }

    oct_tree(all_voxel, 0, all_voxel.size(), 0, origin, -1);
    rebuild_facetoward(all_voxel);

//    for(int i = 0; i < all_voxel.size(); i += 1){
//        std::cout << i << " : " <<std::endl;
//        for(int j = 0; j < all_voxel.at(i).coord.size(); j += 1)
//            std::cout << all_voxel.at(i).coord.at(j) << " ";
//
////        std::cout << all_voxel.at(i).coord_origin.at(0)[0] << " " << all_voxel.at(i).coord_origin.at(0)[1] << " " << all_voxel.at(i).coord_origin.at(0)[2] << std::endl;
//        std::cout << std::endl;
//    }

//    int abc = search_coord(all_voxel, 0, all_voxel.size(), vec3(39.0, 60.0, 90.0), 0);
//    std::cout << abc << std::endl;
//    std::cout << all_voxel.at(58).position[0] << " " << all_voxel.at(58).position[1] << " " << all_voxel.at(58).position[2] << std::endl;
//    std::cout << all_voxel.at(51).position[0] << " " << all_voxel.at(51).position[1] << " " << all_voxel.at(51).position[2] << std::endl;
//    std::cout << all_voxel.at(42).position[0] << " " << all_voxel.at(42).position[1] << " " << all_voxel.at(42).position[2] << std::endl;
//    std::cout << all_voxel.at(9).position[0] << " " << all_voxel.at(9).position[1] << " " << all_voxel.at(9).position[2] << std::endl;
//    std::cout << all_voxel.at(23).position[0] << " " << all_voxel.at(23).position[1] << " " << all_voxel.at(23).position[2] << std::endl;
//    for(int i = 0; i < 6; i += 1){
//        std::cout << all_voxel.at(58).face_toward[i] << " ";
//    }
//    std::cout << std::endl;
//    for(int i = 0; i < 6; i += 1){
//        std::cout << all_voxel.at(51).face_toward[i] << " ";
//    }
//    std::cout << std::endl;
//    for(int i = 0; i < 6; i += 1){
//        std::cout << all_voxel.at(42).face_toward[i] << " ";
//    }
//    std::cout << std::endl;

    for(unsigned int i = 0; i < model->numtriangles; i += 1){
        int test_coord[3];
        vec3 test_p[3];
        for(int j = 0; j < 3; j += 1){
            test_p[j] = vec3(model->vertices->at(3 * model->triangles->at(i).vindices[j] + 0), model->vertices->at(3 * model->triangles->at(i).vindices[j] + 1), model->vertices->at(3 * model->triangles->at(i).vindices[j] + 2));
            test_coord[j] = search_coord(all_voxel, 0, all_voxel.size(), test_p[j], 0);
            if(all_voxel.at(test_coord[j]).show)
                all_voxel.at(test_coord[j]).show = false;
        }
//        std::cout << test_coord[0] << " " << test_coord[1] << " " << test_coord[2] << std::endl;
        cross_edge(all_voxel, test_p[0], test_p[1], test_coord[0], test_coord[1]);
        cross_edge(all_voxel, test_p[1], test_p[2], test_coord[1], test_coord[2]);
        cross_edge(all_voxel, test_p[2], test_p[0], test_coord[2], test_coord[0]);
    }

    GLMmodel *cube = glmReadOBJ("test_model/cube.obj");
    GLMmodel *output = glmCopy(cube);
    int num = 0;
    for(unsigned int i = 0; i < all_voxel.size(); i += 1){
        if(!all_voxel.at(i).show){
            if(num == 0){
                glmScale(output, all_voxel.at(i).scale);
                glmRT(output, all_voxel.at(i).rotation, all_voxel.at(i).position);
            }
            else{
                GLMmodel *temp = glmCopy(cube);
                glmScale(temp, all_voxel.at(i).scale);
                glmRT(temp, all_voxel.at(i).rotation, all_voxel.at(i).position);
                glmCombine(output, temp);
            }
            num += 1;
        }
    }
    glmWriteOBJ(output, "123.obj", GLM_NONE);
}
