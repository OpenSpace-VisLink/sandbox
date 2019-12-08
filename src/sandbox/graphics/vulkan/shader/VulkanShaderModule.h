#ifndef SANDBOX_GRAPHICS_VULKAN_SHADER_VULKAN_SHADER_MODULE_H_
#define SANDBOX_GRAPHICS_VULKAN_SHADER_VULKAN_SHADER_MODULE_H_

#include "sandbox/Component.h"
#include "sandbox/graphics/vulkan/VulkanDeviceRenderer.h"
#include <fstream>

namespace sandbox {

class VulkanShaderModule : public VulkanRenderObject {
public:
	VulkanShaderModule(const std::string& filename, VkShaderStageFlagBits shaderStage) : shaderStage(shaderStage) { 
		addType<VulkanShaderModule>(); 
		code = readFile(filename);

	}
	virtual ~VulkanShaderModule() {}

	VkShaderStageFlagBits getShaderStage() const { return shaderStage; }
	VkShaderModule getShaderModule(const GraphicsContext& context) const { return contextHandler.getSharedState(context)->shaderModule; }

protected:
	void startRender(const GraphicsContext& context, VulkanDeviceState& state) {
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

	void finishRender(const GraphicsContext& context, VulkanDeviceState& state) {
		if (state.getRenderMode().get() == VULKAN_RENDER_CLEANUP_SHARED) {
			vkDestroyShaderModule(state.getDevice()->getDevice(), contextHandler.getSharedState(context)->shaderModule, nullptr);
		}
	}


private:
	struct ShaderModuleState : public ContextState {
		VkShaderModule shaderModule;
	};

    static std::vector<char> readFile(const std::string& filename) {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open()) {
            throw std::runtime_error("failed to open file!");
        }

        size_t fileSize = (size_t) file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        return buffer;
    }

	std::vector<char> code;
	VkShaderStageFlagBits shaderStage;
	GraphicsContextHandler<ShaderModuleState, ContextState> contextHandler;
};


}

#endif