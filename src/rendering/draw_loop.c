#include "draw_loop.h"
#include <stdio.h>

void draw_frame(ApplicationContext* app) {
    // Wait for previous frame to finish
    vkWaitForFences(app->logicalDevice.device, 1, &app->frameSync.inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(app->logicalDevice.device, 1, &app->frameSync.inFlightFence);

    // Acquire next swapchain image
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(app->logicalDevice.device, app->swapchain.swapchain, UINT64_MAX, app->frameSync.imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        // Recreate swapchain on resize (reuse toggle_vsync logic)
        toggle_vsync(app, app->vsyncEnabled);
        return;
    } else if (result != VK_SUCCESS) {
        printf("Failed to acquire swapchain image!\n");
        app->running = false;
        return;
    }

    // Record command buffer for this frame
    VkCommandBuffer cmdBuffer = app->commandBuffers[imageIndex];
    vkResetCommandBuffer(cmdBuffer, 0);
    VkCommandBufferBeginInfo beginInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    VkResult beginResult = vkBeginCommandBuffer(cmdBuffer, &beginInfo);
    if (beginResult != VK_SUCCESS) {
        printf("Failed to begin command buffer: %d\n", beginResult);
        app->running = false;
        return;
    }

    // Begin render pass
    VkRenderPassBeginInfo renderPassInfo = {VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
    renderPassInfo.renderPass = app->renderPass;
    renderPassInfo.framebuffer = app->framebuffers[imageIndex];
    renderPassInfo.renderArea.offset = (VkOffset2D){0, 0};
    renderPassInfo.renderArea.extent = app->swapchain.extent;
    VkClearValue clearValues[2] = {{{0.0f, 0.0f, 0.0f, 1.0f}}, {1.0f, 0}}; // Black background + depth
    renderPassInfo.clearValueCount = 2; // With depth
    renderPassInfo.pClearValues = clearValues;
    vkCmdBeginRenderPass(cmdBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    // Bind pipeline and draw (hardcoded triangle, no vertex buffers)
    vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, app->graphicsPipeline.pipeline);

    // Set dynamic viewport and scissor
    VkViewport viewport = {0.0f, 0.0f, (float)app->swapchain.extent.width, (float)app->swapchain.extent.height, 0.0f, 1.0f};
    vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);
    VkRect2D scissor = {{0, 0}, app->swapchain.extent};
    vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);

    vkCmdDraw(cmdBuffer, 3, 1, 0, 0); // 3 vertices, 1 instance

    vkCmdEndRenderPass(cmdBuffer);

    VkResult endResult = vkEndCommandBuffer(cmdBuffer);
    if (endResult != VK_SUCCESS) {
        printf("Failed to end command buffer: %d\n", endResult);
        app->running = false;
        return;
    }    // Submit to queue
    VkSubmitInfo submitInfo = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
    VkSemaphore waitSemaphores[] = {app->frameSync.imageAvailableSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdBuffer;
    VkSemaphore signalSemaphores[] = {app->frameSync.renderFinishedSemaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
    VkResult submitResult = vkQueueSubmit(app->logicalDevice.graphicsQueue, 1, &submitInfo, app->frameSync.inFlightFence);
    if (submitResult != VK_SUCCESS) {
        printf("Failed to submit queue: %d\n", submitResult);
        app->running = false;
        return;
    }

    // Present
    VkPresentInfoKHR presentInfo = {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    VkSwapchainKHR swapchains[] = {app->swapchain.swapchain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;
    presentInfo.pImageIndices = &imageIndex;
    VkResult presentResult = vkQueuePresentKHR(app->logicalDevice.presentQueue, &presentInfo);
    if (presentResult != VK_SUCCESS) {
        printf("Failed to present: %d\n", presentResult);
        app->running = false;
        return;
    }
}