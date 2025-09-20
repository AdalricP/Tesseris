#ifndef VULKAN_LOGICAL_DEVICE_H
#define VULKAN_LOGICAL_DEVICE_H

#include <vulkan/vulkan.h>
#include "vulkan_physical_device.h"

typedef struct {
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
} VulkanLogicalDevice;

VkResult createLogicalDevice(
    VkPhysicalDevice physicalDevice,
    QueueFamilyIndices indices,
    VulkanLogicalDevice* logicalDevice
);

void destroyLogicalDevice(VulkanLogicalDevice* logicalDevice);

#endif // VULKAN_LOGICAL_DEVICE_H
