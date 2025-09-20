#ifndef RENDERPASS_H
#define RENDERPASS_H

#include <vulkan/vulkan.h>

// Create a render pass with one color attachment + one depth attachment
VkResult createRenderPass(
    VkDevice device,
    VkFormat colorFormat,
    VkFormat depthFormat,
    VkRenderPass* renderPass
);

VkFormat findDepthFormat(
    VkPhysicalDevice physicalDevice
);

VkFormat findSupportedFormat(
    VkPhysicalDevice physicalDevice,
    const VkFormat* candidates,
    uint32_t candidateCount,
    VkImageTiling tiling,
    VkFormatFeatureFlags features
);

// Destroy the render pass
void destroyRenderPass(
    VkDevice device,
    VkRenderPass renderPass
);

#endif // RENDERPASS_H
