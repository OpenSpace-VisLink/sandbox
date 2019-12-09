#ifndef SANDBOX_GRAPHICS_VULKAN_SHADER_VULKAN_DESCRIPTOR_H_
#define SANDBOX_GRAPHICS_VULKAN_SHADER_VULKAN_DESCRIPTOR_H_

#include "sandbox/graphics/vulkan/VulkanDeviceRenderer.h"
#include "sandbox/graphics/vulkan/shader/VulkanShaderObject.h"
#include "sandbox/graphics/vulkan/shader/VulkanShaderModule.h"
#include "sandbox/graphics/vulkan/buffer/VulkanVertexInput.h"
#include "sandbox/graphics/vulkan/VulkanSwapChain.h"

namespace sandbox {


class VulkanDescriptor : public VulkanComponent { //
	    //binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
public:
	VulkanDescriptor(VulkanShaderObject* shaderObject, VkShaderStageFlags shaderStageFlags) : shaderObject(shaderObject), shaderStageFlags(shaderStageFlags) { addType<VulkanDescriptor>(); }
	virtual ~VulkanDescriptor() {}

	void setBinding(VkDescriptorSetLayoutBinding& binding) {
		shaderObject->setBinding(binding);
		binding.stageFlags = shaderStageFlags;
	}

	void setPoolSize(VkDescriptorPoolSize& poolSize) {
		shaderObject->setPoolSize(poolSize);
	}

	void writeDescriptor(const GraphicsContext& context, VkWriteDescriptorSet& descriptorWrite, std::vector<VulkanShaderObject::DescriptorObject*>& descriptorObjects) {
	    descriptorWrite.dstArrayElement = 0;
		shaderObject->writeDescriptor(context, descriptorWrite, descriptorObjects);
	}

private:
	VulkanShaderObject* shaderObject;
	VkShaderStageFlags shaderStageFlags;
};


class VulkanDescriptorSetLayout : public VulkanRenderObject {
public:
	VulkanDescriptorSetLayout() { addType<VulkanDescriptorSetLayout>(); }
	virtual ~VulkanDescriptorSetLayout() {}

	VkDescriptorSetLayout getDescriptorSetLayout(const GraphicsContext& context) const { return contextHandler.getSharedState(context)->descriptorSetLayout; }

protected:
	void startRender(const GraphicsContext& context, VulkanDeviceState& state);
	void finishRender(const GraphicsContext& context, VulkanDeviceState& state);


private:
	struct DescriptorSetLayoutState : public ContextState {
		VkDescriptorSetLayout descriptorSetLayout;
	};

	GraphicsContextHandler<DescriptorSetLayoutState,ContextState> contextHandler;
};

class VulkanDescriptorPool : public VulkanRenderObject {
public:
	VulkanDescriptorPool() { addType<VulkanDescriptorPool>(); }
	virtual ~VulkanDescriptorPool() {}

	virtual VkDescriptorPool getDescriptorPool(const GraphicsContext& context) const = 0;
};

class VulkanSwapChainDescriptorPool : public VulkanDescriptorPool {
public:
	VulkanSwapChainDescriptorPool() { addType<VulkanSwapChainDescriptorPool>(); }
	virtual ~VulkanSwapChainDescriptorPool() {}

	VkDescriptorPool getDescriptorPool(const GraphicsContext& context) const { return contextHandler.getDisplayState(context)->descriptorPool; }

protected:
	void startRender(const GraphicsContext& context, VulkanDeviceState& state);
	void finishRender(const GraphicsContext& context, VulkanDeviceState& state);

private:
	struct DescriptorPoolState : public ContextState {
		VkDescriptorPool descriptorPool;
	};

	DisplayContextHandler<ContextState,DescriptorPoolState,ContextState> contextHandler;
};


class VulkanDescriptorSet : public VulkanRenderObject {
public:
	VulkanDescriptorSet() { addType<VulkanDescriptorSet>(); }
	virtual ~VulkanDescriptorSet() {}

	VkDescriptorSet getDescriptorSet(const GraphicsContext& context) const { return contextHandler.getState(context)->descriptorSet; }

protected:
	void startRender(const GraphicsContext& context, VulkanDeviceState& state);
	void finishRender(const GraphicsContext& context, VulkanDeviceState& state);


private:
	struct DescriptorSetState : public ContextState {
		VkDescriptorSet descriptorSet;
	};

	GraphicsContextHandler<ContextState,DescriptorSetState> contextHandler;
};

}

#endif