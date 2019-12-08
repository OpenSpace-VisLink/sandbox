#ifndef SANDBOX_VULCAN_H_
#define SANDBOX_VULCAN_H_

#include "sandbox/Component.h"
#include "sandbox/graphics/GraphicsContext.h"
#include "sandbox/graphics/RenderNode.h"
#include "sandbox/graphics/GraphicsRenderer.h"

#include "sandbox/graphics/vulkan/Vulkan.h"
#include "sandbox/graphics/vulkan/VulkanInstance.h"
#include "sandbox/graphics/vulkan/VulkanSurface.h"
#include "sandbox/graphics/vulkan/VulkanQueue.h"
#include "sandbox/graphics/vulkan/VulkanDevice.h"
#include "sandbox/graphics/vulkan/VulkanSwapChain.h"
#include "sandbox/graphics/vulkan/render/VulkanRenderPass.h"
#include "sandbox/graphics/vulkan/render/VulkanCommandPool.h"
#include "sandbox/graphics/vulkan/render/VulkanCommandBuffer.h"
#include "sandbox/graphics/vulkan/render/impl/VulkanBasicRenderPass.h"
#include "sandbox/graphics/vulkan/render/impl/VulkanSwapChainFramebuffer.h"
#include "sandbox/graphics/vulkan/render/VulkanFramebuffer.h"
#include "sandbox/graphics/vulkan/VulkanDeviceState.h"
#include "sandbox/graphics/vulkan/VulkanFrameRenderer.h"
#include "sandbox/graphics/vulkan/shader/VulkanShaderModule.h"
#include "sandbox/graphics/vulkan/image/VulkanImage.h"
#include "sandbox/graphics/vulkan/image/VulkanImageView.h"
#include "sandbox/graphics/vulkan/buffer/VulkanBuffer.h"


#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <fstream>


namespace sandbox {


class VulkanVertexInput : public Component {
public:
	VulkanVertexInput(size_t stride) : stride(stride) { addType<VulkanVertexInput>(); }
	virtual ~VulkanVertexInput() {}

	virtual VkVertexInputBindingDescription getBindingDescription(int binding) const {
		VkVertexInputBindingDescription bindingDescription = {};
	    bindingDescription.binding = 0;
	    bindingDescription.stride = stride;//sizeof(Vertex);
	    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	    return bindingDescription;
	}

	std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions(int binding) const {
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
		attributeDescriptions.resize(attributes.size());

		for (int f = 0; f < attributes.size(); f++) {
			attributeDescriptions[f].binding = binding;
	        attributeDescriptions[f].location = f;
	        attributeDescriptions[f].format = attributes[f].format;
	        attributeDescriptions[f].offset = attributes[f].offset;
		}

	    return attributeDescriptions;
	}

	void addAttribute(VkFormat format, size_t offset) {
		Attribute att;
		att.format = format;
		att.offset = offset;
		attributes.push_back(att);
	}

private:
	struct Attribute { VkFormat format; size_t offset; };
	std::vector<Attribute> attributes;
	size_t stride;
};





class VulkanShaderObject : public VulkanRenderObject {
public:
	VulkanShaderObject() { addType<VulkanShaderObject>(); }
	virtual ~VulkanShaderObject() {}

	class DescriptorObject {
	public:
		DescriptorObject(void* value) : value(value) {}
		virtual ~DescriptorObject() {}

		template<typename T>
		T* asType() {
			static const std::type_info& type = typeid(T);
			if (isType(type)) {
				return static_cast<T*>(value); 
			}
			else {
				return NULL;
			}
		}
		virtual bool isType(const std::type_info& type) { return false; }

		void* value;
	};

	template<typename T>
	class TypedDescriptorObject : public DescriptorObject {
	public:
		TypedDescriptorObject(T* value) : DescriptorObject(value) {}
		virtual ~TypedDescriptorObject() { delete static_cast<T*>(value); }

		bool isType(const std::type_info& type) {
			static const std::type_info& objectType = typeid(T);
			return (type == objectType);
		}
	};

