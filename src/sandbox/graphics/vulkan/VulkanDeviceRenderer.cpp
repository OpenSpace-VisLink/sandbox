#include "sandbox/graphics/vulkan/VulkanDeviceRenderer.h"
#include "sandbox/graphics/vulkan/VulkanDeviceState.h"
#include "sandbox/graphics/vulkan/VulkanSwapChain.h"

namespace sandbox {

void VulkanDeviceRenderer::update() {
	GraphicsRenderer::update();

	if (!state) {
		state = &VulkanDeviceState::get(getContext());
	}

	if (!device) {
		device = getEntity().getComponentRecursive<VulkanDevice>(false);
	}

	if (device) {
		state->setDevice(device);
		//state->setRenderPass(getEntity().getComponent<VulkanRenderPass>());
		VulkanSwapChain* swapChain = getEntity().getComponent<VulkanSwapChain>();
		if (swapChain) {
			std::cout << "set render state -> swapChain" << " " << swapChain->getExtent().width << " " << swapChain->getExtent().height << std::endl;
			state->setExtent(swapChain->getExtent());	
			state->getImageFormat().set(swapChain->getImageFormat());
		}
	}
	
}

void VulkanDeviceRenderer::render(VulkanRenderMode renderMode) {
	state->getRenderMode().push(renderMode);
	GraphicsRenderer::render();
	state->getRenderMode().pop();
}


void VulkanRenderObject::startRender(const GraphicsContext& context) {
	VulkanDeviceState& state = VulkanDeviceState::get(context);
	/*switch (state.getRenderMode().get()) {
		case VULKAN_RENDER_OBJECT:
			updateObject(context, state);
			break;
		case VULKAN_RENDER_COMMAND:
			startRenderCommand(context, state);
			break;
		default:
			break;
	}*/
	startRender(context, state);

}
void VulkanRenderObject::finishRender(const GraphicsContext& context) {
	VulkanDeviceState& state = VulkanDeviceState::get(context);
	finishRender(context, state);
}

void AllowRenderModes::startRender(const GraphicsContext& context, VulkanDeviceState& state) {
	VulkanRenderMode currentRenderMode = state.getRenderMode().get();
	state.getRenderMode().push(static_cast<VulkanRenderMode>(renderMode & currentRenderMode));
}

void AllowRenderModes::finishRender(const GraphicsContext& context, VulkanDeviceState& state) {
	state.getRenderMode().pop();
}

}