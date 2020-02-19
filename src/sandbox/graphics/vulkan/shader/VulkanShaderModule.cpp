#include "sandbox/graphics/vulkan/VulkanDeviceState.h"
#include "sandbox/graphics/vulkan/shader/VulkanShaderModule.h"

namespace sandbox {

void VulkanShaderModule::startRender(const GraphicsContext& context, VulkanDeviceState& state) {
	if (state.getRenderMode().get() == VULKAN_RENDER_UPDATE_SHARED) {
		VkShaderModuleCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        if (vkCreateShaderModule(state.getDevice()->getDevice(), &createInfo, nullptr, &contextHandler.getSharedState(context)->shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("failed to create shader module!");
        }			
	}
}

void VulkanShaderModule::finishRender(const GraphicsContext& context, VulkanDeviceState& state) {
	if (state.getRenderMode().get() == VULKAN_RENDER_CLEANUP_SHARED) {
		vkDestroyShaderModule(state.getDevice()->getDevice(), contextHandler.getSharedState(context)->shaderModule, nullptr);
	}
}

}