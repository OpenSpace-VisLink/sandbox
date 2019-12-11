#include "sandbox/graphics/vulkan/VulkanDeviceState.h"
#include "sandbox/graphics/vulkan/render/VulkanRenderPass.h"
#include "sandbox/graphics/vulkan/render/VulkanFramebuffer.h"

namespace sandbox {

void VulkanRenderPass::startRender(const GraphicsContext& context, VulkanDeviceState& state) {
	state.getRenderPass().push(this);

	if ((state.getRenderMode().get() & VULKAN_RENDER_COMMAND) == VULKAN_RENDER_COMMAND) {
		VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = getRenderPass(context);
        renderPassInfo.framebuffer = getEntity().getComponent<VulkanFramebuffer>()->getFramebuffer(context);
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = state.getExtent();

        std::vector<VkClearValue> clearValues;
		clearValues.resize(2);
        clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(state.getCommandBuffer().get()->getCommandBuffer(context), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        std::cout << "Begin render pass command" << std::endl;
	}
	        
}

void VulkanRenderPass::finishRender(const GraphicsContext& context, VulkanDeviceState& state) {
	if ((state.getRenderMode().get() & VULKAN_RENDER_COMMAND) == VULKAN_RENDER_COMMAND) {
        std::cout << "End render pass command" << std::endl;
        vkCmdEndRenderPass(state.getCommandBuffer().get()->getCommandBuffer(context));
	}

	state.getRenderPass().pop();
}

}