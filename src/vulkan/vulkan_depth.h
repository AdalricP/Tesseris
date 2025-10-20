#ifndef VULKAN_DEPTH_H
#define VULKAN_DEPTH_H

#include <vulkan/vulkan.h>

// Find a suitable memory type for the given requirements
uint32_t findMemoryType(
    VkPhysicalDevice physicalDevice,
    uint32_t typeFilter,
    VkMemoryPropertyFlags properties
);

// Create a depth image, allocate memory, bind it, and create an image view
VkResult createDepthResources(
    VkPhysicalDevice physicalDevice,
    VkDevice device,
    VkExtent2D extent,
    VkFormat depthFormat,
    VkImage* depthImage,
    VkDeviceMemory* depthImageMemory,
    VkImageView* depthImageView
);

// Destroy depth resources
void destroyDepthResources(
    VkDevice device,
    VkImage depthImage,
    VkDeviceMemory depthImageMemory,
    VkImageView depthImageView
);

#endif // VULKAN_DEPTH_H
