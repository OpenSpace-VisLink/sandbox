
#ifndef SANDBOX_VULCAN_H_
#define SANDBOX_VULCAN_H_

#include "sandbox/Component.h"
#include "sandbox/graphics/GraphicsContext.h"
#include "sandbox/graphics/RenderNode.h"
#include "sandbox/graphics/GraphicsRenderer.h"

#include "sandbox/graphics/vulkan/Vulkan.h"
#include "sandbox/graphics/vulkan/VulkanInstance.h"
#include "sandbox/graphics/vulkan/VulkanSurface.h"
#include "sandbox/graphics/vulkan/VulkanQueue.h"
#include "sandbox/graphics/vulkan/VulkanDevice.h"
#include "sandbox/graphics/vulkan/VulkanSwapChain.h"
#include "sandbox/graphics/vulkan/render/VulkanRenderPass.h"
#include "sandbox/graphics/vulkan/render/VulkanCommandPool.h"
#include "sandbox/graphics/vulkan/render/VulkanCommandBuffer.h"
#include "sandbox/graphics/vulkan/render/impl/VulkanBasicRenderPass.h"
#include "sandbox/graphics/vulkan/render/impl/VulkanSwapChainFramebuffer.h"
#include "sandbox/graphics/vulkan/render/VulkanFramebuffer.h"
#include "sandbox/graphics/vulkan/VulkanDeviceState.h"
#include "sandbox/graphics/vulkan/VulkanFrameRenderer.h"
#include "sandbox/graphics/vulkan/shader/VulkanShaderModule.h"
#include "sandbox/graphics/vulkan/image/VulkanImage.h"
#include "sandbox/graphics/vulkan/image/VulkanImageView.h"
#include "sandbox/graphics/vulkan/buffer/VulkanBuffer.h"
#include "sandbox/graphics/vulkan/buffer/VulkanVertexInput.h"
#include "sandbox/graphics/vulkan/shader/VulkanShaderObject.h"
#include "sandbox/graphics/vulkan/buffer/VulkanBufferTypes.h"
#include "sandbox/graphics/vulkan/image/VulkanSampler.h"
#include "sandbox/graphics/vulkan/shader/VulkanDescriptor.h"
#include "sandbox/graphics/vulkan/render/VulkanGraphicsPipeline.h"



#include <iostream>
#include <fstream>


namespace sandbox {


class VulkanCmdBindDescriptorSet : public VulkanRenderObject {
public:
	VulkanCmdBindDescriptorSet(VulkanDescriptorSet* descriptorSet, int setBinding) : descriptorSet(descriptorSet), setBinding(setBinding), offset(offset) { addType<VulkanCmdBindDescriptorSet>(); }
	virtual ~VulkanCmdBindDescriptorSet() {}

protected:
	void startRender(const GraphicsContext& context, VulkanDeviceState& state) {
		if ((state.getRenderMode().get() & VULKAN_RENDER_COMMAND) == VULKAN_RENDER_COMMAND) {
			VkDescriptorSet descSet = descriptorSet->getDescriptorSet(context);
            std::cout << VulkanSwapChainState::get(context).getSwapChain()->getName() << " " << descSet << std::endl;
            std::cout << "Pipeline: " << state.getGraphicsPipeline().get()->getGraphicsPipeline(context) << std::endl;
            vkCmdBindDescriptorSets(state.getCommandBuffer().get()->getCommandBuffer(context), VK_PIPELINE_BIND_POINT_GRAPHICS, state.getGraphicsPipeline().get()->getPipelineLayout(context), setBinding, 1, &descSet, 0, nullptr);
		}
	}

private:
	VulkanDescriptorSet* descriptorSet;
	int setBinding;
	uint32_t offset;
};

class VulkanCmdBindDynamicDescriptorSet : public VulkanRenderObject {
public:
	VulkanCmdBindDynamicDescriptorSet(VulkanDescriptorSet* descriptorSet, VulkanUniformBuffer* uniformBuffer, int setBinding, int index) : descriptorSet(descriptorSet), uniformBuffer(uniformBuffer), setBinding(setBinding), index(index) { addType<VulkanCmdBindDynamicDescriptorSet>(); }
	virtual ~VulkanCmdBindDynamicDescriptorSet() {}

protected:
	void startRender(const GraphicsContext& context, VulkanDeviceState& state) {
		if ((state.getRenderMode().get() & VULKAN_RENDER_COMMAND) == VULKAN_RENDER_COMMAND) {
			uint32_t offset = index * uniformBuffer->getRange();
			VkDescriptorSet descSet = descriptorSet->getDescriptorSet(context);
            std::cout << VulkanSwapChainState::get(context).getSwapChain()->getName() << " " << descSet << std::endl;
            vkCmdBindDescriptorSets(state.getCommandBuffer().get()->getCommandBuffer(context), VK_PIPELINE_BIND_POINT_GRAPHICS, state.getGraphicsPipeline().get()->getPipelineLayout(context), setBinding, 1, &descSet, 1, &offset);
		}
	}

private:
	VulkanDescriptorSet* descriptorSet;
	VulkanUniformBuffer* uniformBuffer;
	int setBinding;
	int index;
};


}

#endif