	virtual void setBinding(VkDescriptorSetLayoutBinding& binding) {}
	virtual void writeDescriptor(const GraphicsContext& context, VkWriteDescriptorSet& descriptorWrite, std::vector<DescriptorObject*>& descriptorObjects) {}
	virtual void setPoolSize(VkDescriptorPoolSize& poolSize) {}
};

class VulkanUniformBuffer : public VulkanShaderObject {
public:
	VulkanUniformBuffer() { addType<VulkanUniformBuffer>(); }
	virtual ~VulkanUniformBuffer() {}

	void startRender(const GraphicsContext& context, VulkanDeviceState& state) {
		UniformBufferState* uboState = contextHandler.getState(context);
		if (state.getRenderMode().get() == VULKAN_RENDER_UPDATE) {
			int bufferSize = getBufferSize();
			std::cout << "Create Buffer" << std::endl;
			uboState->buffer = new VulkanBuffer(state.getDevice(), bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		}
		
		if (state.getRenderMode().get() == VULKAN_RENDER_OBJECT) {
			updateBuffer(context, state, uboState->buffer);
		}
	}
	void finishRender(const GraphicsContext& context, VulkanDeviceState& state) {
		if (state.getRenderMode().get() == VULKAN_RENDER_CLEANUP) {
			delete contextHandler.getState(context)->buffer;
		}
	}

	VkBuffer getBuffer(const GraphicsContext& context) const { return contextHandler.getState(context)->buffer->getBuffer(); }

	void setBinding(VkDescriptorSetLayoutBinding& binding) {
		binding.descriptorCount = 1;
	    binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	    binding.pImmutableSamplers = nullptr;
	}

	void writeDescriptor(const GraphicsContext& context, VkWriteDescriptorSet& descriptorWrite, std::vector<DescriptorObject*>& descriptorObjects) {
        VkDescriptorBufferInfo& bufferInfo = *(new VkDescriptorBufferInfo());
        //*bufferInfo = {};
		std::cout << "Write Buffer" << std::endl;
        bufferInfo.buffer = getBuffer(context);
        bufferInfo.offset = 0;
        bufferInfo.range = getBufferSize();
        descriptorObjects.push_back(new TypedDescriptorObject<VkDescriptorBufferInfo>(&bufferInfo));

        /*VkDescriptorImageInfo imageInfo = {};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = mainImage->getComponent<VulkanImageView>()->getImageView(renderer->getContext());
        imageInfo.sampler = sampler->getSampler(renderer->getContext());*/

        //std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};

        //descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        //descriptorWrites[0].dstSet = descriptorSets[i];
        //descriptorWrites[0].dstBinding = 0;
        //descriptorWrites[0].dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;

        /*descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = descriptorSets[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &imageInfo;*/
	}

	void setPoolSize(VkDescriptorPoolSize& poolSize) {
		poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	}

protected:
	virtual size_t getBufferSize() const = 0;
	virtual void updateBuffer(const GraphicsContext& context, VulkanDeviceState& state, VulkanBuffer* buffer) = 0;


private:
	struct UniformBufferState : public ContextState {
		VulkanBuffer* buffer;
	};

	GraphicsContextHandler<ContextState,UniformBufferState> contextHandler;
};

template<typename T>
class VulkanUniformBufferValue : public VulkanUniformBuffer {
public:
	VulkanUniformBufferValue() { addType< VulkanUniformBufferValue<T> >(); }
	virtual ~VulkanUniformBufferValue() {}

	T value;

protected:
	size_t getBufferSize() const { return sizeof(T); }

	void updateBuffer(const GraphicsContext& context, VulkanDeviceState& state, VulkanBuffer* buffer) {
		buffer->update(&value, sizeof(T));
	}
};





class VulkanDeviceBuffer : public VulkanRenderObject {
public:
	VulkanDeviceBuffer(VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, bool sharedContext) :
		usage(usage), properties(properties), sharedContext(sharedContext) { 
			addType<VulkanDeviceBuffer>(); 
			updateMode = sharedContext ? VULKAN_RENDER_UPDATE_SHARED : VULKAN_RENDER_UPDATE;
			cleanupMode = sharedContext ? VULKAN_RENDER_CLEANUP_SHARED : VULKAN_RENDER_CLEANUP;
		}
	virtual ~VulkanDeviceBuffer() {}

