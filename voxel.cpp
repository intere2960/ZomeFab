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
    judge.coord_id = 0;
    if(judge.position[0] <= 0){
        judge.coord_id += 1;
    }
    if(judge.position[1] <= 0){
        judge.coord_id += 2;
    }
    if(judge.position[2] <= 0){
        judge.coord_id += 4;
    }
}

int assign_coord_point(vec3 &p, vec3 &origin){
    int coord = 0;
    if(p[0] <= 0){
        coord += 1;
    }
    if(p[1] <= 0){
        coord += 2;
    }
    if(p[2] <= 0){
        coord += 4;
    }
    return coord;
}

vec2 check_bound(std::vector<voxel> &all_voxel, vec3 bounding_max, vec3 bounding_min)
{
    vec2 t_ans(-1,-1);
    for(unsigned int i = 0; i < all_voxel.size(); i += 1){
        for(int j = 0; j < 6; j += 1){
            if(all_voxel.at(i).face_toward[j] == -1){
                vec3 temp_p = all_voxel.at(i).position + all_voxel.at(i).toward_vector.at(j) * all_voxel.at(i).scale;

                if((temp_p[j / 2] > bounding_min[j / 2]) && (temp_p[j / 2] < bounding_max[j / 2])){
                    t_ans[0] = i;
                    t_ans[1] = j;
                    return t_ans;
                }
            }
        }
    }
    return t_ans;
}

