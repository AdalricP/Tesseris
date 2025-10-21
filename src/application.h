#ifndef APPLICATION_H
#define APPLICATION_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <vulkan/vulkan.h>
#include "vulkan/vulkan_physical_device.h"
#include "vulkan/vulkan_logical_device.h"
#include "swapchain/swapchain.h"
#include "graphics_pipeline/pipeline_layout.h"

/**
 * Application context structure to hold all necessary data
 */
typedef struct {
    SDL_Window* window;
    VkInstance vulkanInstance;
    VkSurfaceKHR surface;
    VkPhysicalDevice physicalDevice;
    QueueFamilyIndices indices;
    VulkanLogicalDevice logicalDevice;
    Swapchain swapchain;

    VkRenderPass renderPass;
    // Depth resources
    VkFormat depthFormat;
    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;

    // Framebuffers
    VkFramebuffer* framebuffers;
    uint32_t framebufferCount;

    // Command pool and buffers
    VkCommandPool commandPool;
    VkCommandBuffer* commandBuffers;
    uint32_t commandBufferCount;

    // Graphics pipeline layouts (descriptor set layouts + pipeline layout)
    PipelineLayouts pipelineLayouts;

    bool vsyncEnabled;
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

/**
 * Toggle VSYNC by recreating the swapchain with the requested present mode
 * @param app - Application context
 * @param vsyncEnabled - true to enable VSYNC (FIFO), false to disable (IMMEDIATE/MAILBOX)
 */
void toggle_vsync(ApplicationContext* app, bool vsyncEnabled);

#endif // APPLICATION_H
