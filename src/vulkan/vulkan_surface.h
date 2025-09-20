#ifndef VULKAN_SURFACE_H
#define VULKAN_SURFACE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.h>

/**
 * Create Vulkan surface for the given SDL window
 * @param window - SDL window to create surface for
 * @param vulkanInstance - VkInstance to use for surface creation
 * @param surface - Pointer to VkSurfaceKHR to store created surface
 * @return 0 on success, -1 on failure
 */
int createVulkanSurface(SDL_Window* window, VkInstance vulkanInstance, VkSurfaceKHR* surface);

/**
 * Destroy Vulkan surface
 * @param vulkanInstance - VkInstance used to create the surface
 * @param surface - VkSurfaceKHR to destroy
 */
void destroyVulkanSurface(VkInstance vulkanInstance, VkSurfaceKHR surface);

#endif // VULKAN_SURFACE_H
