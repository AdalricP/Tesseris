#include "graphics_pipeline.h"
#include "shader_module.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

GraphicsPipelineConfig createDefaultPipelineConfig(
    VkDevice device,
    VkPipelineLayout pipelineLayout,
    VkRenderPass renderPass,
    VkExtent2D viewportExtent
) {
    GraphicsPipelineConfig config = {0};
    
    config.device = device;
    config.pipelineLayout = pipelineLayout;
    config.renderPass = renderPass;
    config.subpass = 0;
    
    config.viewportExtent = viewportExtent;
    
    // Default: no vertex input (can be overridden)
    config.vertexBindings = NULL;
    config.vertexBindingCount = 0;
    config.vertexAttributes = NULL;
    config.vertexAttributeCount = 0;
    
    // Sensible 3D rendering defaults
    config.enableDepthTest = true;
    config.enableDepthWrite = true;
    config.depthCompareOp = VK_COMPARE_OP_LESS;
    
    config.enableBlending = false;
    config.cullMode = VK_CULL_MODE_BACK_BIT;
    config.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    config.polygonMode = VK_POLYGON_MODE_FILL;
    
    config.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    config.lineWidth = 1.0f;
    
    return config;
}

VkResult createGraphicsPipeline(
    const GraphicsPipelineConfig* config,
    GraphicsPipeline* outPipeline
) {
    if (!config || !outPipeline) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    memset(outPipeline, 0, sizeof(*outPipeline));

    VkResult result;

    // Load shader modules
    if (config->vertShaderPath) {
        result = createShaderModuleFromFile(
            config->device,
            config->vertShaderPath,
            &outPipeline->vertShaderModule
        );
        if (result != VK_SUCCESS) {
            printf("Failed to load vertex shader: %s\n", config->vertShaderPath);
            return result;
        }
    }

    if (config->fragShaderPath) {
        result = createShaderModuleFromFile(
            config->device,
            config->fragShaderPath,
            &outPipeline->fragShaderModule
        );
        if (result != VK_SUCCESS) {
            printf("Failed to load fragment shader: %s\n", config->fragShaderPath);
            destroyShaderModule(config->device, outPipeline->vertShaderModule);
            outPipeline->vertShaderModule = VK_NULL_HANDLE;
            return result;
        }
    }

    // --- Shader stages ---
    VkPipelineShaderStageCreateInfo shaderStages[2] = {0};
    uint32_t stageCount = 0;

    if (outPipeline->vertShaderModule != VK_NULL_HANDLE) {
        shaderStages[stageCount].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[stageCount].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStages[stageCount].module = outPipeline->vertShaderModule;
        shaderStages[stageCount].pName = "main";
        stageCount++;
    }

    if (outPipeline->fragShaderModule != VK_NULL_HANDLE) {
        shaderStages[stageCount].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[stageCount].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStages[stageCount].module = outPipeline->fragShaderModule;
        shaderStages[stageCount].pName = "main";
        stageCount++;
    }

    // --- Vertex input state ---
    VkVertexInputBindingDescription* vkBindings = NULL;
    VkVertexInputAttributeDescription* vkAttributes = NULL;

    if (config->vertexBindingCount > 0) {
        vkBindings = malloc(sizeof(VkVertexInputBindingDescription) * config->vertexBindingCount);
        for (uint32_t i = 0; i < config->vertexBindingCount; i++) {
            vkBindings[i].binding = config->vertexBindings[i].binding;
            vkBindings[i].stride = config->vertexBindings[i].stride;
            vkBindings[i].inputRate = config->vertexBindings[i].inputRate;
        }
    }

    if (config->vertexAttributeCount > 0) {
        vkAttributes = malloc(sizeof(VkVertexInputAttributeDescription) * config->vertexAttributeCount);
        for (uint32_t i = 0; i < config->vertexAttributeCount; i++) {
            vkAttributes[i].location = config->vertexAttributes[i].location;
            vkAttributes[i].binding = config->vertexAttributes[i].binding;
            vkAttributes[i].format = config->vertexAttributes[i].format;
            vkAttributes[i].offset = config->vertexAttributes[i].offset;
        }
    }

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {0};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = config->vertexBindingCount;
    vertexInputInfo.pVertexBindingDescriptions = vkBindings;
    vertexInputInfo.vertexAttributeDescriptionCount = config->vertexAttributeCount;
    vertexInputInfo.pVertexAttributeDescriptions = vkAttributes;

    // --- Input assembly ---
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {0};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = config->topology;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    // --- Viewport and scissor ---
    VkViewport viewport = {0};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)config->viewportExtent.width;
    viewport.height = (float)config->viewportExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {0};
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent = config->viewportExtent;

    VkPipelineViewportStateCreateInfo viewportState = {0};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    // --- Rasterizer ---
    VkPipelineRasterizationStateCreateInfo rasterizer = {0};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = config->polygonMode;
    rasterizer.lineWidth = config->lineWidth;
    rasterizer.cullMode = config->cullMode;
    rasterizer.frontFace = config->frontFace;
    rasterizer.depthBiasEnable = VK_FALSE;

    // --- Multisampling (disabled for now) ---
    VkPipelineMultisampleStateCreateInfo multisampling = {0};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    // --- Depth and stencil ---
    VkPipelineDepthStencilStateCreateInfo depthStencil = {0};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = config->enableDepthTest ? VK_TRUE : VK_FALSE;
    depthStencil.depthWriteEnable = config->enableDepthWrite ? VK_TRUE : VK_FALSE;
    depthStencil.depthCompareOp = config->depthCompareOp;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;

    // --- Color blend attachment ---
    VkPipelineColorBlendAttachmentState colorBlendAttachment = {0};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                          VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    
    if (config->enableBlending) {
        colorBlendAttachment.blendEnable = VK_TRUE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
    } else {
        colorBlendAttachment.blendEnable = VK_FALSE;
    }

    VkPipelineColorBlendStateCreateInfo colorBlending = {0};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    // --- Dynamic state (optional, allows changing viewport/scissor at runtime) ---
    VkDynamicState dynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState = {0};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamicStates;

    // --- Create the graphics pipeline ---
    VkGraphicsPipelineCreateInfo pipelineInfo = {0};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = stageCount;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = config->pipelineLayout;
    pipelineInfo.renderPass = config->renderPass;
    pipelineInfo.subpass = config->subpass;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = -1;

    result = vkCreateGraphicsPipelines(
        config->device,
        VK_NULL_HANDLE, // pipeline cache (optional)
        1,
        &pipelineInfo,
        NULL,
        &outPipeline->pipeline
    );

    // Cleanup temporary allocations
    free(vkBindings);
    free(vkAttributes);

    if (result != VK_SUCCESS) {
        printf("Failed to create graphics pipeline! Error code: %d\n", result);
        destroyShaderModule(config->device, outPipeline->vertShaderModule);
        destroyShaderModule(config->device, outPipeline->fragShaderModule);
        outPipeline->vertShaderModule = VK_NULL_HANDLE;
        outPipeline->fragShaderModule = VK_NULL_HANDLE;
        return result;
    }

    printf("\nGraphics Pipeline:\n");
    printf("  Pipeline Handle: %p\n", (void*)outPipeline->pipeline);
    printf("  Vertex Shader Module: %p\n", (void*)outPipeline->vertShaderModule);
    printf("  Fragment Shader Module: %p\n", (void*)outPipeline->fragShaderModule);
    printf("  Viewport Extent: %ux%u\n", config->viewportExtent.width, config->viewportExtent.height);
    printf("  Topology: %d\n", config->topology);
    printf("  Depth Test: %s\n", config->enableDepthTest ? "Enabled" : "Disabled");
    printf("  Blending: %s\n", config->enableBlending ? "Enabled" : "Disabled");
    printf("  Cull Mode: %d\n", config->cullMode);
    printf("  Polygon Mode: %d\n", config->polygonMode);
    
    return VK_SUCCESS;
}

void destroyGraphicsPipeline(
    VkDevice device,
    GraphicsPipeline* pipeline
) {
    if (!pipeline) return;

    if (pipeline->pipeline != VK_NULL_HANDLE) {
        vkDestroyPipeline(device, pipeline->pipeline, NULL);
        pipeline->pipeline = VK_NULL_HANDLE;
    }

    destroyShaderModule(device, pipeline->vertShaderModule);
    destroyShaderModule(device, pipeline->fragShaderModule);
    
    pipeline->vertShaderModule = VK_NULL_HANDLE;
    pipeline->fragShaderModule = VK_NULL_HANDLE;
}
