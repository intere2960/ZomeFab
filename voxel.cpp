#include "voxel.h"
#include "zomedir.h"
#include "glm.h"
#include "glui_internal.h"

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

vec2 check_bound(std::vector<voxel> &all_voxel, vec3 bounding_max, vec3 bounding_min)
{
    vec2 t_ans(-1,-1);
    for(unsigned int i = 0; i < all_voxel.size(); i += 1){
        for(int j = 0; j < 6; j += 1){
            if(all_voxel.at(i).face_toward[j] == -1){
                vec3 temp_p = all_voxel.at(i).position + all_voxel.at(i).toward_vector.at(j) * 3 * all_voxel.at(i).scale;

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

void voxelization(std::vector<voxel> &all_voxel, vec3 &bounding_max, vec3 &bounding_min, int v_color, float v_size)
{
    zomedir t;
    voxel start(v_color, v_size, t.color_length(v_color, v_size) / 2.0);
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
        all_voxel.push_back(temp);

        ans = check_bound(all_voxel, bounding_max, bounding_min);
    }

//    GLMmodel *cube = glmReadOBJ("test_model/cube.obj");
//    GLMmodel *output = glmCopy(cube);
//    for(unsigned int i = 0; i < all_voxel.size(); i += 1){
//        if(i == 0){
//            glmScale(output, start.scale);
//            glmRT(output, start.rotation, start.position);
//        }
//        else{
//            GLMmodel *temp = glmCopy(cube);
//            glmScale(temp, all_voxel.at(i).scale);
//            glmRT(temp, all_voxel.at(i).rotation, all_voxel.at(i).position);
//            glmCombine(output, temp);
//        }
//    }
//    glmWriteOBJ(output, "123.obj", GLM_NONE);
}