	VkBuffer getBuffer(const GraphicsContext& context) const { 
		BufferState* bufferState = sharedContext ? contextHandler.getSharedState(context) : contextHandler.getState(context);
		return bufferState->buffer->getBuffer(); 
	}

protected:
	void startRender(const GraphicsContext& context, VulkanDeviceState& state) {
		BufferState* bufferState = sharedContext ? contextHandler.getSharedState(context) : contextHandler.getState(context);
		if (handleUpdate(context, state)) {
			VkDeviceSize bufferSize = getBufferSize();
			bufferState->buffer = new VulkanBuffer(state.getDevice(), bufferSize, usage, properties);
		}
		
		if (state.getRenderMode().get() == VULKAN_RENDER_OBJECT) {
			VkDeviceSize bufferSize = getBufferSize();
			VulkanBuffer* stagingBuffer = new VulkanBuffer(state.getDevice(), bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			updateBuffer(context, state, stagingBuffer);
			stagingBuffer->copyTo(bufferState->buffer, bufferSize, state.getCommandPool().get(), context);
			delete stagingBuffer;
		}
	}

	void finishRender(const GraphicsContext& context, VulkanDeviceState& state) {
		if (handleCleanup(context, state)) {
			BufferState* bufferState = sharedContext ? contextHandler.getSharedState(context) : contextHandler.getState(context);
			delete bufferState->buffer;
		}
	}


protected:
	virtual bool handleUpdate(const GraphicsContext& context,VulkanDeviceState& state) const { return state.getRenderMode().get() == updateMode; }
	virtual bool handleCleanup(const GraphicsContext& context,VulkanDeviceState& state) const { return state.getRenderMode().get() == cleanupMode; }
	virtual VkDeviceSize getBufferSize() const = 0;
	virtual void updateBuffer(const GraphicsContext& context, VulkanDeviceState& state, VulkanBuffer* buffer) = 0;

private:
	struct BufferState : public ContextState {
		VulkanBuffer* buffer;
	};

	GraphicsContextHandler<BufferState,BufferState> contextHandler;
	VkBufferUsageFlags usage;
	VkMemoryPropertyFlags properties;
	VulkanRenderMode updateMode;
	VulkanRenderMode cleanupMode;
	bool sharedContext;

};

template<typename T>
class VulkanDeviceBufferValue : public VulkanDeviceBuffer {
public:
	VulkanDeviceBufferValue(VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, bool sharedContext) : VulkanDeviceBuffer(usage, properties, sharedContext) { addType< VulkanDeviceBufferValue<T> >(); }
	virtual ~VulkanDeviceBufferValue() {}

	T value;

protected:
	VkDeviceSize getBufferSize() const { return sizeof(T); }
	void updateBuffer(const GraphicsContext& context, VulkanDeviceState& state, VulkanBuffer* buffer) {
		buffer->update(getData(), getBufferSize());
	}
	virtual void* getData() { return &value; }
};

template<typename T>
class VulkanArrayBuffer : public VulkanDeviceBufferValue< std::vector<T> > {
public:
	VulkanArrayBuffer(VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, bool sharedContext) : VulkanDeviceBufferValue< std::vector<T> >(usage, properties, sharedContext) { /*this->addType< VulkanArrayBuffer<T> >();*/ }
	virtual ~VulkanArrayBuffer() {}

protected:
	VkDeviceSize getBufferSize() const { return this->value.size()*sizeof(T); }
	void* getData() { return this->value.data(); }
};

template<typename T>
class VertexArray : public VulkanArrayBuffer<T> {
public:
	VertexArray() : VulkanArrayBuffer<T>(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, true) {}
	virtual ~VertexArray() {}

