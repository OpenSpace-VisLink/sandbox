#ifndef SANDBOX_GRAPHICS_VULKAN_BUFFER_VULKAN_BUFFER_TYPES_H_
#define SANDBOX_GRAPHICS_VULKAN_BUFFER_VULKAN_BUFFER_TYPES_H_

#include "sandbox/graphics/vulkan/buffer/VulkanBuffer.h"
#include "sandbox/graphics/vulkan/shader/VulkanShaderObject.h"

namespace sandbox {

class VulkanUniformBuffer : public VulkanShaderObject {
public:
	VulkanUniformBuffer(bool dynamic = false) : dynamic(dynamic) { addType<VulkanUniformBuffer>(); }
	virtual ~VulkanUniformBuffer() {}

	void startRender(const GraphicsContext& context, VulkanDeviceState& state) {
		UniformBufferState* uboState = contextHandler.getState(context);
		if (state.getRenderMode().get() == VULKAN_RENDER_UPDATE) {
			int bufferSize = getBufferSize();
			std::cout << "Create Buffer " << bufferSize << std::endl;
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
	    binding.descriptorType = dynamic ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC : VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	    binding.pImmutableSamplers = nullptr;
	}

	void writeDescriptor(const GraphicsContext& context, VkWriteDescriptorSet& descriptorWrite, std::vector<DescriptorObject*>& descriptorObjects) {
        VkDescriptorBufferInfo& bufferInfo = *(new VkDescriptorBufferInfo());
        //*bufferInfo = {};
		std::cout << "Write Buffer" << std::endl;
        bufferInfo.buffer = getBuffer(context);
        bufferInfo.offset = 0;
        bufferInfo.range = getRange();
		std::cout << "Write Buffer Size" << getRange() << std::endl;
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
        descriptorWrite.descriptorType = dynamic ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC : VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
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
		poolSize.type = dynamic ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC : VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	}

	virtual size_t getRange() const {getBufferSize();}

protected:
	virtual size_t getBufferSize() const = 0;
	virtual void updateBuffer(const GraphicsContext& context, VulkanDeviceState& state, VulkanBuffer* buffer) = 0;


private:
	struct UniformBufferState : public ContextState {
		VulkanBuffer* buffer;
	};

	GraphicsContextHandler<ContextState,UniformBufferState> contextHandler;
	bool dynamic;
};

template<typename T>
class VulkanUniformBufferValue : public VulkanUniformBuffer {
public:
	VulkanUniformBufferValue(bool dynamic = false) : VulkanUniformBuffer(dynamic) { addType< VulkanUniformBufferValue<T> >(); }
	virtual ~VulkanUniformBufferValue() {}

	T value;

protected:
	size_t getBufferSize() const { return sizeof(T); }

	void updateBuffer(const GraphicsContext& context, VulkanDeviceState& state, VulkanBuffer* buffer) {
		buffer->update(&value, getBufferSize());
	}
};

template<typename T>
class VulkanUniformArrayBuffer : public VulkanUniformBufferValue< std::vector<T> > {
public:
	VulkanUniformArrayBuffer(bool dynamic=  false) : VulkanUniformBufferValue< std::vector<T> >(dynamic) {}
	virtual ~VulkanUniformArrayBuffer() {}
	void updateBuffer(const GraphicsContext& context, VulkanDeviceState& state, VulkanBuffer* buffer) {
		buffer->update(getData(), getBufferSize());
	}

protected:
	VkDeviceSize getBufferSize() const { return this->value.size()*sizeof(T); }
	void* getData() { return this->value.data(); }
};

template<typename T>
class VulkanAlignedUniformArrayBuffer : public VulkanUniformBufferValue<T*> {
public:
	VulkanAlignedUniformArrayBuffer(int numInstances, bool dynamic = false) : VulkanUniformBufferValue<T*>(dynamic), numInstances(numInstances) {
        this->value = NULL;
	}

	void startRender(const GraphicsContext& context, VulkanDeviceState& state) {
		if (!this->value && state.getRenderMode().get() == VULKAN_RENDER_UPDATE) {
			size_t minAlignment = state.getDevice()->getProperties().limits.minUniformBufferOffsetAlignment;
			std::cout << "minAlignment " << minAlignment << std::endl;
	        dynamicAlignment = sizeof(T);
	        if (minAlignment > 0) {
	            dynamicAlignment = (dynamicAlignment + minAlignment - 1) & ~(minAlignment - 1);
	        }
	        std::cout << "dynamicAlignment " << dynamicAlignment << std::endl;
			this->value = static_cast<T*>(alignedAlloc(numInstances*dynamicAlignment, dynamicAlignment));
		}

		VulkanUniformBufferValue<T*>::startRender(context, state);

	}