void addexist_toward(std::vector<voxel> &all_voxel, voxel & check){
    for(int i = 0; i < 6; i += 1){
        if(check.face_toward[i] == -1){
            for(unsigned int j = 0; j < all_voxel.size(); j += 1){
                if(((check.position + check.toward_vector[i] * check.scale * 2) - all_voxel.at(j).position).length() < SMALL_VALUE){
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

#include <iostream>

void voxelization(GLMmodel *model, std::vector<voxel> &all_voxel, vec3 &bounding_max, vec3 &bounding_min, vec3 &bound_center, int v_color, float v_size)
{
    zomedir t;
    std::vector<std::vector<int>> coord(8);
    voxel start(v_color, v_size, t.color_length(v_color, v_size) / 2.0, bound_center, vec3(0.0, 0.0, 0.0));
    vec3 origin = vec3(bound_center) + vec3(start.scale, start.scale, start.scale);

    assign_coord(start, origin);
    coord.at(start.coord_id).push_back(0);
    all_voxel.push_back(start);
    vec2 ans = check_bound(all_voxel, bounding_max, bounding_min);

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
        coord.at(temp.coord_id).push_back(all_voxel.size());
        all_voxel.push_back(temp);

        ans = check_bound(all_voxel, bounding_max, bounding_min);
    }

    for(unsigned int i = 1; i <= model->numvertices; i += 1){
        vec3 p(model->vertices->at(3 * i + 0), model->vertices->at(3 * i + 1), model->vertices->at(3 * i + 2));
        int p_coord = assign_coord_point(p, origin);
        for(unsigned int j = 0; j < coord.at(p_coord).size(); j += 1){
            if(all_voxel.at(coord.at(p_coord).at(j)).show){
                vec2 range_x(all_voxel.at(coord.at(p_coord).at(j)).position[0] + all_voxel.at(coord.at(p_coord).at(j)).toward_vector[0][0] * all_voxel.at(coord.at(p_coord).at(j)).scale, all_voxel.at(coord.at(p_coord).at(j)).position[0] + all_voxel.at(coord.at(p_coord).at(j)).toward_vector[1][0] * all_voxel.at(coord.at(p_coord).at(j)).scale);
                vec2 range_y(all_voxel.at(coord.at(p_coord).at(j)).position[1] + all_voxel.at(coord.at(p_coord).at(j)).toward_vector[2][1] * all_voxel.at(coord.at(p_coord).at(j)).scale, all_voxel.at(coord.at(p_coord).at(j)).position[1] + all_voxel.at(coord.at(p_coord).at(j)).toward_vector[3][1] * all_voxel.at(coord.at(p_coord).at(j)).scale);
                vec2 range_z(all_voxel.at(coord.at(p_coord).at(j)).position[2] + all_voxel.at(coord.at(p_coord).at(j)).toward_vector[4][2] * all_voxel.at(coord.at(p_coord).at(j)).scale, all_voxel.at(coord.at(p_coord).at(j)).position[2] + all_voxel.at(coord.at(p_coord).at(j)).toward_vector[5][2] * all_voxel.at(coord.at(p_coord).at(j)).scale);
                bool check = (p[0] < range_x[0] && p[0] > range_x[1]) && (p[1] < range_y[0] && p[1] > range_y[1]) && (p[2] < range_z[0] && p[2] > range_z[1]);

                if(check){
                    all_voxel.at(coord.at(p_coord).at(j)).show = false;
                }
            }
        }
    }

//    std::vector<int> check;
//    for(unsigned int  i = 0; i < all_voxel.size(); i += 1){
//        if(all_voxel.at(i).show){
//            check.push_back(i);
//        }
//    }
//    std::cout << check.size() << std::endl;

//    for(unsigned int i = 0; i < model->numtriangles; i += 1){
////    for(unsigned int i = 0; i < 1; i += 1){
//        vec3 p1(model->vertices->at(3 * model->triangles->at(i).vindices[0] + 0), model->vertices->at(3 * model->triangles->at(i).vindices[0] + 1), model->vertices->at(3 * model->triangles->at(i).vindices[0] + 2));
////        vec3 p2(model->vertices->at(3 * model->triangles->at(i).vindices[1] + 0), model->vertices->at(3 * model->triangles->at(i).vindices[1] + 1), model->vertices->at(3 * model->triangles->at(i).vindices[1] + 2));
////        vec3 p3(model->vertices->at(3 * model->triangles->at(i).vindices[2] + 0), model->vertices->at(3 * model->triangles->at(i).vindices[2] + 1), model->vertices->at(3 * model->triangles->at(i).vindices[2] + 2));
////        vec3 v1 = p2 - p1;
////        vec3 v2 = p3 - p1;
////        vec3 n = (v1 ^ v2).normalize();
//        vec3 n = vec3(model->facetnorms->at(3 * model->triangles->at(i).findex + 0), model->facetnorms->at(3 * model->triangles->at(i).findex + 1), model->facetnorms->at(3 * model->triangles->at(i).findex + 2));
//        float d = n[0] * p1[0] + n[1] * p1[1] + n[2] * p1[2];
//
//        for(unsigned int j = 0; j < check.size(); j += 1){
//            if(all_voxel.at(check.at(j)).show){
//                int dir[3] = {0};
//    //        for(unsigned int j = 0; j < 1; j += 1){
//                for(int k = 0; k < 8; k += 1){
//                    vec3 v_p = vec3(all_voxel.at(check.at(j)).position);
//                    int dx = 1, dy = 1, dz = 1;
//                    if(k % 2 == 1)
//                        dx = -1;
//                    if((k / 2) % 2 == 1)
//                        dy = -1;
//                    if(k / 4 == 1)
//                        dz = -1;
//                    v_p += vec3(all_voxel.at(check.at(j)).scale * dx, all_voxel.at(check.at(j)).scale * dy, all_voxel.at(check.at(j)).scale * dz);
////                    std::cout << v_p[0] << " " << v_p[1] << " " << v_p[2] << std::endl;
//
//                    float judge = n[0] * v_p[0] + n[1] * v_p[1] + n[2] * v_p[2] - d;
//                    if(judge > 0.0001){
//                        dir[2] += 1;
//                    }
//                    else if(judge < -0.0001){
//                        dir[0] += 1;
//                    }
//                    else{
//                        dir[1] += 1;
//                    }
//                }
//
//                if((dir[0] + dir[1]) != 8){
////                    std::cout << dir[0] << " " << dir[1] << " " << dir[2] << std::endl;
//                    if((dir[2] + dir[1]) != 8){
//                        all_voxel.at(check.at(j)).show = false;
//                    }
//                }
////                std::cout << std::endl;
//            }
//        }
//    }

//    for(unsigned int i = 0; i < check.size(); i += 1){
////        std::cout << i << std::endl;
////    for(unsigned int i = 0; i < 1; i += 1){
//        for(unsigned int j = 0; j < model->numtriangles; j += 1){
//            vec3 p1(model->vertices->at(3 * model->triangles->at(j).vindices[0] + 0), model->vertices->at(3 * model->triangles->at(j).vindices[0] + 1), model->vertices->at(3 * model->triangles->at(j).vindices[0] + 2));
//    //        vec3 v1 = p2 - p1;
//    //        vec3 v2 = p3 - p1;
//    //        vec3 n = (v1 ^ v2).normalize();
//            vec3 n = vec3(model->facetnorms->at(3 * model->triangles->at(j).findex + 0), model->facetnorms->at(3 * model->triangles->at(j).findex + 1), model->facetnorms->at(3 * model->triangles->at(j).findex + 2));
//            float d = n[0] * p1[0] + n[1] * p1[1] + n[2] * p1[2];
//            bool check_end = false;
//
//            for(int k = 0; k < 8; k += 1){
//                vec3 v_p = vec3(all_voxel.at(check.at(i)).position);
//                int dx = 1, dy = 1, dz = 1;
//                if(k % 2 == 1)
//                    dx = -1;
//                if((k / 2) % 2 == 1)
//                    dy = -1;
//                if(k / 4 == 1)
//                    dz = -1;
//                v_p += vec3(all_voxel.at(check.at(i)).scale * dx, all_voxel.at(check.at(i)).scale * dy, all_voxel.at(check.at(i)).scale * dz);
////                std::cout << v_p[0] << " " << v_p[1] << " " << v_p[2] << std::endl;
//
//                float judge = n[0] * v_p[0] + n[1] * v_p[1] + n[2] * v_p[2] - d;
//
//                if(judge > 0.0001){
//                    std::cout << judge << std::endl;
//                    check_end = true;
//                    all_voxel.at(check.at(i)).show = false;
//                    break;
//                }
////                else if(judge < -0.0001){
////                    dir[0] += 1;
////                }
////                else{
////                    dir[1] += 1;
////                }
//            }
////            std::cout << std::endl;
//            if(check_end)
//                break;
//        }
//    }

    for(unsigned int i = 0; i < all_voxel.size(); i += 1){
        if(all_voxel.at(i).show){
//            std::cout << i << std::endl;
            vec2 range_x(all_voxel.at(i).position[0] + all_voxel.at(i).toward_vector[0][0] * all_voxel.at(i).scale, all_voxel.at(i).position[0] + all_voxel.at(i).toward_vector[1][0] * all_voxel.at(i).scale);
            vec2 range_y(all_voxel.at(i).position[1] + all_voxel.at(i).toward_vector[2][1] * all_voxel.at(i).scale, all_voxel.at(i).position[1] + all_voxel.at(i).toward_vector[3][1] * all_voxel.at(i).scale);
            vec2 range_z(all_voxel.at(i).position[2] + all_voxel.at(i).toward_vector[4][2] * all_voxel.at(i).scale, all_voxel.at(i).position[2] + all_voxel.at(i).toward_vector[5][2] * all_voxel.at(i).scale);
            bool check = false;

            if(bounding_max[0] < range_x[0] && bounding_max[0] > range_x[1]){
                check = true;
            }
            if(bounding_max[1] < range_y[0] && bounding_max[1] > range_y[1]){
                check = true;
            }
            if(bounding_max[2] < range_z[0] && bounding_max[2] > range_z[1]){
                check = true;
            }

            if(bounding_min[0] < range_x[0] && bounding_min[0] > range_x[1]){
                check = true;
            }
            if(bounding_min[1] < range_y[0] && bounding_min[1] > range_y[1]){
                check = true;
            }
            if(bounding_min[2] < range_z[0] && bounding_min[2] > range_z[1]){
                check = true;
            }

            if(check){
                all_voxel.at(i).show = false;
            }
        }
    }

//    GLMmodel *cube = glmReadOBJ("test_model/cube.obj");
//    GLMmodel *output = glmCopy(cube);
//    for(unsigned int i = 0; i < all_voxel.size(); i += 1){
//        if(all_voxel.at(i).show){
//            if(i == 0){
//                glmScale(output, start.scale);
//                glmRT(output, start.rotation, start.position);
//            }
//            else{
//                GLMmodel *temp = glmCopy(cube);
//                glmScale(temp, all_voxel.at(i).scale);
//                glmRT(temp, all_voxel.at(i).rotation, all_voxel.at(i).position);
//                glmCombine(output, temp);
//            }
//        }
//    }
//    glmWriteOBJ(output, "123.obj", GLM_NONE);
}
