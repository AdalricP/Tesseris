#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <stddef.h>

// Simple mesh structure
typedef struct {
    float* vertices;    // x,y,z for each vertex
    float* normals;     // nx,ny,nz for each vertex
    float* texcoords;   // u,v for each vertex
    unsigned int* indices; // triangle indices
    size_t num_vertices;
    size_t num_indices;
} Mesh;

// Load OBJ file
// Returns 0 on success, -1 on failure
int load_obj(const char* filename, Mesh* mesh);

// Free mesh data
void free_mesh(Mesh* mesh);

#endif // OBJLOADER_H