	void startRender(const GraphicsContext& context, VulkanDeviceState& state) {
		VulkanArrayBuffer<T>::startRender(context, state);

		if ((state.getRenderMode().get() & VULKAN_RENDER_COMMAND) == VULKAN_RENDER_COMMAND) {
			std::cout << "bind vertex array" << std::endl;
			VkBuffer vertexBuffers[] = {VulkanArrayBuffer<T>::getBuffer(context)};
            VkDeviceSize offsets[] = {0};
            vkCmdBindVertexBuffers(state.getCommandBuffer().get()->getCommandBuffer(context), 0, 1, vertexBuffers, offsets);
		}
	}
};

template<typename T>
class ObjectArray : public VulkanArrayBuffer<T> {
public:
	ObjectArray() : VulkanArrayBuffer<T>(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, true) {}
	virtual ~ObjectArray() {}
};

//typedef ObjectArray<uint16_t> IndexArray;
class IndexArray : public ObjectArray<uint16_t> {
protected:
	void startRender(const GraphicsContext& context, VulkanDeviceState& state) {
		ObjectArray<uint16_t>::startRender(context, state);

		if ((state.getRenderMode().get() & VULKAN_RENDER_COMMAND) == VULKAN_RENDER_COMMAND) {
			std::cout << "bind index array and draw" << std::endl;
            vkCmdBindIndexBuffer(state.getCommandBuffer().get()->getCommandBuffer(context), getBuffer(context), 0, VK_INDEX_TYPE_UINT16);
            vkCmdDrawIndexed(state.getCommandBuffer().get()->getCommandBuffer(context), static_cast<uint32_t>(value.size()), 1, 0, 0, 0);
		}
	}
};





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
	void startRender(const GraphicsContext& context, VulkanDeviceState& state) {
		if (state.getRenderMode().get() == VULKAN_RENDER_UPDATE_SHARED) {
			DescriptorSetLayoutState* layoutState = contextHandler.getSharedState(context);
			std::vector<VulkanDescriptor*> descriptors = getEntity().getComponents<VulkanDescriptor>();
			std::cout << descriptors.size() << std::endl;
			std::vector<VkDescriptorSetLayoutBinding> bindings;
			for (int f = 0; f < descriptors.size(); f++) {
				VkDescriptorSetLayoutBinding binding = {};
				binding.binding = f;
				descriptors[f]->setBinding(binding);
				bindings.push_back(binding);
			}

	        VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	        layoutInfo.pBindings = bindings.data();

	        if (vkCreateDescriptorSetLayout(state.getDevice()->getDevice(), &layoutInfo, nullptr, &layoutState->descriptorSetLayout) != VK_SUCCESS) {
	            throw std::runtime_error("failed to create descriptor set layout!");
	        }
			std::cout << "created descriptor layout "  << layoutState->descriptorSetLayout << std::endl;
		}
	}

	void finishRender(const GraphicsContext& context, VulkanDeviceState& state) {
		if (state.getRenderMode().get() == VULKAN_RENDER_CLEANUP_SHARED) {
			DescriptorSetLayoutState* layoutState = contextHandler.getSharedState(context);
			vkDestroyDescriptorSetLayout(state.getDevice()->getDevice(), layoutState->descriptorSetLayout, nullptr);
			std::cout << "destroy descriptor layout" << std::endl;
		}
	}


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
	void startRender(const GraphicsContext& context, VulkanDeviceState& state) {
		if (state.getRenderMode().get() == VULKAN_RENDER_UPDATE_DISPLAY) {
			DescriptorPoolState* descriptorPoolState = contextHandler.getDisplayState(context);
			std::vector<VulkanDescriptor*> descriptors = getEntity().getComponents<VulkanDescriptor>();
			VulkanSwapChainState& swapChainState = VulkanSwapChainState::get(context);
			int numImages = swapChainState.getNumImages();
			if (numImages == 0) { numImages = 1; }

			std::vector<VkDescriptorPoolSize> poolSizes;
			for (int f = 0; f < descriptors.size(); f++) {
				VkDescriptorPoolSize poolSize = {};
				descriptors[f]->setPoolSize(poolSize);
				poolSize.descriptorCount = static_cast<uint32_t>(numImages);
				poolSizes.push_back(poolSize);
			}

			VkDescriptorPoolCreateInfo poolInfo = {};
	        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	        poolInfo.pPoolSizes = poolSizes.data();
	        poolInfo.maxSets = static_cast<uint32_t>(numImages);

			if (vkCreateDescriptorPool(state.getDevice()->getDevice(), &poolInfo, nullptr, &contextHandler.getDisplayState(context)->descriptorPool) != VK_SUCCESS) {
            	throw std::runtime_error("failed to create descriptor pool!");
        	}

        	std::cout << "created descriptor pool" << std::endl;
		}
	}

