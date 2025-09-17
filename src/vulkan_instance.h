#ifndef VULKAN_INSTANCE_H
#define VULKAN_INSTANCE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.h>

/**
 * Initialize Vulkan instance with proper extensions for the given SDL window
 * @param window - SDL window that will be used for rendering
 * @param vulkanInstance - Pointer to VkInstance to be created
 * @return 0 on success, -1 on failure
 */
int initializeVulkanInstance(SDL_Window* window, VkInstance* vulkanInstance);

/**
 * Destroy Vulkan instance and cleanup
 * @param vulkanInstance - VkInstance to destroy
 */
void destroyVulkanInstance(VkInstance vulkanInstance);

#endif // VULKAN_INSTANCE_H