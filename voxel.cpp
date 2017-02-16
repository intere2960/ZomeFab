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
                vec3 temp_p = all_voxel.at(i).position + all_voxel.at(i).toward_vector.at(j) * all_voxel.at(i).scale * 1;

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

    std::vector<int> mesh_bound;
    for(unsigned int i = 1; i <= model->numvertices; i += 1){
        vec3 p(model->vertices->at(3 * i + 0), model->vertices->at(3 * i + 1), model->vertices->at(3 * i + 2));
        int p_coord = assign_coord_point(p, origin);
        for(unsigned int j = 0; j < coord.at(p_coord).size(); j += 1){
            if(all_voxel.at(coord.at(p_coord).at(j)).show){
                vec2 range_x(all_voxel.at(coord.at(p_coord).at(j)).position[0] + all_voxel.at(coord.at(p_coord).at(j)).toward_vector[0][0] * all_voxel.at(coord.at(p_coord).at(j)).scale, all_voxel.at(coord.at(p_coord).at(j)).position[0] + all_voxel.at(coord.at(p_coord).at(j)).toward_vector[1][0] * all_voxel.at(coord.at(p_coord).at(j)).scale);
                vec2 range_y(all_voxel.at(coord.at(p_coord).at(j)).position[1] + all_voxel.at(coord.at(p_coord).at(j)).toward_vector[2][1] * all_voxel.at(coord.at(p_coord).at(j)).scale, all_voxel.at(coord.at(p_coord).at(j)).position[1] + all_voxel.at(coord.at(p_coord).at(j)).toward_vector[3][1] * all_voxel.at(coord.at(p_coord).at(j)).scale);
                vec2 range_z(all_voxel.at(coord.at(p_coord).at(j)).position[2] + all_voxel.at(coord.at(p_coord).at(j)).toward_vector[4][2] * all_voxel.at(coord.at(p_coord).at(j)).scale, all_voxel.at(coord.at(p_coord).at(j)).position[2] + all_voxel.at(coord.at(p_coord).at(j)).toward_vector[5][2] * all_voxel.at(coord.at(p_coord).at(j)).scale);
                bool check = ((p[0] < range_x[0]) && (p[0] > range_x[1])) && ((p[1] < range_y[0]) && (p[1] > range_y[1])) && ((p[2] < range_z[0]) && (p[2] > range_z[1]));

                if(check){
                    all_voxel.at(coord.at(p_coord).at(j)).show = false;
                    mesh_bound.push_back(coord.at(p_coord).at(j));
                }
            }
        }
    }

    std::vector<vec4> plane;
    for(unsigned int i = 0; i < model->numtriangles; i += 1){
        vec3 n = vec3(model->facetnorms->at(3 * model->triangles->at(i).findex + 0), model->facetnorms->at(3 * model->triangles->at(i).findex + 1), model->facetnorms->at(3 * model->triangles->at(i).findex + 2));
        vec3 t_p = vec3(model->vertices->at(3 * model->triangles->at(i).vindices[0] + 0), model->vertices->at(3 * model->triangles->at(i).vindices[0] + 1), model->vertices->at(3 * model->triangles->at(i).vindices[0] + 2));
        float d = n[0] * t_p[0] + n[1] * t_p[1] + n[2] * t_p[2];
        plane.push_back(vec4(n, d));
    }

    for(unsigned int i = 0; i < all_voxel.size(); i += 1){
        if(all_voxel.at(i).show){
            int intersect = 0;
            for(int j = 0; j < 6; j += 1){
                for(unsigned int k = 0; k < model->numtriangles; k += 1){
                    float t = (plane.at(k)[3] - all_voxel.at(i).position[0] * plane.at(k)[0] - all_voxel.at(i).position[1] * plane.at(k)[1] - all_voxel.at(i).position[2] * plane.at(k)[2]) / (all_voxel.at(i).toward_vector.at(j)[0] * plane.at(k)[0] + all_voxel.at(i).toward_vector.at(j)[1] * plane.at(k)[1] + all_voxel.at(i).toward_vector.at(j)[2] * plane.at(k)[2]);
                    if(t > 0){
                        vec3 new_p = all_voxel.at(i).position + t * all_voxel.at(i).toward_vector.at(j);
                        vec3 p1 = vec3(model->vertices->at(3 * model->triangles->at(k).vindices[0] + 0), model->vertices->at(3 * model->triangles->at(k).vindices[0] + 1), model->vertices->at(3 * model->triangles->at(k).vindices[0] + 2));
                        vec3 p2 = vec3(model->vertices->at(3 * model->triangles->at(k).vindices[1] + 0), model->vertices->at(3 * model->triangles->at(k).vindices[1] + 1), model->vertices->at(3 * model->triangles->at(k).vindices[1] + 2));
                        vec3 p3 = vec3(model->vertices->at(3 * model->triangles->at(k).vindices[2] + 0), model->vertices->at(3 * model->triangles->at(k).vindices[2] + 1), model->vertices->at(3 * model->triangles->at(k).vindices[2] + 2));
                        vec3 n = vec3(plane.at(k)[0], plane.at(k)[1], plane.at(k)[2]);
                        float temp1 = ((p2 - p1) ^ (new_p - p1)) * n;
                        float temp2 = ((p3 - p2) ^ (new_p - p2)) * n;
                        float temp3 = ((p1 - p3) ^ (new_p - p3)) * n;

                        if((temp1 > 0) && (temp2 > 0) && (temp3 > 0)){
                            intersect += 1;
                            break;
                        }
                    }
                }
            }
            if(intersect != 6)
                all_voxel.at(i).show = false;
        }
    }

    GLMmodel *cube = glmReadOBJ("test_model/cube.obj");
    GLMmodel *output = glmCopy(cube);
    int num = 0;
    for(unsigned int i = 0; i < all_voxel.size(); i += 1){
        if(all_voxel.at(i).show){
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
