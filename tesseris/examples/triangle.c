#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include "../src/vector.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

void draw_line(SDL_Renderer* renderer, Vec2 start, Vec2 end) {
    SDL_RenderDrawLine(renderer, 
        (int)start.x, (int)start.y,
        (int)end.x, (int)end.y
    );
}

void draw_triangle(SDL_Renderer* renderer, Vec2 v1, Vec2 v2, Vec2 v3) {
    // Draw three lines to form a triangle
    draw_line(renderer, v1, v2);
    draw_line(renderer, v2, v3);
    draw_line(renderer, v3, v1);
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Triangle Example",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );

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

    // Define triangle vertices
    Vec2 v1 = (Vec2){ SCREEN_WIDTH/2, 100 };                    // Top vertex
    Vec2 v2 = (Vec2){ SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT-100 }; // Bottom left
    Vec2 v3 = (Vec2){ SCREEN_WIDTH/2 + 200, SCREEN_HEIGHT-100 }; // Bottom right

    bool running = true;
    SDL_Event event;
    float rotation = 0.0f;

    while (running) {
        // Handle input
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
                running = false;
        }

        // Clear screen (black)
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Set draw color to white for the triangle
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        
        // Draw the triangle
        draw_triangle(renderer, v1, v2, v3);

        // Present the frame
        SDL_RenderPresent(renderer);

        // Optional: delay to limit FPS
        SDL_Delay(16);  // ~60 FPS
    }

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
