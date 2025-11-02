#include "uniform_buffer.h"
#include <stdio.h>
#include <string.h>

VkResult createUniformBuffer(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    Buffer* outBuffer
) {
    if (!device || !physicalDevice || !outBuffer) {
        printf("Uniform buffer creation failed: Invalid parameters\n");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    printf("  Creating uniform buffer:\n");
    printf("    Size: %zu bytes\n", sizeof(UniformBufferObject));

    BufferCreateInfo createInfo = {0};
    createInfo.size = sizeof(UniformBufferObject);
    createInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    createInfo.properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    VkResult result = createBuffer(device, physicalDevice, &createInfo, outBuffer);
    if (result != VK_SUCCESS) {
        printf("    Failed to create uniform buffer!\n");
        return result;
    }

    printf("    Uniform buffer created successfully\n");
    return VK_SUCCESS;
}

VkResult updateUniformBuffer(
    VkDevice device,
    Buffer* buffer,
    const UniformBufferObject* ubo
) {
    if (!device || !buffer || !ubo) {
        printf("Uniform buffer update failed: Invalid parameters\n");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    printf("  Updating uniform buffer with MVP matrices\n");

    VkResult result = updateBuffer(device, buffer, ubo, sizeof(UniformBufferObject), 0);
    if (result != VK_SUCCESS) {
        printf("    Failed to update uniform buffer!\n");
        return result;
    }

    printf("    Uniform buffer updated successfully\n");
    return VK_SUCCESS;
}