	void finishRender(const GraphicsContext& context, VulkanDeviceState& state) {
		if (state.getRenderMode().get() == VULKAN_RENDER_CLEANUP_DISPLAY) {
			std::cout << "destroy descriptor pool" << std::endl;
			vkDestroyDescriptorPool(state.getDevice()->getDevice(), getDescriptorPool(context), nullptr);
		}
	}

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
	void startRender(const GraphicsContext& context, VulkanDeviceState& state) {
		if (state.getRenderMode().get() == VULKAN_RENDER_UPDATE) {
			DescriptorSetState* descriptorSetState = contextHandler.getState(context);
			std::vector<VulkanDescriptor*> descriptors = getEntity().getComponents<VulkanDescriptor>();

			VkDescriptorSetLayout layout = getEntity().getComponent<VulkanDescriptorSetLayout>()->getDescriptorSetLayout(context);
	        VkDescriptorSetAllocateInfo allocInfo = {};
	        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	        allocInfo.descriptorPool = getEntity().getComponent<VulkanDescriptorPool>()->getDescriptorPool(context);
	        allocInfo.descriptorSetCount = 1;
	        allocInfo.pSetLayouts = &layout;

	        std::cout << "Layout " <<   layout << std::endl;

	        if (vkAllocateDescriptorSets(state.getDevice()->getDevice(), &allocInfo, &descriptorSetState->descriptorSet) != VK_SUCCESS) {
	            throw std::runtime_error("failed to allocate descriptor sets!");
	        }

	        std::cout << "Created desc set: " << descriptorSetState->descriptorSet << std::endl;

	        std::vector<VkWriteDescriptorSet> descriptorWrites;
	        std::vector<VulkanShaderObject::DescriptorObject*> descriptorObjects;

			for (int f = 0; f < descriptors.size(); f++) {
				VkWriteDescriptorSet descriptorWrite = {};
				descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	            descriptorWrite.dstSet = descriptorSetState->descriptorSet;
	            descriptorWrite.dstBinding = f;
	            descriptors[f]->writeDescriptor(context, descriptorWrite, descriptorObjects);
	            descriptorWrites.push_back(descriptorWrite);
			}

			vkUpdateDescriptorSets(state.getDevice()->getDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

			for (int f = 0; f < descriptorObjects.size(); f++) {
				delete descriptorObjects[f];
			}
		}
	}

	void finishRender(const GraphicsContext& context, VulkanDeviceState& state) {
		/*if (state.getRenderMode().get() == VULKAN_RENDER_CLEANUP_SHARED) {
			DescriptorSetState* descriptorSetState = contextHandler.getState(context);
			//vkDestroyDescriptorPool(state.getDevice()->getDevice(), descriptorSetState->descriptorSet, nullptr);
			// Doesn't appear necessary to destroy?
		}*/
	}


private:
	struct DescriptorSetState : public ContextState {
		VkDescriptorSet descriptorSet;
	};

	GraphicsContextHandler<ContextState,DescriptorSetState> contextHandler;
};

class VulkanGraphicsPipeline : public VulkanRenderObject {
public:
	VulkanGraphicsPipeline(VulkanDescriptorSetLayout* descriptorSetLayout) : descriptorSetLayout(descriptorSetLayout) { addType<VulkanGraphicsPipeline>(); }
	virtual ~VulkanGraphicsPipeline() {}