	virtual ~VulkanAlignedUniformArrayBuffer() {
		if (this->value) {
			alignedFree(this->value);
		}
	}

	void updateBuffer(const GraphicsContext& context, VulkanDeviceState& state, VulkanBuffer* buffer) {
		buffer->update(getData(), getBufferSize());
	}
	
	T* getItem(int index) { return (T*)((uint64_t)this->value + (index * dynamicAlignment)); }

protected:
	
	VkDeviceSize getBufferSize() const { return numInstances*dynamicAlignment; }
	size_t getRange() const { return dynamicAlignment; }
	void* getData() { return this->value; }

private:
	// Wrapper functions for aligned memory allocation
	// There is currently no standard for this in C++ that works across all platforms and vendors, so we abstract this
	void* alignedAlloc(size_t size, size_t alignment)
	{
		void *data = nullptr;
	#if defined(_MSC_VER) || defined(__MINGW32__)
		data = _aligned_malloc(size, alignment);
	#else 
		int res = posix_memalign(&data, alignment, size);
		if (res != 0)
			data = nullptr;
	#endif
		return data;
	}

	void alignedFree(void* data)
	{
	#if	defined(_MSC_VER) || defined(__MINGW32__)
		_aligned_free(data);
	#else 
		free(data);
	#endif
	}

	int numInstances;
	size_t dynamicAlignment;
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
	
	virtual int getNumValues() { return 1; }

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

	int getNumValues() { return this->value.size(); }

protected:
	VkDeviceSize getBufferSize() const { return this->value.size()*sizeof(T); }
	void* getData() { return this->value.data(); }
};

template<typename T>
class VertexArray : public VulkanArrayBuffer<T> {
public:
	VertexArray(int binding = 0) : VulkanArrayBuffer<T>(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, true), binding(binding) {}
	virtual ~VertexArray() {}

	void startRender(const GraphicsContext& context, VulkanDeviceState& state) {
		VulkanArrayBuffer<T>::startRender(context, state);

		if ((state.getRenderMode().get() & VULKAN_RENDER_COMMAND) == VULKAN_RENDER_COMMAND) {
			std::cout << "bind vertex array" << std::endl;
			VkBuffer vertexBuffers[] = {VulkanArrayBuffer<T>::getBuffer(context)};
            VkDeviceSize offsets[] = {0};
            vkCmdBindVertexBuffers(state.getCommandBuffer().get()->getCommandBuffer(context), binding, 1, vertexBuffers, offsets);
		}
	}

private:
	int binding;
};

template<typename T>
class ObjectArray : public VulkanArrayBuffer<T> {
public:
	ObjectArray() : VulkanArrayBuffer<T>(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, true) {}
	virtual ~ObjectArray() {}
};

//typedef ObjectArray<uint16_t> IndexArray;
class IndexArray16 : public ObjectArray<uint16_t> {
protected:
	void startRender(const GraphicsContext& context, VulkanDeviceState& state) {
		ObjectArray<uint16_t>::startRender(context, state);

		if ((state.getRenderMode().get() & VULKAN_RENDER_COMMAND) == VULKAN_RENDER_COMMAND) {
			std::cout << "bind index array and draw" << std::endl;
            vkCmdBindIndexBuffer(state.getCommandBuffer().get()->getCommandBuffer(context), getBuffer(context), 0, VK_INDEX_TYPE_UINT16);
            vkCmdDrawIndexed(state.getCommandBuffer().get()->getCommandBuffer(context), static_cast<uint32_t>(value.size()), state.getNumInstances().get(), 0, 0, 0);
		}
	}
};

//typedef ObjectArray<uint16_t> IndexArray;
class IndexArray : public ObjectArray<uint32_t> {
protected:
	void startRender(const GraphicsContext& context, VulkanDeviceState& state) {
		ObjectArray<uint32_t>::startRender(context, state);

		if ((state.getRenderMode().get() & VULKAN_RENDER_COMMAND) == VULKAN_RENDER_COMMAND) {
			std::cout << "bind index array and draw" << std::endl;
            vkCmdBindIndexBuffer(state.getCommandBuffer().get()->getCommandBuffer(context), getBuffer(context), 0, VK_INDEX_TYPE_UINT32);
            vkCmdDrawIndexed(state.getCommandBuffer().get()->getCommandBuffer(context), static_cast<uint32_t>(value.size()), state.getNumInstances().get(), 0, 0, 0);
		}
	}
};



}

#endif