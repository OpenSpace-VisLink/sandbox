#include "sandbox/graphics/vulkan/VulkanSwapChain.h"
#include "sandbox/graphics/vulkan/VulkanDeviceState.h"

namespace sandbox {

void VulkanImageTransition::startRender(const GraphicsContext& context, VulkanDeviceState& state) {
        if ((state.getRenderMode().get() & VULKAN_RENDER_OBJECT) == VULKAN_RENDER_OBJECT) {
        	static int c = 0;
        	c++;
        	if (c > -1) {
        		return;
        	}
        	std::cout << "RenderCommand transition!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
            //state.getGraphicsPipeline().push(this);
            VulkanSwapChainState& swapChainState = VulkanSwapChainState::get(context);
            VulkanSwapChain* swapChain = swapChainState.getSwapChain();
            int currentImage = swapChainState.getImageIndex();
            transitionImageLayout(swapChain->getImages()[currentImage], swapChain->getImageFormat(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, state.getCommandPool().get(), swapChain->getEntity().getComponents<VulkanDeviceRenderer>()[currentImage]->getContext());
        }
}


}