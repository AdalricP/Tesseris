#include "vertex_buffer.h"
#include <stdio.h>

VkResult createVertexBuffer(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    VkDeviceSize size,
    Buffer* outBuffer
) {
    if (!device || !physicalDevice || size == 0 || !outBuffer) {
        printf("Vertex buffer creation failed: Invalid parameters\n");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    printf("  Creating vertex buffer:\n");
    printf("    Size: %llu bytes\n", (unsigned long long)size);

    BufferCreateInfo createInfo = {0};
    createInfo.size = size;
    createInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    createInfo.properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | 
                            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    VkResult result = createBuffer(device, physicalDevice, &createInfo, outBuffer);
    if (result != VK_SUCCESS) {
        printf("    Failed to create vertex buffer!\n");
        return result;
    }

    printf("    Vertex buffer created successfully\n");
    return VK_SUCCESS;
}

VkResult updateVertexBufferWithCube(
    VkDevice device,
    Buffer* buffer
) {
    if (!device || !buffer) {
        printf("Vertex buffer update failed: Invalid parameters\n");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    // Define cube vertices with 3D positions and colors
    // Cube has 8 corners, but we duplicate vertices for each face triangle
    Vertex vertices[36] = {
        // Front face (z = 0.5)
        {vec3_create(-0.5f, -0.5f,  0.5f), vec3_create(1.0f, 0.0f, 0.0f)},  // bottom-left
        {vec3_create( 0.5f, -0.5f,  0.5f), vec3_create(1.0f, 0.0f, 0.0f)},  // bottom-right
        {vec3_create( 0.5f,  0.5f,  0.5f), vec3_create(1.0f, 0.0f, 0.0f)},  // top-right
        {vec3_create( 0.5f,  0.5f,  0.5f), vec3_create(1.0f, 0.0f, 0.0f)},  // top-right
        {vec3_create(-0.5f,  0.5f,  0.5f), vec3_create(1.0f, 0.0f, 0.0f)},  // top-left
        {vec3_create(-0.5f, -0.5f,  0.5f), vec3_create(1.0f, 0.0f, 0.0f)},  // bottom-left

        // Back face (z = -0.5)
        {vec3_create(-0.5f, -0.5f, -0.5f), vec3_create(0.0f, 1.0f, 0.0f)},  // bottom-left
        {vec3_create( 0.5f, -0.5f, -0.5f), vec3_create(0.0f, 1.0f, 0.0f)},  // bottom-right
        {vec3_create( 0.5f,  0.5f, -0.5f), vec3_create(0.0f, 1.0f, 0.0f)},  // top-right
        {vec3_create( 0.5f,  0.5f, -0.5f), vec3_create(0.0f, 1.0f, 0.0f)},  // top-right
        {vec3_create(-0.5f,  0.5f, -0.5f), vec3_create(0.0f, 1.0f, 0.0f)},  // top-left
        {vec3_create(-0.5f, -0.5f, -0.5f), vec3_create(0.0f, 1.0f, 0.0f)},  // bottom-left

        // Left face (x = -0.5)
        {vec3_create(-0.5f, -0.5f, -0.5f), vec3_create(0.0f, 0.0f, 1.0f)},  // bottom-back
        {vec3_create(-0.5f, -0.5f,  0.5f), vec3_create(0.0f, 0.0f, 1.0f)},  // bottom-front
        {vec3_create(-0.5f,  0.5f,  0.5f), vec3_create(0.0f, 0.0f, 1.0f)},  // top-front
        {vec3_create(-0.5f,  0.5f,  0.5f), vec3_create(0.0f, 0.0f, 1.0f)},  // top-front
        {vec3_create(-0.5f,  0.5f, -0.5f), vec3_create(0.0f, 0.0f, 1.0f)},  // top-back
        {vec3_create(-0.5f, -0.5f, -0.5f), vec3_create(0.0f, 0.0f, 1.0f)},  // bottom-back

        // Right face (x = 0.5)
        {vec3_create( 0.5f, -0.5f, -0.5f), vec3_create(1.0f, 1.0f, 0.0f)},  // bottom-back
        {vec3_create( 0.5f, -0.5f,  0.5f), vec3_create(1.0f, 1.0f, 0.0f)},  // bottom-front
        {vec3_create( 0.5f,  0.5f,  0.5f), vec3_create(1.0f, 1.0f, 0.0f)},  // top-front
        {vec3_create( 0.5f,  0.5f,  0.5f), vec3_create(1.0f, 1.0f, 0.0f)},  // top-front
        {vec3_create( 0.5f,  0.5f, -0.5f), vec3_create(1.0f, 1.0f, 0.0f)},  // top-back
        {vec3_create( 0.5f, -0.5f, -0.5f), vec3_create(1.0f, 1.0f, 0.0f)},  // bottom-back

        // Top face (y = 0.5)
        {vec3_create(-0.5f,  0.5f, -0.5f), vec3_create(1.0f, 0.0f, 1.0f)},  // back-left
        {vec3_create( 0.5f,  0.5f, -0.5f), vec3_create(1.0f, 0.0f, 1.0f)},  // back-right
        {vec3_create( 0.5f,  0.5f,  0.5f), vec3_create(1.0f, 0.0f, 1.0f)},  // front-right
        {vec3_create( 0.5f,  0.5f,  0.5f), vec3_create(1.0f, 0.0f, 1.0f)},  // front-right
        {vec3_create(-0.5f,  0.5f,  0.5f), vec3_create(1.0f, 0.0f, 1.0f)},  // front-left
        {vec3_create(-0.5f,  0.5f, -0.5f), vec3_create(1.0f, 0.0f, 1.0f)},  // back-left

        // Bottom face (y = -0.5)
        {vec3_create(-0.5f, -0.5f, -0.5f), vec3_create(0.0f, 1.0f, 1.0f)},  // back-left
        {vec3_create( 0.5f, -0.5f, -0.5f), vec3_create(0.0f, 1.0f, 1.0f)},  // back-right
        {vec3_create( 0.5f, -0.5f,  0.5f), vec3_create(0.0f, 1.0f, 1.0f)},  // front-right
        {vec3_create( 0.5f, -0.5f,  0.5f), vec3_create(0.0f, 1.0f, 1.0f)},  // front-right
        {vec3_create(-0.5f, -0.5f,  0.5f), vec3_create(0.0f, 1.0f, 1.0f)},  // front-left
        {vec3_create(-0.5f, -0.5f, -0.5f), vec3_create(0.0f, 1.0f, 1.0f)}   // back-left
    };

    printf("  Updating vertex buffer with cube data:\n");
    printf("    36 vertices, %zu bytes each\n", sizeof(Vertex));
    printf("    Total size: %zu bytes\n", sizeof(vertices));

    VkResult result = updateBuffer(device, buffer, vertices, sizeof(vertices), 0);
    if (result != VK_SUCCESS) {
        printf("    Failed to update vertex buffer!\n");
        return result;
    }

    printf("    Vertex buffer updated with cube data\n");
    return VK_SUCCESS;
}