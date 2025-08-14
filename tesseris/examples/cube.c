#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "../src/vector.h"
#include "../src/matrix.h"
#include "../src/renderer.h"
#include "../src/camera.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

Vec3 cube_vertices[] = {
    {-2, -2, -2}, { 2, -2, -2}, { 2,  2, -2}, {-2,  2, -2},
    {-2, -2,  2}, { 2, -2,  2}, { 2,  2,  2}, {-2,  2,  2}
};
int cube_edges[][2] = {
    {0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 5}, {5, 6}, {6, 7}, {7, 4}, {0, 4}, {1, 5}, {2, 6}, {3, 7}
};

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }
    SDL_Window* window = SDL_CreateWindow(
        "Rotating 3D Cube",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("SDL_CreateRenderer failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    Camera cam;
    camera_init(&cam, (Vec3){0, 0, 0}, 0.0f, 0.0f);
    float rotation_x = 0.0f, rotation_y = 0.0f, rotation_z = 0.0f;
    bool running = true;
    SDL_Event event;
    const float move_speed = 0.2f;
    while (running) {
        float forward = 0, right = 0, up = 0;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE: running = false; break;
                    case SDLK_w: forward = 1; break;
                    case SDLK_s: forward = -1; break;
                    case SDLK_a: right = -1; break;
                    case SDLK_d: right = 1; break;
                    case SDLK_q: up = 1; break;
                    case SDLK_e: up = -1; break;
                }
            }
        }
        camera_move(&cam, move_speed * forward, move_speed * right, move_speed * up);
        rotation_x += 0.02f;
        rotation_y += 0.015f;
        rotation_z += 0.01f;
        SDL_SetRenderDrawColor(renderer, 0, 0, 50, 255);
        SDL_RenderClear(renderer);
        Mat4 model = mat4_identity();
        model = mat4_mul(mat4_rotation_x(rotation_x), model);
        model = mat4_mul(mat4_rotation_y(rotation_y), model);
        model = mat4_mul(mat4_rotation_z(rotation_z), model);
        model = mat4_mul(mat4_translation(0.0f, 0.0f, -10.0f), model);
        float aspect = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
        Mat4 proj = mat4_perspective(0.8f, aspect, 0.1f, 100.0f);
        Mat4 view = camera_get_view_matrix(&cam);
        Mat4 view_proj = mat4_mul(proj, mat4_mul(view, model));
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        renderer_draw_cube(renderer, view_proj, cube_vertices, cube_edges, 12);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
} 