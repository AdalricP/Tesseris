#ifndef VULKAN_DEVICE_H
#define VULKAN_DEVICE_H

#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include <stdbool.h>

typedef struct {
    uint32_t graphicsFamily;
    uint32_t presentFamily;
    bool hasGraphics;
    bool hasPresent;
} QueueFamilyIndices;

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
VkPhysicalDevice pickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface);
void enumeratePhysicalDevices(VkInstance instance, VkSurfaceKHR surface);

typedef struct {
    VkSurfaceCapabilitiesKHR capabilities;
    VkSurfaceFormatKHR* formats;
    uint32_t formatCount;
    VkPresentModeKHR* presentModes;
    uint32_t presentModeCount;
} SwapChainSupportDetails;

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
void freeSwapChainSupport(SwapChainSupportDetails* details);
VkSurfaceFormatKHR chooseSwapSurfaceFormat(const VkSurfaceFormatKHR* formats, uint32_t formatCount);
VkPresentModeKHR chooseSwapPresentMode(const VkPresentModeKHR* presentModes, uint32_t presentModeCount);

#endif // VULKAN_DEVICE_H
