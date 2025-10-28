#version 450

// Hardcoded triangle vertices (no vertex buffer needed for now)
vec2 positions[3] = vec2[](
    vec2(-0.5, 0.5),   // top-left
    vec2(0.5, 0.5),    // top-right
    vec2(0.0, -0.5)    // bottom-center
);

vec3 colors[3] = vec3[](
    vec3(1.0, 0.0, 0.0),  // red at top-left
    vec3(0.0, 1.0, 0.0),  // green at top-right
    vec3(0.0, 0.0, 1.0)   // blue at bottom
);

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    fragColor = colors[gl_VertexIndex];
}
