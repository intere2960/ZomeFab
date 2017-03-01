
//attribute vec3 position;

varying vec3 N;
varying vec3 v;
//uniform float angle;

//vec4 vertex;

void main(void)
{
   v = vec3(gl_ModelViewMatrix * gl_Vertex);
   N = normalize(gl_NormalMatrix  * gl_Normal);

//   vertex = gl_Vertex;
//   vertex.z += sin(vertex.x * 4.0 +angle);

//   position.z += sin(position.x * 4.0);

//   gl_Position = gl_ModelViewProjectionMatrix * vec4(position,1.0);
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
//   gl_Position = gl_ModelViewProjectionMatrix * vertex;
}
