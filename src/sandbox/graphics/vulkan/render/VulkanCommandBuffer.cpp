#include "sandbox/graphics/vulkan/VulkanDeviceState.h"
#include "sandbox/graphics/vulkan/render/VulkanCommandBuffer.h"

namespace sandbox {

void VulkanCommandBuffer::startRender(const GraphicsContext& context, VulkanDeviceState& state) {
	state.getCommandBuffer().push(this);

	if (state.getRenderMode().get() == VULKAN_RENDER_UPDATE) {
		CommandBufferState* commandBufferState = contextHandler.getState(context);
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = state.getCommandPool().get()->getCommandPool(context);
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(state.getDevice()->getDevice(), &allocInfo, &commandBufferState->commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }   
	}

	if ((state.getRenderMode().get() & VULKAN_RENDER_COMMAND) == VULKAN_RENDER_COMMAND) {
		CommandBufferState* commandBufferState = contextHandler.getState(context);

		std::cout << "Render command" << std::endl;

		/*if (commandBufferState->recorded) {
			// need to reset command buffer
		}*/

		VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBufferState->commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

	}
}

void VulkanCommandBuffer::finishRender(const GraphicsContext& context, VulkanDeviceState& state) {
	if (state.getRenderMode().get() == VULKAN_RENDER_CLEANUP) {
		CommandBufferState* commandBufferState = contextHandler.getState(context);
		vkFreeCommandBuffers(state.getDevice()->getDevice(), state.getCommandPool().get()->getCommandPool(context), 1, &commandBufferState->commandBuffer);
	}

	if (state.getRenderMode().get() == VULKAN_RENDER_COMMAND) {
		CommandBufferState* commandBufferState = contextHandler.getState(context);
		if (vkEndCommandBuffer(commandBufferState->commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }

	}

	state.getCommandBuffer().pop();
}


}