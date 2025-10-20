#include "commandbuffers.h"
#include <stdio.h>
#include <stdlib.h>

VkResult createCommandPool(
    VkDevice device,
    QueueFamilyIndices queueFamilyIndices,
    VkCommandPool* commandPool
) {
    if (!device || !commandPool) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    VkCommandPoolCreateInfo poolInfo = {0};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
    // VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT allows individual command buffers to be reset
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    VkResult result = vkCreateCommandPool(device, &poolInfo, NULL, commandPool);
    if (result != VK_SUCCESS) {
        printf("Failed to create command pool!\n");
        return result;
    }

    return VK_SUCCESS;
}

VkResult allocateCommandBuffers(
    VkDevice device,
    VkCommandPool commandPool,
    uint32_t count,
    VkCommandBuffer** outCommandBuffers
) {
    if (!device || !commandPool || count == 0 || !outCommandBuffers) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    VkCommandBuffer* commandBuffers = (VkCommandBuffer*)malloc(sizeof(VkCommandBuffer) * count);
    if (!commandBuffers) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    VkCommandBufferAllocateInfo allocInfo = {0};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = count;

    VkResult result = vkAllocateCommandBuffers(device, &allocInfo, commandBuffers);
    if (result != VK_SUCCESS) {
        printf("Failed to allocate command buffers!\n");
        free(commandBuffers);
        return result;
    }

    *outCommandBuffers = commandBuffers;
    return VK_SUCCESS;
}

void freeCommandBuffers(
    VkDevice device,
    VkCommandPool commandPool,
    VkCommandBuffer* commandBuffers,
    uint32_t count
) {
    if (!commandBuffers || !device || !commandPool) return;
    
    vkFreeCommandBuffers(device, commandPool, count, commandBuffers);
    free(commandBuffers);
}

void destroyCommandPool(
    VkDevice device,
    VkCommandPool commandPool
) {
    if (commandPool != VK_NULL_HANDLE) {
        vkDestroyCommandPool(device, commandPool, NULL);
    }
}
