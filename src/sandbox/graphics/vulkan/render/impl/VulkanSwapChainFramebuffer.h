#ifndef SANDBOX_GRAPHICS_VULKAN_RENDER_IMPL_VULKAN_SWAP_CHAIN_FRAME_BUFFER_H_
#define SANDBOX_GRAPHICS_VULKAN_RENDER_IMPL_VULKAN_SWAP_CHAIN_FRAME_BUFFER_H_

#include "sandbox/Component.h"
#include "sandbox/graphics/vulkan/render/VulkanFramebuffer.h"

namespace sandbox {

class VulkanSwapChainFramebuffer : public VulkanFramebuffer {
public:
	VulkanSwapChainFramebuffer() { addType<VulkanSwapChainFramebuffer>(); }
	virtual ~VulkanSwapChainFramebuffer() {}

	void startRender(const GraphicsContext& context, VulkanDeviceState& state) {
		if (state.getRenderMode().get() == VULKAN_RENDER_UPDATE) {
			VulkanSwapChainState& swapChainState = VulkanSwapChainState::get(context);
			VulkanSwapChain* swapChain = swapChainState.getSwapChain();

	        /*VkImageView attachments[] = {
	            //swapChain->getImageViews()[0]
	            //state.getImageView()->getImageView(context)
	            swapChain->getImageViews()[VulkanSwapChainState::get(context).getImageIndex()]
	        };

	        VkFramebufferCreateInfo framebufferInfo = {};
	        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	        framebufferInfo.renderPass = state.getRenderPass().get()->getRenderPass(context);
	        framebufferInfo.attachmentCount = 1;
	        framebufferInfo.pAttachments = attachments;
	        framebufferInfo.width = state.getExtent().width;
	        framebufferInfo.height = state.getExtent().height;
	        framebufferInfo.layers = 1;*/

	        std::array<VkImageView, 2> attachments = {
			    swapChain->getImageViews()[VulkanSwapChainState::get(context).getImageIndex()],
			    swapChain->getDepthImageView()
			};

			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	        framebufferInfo.renderPass = state.getRenderPass().get()->getRenderPass(context);
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = state.getExtent().width;
	        framebufferInfo.height = state.getExtent().height;
			framebufferInfo.layers = 1;

	        if (vkCreateFramebuffer(state.getDevice()->getDevice(), &framebufferInfo, nullptr, &contextHandler.getState(context)->framebuffer) != VK_SUCCESS) {
	            throw std::runtime_error("failed to create framebuffer!");
	        }
		}

	}

	VkFramebuffer getFramebuffer(const GraphicsContext& context) const {
		return contextHandler.getState(context)->framebuffer;
	}

private:
	struct FramebufferState : public ContextState {
		VkFramebuffer framebuffer;
	};

	GraphicsContextHandler<ContextState,FramebufferState> contextHandler;

};

}

#endif

