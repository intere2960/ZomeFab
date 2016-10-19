
uniform vec4 l_ambient;
uniform vec4 l_diffuse;
uniform vec4 l_specular;
uniform vec3 light;
uniform float l_shininess;

varying vec3 N;
varying vec3 v;

void main (void)
{
//   vec3 L = normalize(gl_LightSource[0].position.xyz - v);
//   vec3 E = normalize(-v); // we are in Eye Coordinates, so EyePos is (0,0,0)
//   vec3 R = normalize(-reflect(L,N));
//
//   //calculate Ambient Term:
//   vec4 Iamb = gl_FrontLightProduct[0].ambient;
//
//   //calculate Diffuse Term:
//   vec4 Idiff = gl_FrontLightProduct[0].diffuse * max(dot(N,L), 0.0);
//   Idiff = clamp(Idiff, 0.0, 1.0);
//
//   // calculate Specular Term:
//   vec4 Ispec = gl_FrontLightProduct[0].specular
//                * pow(max(dot(R,E),0.0),0.3*gl_FrontMaterial.shininess);
//   Ispec = clamp(Ispec, 0.0, 1.0);
//   // write Total Color:
//   gl_FragColor = gl_FrontLightModelProduct.sceneColor + Iamb + Idiff + Ispec;


    vec3 lightWeighting;
    vec3 lightDirection = normalize(light.xyz - v);
    vec3 normal = N;

    float specularLightWeighting = 0.0;

    vec3 eyeDirection = normalize(-v);
    vec3 reflectionDirection = normalize(-reflect(L,N));

    specularLightWeighting = pow(max(dot(reflectionDirection, eyeDirection), 0.0), l_shininess);


    float diffuseLightWeighting = max(dot(normal, lightDirection), 0.0);
     lightWeighting = l_ambient
        + l_specular * specularLightWeighting
        + l_diffuse * diffuseLightWeighting;

    gl_FragColor = gl_FrontLightModelProduct.sceneColor + lightWeighting;
}

