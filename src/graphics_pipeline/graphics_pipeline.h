#ifndef GRAPHICS_PIPELINE_H
#define GRAPHICS_PIPELINE_H

#include <vulkan/vulkan.h>
#include <stdbool.h>

/**
 * Graphics pipeline container holding the pipeline and associated shader modules
 */
typedef struct {
    VkPipeline pipeline;
    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;
} GraphicsPipeline;

/**
 * Vertex input binding description
 * Describes how vertex data is organized in buffers
 */
typedef struct {
    uint32_t binding;
    uint32_t stride;
    VkVertexInputRate inputRate;
} VertexBindingDescription;

/**
 * Vertex input attribute description
 * Describes individual vertex attributes (position, color, normal, UV, etc.)
 */
typedef struct {
    uint32_t location;
    uint32_t binding;
    VkFormat format;
    uint32_t offset;
} VertexAttributeDescription;

/**
 * Configuration for creating a graphics pipeline
 */
typedef struct {
    // Required: Core components
    VkDevice device;
    VkPipelineLayout pipelineLayout;
    VkRenderPass renderPass;
    uint32_t subpass;

    // Shader paths
    const char* vertShaderPath;
    const char* fragShaderPath;

    // Viewport/Scissor
    VkExtent2D viewportExtent;

    // Vertex input configuration
    VertexBindingDescription* vertexBindings;
    uint32_t vertexBindingCount;
    VertexAttributeDescription* vertexAttributes;
    uint32_t vertexAttributeCount;

    // Pipeline configuration flags
    bool enableDepthTest;
    bool enableDepthWrite;
    VkCompareOp depthCompareOp;
    
    bool enableBlending;
    VkCullModeFlags cullMode;
    VkFrontFace frontFace;
    VkPolygonMode polygonMode;
    
    VkPrimitiveTopology topology;
    float lineWidth;
} GraphicsPipelineConfig;

/**
 * Create a default graphics pipeline configuration
 * Sets sensible defaults for a standard 3D rendering pipeline
 * 
 * @param device - VkDevice handle
 * @param pipelineLayout - Pipeline layout to use
 * @param renderPass - Render pass the pipeline will be used with
 * @param viewportExtent - Viewport dimensions
 * @return Default configuration struct
 */
GraphicsPipelineConfig createDefaultPipelineConfig(
    VkDevice device,
    VkPipelineLayout pipelineLayout,
    VkRenderPass renderPass,
    VkExtent2D viewportExtent
);

/**
 * Create a graphics pipeline from configuration
 * 
 * @param config - Pipeline configuration
 * @param outPipeline - Output graphics pipeline
 * @return VK_SUCCESS on success, error code otherwise
 */
VkResult createGraphicsPipeline(
    const GraphicsPipelineConfig* config,
    GraphicsPipeline* outPipeline
);

/**
 * Destroy a graphics pipeline and its shader modules
 * 
 * @param device - VkDevice handle
 * @param pipeline - Graphics pipeline to destroy
 */
void destroyGraphicsPipeline(
    VkDevice device,
    GraphicsPipeline* pipeline
);

#endif // GRAPHICS_PIPELINE_H
