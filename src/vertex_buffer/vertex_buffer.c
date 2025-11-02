#include "vertex_buffer.h"
#include "../model_loaders/objloader.h"
#include <stdio.h>
#include <stdlib.h>

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

    // Define cube vertices with 3D positions, colors, and normals
    // Cube has 8 corners, but we duplicate vertices for each face triangle
    Vertex vertices[36] = {
        // Front face (z = 0.5) - normal: (0, 0, 1)
        {vec3_create(-0.5f, -0.5f,  0.5f), vec3_create(1.0f, 1.0f, 1.0f), vec3_create(0.0f, 0.0f, 1.0f)},  // bottom-left
        {vec3_create( 0.5f, -0.5f,  0.5f), vec3_create(1.0f, 1.0f, 1.0f), vec3_create(0.0f, 0.0f, 1.0f)},  // bottom-right
        {vec3_create( 0.5f,  0.5f,  0.5f), vec3_create(1.0f, 1.0f, 1.0f), vec3_create(0.0f, 0.0f, 1.0f)},  // top-right
        {vec3_create( 0.5f,  0.5f,  0.5f), vec3_create(1.0f, 1.0f, 1.0f), vec3_create(0.0f, 0.0f, 1.0f)},  // top-right
        {vec3_create(-0.5f,  0.5f,  0.5f), vec3_create(1.0f, 1.0f, 1.0f), vec3_create(0.0f, 0.0f, 1.0f)},  // top-left
        {vec3_create(-0.5f, -0.5f,  0.5f), vec3_create(1.0f, 1.0f, 1.0f), vec3_create(0.0f, 0.0f, 1.0f)},  // bottom-left

        // Back face (z = -0.5) - normal: (0, 0, -1)
        {vec3_create(-0.5f, -0.5f, -0.5f), vec3_create(1.0f, 1.0f, 1.0f), vec3_create(0.0f, 0.0f, -1.0f)},  // bottom-left
        {vec3_create( 0.5f, -0.5f, -0.5f), vec3_create(1.0f, 1.0f, 1.0f), vec3_create(0.0f, 0.0f, -1.0f)},  // bottom-right
        {vec3_create( 0.5f,  0.5f, -0.5f), vec3_create(1.0f, 1.0f, 1.0f), vec3_create(0.0f, 0.0f, -1.0f)},  // top-right
        {vec3_create( 0.5f,  0.5f, -0.5f), vec3_create(1.0f, 1.0f, 1.0f), vec3_create(0.0f, 0.0f, -1.0f)},  // top-right
        {vec3_create(-0.5f,  0.5f, -0.5f), vec3_create(1.0f, 1.0f, 1.0f), vec3_create(0.0f, 0.0f, -1.0f)},  // top-left
        {vec3_create(-0.5f, -0.5f, -0.5f), vec3_create(1.0f, 1.0f, 1.0f), vec3_create(0.0f, 0.0f, -1.0f)},  // bottom-left

        // Left face (x = -0.5) - normal: (-1, 0, 0)
        {vec3_create(-0.5f, -0.5f, -0.5f), vec3_create(1.0f, 1.0f, 1.0f), vec3_create(-1.0f, 0.0f, 0.0f)},  // bottom-back
        {vec3_create(-0.5f, -0.5f,  0.5f), vec3_create(1.0f, 1.0f, 1.0f), vec3_create(-1.0f, 0.0f, 0.0f)},  // bottom-front
        {vec3_create(-0.5f,  0.5f,  0.5f), vec3_create(1.0f, 1.0f, 1.0f), vec3_create(-1.0f, 0.0f, 0.0f)},  // top-front
        {vec3_create(-0.5f,  0.5f,  0.5f), vec3_create(1.0f, 1.0f, 1.0f), vec3_create(-1.0f, 0.0f, 0.0f)},  // top-front
        {vec3_create(-0.5f,  0.5f, -0.5f), vec3_create(1.0f, 1.0f, 1.0f), vec3_create(-1.0f, 0.0f, 0.0f)},  // top-back
        {vec3_create(-0.5f, -0.5f, -0.5f), vec3_create(1.0f, 1.0f, 1.0f), vec3_create(-1.0f, 0.0f, 0.0f)},  // bottom-back

        // Right face (x = 0.5) - normal: (1, 0, 0)
        {vec3_create( 0.5f, -0.5f, -0.5f), vec3_create(1.0f, 1.0f, 1.0f), vec3_create(1.0f, 0.0f, 0.0f)},  // bottom-back
        {vec3_create( 0.5f, -0.5f,  0.5f), vec3_create(1.0f, 1.0f, 1.0f), vec3_create(1.0f, 0.0f, 0.0f)},  // bottom-front
        {vec3_create( 0.5f,  0.5f,  0.5f), vec3_create(1.0f, 1.0f, 1.0f), vec3_create(1.0f, 0.0f, 0.0f)},  // top-front
        {vec3_create( 0.5f,  0.5f,  0.5f), vec3_create(1.0f, 1.0f, 1.0f), vec3_create(1.0f, 0.0f, 0.0f)},  // top-front
        {vec3_create( 0.5f,  0.5f, -0.5f), vec3_create(1.0f, 1.0f, 1.0f), vec3_create(1.0f, 0.0f, 0.0f)},  // top-back
        {vec3_create( 0.5f, -0.5f, -0.5f), vec3_create(1.0f, 1.0f, 1.0f), vec3_create(1.0f, 0.0f, 0.0f)},  // bottom-back

        // Top face (y = 0.5) - normal: (0, 1, 0)
        {vec3_create(-0.5f,  0.5f, -0.5f), vec3_create(1.0f, 1.0f, 1.0f), vec3_create(0.0f, 1.0f, 0.0f)},  // back-left
        {vec3_create( 0.5f,  0.5f, -0.5f), vec3_create(1.0f, 1.0f, 1.0f), vec3_create(0.0f, 1.0f, 0.0f)},  // back-right
        {vec3_create( 0.5f,  0.5f,  0.5f), vec3_create(1.0f, 1.0f, 1.0f), vec3_create(0.0f, 1.0f, 0.0f)},  // front-right
        {vec3_create( 0.5f,  0.5f,  0.5f), vec3_create(1.0f, 1.0f, 1.0f), vec3_create(0.0f, 1.0f, 0.0f)},  // front-right
        {vec3_create(-0.5f,  0.5f,  0.5f), vec3_create(1.0f, 1.0f, 1.0f), vec3_create(0.0f, 1.0f, 0.0f)},  // front-left
        {vec3_create(-0.5f,  0.5f, -0.5f), vec3_create(1.0f, 1.0f, 1.0f), vec3_create(0.0f, 1.0f, 0.0f)},  // back-left

        // Bottom face (y = -0.5) - normal: (0, -1, 0)
        {vec3_create(-0.5f, -0.5f, -0.5f), vec3_create(1.0f, 1.0f, 1.0f), vec3_create(0.0f, -1.0f, 0.0f)},  // back-left
        {vec3_create( 0.5f, -0.5f, -0.5f), vec3_create(1.0f, 1.0f, 1.0f), vec3_create(0.0f, -1.0f, 0.0f)},  // back-right
        {vec3_create( 0.5f, -0.5f,  0.5f), vec3_create(1.0f, 1.0f, 1.0f), vec3_create(0.0f, -1.0f, 0.0f)},  // front-right
        {vec3_create( 0.5f, -0.5f,  0.5f), vec3_create(1.0f, 1.0f, 1.0f), vec3_create(0.0f, -1.0f, 0.0f)},  // front-right
        {vec3_create(-0.5f, -0.5f,  0.5f), vec3_create(1.0f, 1.0f, 1.0f), vec3_create(0.0f, -1.0f, 0.0f)},  // front-left
        {vec3_create(-0.5f, -0.5f, -0.5f), vec3_create(1.0f, 1.0f, 1.0f), vec3_create(0.0f, -1.0f, 0.0f)}   // back-left
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

VkResult updateVertexBufferWithMesh(
    VkDevice device,
    Buffer* buffer,
    Mesh* mesh,
    uint32_t* vertexCount
) {
    if (!device || !buffer || !mesh || !vertexCount) {
        printf("Vertex buffer update failed: Invalid parameters\n");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    // For now, assume mesh has positions and normals, and we use indices to create vertices
    // Since we don't have index buffer, we duplicate vertices
    *vertexCount = mesh->num_indices;

    Vertex* vertices = (Vertex*)malloc(sizeof(Vertex) * mesh->num_indices);
    if (!vertices) {
        printf("Failed to allocate memory for vertices\n");
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    for (size_t i = 0; i < mesh->num_indices; ++i) {
        unsigned int idx = mesh->indices[i];
        if (idx >= mesh->num_vertices) {
            printf("Invalid index %u >= %zu\n", idx, mesh->num_vertices);
            free(vertices);
            return VK_ERROR_INITIALIZATION_FAILED;
        }

        vec3 pos = vec3_create(
            mesh->vertices[idx * 3 + 0],
            mesh->vertices[idx * 3 + 1],
            mesh->vertices[idx * 3 + 2]
        );
        vec3 normal = vec3_create(
            mesh->normals[idx * 3 + 0],
            mesh->normals[idx * 3 + 1],
            mesh->normals[idx * 3 + 2]
        );
        vec3 color = vec3_create(1.0f, 1.0f, 1.0f); // White color

        vertices[i] = (Vertex){pos, color, normal};
    }

    printf("  Updating vertex buffer with mesh data:\n");
    printf("    %u vertices, %zu bytes each\n", *vertexCount, sizeof(Vertex));
    printf("    Total size: %zu bytes\n", sizeof(Vertex) * (*vertexCount));

    VkResult result = updateBuffer(device, buffer, vertices, sizeof(Vertex) * (*vertexCount), 0);
    free(vertices);
    if (result != VK_SUCCESS) {
        printf("    Failed to update vertex buffer!\n");
        return result;
    }

    printf("    Vertex buffer updated with mesh data\n");
    return VK_SUCCESS;
}