#ifndef SDL_WINDOW_H
#define SDL_WINDOW_H

#include <SDL2/SDL.h>

/**
 * Initialize SDL and create the main window
 * @param window - Pointer to SDL_Window* to store the created window
 * @return 0 on success, -1 on failure
 */
int initializeSDLWindow(SDL_Window** window);

/**
 * Cleanup SDL window and quit SDL
 * @param window - SDL_Window to destroy
 */
void cleanupSDLWindow(SDL_Window* window);

#endif // SDL_WINDOW_H