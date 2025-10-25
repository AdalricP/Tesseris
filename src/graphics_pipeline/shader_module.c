#include "shader_module.h"
#include <stdio.h>
#include <stdlib.h>

// Helper function to read a binary file into memory
static uint32_t* readSPIRVFile(const char* filepath, size_t* outSize) {
    if (!filepath || !outSize) {
        return NULL;
    }

    FILE* file = fopen(filepath, "rb");
    if (!file) {
        printf("Failed to open shader file: %s\n", filepath);
        return NULL;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (fileSize <= 0) {
        printf("Invalid shader file size: %s\n", filepath);
        fclose(file);
        return NULL;
    }

    // SPIR-V must be a multiple of 4 bytes (uint32_t aligned)
    if (fileSize % 4 != 0) {
        printf("Warning: SPIR-V file size not aligned to 4 bytes: %s\n", filepath);
    }

    // Allocate buffer
    uint32_t* buffer = (uint32_t*)malloc(fileSize);
    if (!buffer) {
        printf("Failed to allocate memory for shader: %s\n", filepath);
        fclose(file);
        return NULL;
    }

    // Read file
    size_t bytesRead = fread(buffer, 1, fileSize, file);
    fclose(file);

    if (bytesRead != (size_t)fileSize) {
        printf("Failed to read entire shader file: %s\n", filepath);
        free(buffer);
        return NULL;
    }

    *outSize = (size_t)fileSize;
    return buffer;
}

VkResult createShaderModuleFromFile(
    VkDevice device,
    const char* filepath,
    VkShaderModule* outModule
) {
    if (!device || !filepath || !outModule) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    size_t codeSize = 0;
    uint32_t* code = readSPIRVFile(filepath, &codeSize);
    if (!code) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    printf("    Loading shader: %s (%zu bytes)\n", filepath, codeSize);
    VkResult result = createShaderModuleFromCode(device, code, codeSize, outModule);
    
    free(code);
    return result;
}

VkResult createShaderModuleFromCode(
    VkDevice device,
    const uint32_t* code,
    size_t codeSize,
    VkShaderModule* outModule
) {
    if (!device || !code || codeSize == 0 || !outModule) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    VkShaderModuleCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = codeSize;
    createInfo.pCode = code;

    VkResult result = vkCreateShaderModule(device, &createInfo, NULL, outModule);
    if (result != VK_SUCCESS) {
        printf("Failed to create shader module! Error code: %d\n", result);
        return result;
    }

    printf("    Shader module created: handle=%p, size=%zu bytes\n", (void*)*outModule, codeSize);
    return VK_SUCCESS;
}

void destroyShaderModule(
    VkDevice device,
    VkShaderModule module
) {
    if (device != VK_NULL_HANDLE && module != VK_NULL_HANDLE) {
        vkDestroyShaderModule(device, module, NULL);
    }
}
