#version 330 core

#ifdef GL_ES
precision mediump int;
precision mediump float;
#endif

// input
in vec3 pointPosition;
in vec3 pointNormal;
in vec3 pointColor;

uniform bool usePointSize;
uniform float pointSize;

uniform int colorMode;
uniform vec3 itemColor;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

// output
out vec3 fragColor;
out vec3 fragNormal;
out vec3 fragPosition;

void main()
{
	gl_Position = projectionMatrix * (viewMatrix * (modelMatrix * vec4(pointPosition, 1.0)));

	if (usePointSize){
		gl_PointSize = pointSize;
	}

	// pass color to fragment shader
	if (colorMode == 0){ // one color for each item point
		fragColor = pointColor;
	}
	else if (colorMode == 1){ // one color for entire item
		fragColor = itemColor;
	}
	else{
		fragColor = vec3(0.0, 0.0, 0.0);
	}

	// calculate and pass fragment position to fragment shader
	fragPosition = vec3(modelMatrix * vec4(pointPosition, 1.0));

	// calculate and pass normal to fragment shader
	fragNormal = mat3(transpose(inverse(modelMatrix))) * pointNormal;
}
