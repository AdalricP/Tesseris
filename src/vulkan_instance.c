#include "vulkan_instance.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>

int initializeVulkanInstance(SDL_Window* window, VkInstance* vulkanInstance) {
    VkApplicationInfo appInfo = {0};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Tesseris";
    appInfo.apiVersion = VK_API_VERSION_1_0;
    
    VkInstanceCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    
    unsigned int extensionCount;
    if (!SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, NULL)) {
        printf("Failed to get SDL Vulkan instance extensions!\n");
        return -1;
    }
    
    #ifdef PLATFORM_MACOS
        const char** extensions = malloc((extensionCount + 1) * sizeof(const char*));
        if (!SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, extensions)) {
            free(extensions);
            return -1;
        }
        extensions[extensionCount] = "VK_KHR_portability_enumeration";
        createInfo.enabledExtensionCount = extensionCount + 1;
        createInfo.ppEnabledExtensionNames = extensions;
        createInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    #else
        const char** extensions = malloc(extensionCount * sizeof(const char*));
        if (!SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, extensions)) {
            free(extensions);
            return -1;
        }
        createInfo.enabledExtensionCount = extensionCount;
        createInfo.ppEnabledExtensionNames = extensions;
        createInfo.flags = 0;
    #endif
    
    VkResult result = vkCreateInstance(&createInfo, NULL, vulkanInstance);
    if (result != VK_SUCCESS) {
        printf("Failed to create Vulkan instance! Error: %d\n", result);
        free(extensions);
        return -1;
    }
    
    free(extensions);
    return 0;
}

void destroyVulkanInstance(VkInstance vulkanInstance) {
    if (vulkanInstance != VK_NULL_HANDLE) {
        vkDestroyInstance(vulkanInstance, NULL);
    }
}