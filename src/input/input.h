#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>
#include "../math/vector.h"
#include "../math/matrix.h"

// Temporary input system - to be abstracted later
// Simple camera controls for development

typedef struct {
    vec3 position;
    float yaw;    // Horizontal rotation (left/right)
    float pitch;  // Vertical rotation (up/down)
    float speed;
    float sensitivity;
} Camera;

// Initialize camera with default values
void initCamera(Camera* camera, vec3 position);

// Update camera based on input (called each frame)
void updateCamera(Camera* camera, SDL_Window* window, float deltaTime);

// Get view matrix from camera
mat4 getCameraViewMatrix(Camera* camera);

#endif // INPUT_H