#include "synchronization.h"
#include <stdio.h>
#include <string.h>

VkResult createFrameSync(
    VkDevice device,
    FrameSync* outSync
) {
    if (!device || !outSync) {
        printf("Frame sync creation failed: Invalid parameters\n");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    memset(outSync, 0, sizeof(FrameSync));

    printf("  Creating frame synchronization objects:\n");

    // Create semaphores
    VkSemaphoreCreateInfo semaphoreInfo = {0};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkResult result = vkCreateSemaphore(device, &semaphoreInfo, NULL, &outSync->imageAvailableSemaphore);
    if (result != VK_SUCCESS) {
        printf("    Failed to create imageAvailable semaphore! Error: %d\n", result);
        return result;
    }
    printf("    Image Available Semaphore: %p\n", (void*)outSync->imageAvailableSemaphore);

    result = vkCreateSemaphore(device, &semaphoreInfo, NULL, &outSync->renderFinishedSemaphore);
    if (result != VK_SUCCESS) {
        printf("    Failed to create renderFinished semaphore! Error: %d\n", result);
        vkDestroySemaphore(device, outSync->imageAvailableSemaphore, NULL);
        return result;
    }
    printf("    Render Finished Semaphore: %p\n", (void*)outSync->renderFinishedSemaphore);

    // Create fence (starts in signaled state so first frame doesn't wait)
    VkFenceCreateInfo fenceInfo = {0};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;  // Start signaled

    result = vkCreateFence(device, &fenceInfo, NULL, &outSync->inFlightFence);
    if (result != VK_SUCCESS) {
        printf("    Failed to create inFlight fence! Error: %d\n", result);
        vkDestroySemaphore(device, outSync->renderFinishedSemaphore, NULL);
        vkDestroySemaphore(device, outSync->imageAvailableSemaphore, NULL);
        return result;
    }
    printf("    In-Flight Fence: %p (created in signaled state)\n", (void*)outSync->inFlightFence);

    printf("    Frame sync objects created successfully\n");
    return VK_SUCCESS;
}

void destroyFrameSync(
    VkDevice device,
    FrameSync* sync
) {
    if (!device || !sync) return;

    printf("  Destroying frame synchronization objects:\n");

    if (sync->inFlightFence != VK_NULL_HANDLE) {
        printf("    Destroying fence: %p\n", (void*)sync->inFlightFence);
        vkDestroyFence(device, sync->inFlightFence, NULL);
        sync->inFlightFence = VK_NULL_HANDLE;
    }

    if (sync->renderFinishedSemaphore != VK_NULL_HANDLE) {
        printf("    Destroying renderFinished semaphore: %p\n", (void*)sync->renderFinishedSemaphore);
        vkDestroySemaphore(device, sync->renderFinishedSemaphore, NULL);
        sync->renderFinishedSemaphore = VK_NULL_HANDLE;
    }

    if (sync->imageAvailableSemaphore != VK_NULL_HANDLE) {
        printf("    Destroying imageAvailable semaphore: %p\n", (void*)sync->imageAvailableSemaphore);
        vkDestroySemaphore(device, sync->imageAvailableSemaphore, NULL);
        sync->imageAvailableSemaphore = VK_NULL_HANDLE;
    }

    printf("    Frame sync cleanup complete\n");
}

VkResult waitForFence(
    VkDevice device,
    VkFence fence,
    uint64_t timeout
) {
    if (!device || fence == VK_NULL_HANDLE) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    VkResult result = vkWaitForFences(device, 1, &fence, VK_TRUE, timeout);
    
    if (result == VK_SUCCESS) {
        // Fence is signaled
        return VK_SUCCESS;
    } else if (result == VK_TIMEOUT) {
        printf("    Warning: Fence wait timed out\n");
        return VK_TIMEOUT;
    } else {
        printf("    Error waiting for fence! Error: %d\n", result);
        return result;
    }
}

VkResult resetFence(
    VkDevice device,
    VkFence fence
) {
    if (!device || fence == VK_NULL_HANDLE) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    VkResult result = vkResetFences(device, 1, &fence);
    if (result != VK_SUCCESS) {
        printf("    Error resetting fence! Error: %d\n", result);
    }
    return result;
}
