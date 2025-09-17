#include <stdio.h>
#include "application.h"

int main(void) {
    ApplicationContext app = {0};
    
    // Initialize the application
    if (initializeApplication(&app) != 0) {
        printf("Failed to initialize application!\n");
        return -1;
    }
    
    // Print device information
    printDeviceInfo(&app);
    
    // Run the main application loop
    runApplication(&app);
    
    // Cleanup
    cleanupApplication(&app);
    
    return 0;
}
