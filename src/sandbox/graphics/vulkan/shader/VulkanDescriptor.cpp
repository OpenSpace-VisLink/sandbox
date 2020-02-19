#include "sandbox/graphics/vulkan/VulkanDeviceState.h"
#include "sandbox/graphics/vulkan/shader/VulkanDescriptor.h"

namespace sandbox {

	void VulkanDescriptorSetLayout::startRender(const GraphicsContext& context, VulkanDeviceState& state) {
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

	void VulkanDescriptorSetLayout::finishRender(const GraphicsContext& context, VulkanDeviceState& state) {
		if (state.getRenderMode().get() == VULKAN_RENDER_CLEANUP_SHARED) {
			DescriptorSetLayoutState* layoutState = contextHandler.getSharedState(context);
			vkDestroyDescriptorSetLayout(state.getDevice()->getDevice(), layoutState->descriptorSetLayout, nullptr);
			std::cout << "destroy descriptor layout" << std::endl;
		}
	}


	void VulkanSwapChainDescriptorPool::startRender(const GraphicsContext& context, VulkanDeviceState& state) {
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

	void VulkanSwapChainDescriptorPool::finishRender(const GraphicsContext& context, VulkanDeviceState& state) {
		if (state.getRenderMode().get() == VULKAN_RENDER_CLEANUP_DISPLAY) {
			std::cout << "destroy descriptor pool" << std::endl;
			vkDestroyDescriptorPool(state.getDevice()->getDevice(), getDescriptorPool(context), nullptr);
		}
	}

	void VulkanDescriptorSet::startRender(const GraphicsContext& context, VulkanDeviceState& state) {
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

	void VulkanDescriptorSet::finishRender(const GraphicsContext& context, VulkanDeviceState& state) {
		/*if (state.getRenderMode().get() == VULKAN_RENDER_CLEANUP_SHARED) {
			DescriptorSetState* descriptorSetState = contextHandler.getState(context);
			//vkDestroyDescriptorPool(state.getDevice()->getDevice(), descriptorSetState->descriptorSet, nullptr);
			// Doesn't appear necessary to destroy?
		}*/
	}
}