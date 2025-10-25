#ifndef BUFFER_H
#define BUFFER_H

#include <vulkan/vulkan.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * Buffer wrapper with memory info
 */
typedef struct {
    VkBuffer buffer;
    VkDeviceMemory memory;
    VkDeviceSize size;
    void* mapped;  // Non-NULL if persistently mapped
} Buffer;

/**
 * Buffer creation configuration
 */
typedef struct {
    VkDeviceSize size;
    VkBufferUsageFlags usage;         // e.g., VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
    VkMemoryPropertyFlags properties; // e.g., VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
} BufferCreateInfo;

/**
 * Create a buffer with memory allocation
 * 
 * @param device - VkDevice handle
 * @param physicalDevice - VkPhysicalDevice for querying memory properties
 * @param createInfo - Buffer configuration
 * @param outBuffer - Output buffer handle
 * @return VK_SUCCESS on success, error code otherwise
 */
VkResult createBuffer(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    const BufferCreateInfo* createInfo,
    Buffer* outBuffer
);

/**
 * Copy data into a buffer (handles mapping/unmapping)
 * 
 * @param device - VkDevice handle
 * @param buffer - Target buffer
 * @param data - Source data pointer
 * @param size - Number of bytes to copy
 * @param offset - Offset in buffer to start writing
 * @return VK_SUCCESS on success, error code otherwise
 */
VkResult updateBuffer(
    VkDevice device,
    Buffer* buffer,
    const void* data,
    VkDeviceSize size,
    VkDeviceSize offset
);

/**
 * Map buffer memory for reading/writing
 * 
 * @param device - VkDevice handle
 * @param buffer - Buffer to map
 * @param outMappedData - Pointer to mapped memory
 * @return VK_SUCCESS on success, error code otherwise
 */
VkResult mapBuffer(
    VkDevice device,
    Buffer* buffer,
    void** outMappedData
);

/**
 * Unmap buffer memory
 * 
 * @param device - VkDevice handle
 * @param buffer - Buffer to unmap
 */
void unmapBuffer(
    VkDevice device,
    Buffer* buffer
);

/**
 * Copy data from one buffer to another using a command buffer
 * 
 * @param device - VkDevice handle
 * @param commandPool - Command pool for temporary command buffer
 * @param queue - Queue to submit copy command to
 * @param srcBuffer - Source buffer
 * @param dstBuffer - Destination buffer
 * @param size - Number of bytes to copy
 * @return VK_SUCCESS on success, error code otherwise
 */
VkResult copyBuffer(
    VkDevice device,
    VkCommandPool commandPool,
    VkQueue queue,
    Buffer* srcBuffer,
    Buffer* dstBuffer,
    VkDeviceSize size
);

/**
 * Destroy buffer and free memory
 * 
 * @param device - VkDevice handle
 * @param buffer - Buffer to destroy
 */
void destroyBuffer(
    VkDevice device,
    Buffer* buffer
);

#endif // BUFFER_H
