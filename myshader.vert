//varying vec3 lightDir,normal;
//
//void main()
//{
//	lightDir = normalize(vec3(gl_LightSource[0].position));
//	//normal = gl_NormalMatrix * gl_Normal;
//	normal = gl_Normal;
//
//	gl_Position = ftransform();
//}


varying vec3 N;
varying vec3 v;
void main(void)
{
   v = vec3(gl_ModelViewMatrix * gl_Vertex);
   N = normalize(gl_NormalMatrix * gl_Normal);
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
