#include <stdio.h>
#include "application.h"

int main(void) {
    ApplicationContext app = {0};
    
    if (initializeApplication(&app) != 0) {
        printf("Failed to initialize application!\n");
        return -1;
    }
    
    printDeviceInfo(&app);
    
    runApplication(&app);
    
    cleanupApplication(&app);
    
    return 0;
}
