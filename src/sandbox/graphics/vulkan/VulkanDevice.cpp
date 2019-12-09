#include "sandbox/graphics/vulkan/VulkanDevice.h"
#include "sandbox/graphics/vulkan/VulkanQueue.h"

namespace sandbox {

void VulkanDevice::update() {
	if (initialized) {
		return;
	}

	if (instanceEntity) {
		vulkanInstance = instanceEntity->getComponent<VulkanInstance>();
	}

	if (vulkanInstance) {
		instance = vulkanInstance->getInstance();

		PhysicalDeviceCriteriaComposite composite;
		std::vector<VulkanComponent*> criteria = getEntity().getComponentsRecursive<VulkanComponent>();
		for (int f = 0; f < criteria.size(); f++) {
			VulkanPhysicalDeviceCriteria* physicalCriteria = criteria[f]->createPhysicalCriteria();
			if (physicalCriteria) {
				composite.add(physicalCriteria);
			}
		}
		physicalDevice = vulkanInstance->pickPhysicalDevice(composite);


		if (physicalDevice != VK_NULL_HANDLE) {
			vkGetPhysicalDeviceProperties(physicalDevice, &properties);

			std::vector<VulkanDeviceComponent*> deviceComponents = getEntity().getComponentsRecursive<VulkanDeviceComponent>();
			for (VulkanDeviceComponent* component : deviceComponents) {
				component->setDevice(this);
			}
				
			std::vector<VulkanQueue*> queues = getEntity().getComponentsRecursive<VulkanQueue>();

			std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

			std::set<uint32_t> uniqueQueueFamilies;// = {indices.graphicsFamily.value(), indices.presentFamily.value()};
			for (VulkanQueue* queue : queues) {
				uniqueQueueFamilies.insert(queue->getIndex());
			}

			float queuePriority = 1.0f;
	        for (uint32_t queue : uniqueQueueFamilies) {

	            VkDeviceQueueCreateInfo queueCreateInfo = {};
	            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	            queueCreateInfo.queueFamilyIndex = queue;
	            queueCreateInfo.queueCount = 1;
	            queueCreateInfo.pQueuePriorities = &queuePriority;
	            queueCreateInfos.push_back(queueCreateInfo);
	        }

	        VkPhysicalDeviceFeatures deviceFeatures = {};
	        deviceFeatures.samplerAnisotropy = VK_TRUE;

	        VkDeviceCreateInfo createInfo = {};
	        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	        createInfo.pQueueCreateInfos = queueCreateInfos.data();

	        createInfo.pEnabledFeatures = &deviceFeatures;

	        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	        if (vulkanInstance->validationLayersEnabled()) {
	            createInfo.enabledLayerCount = static_cast<uint32_t>(vulkanInstance->getValidationLayers().size());
	            createInfo.ppEnabledLayerNames = vulkanInstance->getValidationLayers().data();
	        } else {
	            createInfo.enabledLayerCount = 0;
	        }

	        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
	            throw std::runtime_error("failed to create logical device!");
	        }

			initialized = true;
		}

	}

}

}