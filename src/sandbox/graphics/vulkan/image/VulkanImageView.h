#ifndef SANDBOX_GRAPHICS_VULKAN_IMAGE_VULKAN_IMAGE_VIEW_H_
#define SANDBOX_GRAPHICS_VULKAN_IMAGE_VULKAN_IMAGE_VIEW_H_

#include "sandbox/graphics/vulkan/image/VulkanImage.h"

namespace sandbox {

class VulkanImageView : public VulkanRenderObject {
public:
	VulkanImageView() : image(NULL) { addType<VulkanImageView>(); }
	virtual ~VulkanImageView() {}

	void update() {
		if (!image) {
			image = getEntity().getComponent<VulkanImage>();
		}
	}

	VkImageView getImageView(const GraphicsContext& context) const { return contextHandler.getSharedState(context)->imageView; }

protected:
	void startRender(const GraphicsContext& context, VulkanDeviceState& state) {
		if (!image) { return; }

		if (state.getRenderMode().get() == VULKAN_RENDER_UPDATE_SHARED) {
			std::cout << "Created Image View" << std::endl;
			ImageViewState* imageViewState = contextHandler.getSharedState(context);
			imageViewState->imageView = state.getDevice()->createImageView(image->getImage(context), VK_FORMAT_R8G8B8A8_UNORM);
		}
	}

	void finishRender(const GraphicsContext& context, VulkanDeviceState& state) {
		if (!image) { return; }

		if (state.getRenderMode().get() == VULKAN_RENDER_CLEANUP_SHARED) {
			ImageViewState* imageViewState = contextHandler.getSharedState(context);
			vkDestroyImageView(state.getDevice()->getDevice(), imageViewState->imageView, nullptr);
		}
	}


private:
	struct ImageViewState : public ContextState {
		VkImageView imageView;
	};

	GraphicsContextHandler<ImageViewState,ContextState> contextHandler;
	VulkanImage* image;

};



}

#endif