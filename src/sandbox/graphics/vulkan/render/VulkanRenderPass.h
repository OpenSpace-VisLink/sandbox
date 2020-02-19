#ifndef SANDBOX_GRAPHICS_VULKAN_RENDER_VULKAN_RENDER_PASS_H_
#define SANDBOX_GRAPHICS_VULKAN_RENDER_VULKAN_RENDER_PASS_H_

#include "sandbox/Component.h"
#include "sandbox/graphics/vulkan/VulkanDeviceRenderer.h"

namespace sandbox {

class VulkanRenderPass : public VulkanRenderObject {
public:
	VulkanRenderPass() { addType<VulkanRenderPass>(); }
	virtual ~VulkanRenderPass() {}

	virtual VkRenderPass getRenderPass(const GraphicsContext& context) const = 0;

	void startRender(const GraphicsContext& context, VulkanDeviceState& state);
	void finishRender(const GraphicsContext& context, VulkanDeviceState& state);
};


}

#endif