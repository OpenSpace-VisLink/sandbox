#ifndef SANDBOX_GRAPHICS_VULKAN_QUEUE_H_
#define SANDBOX_GRAPHICS_VULKAN_QUEUE_H_

#include "sandbox/Component.h"
#include "sandbox/graphics/vulkan/Vulkan.h"
#include "sandbox/graphics/vulkan/VulkanDevice.h"
#include "sandbox/graphics/vulkan/VulkanSurface.h"

namespace sandbox {

class VulkanQueue : public VulkanDeviceComponent {
public:
	VulkanQueue() { addType<VulkanQueue>(); }
	virtual ~VulkanQueue() {}

	void update();

	uint32_t getIndex() const { return index; }
	VkQueue getQueue() const { return queue; }

protected:
	void initDeviceComponent();
	virtual bool isQueueFamily(VkPhysicalDevice device, const VkQueueFamilyProperties& queueFamily, int index) const = 0;

private:
	VkQueue queue;
	uint32_t index;
};

class VulkanGraphicsQueue : public VulkanQueue {
public:
	VulkanGraphicsQueue() { addType<VulkanGraphicsQueue>(); }
	virtual ~VulkanGraphicsQueue() {}

	VulkanPhysicalDeviceCriteria* createPhysicalCriteria() const { return new DeviceGraphicsSupport(); }

protected:
	virtual bool isQueueFamily(VkPhysicalDevice device, const VkQueueFamilyProperties& queueFamily, int index) const {
		return queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT;
	}

private:
	VkQueue queue;
	uint32_t index;
};

class VulkanPresentQueue : public VulkanQueue {
public:
	VulkanPresentQueue(Entity* surfaceEntity) : surfaceEntity(surfaceEntity) { addType<VulkanPresentQueue>(); }
	virtual ~VulkanPresentQueue() {}

	VulkanPhysicalDeviceCriteria* createPhysicalCriteria() const { 
		VulkanSurface* surface = surfaceEntity->getComponent<VulkanSurface>();
		if (surface) {
			return new DevicePresentSupport(surface->getSurface()); 
		}

		return NULL;
	}

protected:
	virtual bool isQueueFamily(VkPhysicalDevice device, const VkQueueFamilyProperties& queueFamily, int index) const {
		VulkanSurface* surface = surfaceEntity->getComponent<VulkanSurface>();
		if (surface) {
			VkBool32 presentSupport = false;
	        vkGetPhysicalDeviceSurfaceSupportKHR(device, index, surface->getSurface(), &presentSupport);
	        return queueFamily.queueCount > 0 && presentSupport;
		}

		return false;
	}

private:
	Entity* surfaceEntity;
	VkQueue queue;
	uint32_t index;
};

inline void VulkanQueue::initDeviceComponent() {
	VkPhysicalDevice device = getDevice().getPhysicalDevice();
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const VkQueueFamilyProperties& queueFamily : queueFamilies) {
    	if (isQueueFamily(device, queueFamily, i)) {
    		index = i;
    		break;
    	}

        i++;
    }
}

inline void VulkanQueue::update() {
	vkGetDeviceQueue(getDevice().getDevice(), index, 0, &queue);
}

}

#endif