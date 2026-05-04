#version 330 core

#ifdef GL_ES
precision mediump int;
precision mediump float;
#endif

// input
in vec3 pointPosition;
in vec3 pointNormal;
in vec3 pointColor;

in vec3 lineStartPosition;
in vec3 lineEndPosition;
in vec3 lineStartColor;
in vec3 lineEndColor;
in float lineSide;
in float lineEndpoint;

uniform bool usePointSize;
uniform float pointSize;

uniform bool useThickLines;
uniform float lineWidthPixels;
uniform vec2 viewportSize;

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
	if (useThickLines){
		vec4 startClip = projectionMatrix * viewMatrix * modelMatrix * vec4(lineStartPosition, 1.0);
		vec4 endClip = projectionMatrix * viewMatrix * modelMatrix * vec4(lineEndPosition, 1.0);

		vec2 startNdc = startClip.xy / startClip.w;
		vec2 endNdc = endClip.xy / endClip.w;

		vec2 dir = endNdc - startNdc;
		float dirLen = length(dir);
		if (dirLen > 0.00001){
			dir /= dirLen;
		}
		else{
			dir = vec2(1.0, 0.0);
		}

		vec2 normal = vec2(-dir.y, dir.x);
		vec2 pixelToNdc = vec2(2.0 / viewportSize.x, 2.0 / viewportSize.y);
		vec2 ndcOffset = normal * lineSide * (0.5 * lineWidthPixels) * pixelToNdc;

		vec4 baseClip = (lineEndpoint < 0.5) ? startClip : endClip;
		baseClip.xy += ndcOffset * baseClip.w;

		gl_Position = baseClip;

		fragColor = mix(lineStartColor, lineEndColor, clamp(lineEndpoint, 0.0, 1.0));
		fragNormal = vec3(0.0, 0.0, 1.0);
		fragPosition = vec3(modelMatrix * vec4((lineEndpoint < 0.5) ? lineStartPosition : lineEndPosition, 1.0));
		return;
	}

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
