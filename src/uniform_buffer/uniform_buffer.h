#ifndef UNIFORM_BUFFER_H
#define UNIFORM_BUFFER_H

#include <vulkan/vulkan.h>
#include "../graphics_pipeline/buffer.h"
#include "../math/matrix.h"

/**
 * Uniform Buffer Object for MVP matrices
 */
typedef struct {
    mat4 model;  // Model matrix
    mat4 view;   // View matrix
    mat4 proj;   // Projection matrix
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