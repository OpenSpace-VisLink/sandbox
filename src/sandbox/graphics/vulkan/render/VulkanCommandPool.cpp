#include "sandbox/graphics/vulkan/VulkanDeviceState.h"
#include "sandbox/graphics/vulkan/render/VulkanCommandPool.h"

namespace sandbox {

void VulkanCommandPool::startRender(const GraphicsContext& context, VulkanDeviceState& state) {
	state.getCommandPool().push(this);

	if (state.getRenderMode().get() == VULKAN_RENDER_UPDATE_SHARED) {
		VkCommandPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queue->getIndex();

        if (vkCreateCommandPool(device->getDevice(), &poolInfo, nullptr, &contextHandler.getSharedState(context)->commandPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics command pool!");
        }
    }
}

void VulkanCommandPool::finishRender(const GraphicsContext& context, VulkanDeviceState& state) {
	if (state.getRenderMode().get() == VULKAN_RENDER_CLEANUP_SHARED) {
		vkDestroyCommandPool(device->getDevice(), getCommandPool(context), nullptr);
		std::cout << "Destroy command pool." << std::endl;
	}

	state.getCommandPool().pop();
}


}