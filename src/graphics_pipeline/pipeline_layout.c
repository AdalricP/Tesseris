#include "pipeline_layout.h"
#include <string.h>

VkResult createPipelineLayouts(
    VkDevice device,
    PipelineLayouts* outLayouts
) {
    if (!device || !outLayouts) return VK_ERROR_INITIALIZATION_FAILED;

    memset(outLayouts, 0, sizeof(*outLayouts));

    // 1) Global set (set=0): one uniform buffer binding for camera + lighting data
    VkDescriptorSetLayoutBinding globalBinding = {0};
    globalBinding.binding = 0; // binding 0
    globalBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    globalBinding.descriptorCount = 1;
    globalBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT; // visible in VS/FS
    globalBinding.pImmutableSamplers = NULL;

    VkDescriptorSetLayoutCreateInfo globalLayoutInfo = {0};
    globalLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    globalLayoutInfo.bindingCount = 1;
    globalLayoutInfo.pBindings = &globalBinding;

    VkResult res = vkCreateDescriptorSetLayout(device, &globalLayoutInfo, NULL, &outLayouts->globalSetLayout);
    if (res != VK_SUCCESS) {
        return res;
    }

    // 2) Material set (set=1): combined image samplers for material textures
    //    0: albedo, 1: metalness, 2: roughness, 3: normal
    VkDescriptorSetLayoutBinding materialBindings[4] = {0};
    for (uint32_t i = 0; i < 4; i++) {
        materialBindings[i].binding = i; // 0..3
        materialBindings[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        materialBindings[i].descriptorCount = 1;
        materialBindings[i].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT; // sampled in fragment shader
        materialBindings[i].pImmutableSamplers = NULL; // could set immutable samplers later
    }

    VkDescriptorSetLayoutCreateInfo materialLayoutInfo = {0};
    materialLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    materialLayoutInfo.bindingCount = 4;
    materialLayoutInfo.pBindings = materialBindings;

    res = vkCreateDescriptorSetLayout(device, &materialLayoutInfo, NULL, &outLayouts->materialSetLayout);
    if (res != VK_SUCCESS) {
        vkDestroyDescriptorSetLayout(device, outLayouts->globalSetLayout, NULL);
        outLayouts->globalSetLayout = VK_NULL_HANDLE;
        return res;
    }

    // 3) Push constant range for per-draw data (model matrix + object ID)
    VkPushConstantRange pushRange = {0};
    pushRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushRange.offset = 0;
    pushRange.size = sizeof(PushConstants); // must be <= device limit (commonly 128 bytes)

    // 4) Pipeline layout that references the two descriptor sets + push constants
    VkDescriptorSetLayout setLayouts[2] = {
        outLayouts->globalSetLayout,   // set = 0
        outLayouts->materialSetLayout  // set = 1
    };

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {0};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 2;
    pipelineLayoutInfo.pSetLayouts = setLayouts;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushRange;

    res = vkCreatePipelineLayout(device, &pipelineLayoutInfo, NULL, &outLayouts->pipelineLayout);
    if (res != VK_SUCCESS) {
        vkDestroyDescriptorSetLayout(device, outLayouts->materialSetLayout, NULL);
        vkDestroyDescriptorSetLayout(device, outLayouts->globalSetLayout, NULL);
        outLayouts->materialSetLayout = VK_NULL_HANDLE;
        outLayouts->globalSetLayout = VK_NULL_HANDLE;
        return res;
    }

    return VK_SUCCESS;
}

void destroyPipelineLayouts(
    VkDevice device,
    PipelineLayouts* layouts
) {
    if (!layouts) return;
    if (layouts->pipelineLayout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(device, layouts->pipelineLayout, NULL);
        layouts->pipelineLayout = VK_NULL_HANDLE;
    }
    if (layouts->materialSetLayout != VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(device, layouts->materialSetLayout, NULL);
        layouts->materialSetLayout = VK_NULL_HANDLE;
    }
    if (layouts->globalSetLayout != VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(device, layouts->globalSetLayout, NULL);
        layouts->globalSetLayout = VK_NULL_HANDLE;
    }
}
