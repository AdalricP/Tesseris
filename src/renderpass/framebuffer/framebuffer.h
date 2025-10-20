#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <vulkan/vulkan.h>
#include <stdint.h>
#include "../../swapchain/swapchain.h"

// Create one VkFramebuffer per swapchain image view.
// depthImageView must be a valid VkImageView of a depth image matching swapchain extent.
// On success, outFramebuffers will be allocated with `count` elements.
VkResult createFramebuffers(
    VkDevice device,
    const Swapchain* swapchain,
    VkImageView depthImageView,
    VkRenderPass renderPass,
    VkFramebuffer** outFramebuffers,
    uint32_t* outCount
);

// Destroy created framebuffers and free the allocated array.
void destroyFramebuffers(
    VkDevice device,
    VkFramebuffer* framebuffers,
    uint32_t count
);

#endif // FRAMEBUFFER_H


