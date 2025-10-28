#include "vulkan_instance.h"
#include "../common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global debug messenger
static VkDebugUtilsMessengerEXT debugMessenger;

// Debug callback function
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {
    (void)pUserData;
    (void)messageType;

    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        printf("Vulkan Debug: %s\n", pCallbackData->pMessage);
    }
    return VK_FALSE;
}

// Function to create debug messenger
VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != NULL) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

// Function to destroy debug messenger
void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != NULL) {
        func(instance, debugMessenger, pAllocator);
    }
}

// Check if validation layer is available
static int checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, NULL);

    VkLayerProperties* availableLayers = malloc(layerCount * sizeof(VkLayerProperties));
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);

    int layerFound = 0;
    for (uint32_t i = 0; i < layerCount; i++) {
        if (strcmp(availableLayers[i].layerName, "VK_LAYER_KHRONOS_validation") == 0) {
            layerFound = 1;
            break;
        }
    }

    free(availableLayers);
    return layerFound;
}

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
    
    // Enable validation layers if available
    const char* validationLayers[] = {"VK_LAYER_KHRONOS_validation"};
    int enableValidationLayers = checkValidationLayerSupport();
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = 1;
        createInfo.ppEnabledLayerNames = validationLayers;
        printf("Vulkan validation layers enabled\n");
    } else {
        createInfo.enabledLayerCount = 0;
        printf("Vulkan validation layers not available\n");
    }
    
    #ifdef PLATFORM_MACOS
        const char** extensions = malloc((extensionCount + 2) * sizeof(const char*)); // +2 for portability and debug utils
        if (!SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, extensions)) {
            free(extensions);
            return -1;
        }
        extensions[extensionCount] = "VK_KHR_portability_enumeration";
        extensions[extensionCount + 1] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
        createInfo.enabledExtensionCount = extensionCount + 2;
        createInfo.ppEnabledExtensionNames = extensions;
        createInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    #else
        const char** extensions = malloc((extensionCount + 1) * sizeof(const char*)); // +1 for debug utils
        if (!SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, extensions)) {
            free(extensions);
            return -1;
        }
        extensions[extensionCount] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
        createInfo.enabledExtensionCount = extensionCount + 1;
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
    
    // Create debug messenger if validation layers are enabled
    if (enableValidationLayers) {
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {0};
        debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | 
                                          VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
                                          VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | 
                                      VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | 
                                      VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugCreateInfo.pfnUserCallback = debugCallback;
        debugCreateInfo.pUserData = NULL;
        
        if (createDebugUtilsMessengerEXT(*vulkanInstance, &debugCreateInfo, NULL, &debugMessenger) != VK_SUCCESS) {
            printf("Failed to set up debug messenger!\n");
        } else {
            printf("Vulkan debug messenger created\n");
        }
    }
    
    return 0;
}

void destroyVulkanInstance(VkInstance vulkanInstance) {
    if (vulkanInstance != VK_NULL_HANDLE) {
        destroyDebugUtilsMessengerEXT(vulkanInstance, debugMessenger, NULL);
        vkDestroyInstance(vulkanInstance, NULL);
    }
}
