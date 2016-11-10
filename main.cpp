#include "main.h"

void drawObj(GLMmodel *myObj)
{
//    if (! myObj) return;
//
//    GLMgroup *groups = myObj->groups;
//
//    glBegin(GL_TRIANGLES);
//    while(groups)
//    {
//        //glBindTexture(GL_TEXTURE_2D, textureid[j]);
//        for(int i=0;i<groups->numtriangles;i+=1)
//        {
//            for (int v=0; v<3; v+=1)
//            {
//                //glColor3fv(& myObj->vertices[myObj->triangles[groups->triangles[i]].vindices[v]*3 ]);
//                glNormal3fv(& myObj->normals[myObj->triangles[groups->triangles[i]].nindices[v]*3 ]);
//                //glTexCoord2fv(& myObj->texcoords[myObj->triangles[groups->triangles[i]].tindices[v]*2 ]);
//                glVertex3fv(& myObj->vertices[myObj->triangles[groups->triangles[i]].vindices[v]*3 ]);
//            }
//        }
//        groups=groups->next;
//    }
//    glEnd();

    glBegin(GL_TRIANGLES);
    for (unsigned int i=0; i<myObj->numtriangles; i+=1) {
        for (int v=0; v<3; v+=1) {
		    //glColor3fv( & myObj->vertices[ myObj->triangles[i].vindices[v]*3 ] );
		    //glNormal3fv( & myObj->normals[ myObj->triangles[i].nindices[v]*3 ] );
		    //glColor3f(& myObj->vertices[ myObj->triangles[i].vindices[v]*3 ]);
		    glNormal3fv( & myObj->facetnorms[3 * (i+1)]);
		    glVertex3fv( & myObj->vertices[ myObj->triangles[i].vindices[v]*3 ] );
        }
    }
    glEnd();
}

void display(void)
{
    /* display callback, clear frame buffer and z buffer,
        rotate cube and draw, swap buffers */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eye_pos[0], eye_pos[1], eye_pos[2], center[0], center[1], center[2], up[0], up[1], up[2]);

    //lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    light0_pos[0] = bound_size[0] * cos(lightTheta*M_PI/180.0);
    light0_pos[1] = bound_size[1] * 1/sin(M_PI/4) * sin(lightTheta*M_PI/180.0);
    light0_pos[2] = bound_size[2] * cos(lightTheta*M_PI/180.0);

    glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);

//    setShaders();

    glPushMatrix();

    glTranslatef(-bound_center[0], -bound_center[1], -bound_center[2]);

    if(show){
        glPolygonMode(GL_FRONT, GL_LINE);
        glPolygonMode(GL_BACK, GL_LINE);
        drawObj(myObj);
//        glmDraw(myObj, GLM_FLAT);
//        glmDraw(myObj,GLM_SMOOTH);
        //angle += 1.0;
//        glmDraw(myObj,GLM_NONE);
        glPolygonMode(GL_FRONT, GL_FILL);
        glPolygonMode(GL_BACK, GL_FILL);
    }


//    drawObj(myObj_inner);
//    glmDraw(myObj_inner, GLM_FLAT);
//    glmDraw(myObj_inner,GLM_SMOOTH);
//    glmDraw(myObj_inner,GLM_NONE);

//    glTranslatef(bound_center[0], bound_center[1], bound_center[2]);

    glPopMatrix();

//    draw_bounding_box();

    glutSwapBuffers();
    glutPostRedisplay();
}

void myReshape(int w, int h)
{
    width = w;
    height = h;
    float ratio = w * 1.0 / h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, w, h);
    gluPerspective(50.0, ratio, 0.1, 100000.0);
    glMatrixMode(GL_MODELVIEW);
}

vector<vec2> *temp_edge = NULL;
vector<edge> all_edge;
vector<bool> is_face_split;

