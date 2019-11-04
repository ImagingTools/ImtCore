#version 330 core

// set default precision to medium
#ifdef GL_ES
precision mediump int;
precision mediump float;
#endif

// input from application source code
uniform bool useNormals;
uniform vec3 viewPosition;
uniform vec3 lightPosition;
uniform vec3 lightColor;

// input from vertex shader
in vec3 fragColor;
in vec3 fragNormal;
in vec3 fragPosition;

// output
out vec4 color;

void main()
{
	if (useNormals){
		// ambient
		float ambientStrength = 0.1;
		vec3 ambient = ambientStrength * lightColor;

		// diffuse 
		vec3 normalizedNormal = normalize(fragNormal);
		vec3 lightDirection = normalize(lightPosition - fragPosition);
		float diff = abs(dot(normalizedNormal, lightDirection));
		vec3 diffuse = diff * lightColor;

		// specular
		float specularStrength = 0.5;
		vec3 viewDirection = normalize(viewPosition - fragPosition);
		vec3 reflectDirection = reflect(-lightDirection, normalizedNormal);
		float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), 32);
		vec3 specular = specularStrength * spec * lightColor;

		color = vec4((ambient + diffuse + specular) * fragColor, 1.0);
	}
	else{
		color = vec4(fragColor, 1.0);
	}
}
