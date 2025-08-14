#ifndef CAMERA_H
#define CAMERA_H

#include "vector.h"
#include "matrix.h"

typedef struct {
    Vec3 position;
    float yaw;
    float pitch;
} Camera;

void camera_init(Camera* cam, Vec3 position, float yaw, float pitch);
void camera_move(Camera* cam, float forward, float right, float up);
Mat4 camera_get_view_matrix(Camera* cam);

#endif 