	VkPipeline getGraphicsPipeline(const GraphicsContext& context) const { return contextHandler.getDisplayState(context)->graphicsPipeline; }
	VkPipelineLayout getPipelineLayout(const GraphicsContext& context) const { return contextHandler.getDisplayState(context)->pipelineLayout; }

protected:
	void startRender(const GraphicsContext& context, VulkanDeviceState& state) {
		if (state.getRenderMode().get() == VULKAN_RENDER_UPDATE_DISPLAY) {
			GraphicsPipelineState* pipelineState = contextHandler.getDisplayState(context);
			VkDevice device = state.getDevice()->getDevice();

			std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

	        std::vector<VulkanShaderModule*> shaderModules = getEntity().getComponents<VulkanShaderModule>();
	        for (int f = 0; f < shaderModules.size(); f++) {
	            VkPipelineShaderStageCreateInfo shaderStageInfo = {};
	            shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	            shaderStageInfo.stage = shaderModules[f]->getShaderStage();
	            shaderStageInfo.module = shaderModules[f]->getShaderModule(context);
	            shaderStageInfo.pName = "main";
	            shaderStages.push_back(shaderStageInfo);
	        }

	        VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	        //auto bindingDescription = Vertex::getBindingDescription();
	        //auto attributeDescriptions = Vertex::getAttributeDescriptions();

	        std::vector<VulkanVertexInput*> vertexInputList = getEntity().getComponents<VulkanVertexInput>();
	        std::vector<VkVertexInputBindingDescription> bindingDescriptions;
	        std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
	        for (int f = 0; f < vertexInputList.size(); f++) {
	        	bindingDescriptions.push_back(vertexInputList[f]->getBindingDescription(f));
	        	std::vector<VkVertexInputAttributeDescription> bindingAttributeDescriptions = vertexInputList[f]->getAttributeDescriptions(f);
	        	attributeDescriptions.insert(attributeDescriptions.end(), bindingAttributeDescriptions.begin(), bindingAttributeDescriptions.end() );
	        }				

	        vertexInputInfo.vertexBindingDescriptionCount = bindingDescriptions.size();
	        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	        vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
	        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	        VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	        inputAssembly.primitiveRestartEnable = VK_FALSE;

	        VkViewport viewport = {};
	        viewport.x = 0.0f;
	        viewport.y = 0.0f;
	        viewport.width = (float) state.getExtent().width;
	        viewport.height = (float) state.getExtent().height;
	        viewport.minDepth = 0.0f;
	        viewport.maxDepth = 1.0f;

	        VkRect2D scissor = {};
	        scissor.offset = {0, 0};
	        scissor.extent = state.getExtent();

	        VkPipelineViewportStateCreateInfo viewportState = {};
	        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	        viewportState.viewportCount = 1;
	        viewportState.pViewports = &viewport;
	        viewportState.scissorCount = 1;
	        viewportState.pScissors = &scissor;

	        VkPipelineRasterizationStateCreateInfo rasterizer = {};
	        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	        rasterizer.depthClampEnable = VK_FALSE;
	        rasterizer.rasterizerDiscardEnable = VK_FALSE;
	        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	        rasterizer.lineWidth = 1.0f;
	        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	        rasterizer.depthBiasEnable = VK_FALSE;

	        VkPipelineMultisampleStateCreateInfo multisampling = {};
	        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	        multisampling.sampleShadingEnable = VK_FALSE;
	        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	        VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	        colorBlendAttachment.blendEnable = VK_FALSE;

	        VkPipelineColorBlendStateCreateInfo colorBlending = {};
	        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	        colorBlending.logicOpEnable = VK_FALSE;
	        colorBlending.logicOp = VK_LOGIC_OP_COPY;
	        colorBlending.attachmentCount = 1;
	        colorBlending.pAttachments = &colorBlendAttachment;
	        colorBlending.blendConstants[0] = 0.0f;
	        colorBlending.blendConstants[1] = 0.0f;
	        colorBlending.blendConstants[2] = 0.0f;
	        colorBlending.blendConstants[3] = 0.0f;

	        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	        pipelineLayoutInfo.setLayoutCount = 1;
	        VkDescriptorSetLayout descLayout = descriptorSetLayout->getDescriptorSetLayout(context);
	        pipelineLayoutInfo.pSetLayouts = &descLayout;

	        if (vkCreatePipelineLayout(state.getDevice()->getDevice(), &pipelineLayoutInfo, nullptr, &pipelineState->pipelineLayout) != VK_SUCCESS) {
	            throw std::runtime_error("failed to create pipeline layout!");
	        }

	        VkGraphicsPipelineCreateInfo pipelineInfo = {};
	        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	        pipelineInfo.stageCount = shaderStages.size();
	        pipelineInfo.pStages = shaderStages.data();
	        pipelineInfo.pVertexInputState = &vertexInputInfo;
	        pipelineInfo.pInputAssemblyState = &inputAssembly;
	        pipelineInfo.pViewportState = &viewportState;
	        pipelineInfo.pRasterizationState = &rasterizer;
	        pipelineInfo.pMultisampleState = &multisampling;
	        pipelineInfo.pColorBlendState = &colorBlending;
	        pipelineInfo.layout = pipelineState->pipelineLayout;
	        pipelineInfo.renderPass = state.getRenderPass().get()->getRenderPass(context);
	        pipelineInfo.subpass = 0;
	        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	        if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipelineState->graphicsPipeline) != VK_SUCCESS) {
	            throw std::runtime_error("failed to create graphics pipeline!");
	        }			
		}

