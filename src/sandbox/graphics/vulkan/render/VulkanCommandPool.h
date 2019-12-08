#ifndef SANDBOX_GRAPHICS_VULKAN_RENDER_VULKAN_COMMAND_POOL_H_
#define SANDBOX_GRAPHICS_VULKAN_RENDER_VULKAN_COMMAND_POOL_H_

#include "sandbox/Component.h"
#include "sandbox/graphics/vulkan/VulkanDeviceRenderer.h"

namespace sandbox {

class VulkanCommandPool : public VulkanRenderObject {
public:
	VulkanCommandPool(VulkanQueue* queue) : queue(queue) { addType<VulkanCommandPool>(); }
	virtual ~VulkanCommandPool() {
	}

	VkCommandPool getCommandPool(const GraphicsContext& context) const {
		return contextHandler.getSharedState(context)->commandPool;
	}

	void update() {
		device = getEntity().getComponentRecursive<VulkanDevice>(false);
	}

	VkCommandBuffer beginSingleTimeCommands(const GraphicsContext& context) {
        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = getCommandPool(context);
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(device->getDevice(), &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        return commandBuffer;
    }

    void endSingleTimeCommands(const GraphicsContext& context, VkCommandBuffer commandBuffer) {
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(queue->getQueue(), 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(queue->getQueue());

        vkFreeCommandBuffers(device->getDevice(), getCommandPool(context), 1, &commandBuffer);
    }

protected:

	void startRender(const GraphicsContext& context, VulkanDeviceState& state);
	void finishRender(const GraphicsContext& context, VulkanDeviceState& state);


private:
	//VkCommandPool commandPool;
	VulkanQueue* queue;
	VulkanDevice* device;

	struct CommandPoolState : public ContextState {
		VkCommandPool commandPool;
	};

	GraphicsContextHandler<CommandPoolState,ContextState> contextHandler;
};

}

#endif