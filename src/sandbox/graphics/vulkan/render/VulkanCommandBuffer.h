#ifndef SANDBOX_GRAPHICS_VULKAN_RENDER_VULKAN_COMMAND_BUFFER_H_
#define SANDBOX_GRAPHICS_VULKAN_RENDER_VULKAN_COMMAND_BUFFER_H_

#include "sandbox/Component.h"
#include "sandbox/graphics/vulkan/VulkanDeviceRenderer.h"

namespace sandbox {

class VulkanCommandBuffer : public VulkanRenderObject {
public:
	VulkanCommandBuffer() { addType<VulkanCommandBuffer>(); }
	virtual ~VulkanCommandBuffer() {}

	VkCommandBuffer getCommandBuffer(const GraphicsContext& context) const { return contextHandler.getState(context)->commandBuffer; }

protected:
	void startRender(const GraphicsContext& context, VulkanDeviceState& state);
	void finishRender(const GraphicsContext& context, VulkanDeviceState& state);

private:
	struct CommandBufferState : public ContextState {
		CommandBufferState() {} 
		VkCommandBuffer commandBuffer;
	};

	GraphicsContextHandler<ContextState,CommandBufferState> contextHandler;
};



}

#endif