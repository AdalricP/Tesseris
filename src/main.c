#include <stdio.h>
#include <string.h>
#include "application.h"
#include "model_loaders/objloader.h"

int main(int argc, char* argv[]) {
    ApplicationContext app = {0};
    
    // Check for OBJ file argument
    bool useMesh = false;
    if (argc > 1) {
        if (load_obj(argv[1], &app.mesh) == 0) {
            useMesh = true;
            printf("Loaded OBJ file: %s\n", argv[1]);
        } else {
            printf("Failed to load OBJ file: %s, using default cube\n", argv[1]);
        }
    } else {
        printf("No OBJ file specified, using default cube\n");
    }
    
    if (!useMesh) {
        // Initialize mesh to empty for cube
        memset(&app.mesh, 0, sizeof(Mesh));
    }
    
    if (initializeApplication(&app) != 0) {
        printf("Failed to initialize application!\n");
        if (useMesh) {
            free_mesh(&app.mesh);
        }
        return -1;
    }
    
    printDeviceInfo(&app);
    
    runApplication(&app);
    
    cleanupApplication(&app);
    
    if (useMesh) {
        free_mesh(&app.mesh);
    }
    
    return 0;
}
