#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec3 fragWorldPos;
layout(location = 0) out vec4 outColor;

// Lighting uniform
layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 lightPos;
    vec3 lightColor;
    vec3 viewPos;
} ubo;

void main() {
    // Normalize the normal (it may not be unit length after interpolation)
    vec3 normal = normalize(fragNormal);

    // Calculate light direction
    vec3 lightDir = normalize(ubo.lightPos - fragWorldPos);

    // View direction (from fragment to camera)
    vec3 viewDir = normalize(ubo.viewPos - fragWorldPos);

    // Ambient component
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * ubo.lightColor;

    // Diffuse component
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * ubo.lightColor;

    // Specular component (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0); // Shininess factor
    vec3 specular = spec * ubo.lightColor;

    // Combine lighting with material color
    vec3 result = (ambient + diffuse + specular) * fragColor;
    outColor = vec4(result, 1.0);
}
