#ifndef SANDBOX_GRAPHICS_VULKAN_IMAGE_VULKAN_SAMPLER_H_
#define SANDBOX_GRAPHICS_VULKAN_IMAGE_VULKAN_SAMPLER_H_

#include "sandbox/graphics/vulkan/image/VulkanImage.h"
#include "sandbox/graphics/vulkan/shader/VulkanShaderObject.h"

namespace sandbox {

class VulkanSampler : public VulkanShaderObject {
public:
	VulkanSampler() { addType<VulkanSampler>(); }
	virtual ~VulkanSampler() {}

	VkSampler getSampler(const GraphicsContext& context) const { return contextHandler.getSharedState(context)->sampler; }

protected:
	void startRender(const GraphicsContext& context, VulkanDeviceState& state) {
		if (state.getRenderMode().get() == VULKAN_RENDER_UPDATE_SHARED) {
			VkSamplerCreateInfo samplerInfo = {};
	        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	        samplerInfo.magFilter = VK_FILTER_LINEAR;
	        samplerInfo.minFilter = VK_FILTER_LINEAR;
	        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	        samplerInfo.anisotropyEnable = VK_TRUE;
	        samplerInfo.maxAnisotropy = 16;
	        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	        samplerInfo.unnormalizedCoordinates = VK_FALSE;
	        samplerInfo.compareEnable = VK_FALSE;
	        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

	        if (vkCreateSampler(state.getDevice()->getDevice(), &samplerInfo, nullptr, &contextHandler.getSharedState(context)->sampler) != VK_SUCCESS) {
	            throw std::runtime_error("failed to create texture sampler!");
	        }
		}
	}

	void finishRender(const GraphicsContext& context, VulkanDeviceState& state) {
		if (state.getRenderMode().get() == VULKAN_RENDER_CLEANUP_SHARED) {
			vkDestroySampler(state.getDevice()->getDevice(), contextHandler.getSharedState(context)->sampler, nullptr);
		}
	}

	void setBinding(VkDescriptorSetLayoutBinding& binding) {
		binding.descriptorCount = 1;
	    binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	    binding.pImmutableSamplers = nullptr;
	}

	void writeDescriptor(const GraphicsContext& context, VkWriteDescriptorSet& descriptorWrite, std::vector<DescriptorObject*>& descriptorObjects) {
        /*VkDescriptorBufferInfo bufferInfo = {};
        bufferInfo.buffer = getBuffer(context);
        bufferInfo.offset = 0;
        bufferInfo.range = getBufferSize();*/

        VkDescriptorImageInfo& imageInfo = *(new VkDescriptorImageInfo());
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        //imageInfo.imageView = mainImage->getComponent<VulkanImageView>()->getImageView(renderer->getContext());
        imageInfo.sampler = getSampler(context);
        descriptorObjects.push_back(new TypedDescriptorObject<VkDescriptorImageInfo>(&imageInfo));

        //std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};

        //descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        //descriptorWrites[0].dstSet = descriptorSets[i];
        //descriptorWrites[0].dstBinding = 0;
        //descriptorWrites[0].dstArrayElement = 0;
        //descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        //descriptorWrite.descriptorCount = 1;
        //descriptorWrite.pBufferInfo = &bufferInfo;

        //descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        //descriptorWrites[1].dstSet = descriptorSets[i];
        //descriptorWrites[1].dstBinding = 1;
        //descriptorWrites[1].dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pImageInfo = &imageInfo;
	}

	void setPoolSize(VkDescriptorPoolSize& poolSize) {
		poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	}

private:
	struct SamplerState : public ContextState {
		VkSampler sampler;
	};

	GraphicsContextHandler<SamplerState,ContextState> contextHandler;
};

class VulkanImageViewDecorator : public VulkanShaderObject {
public:
	VulkanImageViewDecorator(VulkanShaderObject* shaderObject, VulkanImageView* imageView) : shaderObject(shaderObject), imageView(imageView) { addType<VulkanImageViewDecorator>(); }
	virtual ~VulkanImageViewDecorator() {}

	void setBinding(VkDescriptorSetLayoutBinding& binding) {
		shaderObject->setBinding(binding);
	}
	void writeDescriptor(const GraphicsContext& context, VkWriteDescriptorSet& descriptorWrite, std::vector<DescriptorObject*>& descriptorObjects) {
		shaderObject->writeDescriptor(context, descriptorWrite, descriptorObjects);
		std::cout << "write image view " << descriptorObjects.size() << std::endl;

        for (int f = 0; f < descriptorObjects.size(); f++) {
        	void* value = descriptorObjects[f]->value;
	        VkDescriptorImageInfo* imageInfo = descriptorObjects[f]->asType<VkDescriptorImageInfo>();
	        std::cout << f << " ";
	        if (imageInfo) {
	        	std::cout << VulkanSwapChainState::get(context).getSwapChain()->getName() << " image view bound" << std::endl;
	        	imageInfo->imageView = imageView->getImageView(context);
	        }
        }
	}
	void setPoolSize(VkDescriptorPoolSize& poolSize) {
		shaderObject->setPoolSize(poolSize);
	}
private:
	VulkanShaderObject* shaderObject;
	VulkanImageView* imageView;
};


}

#endif