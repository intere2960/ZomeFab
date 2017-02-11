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