		if ((state.getRenderMode().get() & VULKAN_RENDER_COMMAND) == VULKAN_RENDER_COMMAND) {
			GraphicsPipelineState* pipelineState = contextHandler.getDisplayState(context);
			vkCmdBindPipeline(state.getCommandBuffer().get()->getCommandBuffer(context), VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineState->graphicsPipeline);
			std::cout << "bind pipeline" << std::endl;
		}
	}
	void finishRender(const GraphicsContext& context, VulkanDeviceState& state) {
		if (state.getRenderMode().get() == VULKAN_RENDER_CLEANUP_DISPLAY) {
			GraphicsPipelineState* pipelineState = contextHandler.getDisplayState(context);
			vkDestroyPipeline(state.getDevice()->getDevice(), pipelineState->graphicsPipeline, nullptr);
	        vkDestroyPipelineLayout(state.getDevice()->getDevice(), pipelineState->pipelineLayout, nullptr);
			std::cout << "Destroy pipeline." << std::endl;
		}
	}

private:
	struct GraphicsPipelineState : public ContextState {
		VkPipeline graphicsPipeline;
		VkPipelineLayout pipelineLayout;
	};

	DisplayContextHandler<ContextState,GraphicsPipelineState,ContextState> contextHandler;
	VulkanDescriptorSetLayout* descriptorSetLayout;
};


class VulkanCmdBindDescriptorSet : public VulkanRenderObject {
public:
	VulkanCmdBindDescriptorSet(VulkanDescriptorSet* descriptorSet, VulkanGraphicsPipeline* graphicsPipeline) : descriptorSet(descriptorSet), graphicsPipeline(graphicsPipeline) { addType<VulkanCmdBindDescriptorSet>(); }
	virtual ~VulkanCmdBindDescriptorSet() {}

protected:
	void startRender(const GraphicsContext& context, VulkanDeviceState& state) {
		if ((state.getRenderMode().get() & VULKAN_RENDER_COMMAND) == VULKAN_RENDER_COMMAND) {
			VkDescriptorSet descSet = descriptorSet->getDescriptorSet(context);
            std::cout << VulkanSwapChainState::get(context).getSwapChain()->getName() << " " << descSet << std::endl;
            vkCmdBindDescriptorSets(state.getCommandBuffer().get()->getCommandBuffer(context), VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->getPipelineLayout(context), 0, 1, &descSet, 0, nullptr);
		}
	}

private:
	VulkanDescriptorSet* descriptorSet;
	VulkanGraphicsPipeline* graphicsPipeline;
};

inline void VulkanCommandBuffer::startRender(const GraphicsContext& context, VulkanDeviceState& state) {
	state.getCommandBuffer().push(this);

	if (state.getRenderMode().get() == VULKAN_RENDER_UPDATE) {
		CommandBufferState* commandBufferState = contextHandler.getState(context);
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = state.getCommandPool().get()->getCommandPool(context);
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(state.getDevice()->getDevice(), &allocInfo, &commandBufferState->commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }   
	}

	if ((state.getRenderMode().get() & VULKAN_RENDER_COMMAND) == VULKAN_RENDER_COMMAND) {
		CommandBufferState* commandBufferState = contextHandler.getState(context);

		std::cout << "Render command" << std::endl;

		/*if (commandBufferState->recorded) {
			// need to reset command buffer
		}*/

		VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBufferState->commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

	}
}

inline void VulkanCommandBuffer::finishRender(const GraphicsContext& context, VulkanDeviceState& state) {
	if (state.getRenderMode().get() == VULKAN_RENDER_CLEANUP) {
		CommandBufferState* commandBufferState = contextHandler.getState(context);
		vkFreeCommandBuffers(state.getDevice()->getDevice(), state.getCommandPool().get()->getCommandPool(context), 1, &commandBufferState->commandBuffer);
	}

	if (state.getRenderMode().get() == VULKAN_RENDER_COMMAND) {
		CommandBufferState* commandBufferState = contextHandler.getState(context);
		if (vkEndCommandBuffer(commandBufferState->commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }

	}

	state.getCommandBuffer().pop();
}




}

#endif
