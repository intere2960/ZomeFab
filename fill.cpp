#include <vector>
#include "glm.h"
#include "algebra3.h"
#include "glui_internal.h"
#include "fill.h"

void convert_2d(GLMmodel &temp_piece, Loop &loop)
{
    mat3 axis_mat;

    axis_mat[2] = -loop.plane_normal;

    vec3 row_0, row_1;

    float d = 1.0f / axis_mat[2].length();

    row_0[0] = axis_mat[2][1] * d;
    row_0[1] = -axis_mat[2][0] * d;
    row_0[2] = 0.0;

    row_1[0] = -axis_mat[2][2] * row_0[1];
    row_1[1] = axis_mat[2][2] * row_0[0];
    row_1[2] = axis_mat[2][0] * row_0[1] - axis_mat[2][1] * row_0[0];

    vec3 zero(0.0, 0.0, 0.0);
    if(row_0 == zero && row_1 == zero){
        row_0[0] = (axis_mat[2][2] < 0.0) ? -1.0 : 1.0;
        row_1[1] = 1.0;
        axis_mat[0] = row_0;
        axis_mat[1] = row_1;
    }
    else{
        axis_mat[0] = row_0;
        axis_mat[1] = row_1;
    }

    axis_mat = axis_mat.transpose();

    for(unsigned int i = 0; i < loop.loop_line->size(); i += 1){
        vec3 p(temp_piece.vertices->at(3 * loop.loop_line->at(i) + 0), temp_piece.vertices->at(3 * loop.loop_line->at(i) + 1), temp_piece.vertices->at(3 * loop.loop_line->at(i) + 2));
        float temp_x = axis_mat[0][0] * p[0] + axis_mat[1][0] * p[1] + axis_mat[2][0] * p[2];
        float temp_y = axis_mat[0][1] * p[0] + axis_mat[1][1] * p[1] + axis_mat[2][1] * p[2];
        vec2 new_point(temp_x, temp_y);
        loop.two_d_point->push_back(new_point);
    }
}

int span_tri(Loop &loop, int index1, int index2, int index3)
{
    vec2 p1 = loop.two_d_point->at(index1);
    vec2 p2 = loop.two_d_point->at(index2);
    vec2 p3 = loop.two_d_point->at(index3);

    vec2 vector1 = p3 - p2;
    vec2 vector2 = p1 - p2;

    vec3 cross = vector1 ^ vector2;

    float judge = (cross[0] + cross[1] + cross[2]) * loop.sign_flip;
    loop.num_concave += 1;

    if(judge < -0.000001){
        return _concave;
    }
    if(judge > 0.000001){
        loop.num_concave -= 1;
        return _convex;
    }
    return _tangential;
}

void judge_inverse(Loop &loop)
{
    float start_s = loop.sign->at(0);
    float end_s = loop.sign->at(loop.sign->size() - 1);

    bool judge1 = start_s < 0;
    bool judge2 = end_s < 0;
    if(judge1 && judge2){
        loop.sign_flip = -1.0;
        loop.num_concave = 0;
        for(unsigned i = 0; i < loop.sign->size(); i += 1){
            loop.sign->at(i) = loop.sign_flip * loop.sign->at(i);
            if(loop.sign->at(i) != _convex){
                loop.num_concave += 1;
            }
        }
    }
}

void fill_prepare(GLMmodel &temp_piece)
{
    for(unsigned int i = 0; i < temp_piece.loop->size(); i += 1){
        temp_piece.loop->at(i).two_d_point = new std::vector<vec2>();
        convert_2d(temp_piece, temp_piece.loop->at(i));

        temp_piece.loop->at(i).sign_flip = 1.0;
        temp_piece.loop->at(i).travel_reverse = false;
        temp_piece.loop->at(i).num_concave = 0;

        unsigned int line_size = temp_piece.loop->at(i).loop_line->size();
        temp_piece.loop->at(i).sign = new std::vector<int>();
        for(unsigned int j = 0; j < temp_piece.loop->at(i).loop_line->size(); j += 1){
            int prev = (j - 1 + line_size) % line_size;
            int next = (j + 1) % line_size;
            temp_piece.loop->at(i).sign->push_back(span_tri(temp_piece.loop->at(i), prev, j, next));
        }
        if(temp_piece.loop->at(i).sign->size() > 0)
            judge_inverse(temp_piece.loop->at(i));
    }
}

