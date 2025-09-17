#include "vulkan_surface.h"
#include <stdio.h>

int createVulkanSurface(SDL_Window* window, VkInstance vulkanInstance, VkSurfaceKHR* surface) {
    if (!SDL_Vulkan_CreateSurface(window, vulkanInstance, surface)) {
        printf("Failed to create Vulkan surface! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }
    return 0;
}

void destroyVulkanSurface(VkInstance vulkanInstance, VkSurfaceKHR surface) {
    if (surface != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(vulkanInstance, surface, NULL);
    }
}