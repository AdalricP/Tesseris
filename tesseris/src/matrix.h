#ifndef MATRIX_H
#define MATRIX_H

#include "vector.h"

typedef struct {
    float m[4][4];
} Mat4;

// Matrix functions
Mat4 mat4_identity();
Mat4 mat4_mul(Mat4 a, Mat4 b);
Vec4 mat4_mul_vec4(Mat4 m, Vec4 v);
Mat4 mat4_translation(float tx, float ty, float tz);
Mat4 mat4_rotation_x(float angle_rad);
Mat4 mat4_rotation_y(float angle_rad);
Mat4 mat4_rotation_z(float angle_rad);
Mat4 mat4_perspective(float fov, float aspect, float near, float far);

#endif 