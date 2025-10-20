#ifndef COMMANDBUFFERS_H
#define COMMANDBUFFERS_H

#include <vulkan/vulkan.h>
#include <stdint.h>
#include "../../vulkan/vulkan_physical_device.h"

// Create a command pool for allocating command buffers
VkResult createCommandPool(
    VkDevice device,
    QueueFamilyIndices queueFamilyIndices,
    VkCommandPool* commandPool
);

// Allocate command buffers from the command pool
// Typically one per swapchain image (framebuffer)
VkResult allocateCommandBuffers(
    VkDevice device,
    VkCommandPool commandPool,
    uint32_t count,
    VkCommandBuffer** outCommandBuffers
);

// Free command buffers
void freeCommandBuffers(
    VkDevice device,
    VkCommandPool commandPool,
    VkCommandBuffer* commandBuffers,
    uint32_t count
);

// Destroy command pool (this also frees all command buffers allocated from it)
void destroyCommandPool(
    VkDevice device,
    VkCommandPool commandPool
);

#endif // COMMANDBUFFERS_H
