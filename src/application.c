#include "application.h"
#include "sdl_window.h"
#include "vulkan/vulkan_instance.h"
#include "vulkan/vulkan_surface.h"
#include "vulkan/vulkan_depth.h"
#include "renderpass/renderpass.h"
#include "renderpass/framebuffer/framebuffer.h"
#include "renderpass/commandbuffers/commandbuffers.h"
#include "graphics_pipeline/buffer.h"
#include "sync/synchronization.h"
#include "graphics_pipeline/graphics_pipeline.h"
#include "rendering/draw_loop.h"
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
    } // Image views already created within the swapchain, don't need to worry bout creating a new func for dat


    // Find a supported depth format
    VkFormat depthFormat = VK_FORMAT_UNDEFINED; // Temporarily disable depth

    // Store depth format for later use
    app->depthFormat = depthFormat;

    // Create render pass
    result = createRenderPass(app->logicalDevice.device,
                                    app->swapchain.imageFormat,
                                    depthFormat,
                                    &app->renderPass);
    if (result != VK_SUCCESS) {
        printf("Failed to create render pass!\n");
        destroySwapchain(app->logicalDevice.device, &app->swapchain);
        destroyLogicalDevice(&app->logicalDevice);
        destroyVulkanSurface(app->vulkanInstance, app->surface);
        destroyVulkanInstance(app->vulkanInstance);
        cleanupSDLWindow(app->window);
        return -1;
    }

    // Skip depth resources for now
    app->depthImage = VK_NULL_HANDLE;
    app->depthImageMemory = VK_NULL_HANDLE;
    app->depthImageView = VK_NULL_HANDLE;

    // Validate push constant size support
    {
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(app->physicalDevice, &props);
        if (props.limits.maxPushConstantsSize < sizeof(PushConstants)) {
            printf("Device supports only %u bytes of push constants, needed %zu.\n",
                   props.limits.maxPushConstantsSize, sizeof(PushConstants));
            destroyCommandPool(app->logicalDevice.device, app->commandPool);
            destroyFramebuffers(app->logicalDevice.device, app->framebuffers, app->framebufferCount);
            destroyDepthResources(app->logicalDevice.device, app->depthImage, app->depthImageMemory, app->depthImageView);
            destroyRenderPass(app->logicalDevice.device, app->renderPass);
            destroySwapchain(app->logicalDevice.device, &app->swapchain);
            destroyLogicalDevice(&app->logicalDevice);
            destroyVulkanSurface(app->vulkanInstance, app->surface);
            destroyVulkanInstance(app->vulkanInstance);
            cleanupSDLWindow(app->window);
            return -1;
        }
    }

    // Create pipeline layouts (descriptor set layouts + pipeline layout)
    result = createPipelineLayouts(app->logicalDevice.device, &app->pipelineLayouts);
    if (result != VK_SUCCESS) {
        printf("Failed to create pipeline layouts!\n");
        // Cleanup in reverse order
        destroyCommandPool(app->logicalDevice.device, app->commandPool);
        destroyFramebuffers(app->logicalDevice.device, app->framebuffers, app->framebufferCount);
        destroyDepthResources(app->logicalDevice.device, app->depthImage, app->depthImageMemory, app->depthImageView);
        destroyRenderPass(app->logicalDevice.device, app->renderPass);
        destroySwapchain(app->logicalDevice.device, &app->swapchain);
        destroyLogicalDevice(&app->logicalDevice);
        destroyVulkanSurface(app->vulkanInstance, app->surface);
        destroyVulkanInstance(app->vulkanInstance);
        cleanupSDLWindow(app->window);
        return -1;
    }

    // Create framebuffers
    result = createFramebuffers(
        app->logicalDevice.device,
        &app->swapchain,
        app->depthImageView,
        app->renderPass,
        &app->framebuffers,
        &app->framebufferCount
    );
    if (result != VK_SUCCESS) {
        printf("Failed to create framebuffers!\n");
        destroyDepthResources(app->logicalDevice.device, app->depthImage, app->depthImageMemory, app->depthImageView);
        destroyRenderPass(app->logicalDevice.device, app->renderPass);
        destroySwapchain(app->logicalDevice.device, &app->swapchain);
        destroyLogicalDevice(&app->logicalDevice);
        destroyVulkanSurface(app->vulkanInstance, app->surface);
        destroyVulkanInstance(app->vulkanInstance);
        cleanupSDLWindow(app->window);
        return -1;
    }

    // Create command pool
    result = createCommandPool(
        app->logicalDevice.device,
        app->indices,
        &app->commandPool
    );
    if (result != VK_SUCCESS) {
        printf("Failed to create command pool!\n");
        destroyFramebuffers(app->logicalDevice.device, app->framebuffers, app->framebufferCount);
        destroyDepthResources(app->logicalDevice.device, app->depthImage, app->depthImageMemory, app->depthImageView);
        destroyRenderPass(app->logicalDevice.device, app->renderPass);
        destroySwapchain(app->logicalDevice.device, &app->swapchain);
        destroyLogicalDevice(&app->logicalDevice);
        destroyVulkanSurface(app->vulkanInstance, app->surface);
        destroyVulkanInstance(app->vulkanInstance);
        cleanupSDLWindow(app->window);
        return -1;
    }

    // Allocate command buffers (one per framebuffer)
    result = allocateCommandBuffers(
        app->logicalDevice.device,
        app->commandPool,
        app->framebufferCount,
        &app->commandBuffers
    );
    if (result != VK_SUCCESS) {
        printf("Failed to allocate command buffers!\n");
        destroyCommandPool(app->logicalDevice.device, app->commandPool);
        destroyFramebuffers(app->logicalDevice.device, app->framebuffers, app->framebufferCount);
        destroyDepthResources(app->logicalDevice.device, app->depthImage, app->depthImageMemory, app->depthImageView);
        destroyRenderPass(app->logicalDevice.device, app->renderPass);
        destroySwapchain(app->logicalDevice.device, &app->swapchain);
        destroyLogicalDevice(&app->logicalDevice);
        destroyVulkanSurface(app->vulkanInstance, app->surface);
        destroyVulkanInstance(app->vulkanInstance);
        cleanupSDLWindow(app->window);
        return -1;
    }
    app->commandBufferCount = app->framebufferCount;

    // Test buffer system
    printf("\n=== Testing Buffer System ===\n");
    
    BufferCreateInfo testBufferInfo = {0};
    testBufferInfo.size = 1024; // 1KB test buffer
    testBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    testBufferInfo.properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | 
                                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    
    Buffer testBuffer = {0};
    result = createBuffer(
        app->logicalDevice.device,
        app->physicalDevice,
        &testBufferInfo,
        &testBuffer
    );
    
    if (result == VK_SUCCESS) {
        printf("\nTest Buffer Created Successfully!\n");
        
        // Test updating the buffer
        printf("\n=== Testing Buffer Update ===\n");
        float testData[4] = {1.0f, 2.0f, 3.0f, 4.0f};
        result = updateBuffer(app->logicalDevice.device, &testBuffer, testData, sizeof(testData), 0);
        
        if (result == VK_SUCCESS) {
            printf("\nBuffer Update Test: PASSED\n");
        } else {
            printf("\nBuffer Update Test: FAILED\n");
        }
        
        // Clean up test buffer
        printf("\n=== Cleaning Up Test Buffer ===\n");
        destroyBuffer(app->logicalDevice.device, &testBuffer);
        printf("\nBuffer System: Ready\n");
    } else {
        printf("\nBuffer System Test: FAILED\n");
    }

    // Create synchronization primitives
    printf("\n=== Creating Synchronization Primitives ===\n");
    result = createFrameSync(app->logicalDevice.device, &app->frameSync);
    if (result != VK_SUCCESS) {
        printf("Failed to create frame synchronization!\n");
        destroyCommandPool(app->logicalDevice.device, app->commandPool);
        destroyFramebuffers(app->logicalDevice.device, app->framebuffers, app->framebufferCount);
        destroyDepthResources(app->logicalDevice.device, app->depthImage, app->depthImageMemory, app->depthImageView);
        destroyRenderPass(app->logicalDevice.device, app->renderPass);
        destroySwapchain(app->logicalDevice.device, &app->swapchain);
        destroyPipelineLayouts(app->logicalDevice.device, &app->pipelineLayouts);
        destroyLogicalDevice(&app->logicalDevice);
        destroyVulkanSurface(app->vulkanInstance, app->surface);
        destroyVulkanInstance(app->vulkanInstance);
        cleanupSDLWindow(app->window);
        return -1;
    }
    printf("\nFrame Synchronization: Ready\n");

    // Create graphics pipeline
    printf("\n=== Creating Graphics Pipeline ===\n");
    GraphicsPipelineConfig config = createDefaultPipelineConfig(
        app->logicalDevice.device,
        app->pipelineLayouts.pipelineLayout,
        app->renderPass,
        app->swapchain.extent
    );
    config.vertShaderPath = "shaders/basic.vert.spv";
    config.fragShaderPath = "shaders/basic.frag.spv";
    // No vertex input - triangle is hardcoded in shader
    config.vertexBindingCount = 0;
    config.vertexAttributeCount = 0;
    config.enableDepthTest = true;
    config.enableDepthWrite = true;
    config.cullMode = VK_CULL_MODE_NONE;
    
    result = createGraphicsPipeline(&config, &app->graphicsPipeline);
    
    if (result != VK_SUCCESS) {
        printf("Failed to create graphics pipeline!\n");
        destroyFrameSync(app->logicalDevice.device, &app->frameSync);
        destroyCommandPool(app->logicalDevice.device, app->commandPool);
        destroyFramebuffers(app->logicalDevice.device, app->framebuffers, app->framebufferCount);
        destroyDepthResources(app->logicalDevice.device, app->depthImage, app->depthImageMemory, app->depthImageView);
        destroyRenderPass(app->logicalDevice.device, app->renderPass);
        destroySwapchain(app->logicalDevice.device, &app->swapchain);
        destroyPipelineLayouts(app->logicalDevice.device, &app->pipelineLayouts);
        destroyLogicalDevice(&app->logicalDevice);
        destroyVulkanSurface(app->vulkanInstance, app->surface);
        destroyVulkanInstance(app->vulkanInstance);
        cleanupSDLWindow(app->window);
        return -1;
    }
    printf("\nGraphics Pipeline: Ready\n");

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
    for(int i=0; i < (int)app->swapchain.imageCount; i++) {
        printf("   View [%d]: %p\n", i, (void*)app->swapchain.imageViews[i]);
    }
    
    // Print render pass info
    printf("\nRender Pass:\n");
    printf("  Render Pass Handle: %p\n", (void*)app->renderPass);
    printf("  Color Format: %d\n", (int)app->swapchain.imageFormat);

    // Assuming you stored depth format somewhere, e.g., app->depthFormat
    printf("  Depth Format: %d\n", (int)findDepthFormat(app->physicalDevice));

    // Print framebuffer info
    if (app->framebuffers && app->framebufferCount > 0) {
        printf("\nFramebuffers:\n");
        printf("  Framebuffer Count: %u\n", app->framebufferCount);
        for (uint32_t i = 0; i < app->framebufferCount; i++) {
            printf("  Framebuffer[%u]: handle=%p, colorView=%p, depthView=%p\n", 
                   i, 
                   (void*)app->framebuffers[i],
                   (void*)app->swapchain.imageViews[i],
                   (void*)app->depthImageView);
        }
    } else {
        printf("\nFramebuffers: Not created yet\n");
    }

    // Print command pool and buffer info
    printf("\nCommand Pool & Buffers:\n");
    printf("  Command Pool Handle: %p\n", (void*)app->commandPool);
    if (app->commandBuffers && app->commandBufferCount > 0) {
        printf("  Command Buffer Count: %u\n", app->commandBufferCount);
        for (uint32_t i = 0; i < app->commandBufferCount; i++) {
            printf("  CommandBuffer[%u]: %p\n", i, (void*)app->commandBuffers[i]);
        }
    } else {
        printf("  Command Buffers: Not allocated yet\n");
    }

    // Print logical device information
    printf("\nLogical Device:\n");
    printf("  Device Handle: %p\n", (void*)app->logicalDevice.device);
    printf("  Graphics Queue: %p\n", (void*)app->logicalDevice.graphicsQueue);
    printf("  Present Queue: %p\n", (void*)app->logicalDevice.presentQueue);

    // Print pipeline layouts information
    printf("\nPipeline Layouts:\n");
    printf("  Global Set Layout (set=0): %p\n", (void*)app->pipelineLayouts.globalSetLayout);
    printf("    Binding[0]: UNIFORM_BUFFER (VS|FS) — camera + lights UBO\n");

    printf("  Material Set Layout (set=1): %p\n", (void*)app->pipelineLayouts.materialSetLayout);
    const char* materialBindingNames[4] = {"albedo", "metalness", "roughness", "normal"};
    for (int i = 0; i < 4; ++i) {
        printf("    Binding[%d]: COMBINED_IMAGE_SAMPLER (FS) — %s\n", i, materialBindingNames[i]);
    }

    printf("  Pipeline Layout: %p\n", (void*)app->pipelineLayouts.pipelineLayout);

    size_t pcSize = sizeof(PushConstants);
    printf("  Push Constants: size=%zu bytes (device max=%u), stages=VS|FS\n",
           pcSize, deviceProperties.limits.maxPushConstantsSize);

    // Print graphics pipeline system status
    printf("\nGraphics Pipeline System:\n");
    printf("  Pipeline Layout System: Ready\n");
    printf("    Descriptor Set Layouts: 2 (Global + Material)\n");
    printf("    Push Constants: %zu bytes configured\n", pcSize);
    printf("  Shader Module Loader: Ready\n");
    printf("    Supports: SPIR-V from file or memory\n");
    printf("  Graphics Pipeline Builder: Ready\n");
    printf("    Configurable: Vertex input, Depth testing, Blending, Culling\n");
    printf("    Dynamic State: Viewport + Scissor\n");
    printf("  Status: Ready to create graphics pipelines with shaders\n");

    // Print synchronization info
    printf("\nFrame Synchronization:\n");
    printf("  Image Available Semaphore: %p (%s)\n", (void*)app->frameSync.imageAvailableSemaphore, app->frameSync.imageAvailableSemaphore != VK_NULL_HANDLE ? "Valid" : "Invalid");
    printf("  Render Finished Semaphore: %p (%s)\n", (void*)app->frameSync.renderFinishedSemaphore, app->frameSync.renderFinishedSemaphore != VK_NULL_HANDLE ? "Valid" : "Invalid");
    printf("  In-Flight Fence: %p (%s)\n", (void*)app->frameSync.inFlightFence, app->frameSync.inFlightFence != VK_NULL_HANDLE ? "Valid" : "Invalid");
    printf("  Status: Ready for frame rendering\n");

    // Print graphics pipeline info
    printf("\nGraphics Pipeline Instance:\n");
    printf("  Pipeline: %p\n", (void*)app->graphicsPipeline.pipeline);
    printf("  Vertex Shader: %p\n", (void*)app->graphicsPipeline.vertShaderModule);
    printf("  Fragment Shader: %p\n", (void*)app->graphicsPipeline.fragShaderModule);
    printf("  Viewport: %dx%d\n", app->swapchain.extent.width, app->swapchain.extent.height);
    printf("  Topology: Triangle List\n");
    printf("  Depth Test: Enabled (LESS)\n");
    printf("  Blending: Disabled\n");
    printf("  Culling: Back faces (CCW front)\n");
    printf("  Status: Ready to render\n");
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
        
        draw_frame(app);
    }
}

