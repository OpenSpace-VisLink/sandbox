#ifndef SANDBOX_GRAPHICS_VULKAN_BUFFER_VULKAN_BUFFER_H_
#define SANDBOX_GRAPHICS_VULKAN_BUFFER_VULKAN_BUFFER_H_

#include "sandbox/graphics/vulkan/Vulkan.h"

namespace sandbox {

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

        return 0;
    }

private:
	VkBuffer buffer;
	VkDeviceMemory bufferMemory;
	const VulkanDevice* device;
};


}

#endif