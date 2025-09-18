#include "application.h"
#include "sdl_window.h"
#include "vulkan_instance.h"
#include "vulkan_surface.h"
#include <stdio.h>

int initializeApplication(ApplicationContext* app) {
    // Initialize SDL and create window
    if (initializeSDLWindow(&app->window) != 0) {
        return -1;
    }

    // Initialize Vulkan instance
    if (initializeVulkanInstance(app->window, &app->vulkanInstance) != 0) {
        cleanupSDLWindow(app->window);
        return -1;
    }

    // Create Vulkan surface
    if (createVulkanSurface(app->window, app->vulkanInstance, &app->surface) != 0) {
        destroyVulkanInstance(app->vulkanInstance);
        cleanupSDLWindow(app->window);
        return -1;
    }

    // Enumerate and select physical device
    enumeratePhysicalDevices(app->vulkanInstance, app->surface);

    app->physicalDevice = pickPhysicalDevice(app->vulkanInstance, app->surface);
    if (app->physicalDevice == VK_NULL_HANDLE) {
        printf("Failed to find a suitable GPU!\n");
        destroyVulkanSurface(app->vulkanInstance, app->surface);
        destroyVulkanInstance(app->vulkanInstance);
        cleanupSDLWindow(app->window);
        return -1;
    }

    app->indices = findQueueFamilies(app->physicalDevice, app->surface);

    // Create logical device
    VkResult result = createLogicalDevice(app->physicalDevice, app->indices, &app->logicalDevice);
    if (result != VK_SUCCESS) {
        printf("Failed to create logical device!\n");
        destroyVulkanSurface(app->vulkanInstance, app->surface);
        destroyVulkanInstance(app->vulkanInstance);
        cleanupSDLWindow(app->window);
        return -1;
    }

    // default VSYNC enabled
    app->vsyncEnabled = true;

    // Create swapchain
    result = createSwapchain(app->logicalDevice.device, app->physicalDevice, app->surface, app->indices, &app->swapchain, app->vsyncEnabled);
    if (result != VK_SUCCESS) {
        printf("Failed to create swapchain!\n");
        destroyLogicalDevice(&app->logicalDevice);
        destroyVulkanSurface(app->vulkanInstance, app->surface);
        destroyVulkanInstance(app->vulkanInstance);
        cleanupSDLWindow(app->window);
        return -1;
    }

    app->running = true;

    return 0;
}

void printDeviceInfo(ApplicationContext* app) {
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(app->physicalDevice, &deviceProperties);
    
    printf("\nSelected device: %s\n", deviceProperties.deviceName);
    printf("Graphics Support: %s\n", app->indices.hasGraphics ? "Yes" : "No");
    printf("Present Support: %s\n", app->indices.hasPresent ? "Yes" : "No");

    // Query and print swapchain support details
    SwapChainSupportDetails sc = querySwapChainSupport(app->physicalDevice, app->surface);
    VkSurfaceFormatKHR chosenFormat = chooseSwapSurfaceFormat(sc.formats, sc.formatCount);
    VkPresentModeKHR chosenPresentMode = chooseSwapPresentMode(sc.presentModes, sc.presentModeCount);

    printf("\nSwapchain Support:\n");
    printf("  Capabilities: minImages=%u maxImages=%u currentExtent=%ux%u\n",
           sc.capabilities.minImageCount,
           sc.capabilities.maxImageCount,
           sc.capabilities.currentExtent.width,
           sc.capabilities.currentExtent.height);
    printf("  Available Formats: %u\n", sc.formatCount);
    printf("  Available Present Modes: %u\n", sc.presentModeCount);
    printf("  Chosen Format: %d (colorSpace=%d)\n", (int)chosenFormat.format, (int)chosenFormat.colorSpace);
    printf("  Chosen Present Mode: %d\n", (int)chosenPresentMode);
    printf("  VSYNC: %s\n", app->vsyncEnabled ? "On" : "Off");

    freeSwapChainSupport(&sc);

    // Print actual created swapchain information
    printf("\nCreated Swapchain:\n");
    printf("  Swapchain Handle: %p\n", (void*)app->swapchain.swapchain);
    printf("  Image Count: %u\n", app->swapchain.imageCount);
    printf("  Image Format: %d\n", (int)app->swapchain.imageFormat);
    printf("  Extent: %ux%u\n", app->swapchain.extent.width, app->swapchain.extent.height);
    printf("  Images: %p\n", (void*)app->swapchain.images);
    printf("  Image Views: %p\n", (void*)app->swapchain.imageViews);
    for(int i=0;i<3;i++)
    {
        printf("   View [%d]:", i);
        printf("   %p\n",(void*)app->swapchain.imageViews[i]);
    }
    
    
    // Print logical device information
    printf("\nLogical Device:\n");
    printf("  Device Handle: %p\n", (void*)app->logicalDevice.device);
    printf("  Graphics Queue: %p\n", (void*)app->logicalDevice.graphicsQueue);
    printf("  Present Queue: %p\n", (void*)app->logicalDevice.presentQueue);
}

void handleEvents(ApplicationContext* app) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                app->running = false;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    app->running = false;
                }
                break;
        }
    }
}

void runApplication(ApplicationContext* app) {
    while (app->running) {
        handleEvents(app);
        
        // Handle Rendering Here
        // TODO: Add rendering logic when implementing graphics pipeline
    }
}

void cleanupApplication(ApplicationContext* app) {
    destroySwapchain(app->logicalDevice.device, &app->swapchain);
    destroyLogicalDevice(&app->logicalDevice);
    destroyVulkanSurface(app->vulkanInstance, app->surface);
    destroyVulkanInstance(app->vulkanInstance);
    cleanupSDLWindow(app->window);
}

void toggle_vsync(ApplicationContext* app, bool vsyncEnabled) {
    if (!app) return;
    if (app->vsyncEnabled == vsyncEnabled) return;
    app->vsyncEnabled = vsyncEnabled;

    // Recreate swapchain
    destroySwapchain(app->logicalDevice.device, &app->swapchain);
    VkResult result = createSwapchain(app->logicalDevice.device, app->physicalDevice, app->surface, app->indices, &app->swapchain, app->vsyncEnabled);
    if (result != VK_SUCCESS) {
        printf("Failed to recreate swapchain when toggling vsync!\n");
        app->running = false;
    }
}
