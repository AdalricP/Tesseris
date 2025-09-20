#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#include <vulkan/vulkan.h>
#include <stdbool.h>
#include "../vulkan/vulkan_physical_device.h"

typedef struct {
    VkSwapchainKHR swapchain;
    VkImage* images;
    VkImageView* imageViews;
    uint32_t imageCount;
    VkFormat imageFormat;
    VkExtent2D extent;
} Swapchain;

VkResult createSwapchain(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface,
    QueueFamilyIndices indices,
    Swapchain* swapchain,
    bool vsyncEnabled
);

void destroySwapchain(VkDevice device, Swapchain* swapchain);

#endif // SWAPCHAIN_H
