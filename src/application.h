#ifndef APPLICATION_H
#define APPLICATION_H

#include <SDL2/SDL.h>
#include <vulkan/vulkan.h>
#include "vulkan_physical_device.h"

/**
 * Application context structure to hold all necessary data
 */
typedef struct {
    SDL_Window* window;
    VkInstance vulkanInstance;
    VkSurfaceKHR surface;
    VkPhysicalDevice physicalDevice;
    QueueFamilyIndices indices;
    bool running;
} ApplicationContext;

/**
 * Initialize the application context and all Vulkan components
 * @param app - Pointer to ApplicationContext to initialize
 * @return 0 on success, -1 on failure
 */
int initializeApplication(ApplicationContext* app);

/**
 * Print device and swapchain information
 * @param app - Application context containing device info
 */
void printDeviceInfo(ApplicationContext* app);

/**
 * Handle SDL events and update application state
 * @param app - Application context to update
 */
void handleEvents(ApplicationContext* app);

/**
 * Main application loop - handles events and rendering
 * @param app - Application context to run
 */
void runApplication(ApplicationContext* app);

/**
 * Cleanup all application resources
 * @param app - Application context to cleanup
 */
void cleanupApplication(ApplicationContext* app);

#endif // APPLICATION_H
