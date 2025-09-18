#include "swapchain.h"
#include <stdlib.h>

static VkPresentModeKHR choosePresentModeWithVsync(const VkPresentModeKHR* modes, uint32_t count, bool vsyncEnabled) {
    if (vsyncEnabled) {
        return VK_PRESENT_MODE_FIFO_KHR;
    }
    for (uint32_t i = 0; i < count; i++) {
        if (modes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR) return VK_PRESENT_MODE_IMMEDIATE_KHR;
    }
    for (uint32_t i = 0; i < count; i++) {
        if (modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) return VK_PRESENT_MODE_MAILBOX_KHR;
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkResult createSwapchain(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface,
    QueueFamilyIndices indices,
    Swapchain* swapchain,
    bool vsyncEnabled
) {
    SwapChainSupportDetails support = querySwapChainSupport(physicalDevice, surface);
    
    // Pick settings
    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(support.formats, support.formatCount);
    VkPresentModeKHR presentMode = choosePresentModeWithVsync(support.presentModes, support.presentModeCount, vsyncEnabled);
    VkExtent2D extent = support.capabilities.currentExtent;
    
    // Image count: minImageCount + 1
    uint32_t imageCount = support.capabilities.minImageCount + 1;
    if (support.capabilities.maxImageCount > 0 && imageCount > support.capabilities.maxImageCount) {
        imageCount = support.capabilities.maxImageCount;
    }
    
    // Create swapchain
    VkSwapchainCreateInfoKHR createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    
    // Queue family indices
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily, indices.presentFamily};
    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = NULL;
    }
    
    createInfo.preTransform = support.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;
    
    VkResult result = vkCreateSwapchainKHR(device, &createInfo, NULL, &swapchain->swapchain);
    if (result != VK_SUCCESS) {
        freeSwapChainSupport(&support);
        return result;
    }
    
    // Get swapchain images
    vkGetSwapchainImagesKHR(device, swapchain->swapchain, &swapchain->imageCount, NULL);
    swapchain->images = (VkImage*)malloc(swapchain->imageCount * sizeof(VkImage));
    vkGetSwapchainImagesKHR(device, swapchain->swapchain, &swapchain->imageCount, swapchain->images);
    
    swapchain->imageFormat = surfaceFormat.format;
    swapchain->extent = extent;
    
    // Create image views for each swapchain image
    swapchain->imageViews = (VkImageView*)malloc(swapchain->imageCount * sizeof(VkImageView));
    for (uint32_t i = 0; i < swapchain->imageCount; i++) {
        VkImageViewCreateInfo viewCreateInfo = {0};
        viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewCreateInfo.image = swapchain->images[i];
        viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewCreateInfo.format = swapchain->imageFormat;
        viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewCreateInfo.subresourceRange.baseMipLevel = 0;
        viewCreateInfo.subresourceRange.levelCount = 1;
        viewCreateInfo.subresourceRange.baseArrayLayer = 0;
        viewCreateInfo.subresourceRange.layerCount = 1;
        
        VkResult viewResult = vkCreateImageView(device, &viewCreateInfo, NULL, &swapchain->imageViews[i]);
        if (viewResult != VK_SUCCESS) {
            // Clean up already created image views
            for (uint32_t j = 0; j < i; j++) {
                vkDestroyImageView(device, swapchain->imageViews[j], NULL);
            }
            free(swapchain->imageViews);
            free(swapchain->images);
            vkDestroySwapchainKHR(device, swapchain->swapchain, NULL);
            freeSwapChainSupport(&support);
            return viewResult;
        }
    }
    
    freeSwapChainSupport(&support);
    return VK_SUCCESS;
}

void destroySwapchain(VkDevice device, Swapchain* swapchain) {
    if (!swapchain) return;
    
    // Destroy image views
    if (swapchain->imageViews) {
        for (uint32_t i = 0; i < swapchain->imageCount; i++) {
            vkDestroyImageView(device, swapchain->imageViews[i], NULL);
        }
        free(swapchain->imageViews);
        swapchain->imageViews = NULL;
    }
    
    if (swapchain->images) {
        free(swapchain->images);
        swapchain->images = NULL;
    }
    
    if (swapchain->swapchain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(device, swapchain->swapchain, NULL);
        swapchain->swapchain = VK_NULL_HANDLE;
    }
    
    swapchain->imageCount = 0;
}
