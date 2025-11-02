#include "input.h"
#include <math.h>
#include "../math/matrix.h"

void initCamera(Camera* camera, vec3 position) {
    camera->position = position;
    camera->yaw = -90.0f;  // Start facing negative Z
    camera->pitch = 0.0f;
    camera->speed = 2.5f;
    camera->sensitivity = 0.1f;
}

void updateCamera(Camera* camera, SDL_Window* window, float deltaTime) {
    // Mouse look using relative movement
    int mouseX, mouseY;
    SDL_GetRelativeMouseState(&mouseX, &mouseY);

    float xoffset = (float)mouseX * camera->sensitivity;
    float yoffset = (float)mouseY * camera->sensitivity; // Reversed for Y

    camera->yaw += xoffset;
    camera->pitch -= yoffset; // Note: changed to -= for correct direction

    // Wrap yaw to prevent floating point precision issues
    if (camera->yaw > 360.0f) camera->yaw -= 360.0f;
    if (camera->yaw < 0.0f) camera->yaw += 360.0f;

    // Constrain pitch
    if (camera->pitch > 89.0f) camera->pitch = 89.0f;
    if (camera->pitch < -89.0f) camera->pitch = -89.0f;

    // Keyboard movement
    const Uint8* keys = SDL_GetKeyboardState(NULL);

    vec3 front = vec3_create(
        cosf(camera->yaw * (3.14159f / 180.0f)) * cosf(camera->pitch * (3.14159f / 180.0f)),
        sinf(camera->pitch * (3.14159f / 180.0f)),
        sinf(camera->yaw * (3.14159f / 180.0f)) * cosf(camera->pitch * (3.14159f / 180.0f))
    );
    front = vec3_normalize(front);

    vec3 right = vec3_cross(front, vec3_create(0.0f, 1.0f, 0.0f));
    right = vec3_normalize(right);

    vec3 up = vec3_cross(right, front);

    float velocity = camera->speed * deltaTime;

    if (keys[SDL_SCANCODE_W])
        camera->position = vec3_add(camera->position, vec3_mul(front, velocity));
    if (keys[SDL_SCANCODE_S])
        camera->position = vec3_sub(camera->position, vec3_mul(front, velocity));
    if (keys[SDL_SCANCODE_A])
        camera->position = vec3_sub(camera->position, vec3_mul(right, velocity));
    if (keys[SDL_SCANCODE_D])
        camera->position = vec3_add(camera->position, vec3_mul(right, velocity));
    if (keys[SDL_SCANCODE_SPACE])
        camera->position = vec3_add(camera->position, vec3_mul(up, velocity));
    if (keys[SDL_SCANCODE_LSHIFT])
        camera->position = vec3_sub(camera->position, vec3_mul(up, velocity));
}

mat4 getCameraViewMatrix(Camera* camera) {
    vec3 front = vec3_create(
        cosf(camera->yaw * (3.14159f / 180.0f)) * cosf(camera->pitch * (3.14159f / 180.0f)),
        sinf(camera->pitch * (3.14159f / 180.0f)),
        sinf(camera->yaw * (3.14159f / 180.0f)) * cosf(camera->pitch * (3.14159f / 180.0f))
    );
    front = vec3_normalize(front);

    vec3 center = vec3_add(camera->position, front);
    vec3 up = vec3_create(0.0f, 1.0f, 0.0f);

    return mat4_look_at(camera->position, center, up);
}