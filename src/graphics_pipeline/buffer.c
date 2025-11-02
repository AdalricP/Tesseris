#include "buffer.h"
#include <stdio.h>
#include <string.h>

static uint32_t findMemoryType(
    VkPhysicalDevice physicalDevice,
    uint32_t typeFilter,
    VkMemoryPropertyFlags properties
) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && 
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    printf("Failed to find suitable memory type!\n");
    return UINT32_MAX;
}

VkResult createBuffer(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    const BufferCreateInfo* createInfo,
    Buffer* outBuffer
) {
    if (!device || !physicalDevice || !createInfo || !outBuffer) {
        printf("Buffer creation failed: Invalid parameters\n");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    memset(outBuffer, 0, sizeof(Buffer));
    outBuffer->size = createInfo->size;

    printf("  Creating buffer:\n");
    printf("    Size: %llu bytes\n", (unsigned long long)createInfo->size);
    printf("    Usage: 0x%x\n", createInfo->usage);
    printf("    Properties: 0x%x\n", createInfo->properties);

    // Create buffer
    VkBufferCreateInfo bufferInfo = {0};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = createInfo->size;
    bufferInfo.usage = createInfo->usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult result = vkCreateBuffer(device, &bufferInfo, NULL, &outBuffer->buffer);
    if (result != VK_SUCCESS) {
        printf("    Failed to create VkBuffer! Error: %d\n", result);
        return result;
    }
    printf("    VkBuffer created: %p\n", (void*)outBuffer->buffer);

    // Get memory requirements
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, outBuffer->buffer, &memRequirements);
    printf("    Memory requirements:\n");
    printf("      Required size: %llu bytes\n", (unsigned long long)memRequirements.size);
    printf("      Alignment: %llu bytes\n", (unsigned long long)memRequirements.alignment);

    // Allocate memory
    VkMemoryAllocateInfo allocInfo = {0};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(
        physicalDevice,
        memRequirements.memoryTypeBits,
        createInfo->properties
    );

    if (allocInfo.memoryTypeIndex == UINT32_MAX) {
        printf("    Failed to find suitable memory type!\n");
        vkDestroyBuffer(device, outBuffer->buffer, NULL);
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    printf("    Memory type index: %u\n", allocInfo.memoryTypeIndex);

    result = vkAllocateMemory(device, &allocInfo, NULL, &outBuffer->memory);
    if (result != VK_SUCCESS) {
        printf("    Failed to allocate buffer memory! Error: %d\n", result);
        vkDestroyBuffer(device, outBuffer->buffer, NULL);
        return result;
    }
    printf("    VkDeviceMemory allocated: %p (%llu bytes)\n", 
           (void*)outBuffer->memory, (unsigned long long)memRequirements.size);

    // Bind buffer to memory
    result = vkBindBufferMemory(device, outBuffer->buffer, outBuffer->memory, 0);
    if (result != VK_SUCCESS) {
        printf("    Failed to bind buffer memory! Error: %d\n", result);
        vkFreeMemory(device, outBuffer->memory, NULL);
        vkDestroyBuffer(device, outBuffer->buffer, NULL);
        return result;
    }
    printf("    Buffer successfully bound to memory\n");

    return VK_SUCCESS;
}

VkResult updateBuffer(
    VkDevice device,
    Buffer* buffer,
    const void* data,
    VkDeviceSize size,
    VkDeviceSize offset
) {
    if (!device || !buffer || !data || size == 0) {
        printf("Buffer update failed: Invalid parameters\n");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    if (offset + size > buffer->size) {
        printf("Buffer update failed: Size exceeds buffer bounds\n");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    void* mappedData = NULL;
    VkResult result = vkMapMemory(device, buffer->memory, offset, size, 0, &mappedData);
    if (result != VK_SUCCESS) {
        printf("    Failed to map buffer memory! Error: %d\n", result);
        return result;
    }

    memcpy(mappedData, data, size);
    vkUnmapMemory(device, buffer->memory);
    
    return VK_SUCCESS;
}

VkResult mapBuffer(
    VkDevice device,
    Buffer* buffer,
    void** outMappedData
) {
    if (!device || !buffer || !outMappedData) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    if (buffer->mapped) {
        *outMappedData = buffer->mapped;
        return VK_SUCCESS;
    }

    VkResult result = vkMapMemory(device, buffer->memory, 0, buffer->size, 0, &buffer->mapped);
    if (result == VK_SUCCESS) {
        *outMappedData = buffer->mapped;
        printf("  Buffer mapped: %p -> %p\n", (void*)buffer->buffer, buffer->mapped);
    }
    return result;
}

void unmapBuffer(
    VkDevice device,
    Buffer* buffer
) {
    if (device && buffer && buffer->mapped) {
        vkUnmapMemory(device, buffer->memory);
        buffer->mapped = NULL;
        printf("  Buffer unmapped: %p\n", (void*)buffer->buffer);
    }
}

VkResult copyBuffer(
    VkDevice device,
    VkCommandPool commandPool,
    VkQueue queue,
    Buffer* srcBuffer,
    Buffer* dstBuffer,
    VkDeviceSize size
) {
    if (!device || !commandPool || !queue || !srcBuffer || !dstBuffer) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    printf("  Copying buffer: %llu bytes\n", (unsigned long long)size);
    printf("    Source: %p\n", (void*)srcBuffer->buffer);
    printf("    Destination: %p\n", (void*)dstBuffer->buffer);

    // Allocate temporary command buffer
    VkCommandBufferAllocateInfo allocInfo = {0};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    VkResult result = vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);
    if (result != VK_SUCCESS) {
        printf("    Failed to allocate command buffer! Error: %d\n", result);
        return result;
    }

    // Begin recording
    VkCommandBufferBeginInfo beginInfo = {0};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion = {0};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer->buffer, dstBuffer->buffer, 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    // Submit and wait
    VkSubmitInfo submitInfo = {0};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    result = vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
    if (result == VK_SUCCESS) {
        vkQueueWaitIdle(queue);
        printf("    Buffer copy completed\n");
    } else {
        printf("    Buffer copy failed! Error: %d\n", result);
    }

    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
    return result;
}

void destroyBuffer(
    VkDevice device,
    Buffer* buffer
) {
    if (!device || !buffer) return;

    if (buffer->mapped) {
        vkUnmapMemory(device, buffer->memory);
        buffer->mapped = NULL;
    }

    if (buffer->buffer != VK_NULL_HANDLE) {
        printf("  Destroying buffer: %p\n", (void*)buffer->buffer);
        vkDestroyBuffer(device, buffer->buffer, NULL);
        buffer->buffer = VK_NULL_HANDLE;
    }

    if (buffer->memory != VK_NULL_HANDLE) {
        printf("  Freeing buffer memory: %p\n", (void*)buffer->memory);
        vkFreeMemory(device, buffer->memory, NULL);
        buffer->memory = VK_NULL_HANDLE;
    }

    buffer->size = 0;
}
