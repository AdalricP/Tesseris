#version 450

// Vertex input attributes
layout(location = 0) in vec3 inPosition;  // vec3 position from vertex buffer
layout(location = 1) in vec3 inColor;     // vec3 color from vertex buffer
layout(location = 2) in vec3 inNormal;    // vec3 normal from vertex buffer

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec3 fragWorldPos;

// MVP matrices uniform
layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

void main() {
    vec4 worldPos = ubo.model * vec4(inPosition, 1.0);
    gl_Position = ubo.proj * ubo.view * worldPos;
    fragColor = inColor;
    fragNormal = mat3(transpose(inverse(ubo.model))) * inNormal; // Transform normal to world space
    fragWorldPos = worldPos.xyz;
}
