//varying vec3 lightDir,normal;
//
//void main()
//{
//
//	float intensity;
//	vec4 color;
//
//	// normalizing the lights position to be on the safe side
//
//	vec3 n = normalize(normal);
//
//	intensity = dot(lightDir,n);
//
//	if (intensity > 0.95)
//		color = vec4(1.0,1.0,1.0,1.0);
//	else if (intensity > 0.5)
//		color = vec4(0.8,0.8,0.8,1.0);
//	else if (intensity > 0.25)
//		color = vec4(0.6,0.6,0.6,1.0);
//	else
//		color = vec4(0.4,0.4,0.4,1.0);
//
//    //color = vec4(intensity,intensity,intensity,1.0);
//    //color = vec4(n,1.0);
//
//	gl_FragColor = color;
//}


varying vec3 N;
varying vec3 v;
void main (void)
{
   vec3 L = normalize(gl_LightSource[0].position.xyz - v);
   vec3 E = normalize(-v); // we are in Eye Coordinates, so EyePos is (0,0,0)
   vec3 R = normalize(-reflect(L,N));

   //calculate Ambient Term:
   vec4 Iamb = gl_FrontLightProduct[0].ambient;

   //calculate Diffuse Term:
   vec4 Idiff = gl_FrontLightProduct[0].diffuse * max(dot(N,L), 0.0);
   Idiff = clamp(Idiff, 0.0, 1.0);

   // calculate Specular Term:
   vec4 Ispec = gl_FrontLightProduct[0].specular
                * pow(max(dot(R,E),0.0),0.3*gl_FrontMaterial.shininess);
   Ispec = clamp(Ispec, 0.0, 1.0);
   // write Total Color:
   gl_FragColor = gl_FrontLightModelProduct.sceneColor + Iamb + Idiff + Ispec;
}

