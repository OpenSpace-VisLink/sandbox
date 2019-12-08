#ifndef SANDBOX_GRAPHICS_VULKAN_RENDER_VULKAN_FRAME_BUFFER_H_
#define SANDBOX_GRAPHICS_VULKAN_RENDER_VULKAN_FRAME_BUFFER_H_

#include "sandbox/Component.h"
#include "sandbox/graphics/vulkan/VulkanDeviceRenderer.h"

namespace sandbox {

class VulkanFramebuffer : public VulkanRenderObject {
public:
	VulkanFramebuffer() { addType<VulkanFramebuffer>(); }
	virtual ~VulkanFramebuffer() {}

	void finishRender(const GraphicsContext& context, VulkanDeviceState& state) {
		if (state.getRenderMode().get() == VULKAN_RENDER_CLEANUP) {
			vkDestroyFramebuffer(state.getDevice()->getDevice(), getFramebuffer(context), nullptr);
			std::cout << "Destroy framebuffer." << std::endl;
		}
	}

	virtual VkFramebuffer getFramebuffer(const GraphicsContext& context) const = 0;
};


}

#endif