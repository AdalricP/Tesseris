#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#ifdef __APPLE__
#define PLATFORM_MACOS
#elif defined(_WIN32) || defined(_WIN64)
#define PLATFORM_WINDOWS
#elif defined(__linux__)
#define PLATFORM_LINUX
#endif

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_TITLE "tesseris"

#define BACKGROUND_R 25
#define BACKGROUND_G 25
#define BACKGROUND_B 112

int initializeVulkan(SDL_Window* window, VkInstance* vulkanInstance);
void enumeratePhysicalDevices(VkInstance instance);

void enumeratePhysicalDevices(VkInstance instance) {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);
    
    if (deviceCount == 0) {
        printf("Failed to find GPUs with Vulkan support!\n");
        return;
    }

    printf("Found %d device(s) with Vulkan support:\n", deviceCount);

    VkPhysicalDevice* devices = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices);

    for (uint32_t i = 0; i < deviceCount; i++) {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(devices[i], &deviceProperties);
        printf("%d. Device name: %s\n", i + 1, deviceProperties.deviceName);
    }

    free(devices);
}

int initializeVulkan(SDL_Window* window, VkInstance* vulkanInstance) {
    VkApplicationInfo appInfo = {0};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Tesseris";
    appInfo.apiVersion = VK_API_VERSION_1_0;
    
    VkInstanceCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    
    unsigned int extensionCount;
    if (!SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, NULL)) {
        return -1;
    }
    
    #ifdef PLATFORM_MACOS
        const char** extensions = malloc((extensionCount + 1) * sizeof(const char*));
        if (!SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, extensions)) {
            free(extensions);
            return -1;
        }
        extensions[extensionCount] = "VK_KHR_portability_enumeration";
        createInfo.enabledExtensionCount = extensionCount + 1;
        createInfo.ppEnabledExtensionNames = extensions;
        createInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    #else
        const char** extensions = malloc(extensionCount * sizeof(const char*));
        if (!SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, extensions)) {
            free(extensions);
            return -1;
        }
        createInfo.enabledExtensionCountq = extensionCount;
        createInfo.ppEnabledExtensionNames = extensions;
        createInfo.flags = 0;
    #endif
    
    VkResult result = vkCreateInstance(&createInfo, NULL, vulkanInstance);
    if (result != VK_SUCCESS) {
        free(extensions);
        return -1;
    }
    
    free(extensions);
    return 0;
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN
    );

    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    VkInstance vulkanInstance;
    if (initializeVulkan(window, &vulkanInstance) != 0) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    enumeratePhysicalDevices(vulkanInstance);

    // Main loop
    bool running = true;
    SDL_Event event;

    while (running) {
        // Handle events
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        running = false;
                    }
                    break;
            }
        }

        // Handle Rendering Here
    }


    // Cleanup 
    vkDestroyInstance(vulkanInstance, NULL);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
