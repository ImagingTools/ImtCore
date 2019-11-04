attribute highp vec3 posAttr;
attribute lowp vec3 normAttr;

varying lowp vec3 norm;
varying lowp vec3 pos;
varying lowp vec3 viewPos;

uniform highp mat4 M;
uniform highp mat4 N;
uniform highp mat4 V;
uniform highp mat4 P;
uniform highp mat4 VP;
uniform highp mat4 MVP;

void main()
{
	vec4 tmp;

	tmp = N * vec4(normAttr,1);
	norm = tmp.xyz / tmp.w;
	
	tmp = M * vec4(posAttr,1);
	pos = tmp.xyz / tmp.w;

	tmp = V*M*vec4(posAttr,1);
	viewPos = tmp.xyz / tmp.w;

	gl_Position = MVP * vec4(posAttr,1);
}