bool check_ear_index(Loop &loop, int current)
{
    if(loop.num_concave == 0)
        return true;

    if(loop.sign->at(current) == _concave)
        return false;

    int prev_index = (current - 1 + loop.loop_line->size()) % loop.loop_line->size();
    int next_index = (current + 1) % loop.loop_line->size();
    int concave_check = 0;

    for(unsigned int i = 0; i < loop.sign->size(); i += 1){
        int index = (current + i + 2) % loop.loop_line->size();
        if(index == prev_index)
            break;

        if(loop.sign->at(index) != _convex){
            if(span_tri(loop, next_index, prev_index, index) != _concave &&
               span_tri(loop, prev_index, current, index) != _concave &&
               span_tri(loop, current, next_index, index) != _concave ){
                return false;
            }

            concave_check += 1;
            if(concave_check == loop.num_concave) {
                break;
            }
        }
    }
    return true;
}

int find_ear_index(Loop &loop, int current)
{
    int index = current;
    int total_size = loop.sign->size();
    for(unsigned int i = 0; i < loop.sign->size(); i += 1){
        if(check_ear_index(loop, index)){
            return index;
        }
        if(loop.travel_reverse)
            index = (index - 1 + total_size) % total_size;
        else
            index = (index + 1) % total_size;
    }

    index = current;
    for(unsigned int i = 0; i < loop.sign->size(); i += 1){
        if(loop.sign->at(index) != _concave){
            return index;
        }
        index = (index + 1) % total_size;
    }
    return index;
}

void triangulate(Loop &loop)
{
    int travel_index = 0;
    loop.tri = new std::vector<vec3>();
    while(loop.loop_line->size() > 3){
        int ear_index = find_ear_index(loop, travel_index);
        int prev_index = (ear_index - 1 + loop.loop_line->size()) % loop.loop_line->size();
        int next_index = (ear_index + 1) % loop.loop_line->size();

        if(loop.sign->at(ear_index) != _convex){
            loop.num_concave -= 1;
        }

        vec3 tri_vindex;
        tri_vindex[0] = loop.loop_line->at(prev_index);
        tri_vindex[1] = loop.loop_line->at(ear_index);
        tri_vindex[2] = loop.loop_line->at(next_index);
        loop.tri->push_back(tri_vindex);

        if(loop.sign->at(prev_index) != _convex){
            int prev_prev_index = (ear_index - 2 + loop.loop_line->size()) % loop.loop_line->size();
            loop.sign->at(prev_index) = span_tri(loop, prev_prev_index, prev_index, next_index);
            if(loop.sign->at(prev_index) == _convex){
                loop.num_concave -= 1;
            }
        }

        if(loop.sign->at(next_index) != _convex){
            int next_next_index = (ear_index + 2) % loop.loop_line->size();
            loop.sign->at(next_index) = span_tri(loop, prev_index, next_index, next_next_index);
            if(loop.sign->at(next_index) == _convex){
                loop.num_concave -= 1;
            }
        }

        loop.loop_line->erase(loop.loop_line->begin() + ear_index);
        loop.sign->erase(loop.sign->begin() + ear_index);
        loop.two_d_point->erase(loop.two_d_point->begin() + ear_index);

        if(loop.travel_reverse)
            travel_index = (prev_index - 1 + loop.loop_line->size()) % loop.loop_line->size();
        else
            travel_index = next_index % loop.loop_line->size();


        if(loop.sign->at(travel_index) != _convex){
            if(loop.travel_reverse)
                travel_index = (travel_index - 1 + loop.loop_line->size()) % loop.loop_line->size();
            else
                travel_index = (travel_index + 1) % loop.loop_line->size();
            loop.travel_reverse = !loop.travel_reverse;
        }
    }

    if(loop.loop_line->size() == 3) {
        vec3 tri_vindex;
        tri_vindex[0] = loop.loop_line->at(0);
        tri_vindex[1] = loop.loop_line->at(1);
        tri_vindex[2] = loop.loop_line->at(2);
        loop.tri->push_back(tri_vindex);

	}
}

void fill_hole(GLMmodel &temp_piece)
{
    fill_prepare(temp_piece);
    for(unsigned int i = 0; i < temp_piece.loop->size(); i += 1){
        triangulate(temp_piece.loop->at(i));
        for(unsigned int j = 0; j < temp_piece.loop->at(i).tri->size(); j += 1){
            vec3 tri_vindex = temp_piece.loop->at(i).tri->at(j);
            GLMtriangle temp;
            temp.vindices[0] = tri_vindex[0];
            temp.vindices[1] = tri_vindex[1];
            temp.vindices[2] = tri_vindex[2];
            temp_piece.triangles->push_back(temp);
            temp_piece.numtriangles += 1;
        }
    }
}
