#ifndef UNIFORM_BUFFER_H
#define UNIFORM_BUFFER_H

#include <vulkan/vulkan.h>
#include "../graphics_pipeline/buffer.h"
#include "../math/matrix.h"

/**
 * Uniform Buffer Object for MVP matrices and lighting
 * Matches GLSL std140 layout with proper padding
 */
typedef struct {
    mat4 model;        // Model matrix (64 bytes)
    mat4 view;         // View matrix (64 bytes)
    mat4 proj;         // Projection matrix (64 bytes)
    vec3 lightPos;     // Light position in world space (12 bytes + 4 padding = 16 bytes)
    float _pad1;       // Padding for std140 alignment
    vec3 lightColor;   // Light color (RGB) (12 bytes + 4 padding = 16 bytes)
    float _pad2;       // Padding for std140 alignment
    vec3 viewPos;      // Camera/view position in world space (12 bytes + 4 padding = 16 bytes)
    float _pad3;       // Padding for std140 alignment
} UniformBufferObject;

/**
 * Create a uniform buffer for storing MVP matrices
 *
 * @param device - VkDevice handle
 * @param physicalDevice - VkPhysicalDevice for querying memory properties
 * @param outBuffer - Output buffer handle
 * @return VK_SUCCESS on success, error code otherwise
 */
VkResult createUniformBuffer(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    Buffer* outBuffer
);

/**
 * Update uniform buffer with new MVP matrices
 *
 * @param device - VkDevice handle
 * @param buffer - Uniform buffer to update
 * @param ubo - Uniform buffer object data
 * @return VK_SUCCESS on success, error code otherwise
 */
VkResult updateUniformBuffer(
    VkDevice device,
    Buffer* buffer,
    const UniformBufferObject* ubo
);

#endif // UNIFORM_BUFFER_H