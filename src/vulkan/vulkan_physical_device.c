#include "vulkan_physical_device.h"
#include <stdio.h>
#include <stdlib.h>

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {
    QueueFamilyIndices indices = {0};
    indices.graphicsFamily = UINT32_MAX;
    indices.presentFamily = UINT32_MAX;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);

    VkQueueFamilyProperties* queueFamilies = malloc(queueFamilyCount * sizeof(VkQueueFamilyProperties));
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);

    for (uint32_t i = 0; i < queueFamilyCount; i++) {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
            indices.hasGraphics = true;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        if (presentSupport) {
            indices.presentFamily = i;
            indices.hasPresent = true;
        }
    }

    free(queueFamilies);
    return indices;
}

VkPhysicalDevice pickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface) {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);
    
    if (deviceCount == 0) {
        printf("Failed to find GPUs with Vulkan support!\n");
        return VK_NULL_HANDLE;
    }

    VkPhysicalDevice* devices = malloc(sizeof(VkPhysicalDevice) * deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices);
    
    // ideally we try to find a device with queue family that supports both
    VkPhysicalDevice selectedDevice = VK_NULL_HANDLE;
    for (uint32_t i = 0; i < deviceCount; i++) {
        QueueFamilyIndices indices = findQueueFamilies(devices[i], surface);

        // checking for queue family
        if (indices.hasGraphics && indices.hasPresent && 
            indices.graphicsFamily == indices.presentFamily) {
            selectedDevice = devices[i];
            break;
        }
    }

    // otherwise we'll just go with two queue families
    if (selectedDevice == VK_NULL_HANDLE) {
        for (uint32_t i = 0; i < deviceCount; i++) {
            QueueFamilyIndices indices = findQueueFamilies(devices[i], surface);
            if (indices.hasGraphics && indices.hasPresent) {
                selectedDevice = devices[i];
                break;
            }
        }
    }

    free(devices);
    return selectedDevice;
}

void enumeratePhysicalDevices(VkInstance instance, VkSurfaceKHR surface) {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);
    
    if (deviceCount == 0) {
        printf("Failed to find GPUs with Vulkan support!\n");
        return;
    }

    printf("Found %d device(s) with Vulkan support:\n", deviceCount);

    VkPhysicalDevice* devices = malloc(sizeof(VkPhysicalDevice) * deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices);

    for (uint32_t i = 0; i < deviceCount; i++) {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(devices[i], &deviceProperties);
        QueueFamilyIndices indices = findQueueFamilies(devices[i], surface);
        printf("%d. Device name: %s\n", i + 1, deviceProperties.deviceName);
        printf("   Graphics Queue Family: %d\n", indices.graphicsFamily);
        printf("   Present Queue Family: %d\n", indices.presentFamily);
        if (indices.hasGraphics && indices.hasPresent) {
            if (indices.graphicsFamily == indices.presentFamily) {
                printf("   Optimal: Same queue family (%d) supports both graphics and present!\n", 
                       indices.graphicsFamily);
            } else {
                printf("   Using separate queue families for graphics (%d) and present (%d)\n", 
                       indices.graphicsFamily, indices.presentFamily);
            }
        }
    }

    free(devices);
}

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
    SwapChainSupportDetails details = {0};

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    details.formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &details.formatCount, NULL);
    if (details.formatCount > 0) {
        details.formats = (VkSurfaceFormatKHR*)malloc(details.formatCount * sizeof(VkSurfaceFormatKHR));
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &details.formatCount, details.formats);
    }

    details.presentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &details.presentModeCount, NULL);
    if (details.presentModeCount > 0) {
        details.presentModes = (VkPresentModeKHR*)malloc(details.presentModeCount * sizeof(VkPresentModeKHR));
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &details.presentModeCount, details.presentModes);
    }

    return details;
}

void freeSwapChainSupport(SwapChainSupportDetails* details) {
    if (!details) return;
    if (details->formats) {
        free(details->formats);
        details->formats = NULL;
        details->formatCount = 0;
    }
    if (details->presentModes) {
        free(details->presentModes);
        details->presentModes = NULL;
        details->presentModeCount = 0;
    }
}

VkSurfaceFormatKHR chooseSwapSurfaceFormat(const VkSurfaceFormatKHR* formats, uint32_t formatCount) {
    // Prefer B8G8R8A8_SRGB + SRGB_NONLINEAR
    for (uint32_t i = 0; i < formatCount; i++) {
        if (formats[i].format == VK_FORMAT_B8G8R8A8_SRGB &&
            formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return formats[i];
        }
    }
    // Fallback to the first available
    return formatCount > 0 ? formats[0] : (VkSurfaceFormatKHR){0};
}

VkPresentModeKHR chooseSwapPresentMode(const VkPresentModeKHR* presentModes, uint32_t presentModeCount) {
    // Prefer MAILBOX, else FIFO
    for (uint32_t i = 0; i < presentModeCount; i++) {
        if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            return VK_PRESENT_MODE_MAILBOX_KHR;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}
