#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H

#include <vulkan/vulkan.h>
#include "../graphics_pipeline/buffer.h"
#include "../math/vector.h"

/**
 * Vertex structure for 3D rendering
 * Position: 3D coordinates (vec3)
 * Color: RGB color values (vec3)
 */
typedef struct {
    vec3 position;  // vec3 position
    vec3 color;     // vec3 color
} Vertex;

/**
 * Create a vertex buffer for storing vertex data
 * 
 * @param device - VkDevice handle
 * @param physicalDevice - VkPhysicalDevice for querying memory properties
 * @param size - Size of the buffer in bytes
 * @param outBuffer - Output buffer handle
 * @return VK_SUCCESS on success, error code otherwise
 */
VkResult createVertexBuffer(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    VkDeviceSize size,
    Buffer* outBuffer
);

/**
 * Update vertex buffer with cube vertex data
 * Creates a colored cube with 6 faces
 * 
 * @param device - VkDevice handle
 * @param buffer - Vertex buffer to update
 * @return VK_SUCCESS on success, error code otherwise
 */
VkResult updateVertexBufferWithCube(
    VkDevice device,
    Buffer* buffer
);

#endif // VERTEX_BUFFER_H