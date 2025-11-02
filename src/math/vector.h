#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>
#include <stdbool.h>

/**
 * 2D vector type
 */
typedef struct {
    float x, y;
} vec2;

/**
 * 3D vector type
 */
typedef struct {
    float x, y, z;
} vec3;

/**
 * 4D vector type
 */
typedef struct {
    float x, y, z, w;
} vec4;

/**
 * Vector creation functions
 */
static inline vec2 vec2_create(float x, float y) {
    return (vec2){x, y};
}

static inline vec3 vec3_create(float x, float y, float z) {
    return (vec3){x, y, z};
}

static inline vec4 vec4_create(float x, float y, float z, float w) {
    return (vec4){x, y, z, w};
}

/**
 * Vector operations
 */
static inline vec3 vec3_add(vec3 a, vec3 b) {
    return (vec3){a.x + b.x, a.y + b.y, a.z + b.z};
}

static inline vec3 vec3_sub(vec3 a, vec3 b) {
    return (vec3){a.x - b.x, a.y - b.y, a.z - b.z};
}

static inline vec3 vec3_mul(vec3 a, float scalar) {
    return (vec3){a.x * scalar, a.y * scalar, a.z * scalar};
}

static inline float vec3_dot(vec3 a, vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static inline vec3 vec3_cross(vec3 a, vec3 b) {
    return (vec3){
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

static inline float vec3_length(vec3 v) {
    return sqrtf(vec3_dot(v, v));
}

static inline vec3 vec3_normalize(vec3 v) {
    float len = vec3_length(v);
    if (len > 0.0f) {
        return vec3_mul(v, 1.0f / len);
    }
    return v;
}

static inline bool vec3_equals(vec3 a, vec3 b, float epsilon) {
    return fabsf(a.x - b.x) < epsilon &&
           fabsf(a.y - b.y) < epsilon &&
           fabsf(a.z - b.z) < epsilon;
}

#endif // VECTOR_H