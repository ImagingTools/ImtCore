#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec3 fragPosition;

layout(location = 0) out vec4 outColor;

// Must match vertex shader bindings exactly
layout(std140, binding = 0) uniform GlobalUBO {
    mat4 viewMatrix;
    mat4 projectionMatrix;
    vec4 viewPosition;
    vec4 lightPosition;
    vec4 lightColor;
} global;

layout(std140, binding = 1) uniform DrawUBO {
    mat4  modelMatrix;
    vec4  itemColor;
    int   colorMode;
    int   useNormals;
    int   usePointSize;
    float pointSize;
} draw;

void main()
{
    if (draw.useNormals != 0) {
        float ambientStrength = 0.1;
        vec3  ambient = ambientStrength * global.lightColor.xyz;

        vec3  n    = normalize(fragNormal);
        vec3  l    = normalize(global.lightPosition.xyz - fragPosition);
        float diff = max(dot(n, l), 0.0);
        vec3  diffuse = diff * global.lightColor.xyz;

        float specStr   = 0.5;
        vec3  v         = normalize(global.viewPosition.xyz - fragPosition);
        vec3  r         = reflect(-l, n);
        float spec      = pow(max(dot(v, r), 0.0), 32.0);
        vec3  specular  = specStr * spec * global.lightColor.xyz;

        outColor = vec4((ambient + diffuse + specular) * fragColor, 1.0);
    } else {
        outColor = vec4(fragColor, 1.0);
    }
}
