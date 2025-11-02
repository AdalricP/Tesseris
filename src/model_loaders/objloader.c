#include "objloader.h"
#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tinyobj_loader_c.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// File reader callback for tinyobj
static void my_file_reader(void* ctx, const char* filename, int is_mtl, const char* obj_filename, char** buf, size_t* len) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        *buf = NULL;
        *len = 0;
        return;
    }

    fseek(file, 0, SEEK_END);
    *len = ftell(file);
    fseek(file, 0, SEEK_SET);

    *buf = (char*)malloc(*len);
    if (*buf) {
        fread(*buf, 1, *len, file);
    } else {
        *len = 0;
    }

    fclose(file);
}

int load_obj(const char* filename, Mesh* mesh) {
    if (!mesh) return -1;

    tinyobj_attrib_t attrib;
    tinyobj_shape_t* shapes = NULL;
    size_t num_shapes;
    tinyobj_material_t* materials = NULL;
    size_t num_materials;

    int result = tinyobj_parse_obj(&attrib, &shapes, &num_shapes, &materials,
                                   &num_materials, filename, my_file_reader,
                                   NULL, TINYOBJ_FLAG_TRIANGULATE);

    if (result != TINYOBJ_SUCCESS) {
        return -1;
    }

    // For simplicity, assume one shape and triangulated faces
    if (num_shapes == 0 || attrib.num_faces == 0) {
        tinyobj_attrib_free(&attrib);
        tinyobj_shapes_free(shapes, num_shapes);
        tinyobj_materials_free(materials, num_materials);
        return -1;
    }

    // Allocate mesh data
    mesh->num_vertices = attrib.num_vertices;
    mesh->num_indices = attrib.num_faces * 3; // Assuming triangles

    mesh->vertices = (float*)malloc(sizeof(float) * 3 * mesh->num_vertices);
    mesh->normals = (float*)malloc(sizeof(float) * 3 * mesh->num_vertices);
    mesh->texcoords = (float*)malloc(sizeof(float) * 2 * mesh->num_vertices);
    mesh->indices = (unsigned int*)malloc(sizeof(unsigned int) * mesh->num_indices);

    if (!mesh->vertices || !mesh->normals || !mesh->texcoords || !mesh->indices) {
        free_mesh(mesh);
        tinyobj_attrib_free(&attrib);
        tinyobj_shapes_free(shapes, num_shapes);
        tinyobj_materials_free(materials, num_materials);
        return -1;
    }

    // Copy vertices
    memcpy(mesh->vertices, attrib.vertices, sizeof(float) * 3 * mesh->num_vertices);

    // Copy normals if available
    if (attrib.num_normals > 0) {
        memcpy(mesh->normals, attrib.normals, sizeof(float) * 3 * mesh->num_vertices);
    } else {
        // Generate dummy normals (pointing up)
        for (size_t i = 0; i < mesh->num_vertices; ++i) {
            mesh->normals[i*3 + 0] = 0.0f;
            mesh->normals[i*3 + 1] = 1.0f;
            mesh->normals[i*3 + 2] = 0.0f;
        }
    }

    // Copy texcoords if available
    if (attrib.num_texcoords > 0) {
        memcpy(mesh->texcoords, attrib.texcoords, sizeof(float) * 2 * mesh->num_vertices);
    } else {
        // Generate dummy texcoords
        memset(mesh->texcoords, 0, sizeof(float) * 2 * mesh->num_vertices);
    }

    // Copy indices
    for (size_t i = 0; i < attrib.num_faces; ++i) {
        tinyobj_vertex_index_t idx = attrib.faces[i];
        mesh->indices[i] = (unsigned int)idx.v_idx;
    }

    // Cleanup
    tinyobj_attrib_free(&attrib);
    tinyobj_shapes_free(shapes, num_shapes);
    tinyobj_materials_free(materials, num_materials);

    return 0;
}

void free_mesh(Mesh* mesh) {
    if (!mesh) return;

    free(mesh->vertices);
    free(mesh->normals);
    free(mesh->texcoords);
    free(mesh->indices);

    mesh->vertices = NULL;
    mesh->normals = NULL;
    mesh->texcoords = NULL;
    mesh->indices = NULL;

    mesh->num_vertices = 0;
    mesh->num_indices = 0;
}