#ifndef SYNCHRONIZATION_H
#define SYNCHRONIZATION_H

#include <vulkan/vulkan.h>
#include <stdbool.h>

/**
 * Frame synchronization objects
 * Used to coordinate GPU-GPU and CPU-GPU synchronization for frame rendering
 */
typedef struct {
    VkSemaphore imageAvailableSemaphore;  // Signals when swapchain image is ready
    VkSemaphore renderFinishedSemaphore;  // Signals when rendering is complete
    VkFence inFlightFence;                // Signals when frame has finished rendering (CPU waits)
} FrameSync;

/**
 * Create synchronization objects for frame rendering
 * 
 * @param device - VkDevice handle
 * @param outSync - Output synchronization objects
 * @return VK_SUCCESS on success, error code otherwise
 */
VkResult createFrameSync(
    VkDevice device,
    FrameSync* outSync
);

/**
 * Destroy synchronization objects
 * 
 * @param device - VkDevice handle
 * @param sync - Synchronization objects to destroy
 */
void destroyFrameSync(
    VkDevice device,
    FrameSync* sync
);

/**
 * Wait for a fence to be signaled
 * 
 * @param device - VkDevice handle
 * @param fence - Fence to wait for
 * @param timeout - Timeout in nanoseconds (UINT64_MAX for infinite)
 * @return VK_SUCCESS if fence signaled, VK_TIMEOUT if timeout, error otherwise
 */
VkResult waitForFence(
    VkDevice device,
    VkFence fence,
    uint64_t timeout
);

/**
 * Reset a fence to unsignaled state
 * 
 * @param device - VkDevice handle
 * @param fence - Fence to reset
 * @return VK_SUCCESS on success, error code otherwise
 */
VkResult resetFence(
    VkDevice device,
    VkFence fence
);

#endif // SYNCHRONIZATION_H
