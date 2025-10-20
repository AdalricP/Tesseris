#include "framebuffer.h"
#include <stdlib.h>

VkResult createFramebuffers(
    VkDevice device,
    const Swapchain* swapchain,
    VkImageView depthImageView,
    VkRenderPass renderPass,
    VkFramebuffer** outFramebuffers,
    uint32_t* outCount
) {
    if (!device || !swapchain || !swapchain->imageViews || swapchain->imageCount == 0 ||
        !renderPass || !outFramebuffers || !outCount) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    uint32_t count = swapchain->imageCount;
    VkFramebuffer* framebuffers = (VkFramebuffer*)malloc(sizeof(VkFramebuffer) * count);
    if (!framebuffers) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    VkExtent2D extent = swapchain->extent;

    for (uint32_t i = 0; i < count; i++) {
        VkImageView attachments[2] = {
            swapchain->imageViews[i],
            depthImageView
        };

        VkFramebufferCreateInfo fbInfo = {0};
        fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fbInfo.renderPass = renderPass;
        fbInfo.attachmentCount = 2;
        fbInfo.pAttachments = attachments;
        fbInfo.width = extent.width;
        fbInfo.height = extent.height;
        fbInfo.layers = 1;

        VkResult res = vkCreateFramebuffer(device, &fbInfo, NULL, &framebuffers[i]);
        if (res != VK_SUCCESS) {
            for (uint32_t j = 0; j < i; j++) {
                vkDestroyFramebuffer(device, framebuffers[j], NULL);
            }
            free(framebuffers);
            return res;
        }
    }

    *outFramebuffers = framebuffers;
    *outCount = count;
    return VK_SUCCESS;
}

void destroyFramebuffers(
    VkDevice device,
    VkFramebuffer* framebuffers,
    uint32_t count
) {
    if (!framebuffers) return;
    for (uint32_t i = 0; i < count; i++) {
        if (framebuffers[i] != VK_NULL_HANDLE) {
            vkDestroyFramebuffer(device, framebuffers[i], NULL);
        }
    }
    free(framebuffers);
}


