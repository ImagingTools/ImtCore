#version 450

// Canonical vertex format produced by CRhiGeometryResource:
//   location 0 — position  (vec3, stride offset  0)
//   location 1 — normal    (vec3, stride offset 12)
//   location 2 — color     (vec3, stride offset 24)

layout(location = 0) in vec3 pointPosition;
layout(location = 1) in vec3 pointNormal;
layout(location = 2) in vec3 pointColor;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec3 fragPosition;

// Binding 0 — per-frame globals (updated once in EndFrame)
layout(std140, binding = 0) uniform GlobalUBO {
    mat4 viewMatrix;
    mat4 projectionMatrix;
    vec4 viewPosition;      // xyz used, w padding
    vec4 lightPosition;     // xyz used, w padding
    vec4 lightColor;        // xyz used, w padding
} global;

// Binding 1 — per-draw data (dynamic offset into a shared buffer)
layout(std140, binding = 1) uniform DrawUBO {
    mat4  modelMatrix;
    vec4  itemColor;        // xyz = solid colour, w padding
    int   colorMode;        // 0 = per-vertex, 1 = solid
    int   useNormals;       // bool
    int   usePointSize;     // bool
    float pointSize;
} draw;

void main()
{
    gl_Position = global.projectionMatrix
                * global.viewMatrix
                * draw.modelMatrix
                * vec4(pointPosition, 1.0);

    if (draw.usePointSize != 0) {
        gl_PointSize = draw.pointSize;
    }

    if (draw.colorMode == 0) {
        fragColor = pointColor;
    } else if (draw.colorMode == 1) {
        fragColor = draw.itemColor.xyz;
    } else {
        fragColor = vec3(0.0, 0.0, 0.0);
    }

    fragPosition = vec3(draw.modelMatrix * vec4(pointPosition, 1.0));
    fragNormal   = mat3(transpose(inverse(draw.modelMatrix))) * pointNormal;
}
