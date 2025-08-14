#include "camera.h"
#include <math.h>

void camera_init(Camera* cam, Vec3 position, float yaw, float pitch) {
    cam->position = position;
    cam->yaw = yaw;
    cam->pitch = pitch;
}

void camera_move(Camera* cam, float forward, float right, float up) {
    // Calculate forward and right vectors from yaw and pitch
    float cy = cosf(cam->yaw);
    float sy = sinf(cam->yaw);
    float cp = cosf(cam->pitch);
    float sp = sinf(cam->pitch);
    Vec3 forward_vec = {sy * cp, sp, -cy * cp};
    Vec3 right_vec = {cy, 0, sy};
    Vec3 up_vec = {0, 1, 0};
    cam->position.x += forward * forward_vec.x + right * right_vec.x + up * up_vec.x;
    cam->position.y += forward * forward_vec.y + right * right_vec.y + up * up_vec.y;
    cam->position.z += forward * forward_vec.z + right * right_vec.z + up * up_vec.z;
}

Mat4 camera_get_view_matrix(Camera* cam) {
    // Simple look-at from position, yaw, pitch
    float cy = cosf(cam->yaw);
    float sy = sinf(cam->yaw);
    float cp = cosf(cam->pitch);
    float sp = sinf(cam->pitch);
    Vec3 forward = {sy * cp, sp, -cy * cp};
    Vec3 up = {0, 1, 0};
    Vec3 center = {cam->position.x + forward.x, cam->position.y + forward.y, cam->position.z + forward.z};
    // Build look-at matrix
    Vec3 f = {center.x - cam->position.x, center.y - cam->position.y, center.z - cam->position.z};
    float f_mag = sqrtf(f.x*f.x + f.y*f.y + f.z*f.z);
    f.x /= f_mag; f.y /= f_mag; f.z /= f_mag;
    Vec3 s = {up.y * f.z - up.z * f.y, up.z * f.x - up.x * f.z, up.x * f.y - up.y * f.x};
    float s_mag = sqrtf(s.x*s.x + s.y*s.y + s.z*s.z);
    s.x /= s_mag; s.y /= s_mag; s.z /= s_mag;
    Vec3 u = {f.y * s.z - f.z * s.y, f.z * s.x - f.x * s.z, f.x * s.y - f.y * s.x};
    Mat4 m = mat4_identity();
    m.m[0][0] = s.x; m.m[0][1] = s.y; m.m[0][2] = s.z;
    m.m[1][0] = u.x; m.m[1][1] = u.y; m.m[1][2] = u.z;
    m.m[2][0] = -f.x; m.m[2][1] = -f.y; m.m[2][2] = -f.z;
    m.m[0][3] = - (s.x * cam->position.x + s.y * cam->position.y + s.z * cam->position.z);
    m.m[1][3] = - (u.x * cam->position.x + u.y * cam->position.y + u.z * cam->position.z);
    m.m[2][3] = (f.x * cam->position.x + f.y * cam->position.y + f.z * cam->position.z);
    return m;
} 