void collect_edge()
{
    int count = 0;
    temp_edge = new vector<vec2>[myObj->numvertices + 1];
    vector<int> *temp_point_tri = new std::vector<int>[myObj->numvertices + 1];

    is_face_split.resize(myObj->numtriangles);

    for(unsigned int i = 0; i < myObj->numtriangles ; i += 1){

        int min_index = myObj->triangles[i].vindices[0], temp_index = 0;
        is_face_split[i] = true;

        if((unsigned int)min(min_index, (int)myObj->triangles[i].vindices[1]) == myObj->triangles[i].vindices[1]){
            min_index = myObj->triangles[i].vindices[1];
            temp_index = 1;
        }
        if((unsigned int)min(min_index, (int)myObj->triangles[i].vindices[2]) == myObj->triangles[i].vindices[2]){
            min_index = myObj->triangles[i].vindices[2];
            temp_index = 2;
        }

        int mid_index = myObj->triangles[i].vindices[(temp_index + 1) % 3], temp_mid_index = (temp_index + 1) % 3;
        if(mid_index > myObj->triangles[i].vindices[(temp_index + 2) % 3]){
            mid_index = myObj->triangles[i].vindices[(temp_index + 2) % 3];
            temp_mid_index = (temp_index + 2) % 3;
        }

        vec2 push_index1(min_index, mid_index), push_index2(min_index, myObj->triangles[i].vindices[(3 - temp_index - temp_mid_index) % 3]), push_index3(mid_index, myObj->triangles[i].vindices[(3 - temp_index - temp_mid_index) % 3]);

        bool add1 = true, add2 = true, add3 = true;

        for(unsigned int j = 0; j < temp_edge[min_index].size(); j += 1){
            if(push_index1[1] == temp_edge[min_index][j][1]){
                add1 = false;
                break;
            }
        }
        for(unsigned int j = 0; j < temp_edge[min_index].size(); j += 1){
            if(push_index2[1] == temp_edge[min_index][j][1]){
                add2 = false;
                break;
            }
        }
        for(unsigned int j = 0; j < temp_edge[mid_index].size(); j += 1){
            if(push_index3[1] == temp_edge[mid_index][j][1]){
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
            temp_point_tri[myObj->triangles[i].vindices[j]].push_back(i);
        }
    }

    for(unsigned int i = 0; i < myObj->numvertices + 1; i += 1){
        for(unsigned int j = 0; j < temp_edge[i].size(); j += 1){
            vec3 p1(myObj->vertices[(int)temp_edge[i][j][0] * 3 + 0], myObj->vertices[(int)temp_edge[i][j][0] * 3 + 1], myObj->vertices[(int)temp_edge[i][j][0] * 3 + 2]);
            vec3 p2(myObj->vertices[(int)temp_edge[i][j][1] * 3 + 0], myObj->vertices[(int)temp_edge[i][j][1] * 3 + 1], myObj->vertices[(int)temp_edge[i][j][1] * 3 + 2]);
            edge temp((int)temp_edge[i][j][0], p1, (int)temp_edge[i][j][1], p2);
//            cout << temp.index[0] << " " << temp.index[1] << endl;
            all_edge.push_back(temp);
        }
    }

    for(unsigned int i = 1; i <= myObj->numvertices; i += 1)
        sort(temp_point_tri[i].begin(), temp_point_tri[i].begin() + temp_point_tri[i].size());

//    for(unsigned int i = 1; i <= myObj->numvertices; i += 1){
//        for(unsigned int j = 0; j < temp_point_tri[i].size(); j += 1){
//            cout << temp_point_tri[i][j] << " ";
//        }
//        cout << endl;
//    }

    for(unsigned int i = 0; i < all_edge.size(); i += 1){
        vector<int> temp_vector(temp_point_tri[all_edge[i].index[0]].size() + temp_point_tri[all_edge[i].index[1]].size());
        vector<int>::iterator it;

        it = set_intersection(temp_point_tri[all_edge[i].index[0]].begin(), temp_point_tri[all_edge[i].index[0]].begin() + temp_point_tri[all_edge[i].index[0]].size(), temp_point_tri[all_edge[i].index[1]].begin(), temp_point_tri[all_edge[i].index[1]].begin() + temp_point_tri[all_edge[i].index[1]].size(), temp_vector.begin());
        temp_vector.resize(it - temp_vector.begin());

        all_edge[i].face_id[0] = temp_vector[0];
        all_edge[i].face_id[1] = temp_vector[1];

//        cout << all_edge[i].index[0] << " " << all_edge[i].index[1] << endl;
//        cout << " " << all_edge[i].face_id[0] << " " << all_edge[i].face_id[1] << endl;

        for(int j = 0; j < 3; j += 1){
            if(myObj->triangles[temp_vector[0]].edge_index[j] == -1){
                myObj->triangles[temp_vector[0]].edge_index[j] = i;
                break;
            }
        }

        for(int j = 0; j < 3; j += 1){
            if(myObj->triangles[temp_vector[1]].edge_index[j] == -1){
                myObj->triangles[temp_vector[1]].edge_index[j] = i;
                break;
            }
        }
    }

//    for(int i = 0; i < myObj->numtriangles; i += 1){
//        cout << myObj->triangles[i].edge_index[0] << " " << myObj->triangles[i].edge_index[1] << " " << myObj->triangles[i].edge_index[2] << endl;
//    }

    delete temp_edge;
    delete temp_point_tri;
}

float plane_dir_point(vec3 &point, float plane[4])
{
    float judge = plane[0] * point[0] + plane[1] * point[1] + plane[2] * point[2] - plane[3];

    if(judge > 0)
        judge = 1;
    else if(judge == 0)
        judge = 0;
    else
        judge = -1;

    return judge;
}

void plane_dir(edge &temp, float plane[4], int dir[2])
{
    float judge1 = plane[0] * temp.point[0][0] + plane[1] * temp.point[0][1] + plane[2] * temp.point[0][2] - plane[3];
    float judge2 = plane[0] * temp.point[1][0] + plane[1] * temp.point[1][1] + plane[2] * temp.point[1][2] - plane[3];

    if(judge1 > 0)
        dir[0] = 1;
    else if(judge1 == 0)
        dir[0] = 0;
    else
        dir[0] = -1;

    if(judge2 > 0)
        dir[1] = 1;
    else if(judge2 == 0)
        dir[1] = 0;
    else
        dir[1] = -1;
}

void plane_dist(edge &temp, float plane[4], float dist[2])
{
    float judge1 = plane[0] * temp.point[0][0] + plane[1] * temp.point[0][1] + plane[2] * temp.point[0][2] - plane[3];
    float judge2 = plane[0] * temp.point[1][0] + plane[1] * temp.point[1][1] + plane[2] * temp.point[1][2] - plane[3];

    dist[0] = fabs(judge1) / sqrt(pow(plane[0],2) + pow(plane[1],2) + pow(plane[2],2));
    dist[1] = fabs(judge2) / sqrt(pow(plane[0],2) + pow(plane[1],2) + pow(plane[2],2));
}

float test_plane[4] = {0.0, 1.0, 0.0, -1.0};
vector<int> split_edge_index;

void split()
{
    for(unsigned int i = 0; i < all_edge.size(); i +=1){
        int dir[2];
        float dist[2];
        plane_dir(all_edge[i], test_plane, dir);
        plane_dist(all_edge[i], test_plane, dist);

        if(dir[0] && dir[1] && (dir[0] != dir[1])){
            float edge_ratio = dist[0] / (dist[0] + dist[1]);
            vec3 new_point = all_edge[i].point[0] + edge_ratio * (all_edge[i].point[1] - all_edge[i].point[0]);
            all_edge[i].is_split = true;
            all_edge[i].split_point = new_point;

            split_edge_index.push_back(i);

            if(is_face_split[all_edge[i].face_id[0]])
                is_face_split[all_edge[i].face_id[0]] = false;
            if(is_face_split[all_edge[i].face_id[1]])
                is_face_split[all_edge[i].face_id[1]] = false;
        }
        else if(dir[0] == 0 || dir[1] == 0){
            all_edge[i].is_split = true;

            if(dir[0] == 0){
                all_edge[i].split_point = all_edge[i].point[0];
                all_edge[i].vertex_push_index = all_edge[i].index[0];
            }
            else if(dir[1] == 0){
                all_edge[i].split_point = all_edge[i].point[1];
                all_edge[i].vertex_push_index = all_edge[i].index[1];
            }

            split_edge_index.push_back(i);

            if(is_face_split[all_edge[i].face_id[0]])
                is_face_split[all_edge[i].face_id[0]] = false;
            if(is_face_split[all_edge[i].face_id[1]])
                is_face_split[all_edge[i].face_id[1]] = false;

            //whether judge align (dir[0] == 0 && dir[1] == 0) condition ?
        }
    }

//    for(unsigned int i = 0; i < myObj->numtriangles; i +=1){
//        cout << myObj->triangles[i].vindices[0] << " " << myObj->triangles[i].vindices[1] << " " << myObj->triangles[i].vindices[2] << endl;
//        cout << " " << myObj->triangles[i].edge_index[0] << " " << myObj->triangles[i].edge_index[1] << endl;
//    }

//    cout << endl;
//    for(unsigned int i = 0; i < all_edge.size(); i +=1){
//        cout << i << " : " << all_edge[i].index[0] << " " << all_edge[i].index[1] << " : " << all_edge[i].face_id[0] << " " << all_edge[i].face_id[1] << endl;
//    }
//    cout << endl;
//
//    for(unsigned int i = 0; i < split_edge_index.size(); i +=1){
//        cout << split_edge_index[i] << " : ";
//        cout << all_edge[split_edge_index[i]].index[0] << " " << all_edge[split_edge_index[i]].index[1] << " : " << all_edge[split_edge_index[i]].face_id[0] << " " << all_edge[split_edge_index[i]].face_id[1] << endl;
//    }
//    cout << myObj->numvertices << endl;

    //bool is_face_split[myObj->numtriangles];
//    cout << endl;

    unsigned int current_numtri = myObj->numtriangles;
    for(unsigned int i = 0; i < current_numtri; i += 1){
        if(!is_face_split[i]){

            vec3 point_dir;
            int dir_count[3] = {0, 0, 0};
            for(int j = 0; j < 3; j += 1){
                vec3 temp(myObj->vertices[3 * (myObj->triangles[i].vindices[j]) + 0], myObj->vertices[3 * (myObj->triangles[i].vindices[j]) + 1], myObj->vertices[3 * (myObj->triangles[i].vindices[j]) + 2]);
                point_dir[j] = plane_dir_point(temp,test_plane);
                dir_count[(int)point_dir[j] + 1] += 1;
//                cout << point_dir[j] << " ";
            }
//            cout << endl;
            int choose_dir = -1;
            if(dir_count[2] == 1)
                choose_dir = 1;

            int choose_index = 0;
            for(int j = 0; j < 3; j += 1){
                if(point_dir[j] == choose_dir)
                    choose_index = j;
            }

            vec2 choose_edge(-1,-1);
            vec2 choose_vertex_index(-1,-1);
            int x = 0;

            cout << choose_index << endl;
            cout << myObj->triangles[i].vindices[choose_index] << endl;
            cout << myObj->vertices[3 * (myObj->triangles[i].vindices[choose_index]) + 0] << " " << myObj->vertices[3 * (myObj->triangles[i].vindices[choose_index]) + 1] << " " << myObj->vertices[3 * (myObj->triangles[i].vindices[choose_index]) + 2] << endl;

            for(int j = 0; j < 3; j += 1){
                if(all_edge[myObj->triangles[i].edge_index[j]].index[0] == myObj->triangles[i].vindices[choose_index]){
//                    cout << "index[0] " << all_edge[myObj->triangles[i].edge_index[j]].index[0] << " " << myObj->triangles[i].vindices[choose_index] << endl;
                    if(choose_edge[0] == -1){
                        choose_edge[0] = myObj->triangles[i].edge_index[j];
                        choose_vertex_index[0] = 1;
                    }
                    else{
                        choose_edge[1] = myObj->triangles[i].edge_index[j];
                        choose_vertex_index[1] = 1;
                    }
                }
                if(all_edge[myObj->triangles[i].edge_index[j]].index[1] == myObj->triangles[i].vindices[choose_index]){
//                    cout << "index[1] " << all_edge[myObj->triangles[i].edge_index[j]].index[1] << " " << myObj->triangles[i].vindices[choose_index] << endl;
                    if(choose_edge[0] == -1){
                        choose_edge[0] = myObj->triangles[i].edge_index[j];
                        choose_vertex_index[0] = 0;
                    }
                    else{
                        choose_edge[1] = myObj->triangles[i].edge_index[j];
                        choose_vertex_index[1] = 0;
                    }
                }
//                cout << "judge : " << myObj->triangles[i].edge_index[j] << endl;
//                cout << choose_edge[0] << " " << choose_edge[1] << endl;
            }

            cout << choose_edge[0] << " " << choose_edge[1] << endl;
            cout << choose_vertex_index[0] << " " << choose_vertex_index[1] << endl;

            if(all_edge[choose_edge[0]].vertex_push_index == -1){
                myObj->vertices.push_back(all_edge[choose_edge[0]].split_point[0]);
                myObj->vertices.push_back(all_edge[choose_edge[0]].split_point[1]);
                myObj->vertices.push_back(all_edge[choose_edge[0]].split_point[2]);
                myObj->numvertices += 1;
                all_edge[choose_edge[0]].vertex_push_index = myObj->numvertices;
            }

            if(all_edge[choose_edge[1]].vertex_push_index == -1){
                myObj->vertices.push_back(all_edge[choose_edge[1]].split_point[0]);
                myObj->vertices.push_back(all_edge[choose_edge[1]].split_point[1]);
                myObj->vertices.push_back(all_edge[choose_edge[1]].split_point[2]);
                myObj->numvertices += 1;
                all_edge[choose_edge[1]].vertex_push_index = myObj->numvertices;
            }

            cout << "edge : " << myObj->triangles[i].edge_index[0] << " " << myObj->triangles[i].edge_index[1] << " " << myObj->triangles[i].edge_index[2] << endl;
            cout << "choose edge : " << choose_edge[0] << " " << choose_edge[1] << endl;
            cout << "edge info 1 : " << all_edge[choose_edge[0]].index[0] << " " << all_edge[choose_edge[0]].index[1] << " " << all_edge[choose_edge[0]].vertex_push_index << endl;
            cout << "edge info 2 : " << all_edge[choose_edge[1]].index[0] << " " << all_edge[choose_edge[1]].index[1] << " " << all_edge[choose_edge[1]].vertex_push_index << endl;
            cout << "origin : " << myObj->triangles[i].vindices[0] << " " << myObj->triangles[i].vindices[1] << " " << myObj->triangles[i].vindices[2] << endl;

            GLMtriangle temp1,temp2;

            temp1.vindices[0] = all_edge[choose_edge[0]].vertex_push_index;
            temp1.vindices[1] = all_edge[choose_edge[1]].vertex_push_index;
            temp1.vindices[2] = all_edge[choose_edge[1]].index[(int)choose_vertex_index[1]];
            myObj->triangles.push_back(temp1);
            myObj->numtriangles += 1;

            cout << "1 : " << all_edge[choose_edge[0]].vertex_push_index << " " << all_edge[choose_edge[1]].vertex_push_index << " " << all_edge[choose_edge[1]].index[(int)choose_vertex_index[1]] << endl;

            temp2.vindices[0] = all_edge[choose_edge[0]].vertex_push_index;
            temp2.vindices[1] = all_edge[choose_edge[1]].index[(int)choose_vertex_index[1]];
            temp2.vindices[2] = all_edge[choose_edge[0]].index[(int)choose_vertex_index[0]];
            myObj->triangles.push_back(temp2);
            myObj->numtriangles += 1;

            cout << "2 : " << all_edge[choose_edge[0]].vertex_push_index << " " << all_edge[choose_edge[1]].index[(int)choose_vertex_index[1]] << " " << all_edge[choose_edge[0]].index[(int)choose_vertex_index[0]] << endl;

            myObj->triangles[i].vindices[0] = myObj->triangles[i].vindices[choose_index];
            myObj->triangles[i].vindices[1] = all_edge[choose_edge[0]].vertex_push_index;
            myObj->triangles[i].vindices[2] = all_edge[choose_edge[1]].vertex_push_index;

            cout << "3 : " << all_edge[choose_edge[1]].index[1 - (int)choose_vertex_index[1]] << " " << all_edge[choose_edge[0]].vertex_push_index << " " << all_edge[choose_edge[1]].vertex_push_index << endl;
            cout << endl;
        }
    }

//    cout << endl;
//    for(int i = 0; i < myObj->numtriangles; i += 1){
//        cout << is_face_split[i] << " ";
//    }
//    cout << endl;

//    for(unsigned int i = 0; i < all_edge.size(); i += 1){
////        cout << i << " : " << all_edge[i].split_point[0] << " " << all_edge[i].split_point[1] << " " << all_edge[i].split_point[2] << endl;
//        cout << all_edge[i].vertex_push_index << endl;
//    }
//    cout << endl;

    glmFacetNormals(myObj);
}

void init()
{
    bounding_box();

    eye_pos[2] = eye_pos[2] + 2.0 * bound_size[2];

    recount_normal(myObj, point_tri);
    process_inner(myObj, myObj_inner);

    collect_edge();
    split();
}

int main(int argc, char **argv)
{
    //myObj = glmReadOBJ("test_model/sponza.obj");
    myObj = glmReadOBJ("test_model/cube.obj");
//    myObj = glmReadOBJ("test_model/bunny.obj");
    //myObj = glmReadOBJ("test_model/alduin.obj");
    myObj_inner = glmReadOBJ("test_model/cube.obj");

    init();

	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(1000,1000);

    glutCreateWindow("Zometool");
	glutDisplayFunc(display);
	glutReshapeFunc(myReshape);
	glutMouseFunc(mouse);
    glutMotionFunc(mouseMotion);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
	glEnable(GL_DEPTH_TEST); /* Enable hidden--surface--removal */

	glewInit();

	setShaders();

	glutMainLoop();


    return 0;
}

