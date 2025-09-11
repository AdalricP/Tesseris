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

#endif // VULKAN_DEVICE_H
