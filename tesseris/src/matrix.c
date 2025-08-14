#include "vector.h"
#include <math.h>

typedef struct {
    float m[4][4];
} Mat4;

Mat4 mat4_identity() {
    Mat4 m = {0};
    for (int i = 0; i < 4; ++i) m.m[i][i] = 1.0f;
    return m;
}

Mat4 mat4_mul(Mat4 a, Mat4 b) {
    Mat4 result = {0};
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            for (int k = 0; k < 4; ++k) {
                result.m[row][col] += a.m[row][k] * b.m[k][col];
            }
        }
    }
    return result;
}

Vec4 mat4_mul_vec4(Mat4 m, Vec4 v) {
    Vec4 result;
    result.x = m.m[0][0]*v.x + m.m[0][1]*v.y + m.m[0][2]*v.z + m.m[0][3]*v.w;
    result.y = m.m[1][0]*v.x + m.m[1][1]*v.y + m.m[1][2]*v.z + m.m[1][3]*v.w;
    result.z = m.m[2][0]*v.x + m.m[2][1]*v.y + m.m[2][2]*v.z + m.m[2][3]*v.w;
    result.w = m.m[3][0]*v.x + m.m[3][1]*v.y + m.m[3][2]*v.z + m.m[3][3]*v.w;
    return result;
}

Mat4 mat4_translation(float tx, float ty, float tz) {
    Mat4 m = mat4_identity();
    m.m[0][3] = tx;
    m.m[1][3] = ty;
    m.m[2][3] = tz;
    return m;
}

Mat4 mat4_rotation_x(float angle_rad) {
    Mat4 m = mat4_identity();
    float c = cosf(angle_rad);
    float s = sinf(angle_rad);
    m.m[1][1] = c;
    m.m[1][2] = -s;
    m.m[2][1] = s;
    m.m[2][2] = c;
    return m;
}

Mat4 mat4_rotation_y(float angle_rad) {
    Mat4 m = mat4_identity();
    float c = cosf(angle_rad);
    float s = sinf(angle_rad);
    m.m[0][0] = c;
    m.m[0][2] = s;
    m.m[2][0] = -s;
    m.m[2][2] = c;
    return m;
}

Mat4 mat4_rotation_z(float angle_rad) {
    Mat4 m = mat4_identity();
    float c = cosf(angle_rad);
    float s = sinf(angle_rad);
    m.m[0][0] = c;
    m.m[0][1] = -s;
    m.m[1][0] = s;
    m.m[1][1] = c;
    return m; 
}

Mat4 mat4_perspective(float fov, float aspect, float near, float far) {
    Mat4 m = {0}; 
    float f = 1.0f / tanf(fov / 2.0f);
    m.m[0][0] = f / aspect;
    m.m[1][1] = f;
    m.m[2][2] = (far + near) / (near - far);
    m.m[2][3] = (2 * far * near) / (near - far);
    m.m[3][2] = -1.0f;
    return m;
}