void cleanupApplication(ApplicationContext* app) {
    // Wait for device to be idle before cleanup
    vkDeviceWaitIdle(app->logicalDevice.device);

    // Destroy graphics pipeline
    printf("\n=== Cleaning Up Graphics Pipeline ===\n");
    destroyGraphicsPipeline(app->logicalDevice.device, &app->graphicsPipeline);

    // Destroy synchronization objects
    printf("\n=== Cleaning Up Synchronization ===\n");
    destroyFrameSync(app->logicalDevice.device, &app->frameSync);

    // Destroy command buffers and command pool
    if (app->commandBuffers) {
        freeCommandBuffers(app->logicalDevice.device, app->commandPool, app->commandBuffers, app->commandBufferCount);
        app->commandBuffers = NULL;
        app->commandBufferCount = 0;
    }

    if (app->commandPool != VK_NULL_HANDLE) {
        destroyCommandPool(app->logicalDevice.device, app->commandPool);
        app->commandPool = VK_NULL_HANDLE;
    }

    if (app->framebuffers) {
        destroyFramebuffers(app->logicalDevice.device, app->framebuffers, app->framebufferCount);
        app->framebuffers = NULL;
        app->framebufferCount = 0;
    }

    destroyDepthResources(app->logicalDevice.device, app->depthImage, app->depthImageMemory, app->depthImageView);
    app->depthImage = VK_NULL_HANDLE;
    app->depthImageMemory = VK_NULL_HANDLE;
    app->depthImageView = VK_NULL_HANDLE;

    if (app->renderPass != VK_NULL_HANDLE) {
        destroyRenderPass(app->logicalDevice.device, app->renderPass);
        app->renderPass = VK_NULL_HANDLE;
    }

    destroySwapchain(app->logicalDevice.device, &app->swapchain);
    destroyPipelineLayouts(app->logicalDevice.device, &app->pipelineLayouts);
    destroyLogicalDevice(&app->logicalDevice);
    destroyVulkanSurface(app->vulkanInstance, app->surface);
    destroyVulkanInstance(app->vulkanInstance);
    cleanupSDLWindow(app->window);
}

