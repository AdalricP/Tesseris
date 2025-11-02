#ifndef MATRIX_H
#define MATRIX_H

#include <math.h>
#include <stdbool.h>
#include "vector.h"

/**
 * 4x4 matrix type (column-major order)
 */
typedef struct {
    float m[16];  // 4x4 matrix stored in column-major order
} mat4;

/**
 * Matrix creation functions
 */
mat4 mat4_identity(void);
mat4 mat4_perspective(float fov, float aspect, float near, float far);
mat4 mat4_orthographic(float left, float right, float bottom, float top, float near, float far);
mat4 mat4_look_at(vec3 eye, vec3 center, vec3 up);
mat4 mat4_translate(vec3 translation);
mat4 mat4_rotate_x(float angle);
mat4 mat4_rotate_y(float angle);
mat4 mat4_rotate_z(float angle);
mat4 mat4_scale(vec3 scale);

/**
 * Matrix operations
 */
mat4 mat4_multiply(mat4 a, mat4 b);
vec4 mat4_multiply_vec4(mat4 m, vec4 v);
mat4 mat4_transpose(mat4 m);
mat4 mat4_inverse(mat4 m);

/**
 * Utility functions
 */
void mat4_print(mat4 m);

#endif // MATRIX_H