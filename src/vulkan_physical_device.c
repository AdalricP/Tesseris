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
