#ifndef COMMON_H
#define COMMON_H

// Platform detection
#ifdef __APPLE__
#define PLATFORM_MACOS
#elif defined(_WIN32) || defined(_WIN64)
#define PLATFORM_WINDOWS
#elif defined(__linux__)
#define PLATFORM_LINUX
#endif

// Window constants
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_TITLE "tesseris"

// Background color constants
#define BACKGROUND_R 25
#define BACKGROUND_G 25
#define BACKGROUND_B 112

#endif // COMMON_H
