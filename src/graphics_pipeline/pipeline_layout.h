#ifndef PIPELINE_LAYOUT_H
#define PIPELINE_LAYOUT_H

#include <vulkan/vulkan.h>
#include <stdint.h>

typedef struct {
    float model[16];  
    uint32_t objectID;
} PushConstants;

typedef struct {
    VkDescriptorSetLayout globalSetLayout;
    VkDescriptorSetLayout materialSetLayout;

    VkPipelineLayout pipelineLayout;
} PipelineLayouts;

VkResult createPipelineLayouts(
    VkDevice device,
    PipelineLayouts* outLayouts
);

// Destroy layouts created by createPipelineLayouts.
void destroyPipelineLayouts(
    VkDevice device,
    PipelineLayouts* layouts
);

#endif // PIPELINE_LAYOUT_H
