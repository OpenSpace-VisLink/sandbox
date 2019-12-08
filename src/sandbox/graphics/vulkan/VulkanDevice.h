#ifndef SANDBOX_GRAPHICS_VULKAN_DEVICE_H_
#define SANDBOX_GRAPHICS_VULKAN_DEVICE_H_

#include "sandbox/Component.h"
#include "sandbox/graphics/vulkan/Vulkan.h"
#include "sandbox/graphics/vulkan/VulkanInstance.h"

namespace sandbox {

class VulkanDevice;

class VulkanDeviceComponent : public VulkanComponent {
public:
	VulkanDeviceComponent() { addType<VulkanDeviceComponent>(); }

	void setDevice(VulkanDevice* device) {
		this->device = device;
		initDeviceComponent();
	}
	VulkanDevice& getDevice() { return *device; }

protected:
	virtual void initDeviceComponent() {}

private:
	VulkanDevice* device;
};



class VulkanDevice : public VulkanComponent {
public:
	VulkanDevice(Entity* instanceEntity) : instanceEntity(instanceEntity), instance(NULL), initialized(false) { addType<VulkanDevice>(); }
	virtual ~VulkanDevice() {
		vkDestroyDevice(device, nullptr);
	}

	void update();

	VulkanPhysicalDeviceCriteria* createPhysicalCriteria() const { 
		PhysicalDeviceCriteriaComposite* composite = new PhysicalDeviceCriteriaComposite();
		composite->add(new DeviceSamplerAnisotropySupport());
		composite->add(new DeviceExtensionSupport(deviceExtensions));
		return composite; 
	}

	virtual const VkDevice& getDevice() const { return device; }
	virtual const VkPhysicalDevice& getPhysicalDevice() const { return physicalDevice; }
	virtual const VulkanInstance& getInstance() const { return *vulkanInstance; }

	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

    VkImageView createImageView(VkImage image, VkFormat format) const {
        VkImageViewCreateInfo viewInfo = {};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        VkImageView imageView;
        if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture image view!");
        }

        return imageView;
    }

private:
	bool initialized;
	Entity* instanceEntity;
	VulkanInstance* vulkanInstance;
	VkDevice device;
	VkPhysicalDevice physicalDevice;
	VkInstance instance;

	const std::vector<const char*> deviceExtensions = {
	    VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
};

}

#endif