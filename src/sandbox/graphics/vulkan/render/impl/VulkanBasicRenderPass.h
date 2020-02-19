#ifndef SANDBOX_GRAPHICS_VULKAN_RENDER_IMPL_VULKAN_BASIC_RENDER_PASS_H_
#define SANDBOX_GRAPHICS_VULKAN_RENDER_IMPL_VULKAN_BASIC_RENDER_PASS_H_

#include "sandbox/Component.h"
#include "sandbox/graphics/vulkan/render/VulkanRenderPass.h"
#include "sandbox/graphics/vulkan/VulkanDeviceState.h"

namespace sandbox {

class VulkanBasicRenderPass : public VulkanRenderPass {
public:
	VulkanBasicRenderPass() { addType<VulkanBasicRenderPass>(); }
	virtual ~VulkanBasicRenderPass() {}

	void startRender(const GraphicsContext& context, VulkanDeviceState& state) {
		VulkanRenderPass::startRender(context, state);

		if (state.getRenderMode().get() == VULKAN_RENDER_UPDATE_DISPLAY) {
			RenderPassState* sharedState = contextHandler.getDisplayState(context);
			VkAttachmentDescription colorAttachment = {};
	        colorAttachment.format = state.getImageFormat().get();
	        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	        VkAttachmentReference colorAttachmentRef = {};
	        colorAttachmentRef.attachment = 0;
	        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	        VkAttachmentDescription depthAttachment = {};
			depthAttachment.format = state.getDevice()->findDepthFormat();
			depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
			depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			VkAttachmentReference depthAttachmentRef = {};
			depthAttachmentRef.attachment = 1;
			depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	        VkSubpassDescription subpass = {};
	        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	        subpass.colorAttachmentCount = 1;
	        subpass.pColorAttachments = &colorAttachmentRef;
			subpass.pDepthStencilAttachment = &depthAttachmentRef;

	        VkSubpassDependency dependency = {};
	        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	        dependency.dstSubpass = 0;
	        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	        dependency.srcAccessMask = 0;
	        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	        std::vector<VkAttachmentDescription> attachments;
			attachments.resize(2);
			attachments[0] = colorAttachment;
			attachments[1] = depthAttachment;
			VkRenderPassCreateInfo renderPassInfo = {};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			renderPassInfo.pAttachments = attachments.data();
			renderPassInfo.subpassCount = 1;
			renderPassInfo.pSubpasses = &subpass;
			renderPassInfo.dependencyCount = 1;
			renderPassInfo.pDependencies = &dependency;

	        if (vkCreateRenderPass(state.getDevice()->getDevice(), &renderPassInfo, nullptr, &sharedState->renderPass) != VK_SUCCESS) {
	            throw std::runtime_error("failed to create render pass!");
	        }
		}
	}

	void finishRender(const GraphicsContext& context, VulkanDeviceState& state) {
		if (state.getRenderMode().get() == VULKAN_RENDER_CLEANUP_DISPLAY) {
			vkDestroyRenderPass(state.getDevice()->getDevice(), contextHandler.getDisplayState(context)->renderPass, nullptr);

			std::cout << "Destroy render pass." << std::endl;
		}

		VulkanRenderPass::finishRender(context, state);
	}

	VkRenderPass getRenderPass(const GraphicsContext& context) const {
		return contextHandler.getDisplayState(context)->renderPass;
	}

private:
	struct RenderPassState : public ContextState {
		VkRenderPass renderPass;
	};

	
	DisplayContextHandler<ContextState,RenderPassState,ContextState> contextHandler;
};


}

#endif

