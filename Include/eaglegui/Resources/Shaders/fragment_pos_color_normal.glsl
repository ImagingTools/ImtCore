varying lowp vec3 norm;
varying lowp vec3 pos;
varying lowp vec3 viewPos;

uniform vec3 dir_to_light;

uniform vec4 color;

void main()
{
	vec3 n = normalize(norm);
	float t = abs(dot(n,normalize(dir_to_light)));
	
	vec4 I = vec4(1.0,1.0,1.0,0.0);
	
	float k_diffuse = 0.7f;
	float k_ambient = 0.3f;
	float k_specular = 0.0f;
	
	float roughness = 5.0f;
	
	vec4 diffuse = k_diffuse * t * color;
	vec4 ambient = k_ambient * color;
	vec4 specular = k_specular * pow(cos(t),roughness) * I;
	
	vec4 frag_col = diffuse + ambient + specular;
	
	frag_col.w = 0.0f;	

	gl_FragColor = frag_col;
}

