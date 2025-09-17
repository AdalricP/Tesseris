#include "swapchain.h"
#include <stdlib.h>

VkResult createSwapchain(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface,
    QueueFamilyIndices indices,
    Swapchain* swapchain
) {
    SwapChainSupportDetails support = querySwapChainSupport(physicalDevice, surface);
    
    // Pick settings
    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(support.formats, support.formatCount);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(support.presentModes, support.presentModeCount);
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
    
    freeSwapChainSupport(&support);
    return VK_SUCCESS;
}

void destroySwapchain(VkDevice device, Swapchain* swapchain) {
    if (!swapchain) return;
    
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
