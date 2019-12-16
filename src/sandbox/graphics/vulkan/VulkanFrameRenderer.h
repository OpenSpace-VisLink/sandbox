#ifndef SANDBOX_GRAPHICS_VULKAN_FRAME_RENDERER_H_
#define SANDBOX_GRAPHICS_VULKAN_FRAME_RENDERER_H_

#include "sandbox/Component.h"
#include "sandbox/graphics/vulkan/VulkanDevice.h"
#include "sandbox/graphics/vulkan/VulkanQueue.h"

namespace sandbox {

class VulkanFrameRenderer : public VulkanDeviceComponent {
public:
	VulkanFrameRenderer(VulkanQueue* graphicsQueue, VulkanQueue* presentQueue) : graphicsQueue(graphicsQueue), presentQueue(presentQueue), initialized(false) { addType<VulkanFrameRenderer>(); }
	virtual ~VulkanFrameRenderer() {
		for (size_t i = 0; i < maxFramesInFlight; i++) {
            vkDestroySemaphore(getDevice().getDevice(), renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(getDevice().getDevice(), imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(getDevice().getDevice(), inFlightFences[i], nullptr);
        }
	}

	void recreateSwapChain(VulkanSwapChain* swapChain) {
        /*int width = 0, height = 0;
        while (width == 0 || height == 0) {
            glfwGetFramebufferSize(window, &width, &height);
            glfwWaitEvents();
        }*/

        vkDeviceWaitIdle(getDevice().getDevice());

        swapChain->getEntity().incrementVersion();
        swapChain->getEntity().update();

        std::cout << swapChain << std::endl;
        std::vector<VulkanDeviceRenderer*> renderers = swapChain->getEntity().getComponentsRecursive<VulkanDeviceRenderer>();
        std::cout << "num renderers: " << renderers.size() << std::endl;
        renderers[0]->render(VULKAN_RENDER_UPDATE_SHARED);
        renderers[0]->render(VULKAN_RENDER_UPDATE_DISPLAY);
        for (int f = 0; f < renderers.size(); f++) {  
            renderers[f]->render(VULKAN_RENDER_UPDATE);
            renderers[f]->render(VULKAN_RENDER_OBJECT);
            renderers[f]->render(VULKAN_RENDER_COMMAND);
        }

    }

    void update() {
    	if (!initialized) {
    		createSyncObjects();
    		initialized = true;
    	}
    }

    void createSyncObjects() {
        imageAvailableSemaphores.resize(maxFramesInFlight);
        renderFinishedSemaphores.resize(maxFramesInFlight);
        inFlightFences.resize(maxFramesInFlight);

        VkSemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < maxFramesInFlight; i++) {
            if (vkCreateSemaphore(getDevice().getDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(getDevice().getDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(getDevice().getDevice(), &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create synchronization objects for a frame!");
            }
        }
    }

    void drawFrame(bool incrementCurrentFrame = true, Entity* renderNode = NULL) {
    	if (!renderNode) {
    		renderNode = &getEntity();
    	}

        vkWaitForFences(getDevice().getDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

        VulkanSwapChain* swapChain = renderNode->getComponent<VulkanSwapChain>();

        uint32_t imageIndex;
        VkResult result = swapChain->acquireNextImage(imageAvailableSemaphores[currentFrame], &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            std::cout << "Out of date" << std::endl;
            recreateSwapChain(swapChain);
            return;
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        VulkanDeviceRenderer* vulkanRenderer = renderNode->getComponents<VulkanDeviceRenderer>()[imageIndex];
        vulkanRenderer->render(VULKAN_RENDER_OBJECT);

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        VkCommandBuffer cmdBuffer = renderNode->getComponentRecursive<VulkanCommandBuffer>()->getCommandBuffer(vulkanRenderer->getContext());
        submitInfo.pCommandBuffers = &cmdBuffer;

        VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        vkResetFences(getDevice().getDevice(), 1, &inFlightFences[currentFrame]);

        if (vkQueueSubmit(graphicsQueue->getQueue(), 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        /*VkSwapchainKHR swapChains[] = {swapChain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;*/

        presentInfo.pImageIndices = &imageIndex;

        result = swapChain->queuePresent(presentQueue->getQueue(), presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
            std::cout << "Out of date / suboptimal / resized" << std::endl;
            framebufferResized = false;
            recreateSwapChain(renderNode->getComponent<VulkanSwapChain>());
        } else if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }

        if (incrementCurrentFrame) {
        	incrementFrame();
        }
    }

    void incrementFrame() {
    	currentFrame = (currentFrame + 1) % maxFramesInFlight;
    }

private:
	std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    size_t currentFrame = 0;
    bool framebufferResized = false;
    int maxFramesInFlight = 2;
    VulkanQueue* graphicsQueue;
    VulkanQueue* presentQueue;
    bool initialized;
};

}

#endif