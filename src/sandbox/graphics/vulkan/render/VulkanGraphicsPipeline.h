#ifndef SANDBOX_GRAPHICS_VULKAN_RENDER_VULKAN_GRAPHICS_PIPELINE_H_
#define SANDBOX_GRAPHICS_VULKAN_RENDER_VULKAN_GRAPHICS_PIPELINE_H_

#include "sandbox/graphics/vulkan/VulkanDeviceRenderer.h"
#include "sandbox/graphics/vulkan/shader/VulkanDescriptor.h"

namespace sandbox {



class VulkanGraphicsPipeline : public VulkanRenderObject {
public:
	VulkanGraphicsPipeline(const std::vector<VulkanDescriptorSetLayout*>& descriptorSetLayouts) : descriptorSetLayouts(descriptorSetLayouts) { addType<VulkanGraphicsPipeline>(); }
	virtual ~VulkanGraphicsPipeline() {}

	VkPipeline getGraphicsPipeline(const GraphicsContext& context) const { return contextHandler.getDisplayState(context)->graphicsPipeline; }
	VkPipelineLayout getPipelineLayout(const GraphicsContext& context) const { return contextHandler.getDisplayState(context)->pipelineLayout; }

protected:
	void startRender(const GraphicsContext& context, VulkanDeviceState& state);
	void finishRender(const GraphicsContext& context, VulkanDeviceState& state);

private:
	struct GraphicsPipelineState : public ContextState {
		VkPipeline graphicsPipeline;
		VkPipelineLayout pipelineLayout;
	}; 

	DisplayContextHandler<ContextState,GraphicsPipelineState,ContextState> contextHandler;
	std::vector<VulkanDescriptorSetLayout*> descriptorSetLayouts;
};


}

#endif