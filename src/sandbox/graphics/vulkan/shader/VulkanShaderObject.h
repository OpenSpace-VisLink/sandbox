#ifndef SANDBOX_GRAPHICS_VULKAN_SHADER_VULKAN_SHADER_OBJECT_H_
#define SANDBOX_GRAPHICS_VULKAN_SHADER_VULKAN_SHADER_OBJECT_H_

#include "sandbox/Component.h"
#include "sandbox/graphics/vulkan/VulkanDeviceRenderer.h"

namespace sandbox {

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

}

#endif