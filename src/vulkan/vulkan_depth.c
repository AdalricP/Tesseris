#include "vulkan_depth.h"
#include <stdio.h>

uint32_t findMemoryType(
    VkPhysicalDevice physicalDevice,
    uint32_t typeFilter,
    VkMemoryPropertyFlags properties
) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    printf("Failed to find suitable memory type!\n");
    return 0;
}

VkResult createDepthResources(
    VkPhysicalDevice physicalDevice,
    VkDevice device,
    VkExtent2D extent,
    VkFormat depthFormat,
    VkImage* depthImage,
    VkDeviceMemory* depthImageMemory,
    VkImageView* depthImageView
) {
    // Create depth image
    VkImageCreateInfo imageInfo = {0};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = extent.width;
    imageInfo.extent.height = extent.height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = depthFormat;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult result = vkCreateImage(device, &imageInfo, NULL, depthImage);
    if (result != VK_SUCCESS) {
        printf("Failed to create depth image!\n");
        return result;
    }

    // Allocate memory for depth image
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device, *depthImage, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {0};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(
        physicalDevice,
        memRequirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );

    result = vkAllocateMemory(device, &allocInfo, NULL, depthImageMemory);
    if (result != VK_SUCCESS) {
        printf("Failed to allocate depth image memory!\n");
        vkDestroyImage(device, *depthImage, NULL);
        return result;
    }

    vkBindImageMemory(device, *depthImage, *depthImageMemory, 0);

    // Create depth image view
    VkImageViewCreateInfo viewInfo = {0};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = *depthImage;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = depthFormat;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    result = vkCreateImageView(device, &viewInfo, NULL, depthImageView);
    if (result != VK_SUCCESS) {
        printf("Failed to create depth image view!\n");
        vkFreeMemory(device, *depthImageMemory, NULL);
        vkDestroyImage(device, *depthImage, NULL);
        return result;
    }

    return VK_SUCCESS;
}

void destroyDepthResources(
    VkDevice device,
    VkImage depthImage,
    VkDeviceMemory depthImageMemory,
    VkImageView depthImageView
) {
    if (depthImageView != VK_NULL_HANDLE) {
        vkDestroyImageView(device, depthImageView, NULL);
    }
    if (depthImageMemory != VK_NULL_HANDLE) {
        vkFreeMemory(device, depthImageMemory, NULL);
    }
    if (depthImage != VK_NULL_HANDLE) {
        vkDestroyImage(device, depthImage, NULL);
    }
}
