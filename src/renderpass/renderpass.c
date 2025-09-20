#include "renderpass.h"

#include <vulkan/vulkan.h>
#include <stdio.h>

VkResult createRenderPass(VkDevice device, VkFormat swapchainImageFormat, VkFormat depthFormat, VkRenderPass* renderPass) {
    // --- Color attachment (the swapchain image) ---
    VkAttachmentDescription colorAttachment = {0}; // <=> memset(&colorAttachment, 0, sizeof(VkAttachmentDescription));
    colorAttachment.format = swapchainImageFormat;               
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;             
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;        
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;      
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;   
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; 

    VkAttachmentReference colorAttachmentRef = {0};
    colorAttachmentRef.attachment = 0;                           
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // --- Depth attachment (our own image) ---
    VkAttachmentDescription depthAttachment = {0};
    depthAttachment.format = depthFormat;                        // Picked with findDepthFormat()
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;        
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;  
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef = {0};
    depthAttachmentRef.attachment = 1;                           // Index 1 → second attachment
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    // --- Subpass ---
    VkSubpassDescription subpass = {0};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    // --- Subpass dependency ---
    VkSubpassDependency dependency = {0};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    // --- Attachments array ---
    VkAttachmentDescription attachments[2] = { colorAttachment, depthAttachment };

    VkRenderPassCreateInfo renderPassInfo = {0};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 2;
    renderPassInfo.pAttachments = attachments;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(device, &renderPassInfo, NULL, renderPass) != VK_SUCCESS) {
        printf("Failed to create render pass!\n");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    return VK_SUCCESS;
}



// Helper func
VkFormat findDepthFormat(VkPhysicalDevice physicalDevice) {
    VkFormat depthFormats[] = {
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT
    };

    return findSupportedFormat(
        physicalDevice,
        depthFormats,
        3,
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}

// helper func helper func

VkFormat findSupportedFormat( // basically just finds supported format for depth
    VkPhysicalDevice physicalDevice,
    const VkFormat* candidates,
    uint32_t candidateCount,
    VkImageTiling tiling,
    VkFormatFeatureFlags features
) {
    for (uint32_t i = 0; i < candidateCount; i++) {
        VkFormat format = candidates[i];

        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR &&
            (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL &&
                   (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    return VK_FORMAT_UNDEFINED; // none supported
}

void destroyRenderPass(VkDevice device, VkRenderPass renderPass) {
    if (renderPass != VK_NULL_HANDLE) {
        vkDestroyRenderPass(device, renderPass, NULL);
    }
}