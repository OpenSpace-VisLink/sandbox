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
#include "sandbox/graphics/vulkan/render/VulkanFramebuffer.h"
#include "sandbox/graphics/vulkan/VulkanDeviceState.h"


#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <fstream>
#include <chrono>

#include <sandbox/image/Image.h>

namespace sandbox {






void VulkanRenderPass::startRender(const GraphicsContext& context, VulkanDeviceState& state) {
	state.getRenderPass().push(this);

	if ((state.getRenderMode().get() & VULKAN_RENDER_COMMAND) == VULKAN_RENDER_COMMAND) {
		VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = getRenderPass(context);
        renderPassInfo.framebuffer = getEntity().getComponent<VulkanFramebuffer>()->getFramebuffer(context);
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = state.getExtent();

        VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(state.getCommandBuffer().get()->getCommandBuffer(context), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        std::cout << "Begin render pass command" << std::endl;
	}
	        
}

void VulkanRenderPass::finishRender(const GraphicsContext& context, VulkanDeviceState& state) {
	if ((state.getRenderMode().get() & VULKAN_RENDER_COMMAND) == VULKAN_RENDER_COMMAND) {
        std::cout << "End render pass command" << std::endl;
        vkCmdEndRenderPass(state.getCommandBuffer().get()->getCommandBuffer(context));
	}

	state.getRenderPass().pop();
}



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


class VulkanSwapChainFramebuffer : public VulkanFramebuffer {
public:
	VulkanSwapChainFramebuffer() { addType<VulkanSwapChainFramebuffer>(); }
	virtual ~VulkanSwapChainFramebuffer() {}

	void startRender(const GraphicsContext& context, VulkanDeviceState& state) {
		if (state.getRenderMode().get() == VULKAN_RENDER_UPDATE) {
			VulkanSwapChainState& swapChainState = VulkanSwapChainState::get(context);
			VulkanSwapChain* swapChain = swapChainState.getSwapChain();

	        VkImageView attachments[] = {
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


inline void VulkanDeviceRenderer::update() {
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

inline void VulkanDeviceRenderer::render(VulkanRenderMode renderMode) {
	state->getRenderMode().push(renderMode);
	GraphicsRenderer::render();
	state->getRenderMode().pop();
}


inline void VulkanRenderObject::startRender(const GraphicsContext& context) {
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
inline void VulkanRenderObject::finishRender(const GraphicsContext& context) {
	VulkanDeviceState& state = VulkanDeviceState::get(context);
	finishRender(context, state);
}


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



struct UniformBufferObject {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

class VulkanBuffer {
public:
	VulkanBuffer(const VulkanDevice* device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) : device(device) {
		createBuffer(size, usage, properties, buffer, bufferMemory);
	}
	virtual ~VulkanBuffer() {
        vkDestroyBuffer(device->getDevice(), buffer, nullptr);
        vkFreeMemory(device->getDevice(), bufferMemory, nullptr);
	}

	void update(const void* newData, size_t size) {
        void* data;
        vkMapMemory(device->getDevice(), bufferMemory, 0, size, 0, &data);
            memcpy(data, newData, size);
        vkUnmapMemory(device->getDevice(), bufferMemory);
	}

	void copyTo(VulkanBuffer* vulkanBuffer, VkDeviceSize size, VulkanCommandPool* commandPool, const GraphicsContext& context) {
        VkCommandBuffer commandBuffer = commandPool->beginSingleTimeCommands(context);

        VkBufferCopy copyRegion = {};
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, buffer, vulkanBuffer->buffer, 1, &copyRegion);

        commandPool->endSingleTimeCommands(context, commandBuffer);
    }

	VkBuffer getBuffer() const { return buffer; }

private:
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(device->getDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create buffer!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device->getDevice(), buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(device->getDevice(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate buffer memory!");
        }

        vkBindBufferMemory(device->getDevice(), buffer, bufferMemory, 0);
    }

    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(device->getPhysicalDevice(), &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }
    }

private:
	VkBuffer buffer;
	VkDeviceMemory bufferMemory;
	const VulkanDevice* device;
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

class MainUniformBuffer : public VulkanUniformBufferValue<UniformBufferObject> {

protected:
	void updateBuffer(const GraphicsContext& context, VulkanDeviceState& state, VulkanBuffer* buffer) {
		static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
		//UniformBufferObject ubo = {};
		UniformBufferObject ubo = {};
        ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		if (VulkanSwapChainState::get(context).getSwapChain()->getName() == "window 2") {
        	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		}
		if (VulkanSwapChainState::get(context).getSwapChain()->getName() == "window 3") {
        	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		}
        ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.proj = glm::perspective(glm::radians(45.0f), (float) state.getExtent().width / (float) state.getExtent().height, 0.1f, 10.0f);
        //std::cout << VulkanSwapChainState::get(context).getSwapChain()->getName() << " " << state.getExtent().width << " " << (float) state.getExtent().height << std::endl;
        ubo.proj[1][1] *= -1;
		//VulkanUniformBufferValue<UniformBufferObject>::updateBuffer(context, state, buffer);

		buffer->update(&ubo, sizeof(ubo));
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

class VulkanImage : public VulkanRenderObject {
public:
	VulkanImage() : image(NULL) { addType<VulkanImage>(); }
	virtual ~VulkanImage() {}

	void update() {
		if (!image) {
			image = getEntity().getComponent<Image>();
		}
	}

	VkImage getImage(const GraphicsContext& context) const { return contextHandler.getSharedState(context)->image; }

protected:
	void startRender(const GraphicsContext& context, VulkanDeviceState& state) {
		if (!image) { return; }

		if (state.getRenderMode().get() == VULKAN_RENDER_UPDATE_SHARED) {
			ImageState* imageState = contextHandler.getSharedState(context);
			int texWidth, texHeight, texChannels;
	        texHeight = image->getHeight();
	        texWidth = image->getWidth();
	        texChannels = image->getComponents();
	        std::cout << "blah " << texChannels << std::endl;
	        const unsigned char* pixels = image->getData();
	        VkDeviceSize imageSize = texWidth * texHeight * texChannels; 

	        if (!pixels) {
	            throw std::runtime_error("failed to load texture image!");
	        }

	        VulkanBuffer* stagingBuffer = new VulkanBuffer(state.getDevice(), imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	        //VkBuffer stagingBuffer;
	        //VkDeviceMemory stagingBufferMemory;
	        //createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	        stagingBuffer->update(pixels, imageSize);
	        /*void* data;
	        vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
	            memcpy(data, pixels, static_cast<size_t>(imageSize));
	        vkUnmapMemory(device, stagingBufferMemory);*/

	        //stbi_image_free(pixels);

	        createImage(state.getDevice(), texWidth, texHeight, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, imageState->image, imageState->imageMemory);

	        transitionImageLayout(imageState->image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, state.getCommandPool().get(), context);
	            copyBufferToImage(stagingBuffer->getBuffer(), imageState->image, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), state.getCommandPool().get(), context);
	        transitionImageLayout(imageState->image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, state.getCommandPool().get(), context);

	        //vkDestroyBuffer(state.getDevice()->getDevice(), stagingBuffer, nullptr);
	        //vkFreeMemory(state.getDevice()->getDevice(), stagingBufferMemory, nullptr);
	        delete stagingBuffer;
		}
	}

	void finishRender(const GraphicsContext& context, VulkanDeviceState& state) {
		if (!image) { return; }

		if (state.getRenderMode().get() == VULKAN_RENDER_CLEANUP_SHARED) {
			ImageState* imageState = contextHandler.getSharedState(context);
			vkDestroyImage(state.getDevice()->getDevice(), imageState->image, nullptr);
			vkFreeMemory(state.getDevice()->getDevice(), imageState->imageMemory, nullptr);
		}
	}


private:
	struct ImageState : public ContextState {
		VkImage image;
    	VkDeviceMemory imageMemory;
	};

	void createImage(const VulkanDevice* device, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) {
        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateImage(device->getDevice(), &imageInfo, nullptr, &image) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image!");
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(device->getDevice(), image, &memRequirements);

        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = device->findMemoryType(memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(device->getDevice(), &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate image memory!");
        }

        vkBindImageMemory(device->getDevice(), image, imageMemory, 0);
    }

    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VulkanCommandPool* commandPool, const GraphicsContext& context) {
        VkCommandBuffer commandBuffer = commandPool->beginSingleTimeCommands(context);

        VkImageMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else {
            throw std::invalid_argument("unsupported layout transition!");
        }

        vkCmdPipelineBarrier(
            commandBuffer,
            sourceStage, destinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );

        commandPool->endSingleTimeCommands(context, commandBuffer);
    }

    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, VulkanCommandPool* commandPool, const GraphicsContext& context) {
        VkCommandBuffer commandBuffer = commandPool->beginSingleTimeCommands(context);

        VkBufferImageCopy region = {};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = {0, 0, 0};
        region.imageExtent = {
            width,
            height,
            1
        };

        vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        commandPool->endSingleTimeCommands(context, commandBuffer);
    }

	GraphicsContextHandler<ImageState,ContextState> contextHandler;
	sandbox::Image* image;

};

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

class AllowRenderModes : public VulkanRenderObject {
public:
	AllowRenderModes(VulkanRenderMode renderMode) : renderMode(renderMode) { addType<AllowRenderModes>(); }
	AllowRenderModes(int renderMode) : renderMode(static_cast<VulkanRenderMode>(renderMode)) { addType<AllowRenderModes>(); }
	virtual ~AllowRenderModes() {}

protected:
	void startRender(const GraphicsContext& context, VulkanDeviceState& state) {
		VulkanRenderMode currentRenderMode = state.getRenderMode().get();
		state.getRenderMode().push(static_cast<VulkanRenderMode>(renderMode & currentRenderMode));
	}

	void finishRender(const GraphicsContext& context, VulkanDeviceState& state) {
		state.getRenderMode().pop();
	}

private:
	VulkanRenderMode renderMode;
};

class VulkanFrameRenderer : public VulkanDeviceComponent {
public:
	VulkanFrameRenderer(VulkanQueue* graphicsQueue, VulkanQueue* presentQueue) : graphicsQueue(graphicsQueue), presentQueue(presentQueue), initialized(false) { addType<VulkanFrameRenderer>(); }
	virtual ~VulkanFrameRenderer() {
		for (size_t i = 0; i < maxFramesInFlight; i++) {
            vkDestroySemaphore(getDevice().getDevice(), renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(getDevice().getDevice(), imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(getDevice().getDevice(), inFlightFences[i], nullptr);
        }
	}

	void recreateSwapChain(VulkanSwapChain* swapChain) {
        /*int width = 0, height = 0;
        while (width == 0 || height == 0) {
            glfwGetFramebufferSize(window, &width, &height);
            glfwWaitEvents();
        }*/

        vkDeviceWaitIdle(getDevice().getDevice());

        swapChain->getEntity().incrementVersion();
        swapChain->getEntity().update();

        std::cout << swapChain << std::endl;
        std::vector<VulkanDeviceRenderer*> renderers = swapChain->getEntity().getComponentsRecursive<VulkanDeviceRenderer>();
        std::cout << "num renderers: " << renderers.size() << std::endl;
        renderers[0]->render(VULKAN_RENDER_UPDATE_SHARED);
        renderers[0]->render(VULKAN_RENDER_UPDATE_DISPLAY);
        for (int f = 0; f < renderers.size(); f++) {  
            renderers[f]->render(VULKAN_RENDER_UPDATE);
            renderers[f]->render(VULKAN_RENDER_OBJECT);
            renderers[f]->render(VULKAN_RENDER_COMMAND);
        }

    }

    void update() {
    	if (!initialized) {
    		createSyncObjects();
    		initialized = true;
    	}
    }

    void createSyncObjects() {
        imageAvailableSemaphores.resize(maxFramesInFlight);
        renderFinishedSemaphores.resize(maxFramesInFlight);
        inFlightFences.resize(maxFramesInFlight);

        VkSemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < maxFramesInFlight; i++) {
            if (vkCreateSemaphore(getDevice().getDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(getDevice().getDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(getDevice().getDevice(), &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create synchronization objects for a frame!");
            }
        }
    }

    void drawFrame(bool incrementCurrentFrame = true, Entity* renderNode = NULL) {
    	if (!renderNode) {
    		renderNode = &getEntity();
    	}

        vkWaitForFences(getDevice().getDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

        VkSwapchainKHR swapChain = renderNode->getComponent<VulkanSwapChain>()->getSwapChain();

        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(getDevice().getDevice(), swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            std::cout << "Out of date" << std::endl;
            recreateSwapChain(renderNode->getComponent<VulkanSwapChain>());
            return;
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        VulkanDeviceRenderer* vulkanRenderer = renderNode->getComponents<VulkanDeviceRenderer>()[imageIndex];
        vulkanRenderer->render(VULKAN_RENDER_OBJECT);

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        VkCommandBuffer cmdBuffer = renderNode->getComponentRecursive<VulkanCommandBuffer>()->getCommandBuffer(vulkanRenderer->getContext());
        submitInfo.pCommandBuffers = &cmdBuffer;

        VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        vkResetFences(getDevice().getDevice(), 1, &inFlightFences[currentFrame]);

        if (vkQueueSubmit(graphicsQueue->getQueue(), 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {swapChain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;

        presentInfo.pImageIndices = &imageIndex;

        result = vkQueuePresentKHR(presentQueue->getQueue(), &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
            std::cout << "Out of date / suboptimal / resized" << std::endl;
            framebufferResized = false;
            recreateSwapChain(renderNode->getComponent<VulkanSwapChain>());
        } else if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }

        if (incrementCurrentFrame) {
        	incrementFrame();
        }
    }

    void incrementFrame() {
    	currentFrame = (currentFrame + 1) % maxFramesInFlight;
    }

private:
	std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    size_t currentFrame = 0;
    bool framebufferResized = false;
    int maxFramesInFlight = 2;
    VulkanQueue* graphicsQueue;
    VulkanQueue* presentQueue;
    bool initialized;
};

}

#endif
