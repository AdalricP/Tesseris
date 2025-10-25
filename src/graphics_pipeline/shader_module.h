#ifndef SHADER_MODULE_H
#define SHADER_MODULE_H

#include <vulkan/vulkan.h>
#include <stddef.h>

/**
 * Create a Vulkan shader module from a SPIR-V file.
 * 
 * @param device - VkDevice handle
 * @param filepath - Path to the .spv file
 * @param outModule - Output shader module handle
 * @return VK_SUCCESS on success, error code otherwise
 */
VkResult createShaderModuleFromFile(
    VkDevice device,
    const char* filepath,
    VkShaderModule* outModule
);

/**
 * Create a Vulkan shader module from SPIR-V bytecode in memory.
 * 
 * @param device - VkDevice handle
 * @param code - Pointer to SPIR-V bytecode (must be uint32_t aligned)
 * @param codeSize - Size of the code in bytes
 * @param outModule - Output shader module handle
 * @return VK_SUCCESS on success, error code otherwise
 */
VkResult createShaderModuleFromCode(
    VkDevice device,
    const uint32_t* code,
    size_t codeSize,
    VkShaderModule* outModule
);

/**
 * Destroy a shader module.
 * 
 * @param device - VkDevice handle
 * @param module - Shader module to destroy
 */
void destroyShaderModule(
    VkDevice device,
    VkShaderModule module
);

#endif // SHADER_MODULE_H
