#version 450

// Vertex input attributes
layout(location = 0) in vec3 inPosition;  // vec3 position from vertex buffer
layout(location = 1) in vec3 inColor;     // vec3 color from vertex buffer

layout(location = 0) out vec3 fragColor;

// MVP matrices uniform
layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    fragColor = inColor;
}
