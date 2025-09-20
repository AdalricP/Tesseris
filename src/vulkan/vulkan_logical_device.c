#include "vulkan_logical_device.h"
#include <stdlib.h>

VkResult createLogicalDevice(
    VkPhysicalDevice physicalDevice,
    QueueFamilyIndices indices,
    VulkanLogicalDevice* logicalDevice
) {
    // Create queue create infos for unique queue families
    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfos[2] = {0};
    uint32_t queueCreateInfoCount = 0;

    // Always need graphics queue
    queueCreateInfos[queueCreateInfoCount] = (VkDeviceQueueCreateInfo) {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = indices.graphicsFamily,
        .queueCount = 1,
        .pQueuePriorities = &queuePriority
    };
    queueCreateInfoCount++;

    // Add present queue if it's different from graphics queue
    if (indices.graphicsFamily != indices.presentFamily) {
        queueCreateInfos[queueCreateInfoCount] = (VkDeviceQueueCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = indices.presentFamily,
            .queueCount = 1,
            .pQueuePriorities = &queuePriority
        };
        queueCreateInfoCount++;
    }

    // Device features we'll be using
    VkPhysicalDeviceFeatures deviceFeatures = {0};
    // Add required features here as we need them

    // Required extensions
    const char* deviceExtensions[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    // Create the logical device
    VkDeviceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = queueCreateInfoCount,
        .pQueueCreateInfos = queueCreateInfos,
        .pEnabledFeatures = &deviceFeatures,
        .enabledExtensionCount = 1,
        .ppEnabledExtensionNames = deviceExtensions,
    };

    // Create the logical device
    VkResult result = vkCreateDevice(physicalDevice, &createInfo, NULL, &logicalDevice->device);
    if (result != VK_SUCCESS) {
        return result;
    }

    // Get queue handles
    vkGetDeviceQueue(logicalDevice->device, indices.graphicsFamily, 0, &logicalDevice->graphicsQueue);
    vkGetDeviceQueue(logicalDevice->device, indices.presentFamily, 0, &logicalDevice->presentQueue);

    return VK_SUCCESS;
}

void destroyLogicalDevice(VulkanLogicalDevice* logicalDevice) {
    if (logicalDevice->device != VK_NULL_HANDLE) {
        vkDestroyDevice(logicalDevice->device, NULL);
        logicalDevice->device = VK_NULL_HANDLE;
    }
}
