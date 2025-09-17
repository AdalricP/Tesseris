#include "sdl_window.h"
#include "common.h"
#include <stdio.h>

int initializeSDLWindow(SDL_Window** window) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    *window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN
    );

    if (*window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    return 0;
}

void cleanupSDLWindow(SDL_Window* window) {
    if (window) {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
<<<<<<< HEAD
}
=======
}
>>>>>>> 813ef9202927b4729e27964c7099ea970d1998a6
