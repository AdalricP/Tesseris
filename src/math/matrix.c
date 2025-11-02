#include "matrix.h"
#include <stdio.h>
#include <string.h>

mat4 mat4_identity(void) {
    mat4 result = {0};
    result.m[0] = 1.0f;
    result.m[5] = 1.0f;
    result.m[10] = 1.0f;
    result.m[15] = 1.0f;
    return result;
}

mat4 mat4_perspective(float fov, float aspect, float near, float far) {
    mat4 result = {0};
    float tan_half_fov = tanf(fov / 2.0f);

    result.m[0] = 1.0f / (aspect * tan_half_fov);
    result.m[5] = 1.0f / tan_half_fov;
    result.m[10] = -(far + near) / (far - near);
    result.m[11] = -1.0f;
    result.m[14] = -(2.0f * far * near) / (far - near);

    return result;
}

mat4 mat4_orthographic(float left, float right, float bottom, float top, float near, float far) {
    mat4 result = {0};

    result.m[0] = 2.0f / (right - left);
    result.m[5] = 2.0f / (top - bottom);
    result.m[10] = -2.0f / (far - near);
    result.m[12] = -(right + left) / (right - left);
    result.m[13] = -(top + bottom) / (top - bottom);
    result.m[14] = -(far + near) / (far - near);
    result.m[15] = 1.0f;

    return result;
}

mat4 mat4_look_at(vec3 eye, vec3 center, vec3 up) {
    vec3 f = vec3_normalize(vec3_sub(center, eye));
    vec3 s = vec3_normalize(vec3_cross(f, up));
    vec3 u = vec3_cross(s, f);

    mat4 result = mat4_identity();
    result.m[0] = s.x;
    result.m[1] = u.x;
    result.m[2] = -f.x;
    result.m[4] = s.y;
    result.m[5] = u.y;
    result.m[6] = -f.y;
    result.m[8] = s.z;
    result.m[9] = u.z;
    result.m[10] = -f.z;
    result.m[12] = -vec3_dot(s, eye);
    result.m[13] = -vec3_dot(u, eye);
    result.m[14] = vec3_dot(f, eye);

    return result;
}

mat4 mat4_translate(vec3 translation) {
    mat4 result = mat4_identity();
    result.m[12] = translation.x;
    result.m[13] = translation.y;
    result.m[14] = translation.z;
    return result;
}

mat4 mat4_rotate_x(float angle) {
    mat4 result = mat4_identity();
    float c = cosf(angle);
    float s = sinf(angle);
    result.m[5] = c;
    result.m[6] = s;
    result.m[9] = -s;
    result.m[10] = c;
    return result;
}

mat4 mat4_rotate_y(float angle) {
    mat4 result = mat4_identity();
    float c = cosf(angle);
    float s = sinf(angle);
    result.m[0] = c;
    result.m[2] = -s;
    result.m[8] = s;
    result.m[10] = c;
    return result;
}

mat4 mat4_rotate_z(float angle) {
    mat4 result = mat4_identity();
    float c = cosf(angle);
    float s = sinf(angle);
    result.m[0] = c;
    result.m[1] = s;
    result.m[4] = -s;
    result.m[5] = c;
    return result;
}

mat4 mat4_scale(vec3 scale) {
    mat4 result = mat4_identity();
    result.m[0] = scale.x;
    result.m[5] = scale.y;
    result.m[10] = scale.z;
    return result;
}

mat4 mat4_multiply(mat4 a, mat4 b) {
    mat4 result = {0};
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 4; k++) {
                result.m[i * 4 + j] += a.m[k * 4 + j] * b.m[i * 4 + k];
            }
        }
    }
    return result;
}

vec4 mat4_multiply_vec4(mat4 m, vec4 v) {
    vec4 result;
    result.x = m.m[0] * v.x + m.m[4] * v.y + m.m[8] * v.z + m.m[12] * v.w;
    result.y = m.m[1] * v.x + m.m[5] * v.y + m.m[9] * v.z + m.m[13] * v.w;
    result.z = m.m[2] * v.x + m.m[6] * v.y + m.m[10] * v.z + m.m[14] * v.w;
    result.w = m.m[3] * v.x + m.m[7] * v.y + m.m[11] * v.z + m.m[15] * v.w;
    return result;
}

mat4 mat4_transpose(mat4 m) {
    mat4 result;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result.m[i * 4 + j] = m.m[j * 4 + i];
        }
    }
    return result;
}

mat4 mat4_inverse(mat4 m) {
    // Simplified inverse for affine matrices (translation, rotation, scale)
    // For full inverse, would need more complex implementation
    mat4 result = mat4_identity();

    // Extract rotation/scale part (3x3)
    float rot_scale[9] = {
        m.m[0], m.m[1], m.m[2],
        m.m[4], m.m[5], m.m[6],
        m.m[8], m.m[9], m.m[10]
    };

    // Transpose rotation/scale
    result.m[0] = rot_scale[0];
    result.m[1] = rot_scale[3];
    result.m[2] = rot_scale[6];
    result.m[4] = rot_scale[1];
    result.m[5] = rot_scale[4];
    result.m[6] = rot_scale[7];
    result.m[8] = rot_scale[2];
    result.m[9] = rot_scale[5];
    result.m[10] = rot_scale[8];

    // Handle translation
    vec3 translation = {m.m[12], m.m[13], m.m[14]};
    vec3 inv_translation = vec3_mul(translation, -1.0f);
    vec4 inv_trans_vec4 = {inv_translation.x, inv_translation.y, inv_translation.z, 1.0f};
    vec4 transformed = mat4_multiply_vec4(result, inv_trans_vec4);
    result.m[12] = transformed.x;
    result.m[13] = transformed.y;
    result.m[14] = transformed.z;

    return result;
}

void mat4_print(mat4 m) {
    printf("Matrix:\n");
    for (int i = 0; i < 4; i++) {
        printf("  %.3f %.3f %.3f %.3f\n",
               m.m[i * 4 + 0], m.m[i * 4 + 1], m.m[i * 4 + 2], m.m[i * 4 + 3]);
    }
}