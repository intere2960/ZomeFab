varying vec3 lightDir,normal;

void main()
{

	float intensity;
	vec4 color;
	
	// normalizing the lights position to be on the safe side
	
	vec3 n = normalize(normal);
	
	intensity = dot(lightDir,n);

	color = vec4(intensity,intensity,intensity,1.0);
	
	
	gl_FragColor = color;
} 
