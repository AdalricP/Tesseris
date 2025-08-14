// vector.c

#include "vector.h"
#include <math.h>

// Vec2
Vec2 vec2_add(Vec2 a, Vec2 b) {
    return (Vec2){ a.x + b.x, a.y + b.y };
}

Vec2 vec2_sub(Vec2 a, Vec2 b) {
    return (Vec2){ a.x - b.x, a.y - b.y };
}

// Vec3
Vec3 vec3_add(Vec3 a, Vec3 b) {
    return (Vec3){ a.x + b.x, a.y + b.y, a.z + b.z };
}

Vec3 vec3_sub(Vec3 a, Vec3 b) {
    return (Vec3){ a.x - b.x, a.y - b.y, a.z - b.z };
}

Vec3 vec3_cross(Vec3 a, Vec3 b) {
    return (Vec3){
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

float vec3_dot(Vec3 a, Vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3 vec3_scale(Vec3 v, float s) {
    return (Vec3){ v.x * s, v.y * s, v.z * s };
}

Vec3 vec3_normalize(Vec3 v) {
    float mag = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    if (mag == 0.0f) return v;
    return vec3_scale(v, 1.0f / mag);
}

// Vec4
Vec4 vec4_add(Vec4 a, Vec4 b) {
    return (Vec4){ a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
}

Vec4 vec4_sub(Vec4 a, Vec4 b) {
    return (Vec4){ a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
}

Vec3 vec4_to_vec3(Vec4 v) {
    if (v.w == 0.0f) return (Vec3){ v.x, v.y, v.z }; // avoid div-by-zero
    return (Vec3){ v.x / v.w, v.y / v.w, v.z / v.w };
}