void toggle_vsync(ApplicationContext* app, bool vsyncEnabled) {
    if (!app) return;
    if (app->vsyncEnabled == vsyncEnabled) return;
    app->vsyncEnabled = vsyncEnabled;

    // Free command buffers (they reference the old framebuffers)
    if (app->commandBuffers) {
        freeCommandBuffers(app->logicalDevice.device, app->commandPool, app->commandBuffers, app->commandBufferCount);
        app->commandBuffers = NULL;
        app->commandBufferCount = 0;
    }

    // Destroy framebuffers before recreating swapchain
    if (app->framebuffers) {
        destroyFramebuffers(app->logicalDevice.device, app->framebuffers, app->framebufferCount);
        app->framebuffers = NULL;
        app->framebufferCount = 0;
    }

    // Destroy depth resources before recreating swapchain
    destroyDepthResources(app->logicalDevice.device, app->depthImage, app->depthImageMemory, app->depthImageView);

    // Recreate swapchain
    destroySwapchain(app->logicalDevice.device, &app->swapchain);
    VkResult result = createSwapchain(app->logicalDevice.device, app->physicalDevice, app->surface, app->indices, &app->swapchain, app->vsyncEnabled);
    if (result != VK_SUCCESS) {
        printf("Failed to recreate swapchain when toggling vsync!\n");
        app->running = false;
        return;
    }

    // Recreate depth resources with new swapchain extent
    result = createDepthResources(
        app->physicalDevice,
        app->logicalDevice.device,
        app->swapchain.extent,
        app->depthFormat,
        &app->depthImage,
        &app->depthImageMemory,
        &app->depthImageView
    );
    if (result != VK_SUCCESS) {
        printf("Failed to recreate depth resources when toggling vsync!\n");
        app->running = false;
        return;
    }

    // Recreate framebuffers
    result = createFramebuffers(
        app->logicalDevice.device,
        &app->swapchain,
        app->depthImageView,
        app->renderPass,
        &app->framebuffers,
        &app->framebufferCount
    );
    if (result != VK_SUCCESS) {
        printf("Failed to recreate framebuffers when toggling vsync!\n");
        app->running = false;
        return;
    }

    // Reallocate command buffers
    result = allocateCommandBuffers(
        app->logicalDevice.device,
        app->commandPool,
        app->framebufferCount,
        &app->commandBuffers
    );
    if (result != VK_SUCCESS) {
        printf("Failed to reallocate command buffers when toggling vsync!\n");
        app->running = false;
        return;
    }
    app->commandBufferCount = app->framebufferCount;
}
