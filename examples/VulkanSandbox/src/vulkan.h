#ifndef SANDBOX_VULCAN_H_
#define SANDBOX_VULCAN_H_

#include "sandbox/Component.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <set>

namespace sandbox {

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class VulkanInstance;

class VulkanPhysicalDeviceCriteria {
public:
	virtual ~VulkanPhysicalDeviceCriteria() {}
	virtual bool isDeviceSuitable(const VulkanInstance& instance, VkPhysicalDevice device) const = 0;
};

class VulkanComponent : public Component {
public:
	virtual ~VulkanComponent() {}
	virtual VulkanPhysicalDeviceCriteria* createPhysicalCriteria() const { return NULL; }
};

class VulkanInstance : public VulkanComponent {
public:
	VulkanInstance() : initialized(false) { addType<VulkanInstance>(); }
	virtual ~VulkanInstance() {
		if (enableValidationLayers) {
            DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        }

		vkDestroyInstance(instance, nullptr);
	}

	void update() {
		if (initialized) {
			return;
		}
		initialized = true;

        if (enableValidationLayers && !checkValidationLayerSupport()) {
            throw std::runtime_error("validation layers requested, but not available!");
        }

        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        auto extensions = getRequiredExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();

            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
        } else {
            createInfo.enabledLayerCount = 0;

            createInfo.pNext = nullptr;
        }

        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
            throw std::runtime_error("failed to create instance!");
        }

        setupDebugMessenger();

	}

	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
    }

    void setupDebugMessenger() {
        if (!enableValidationLayers) return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        populateDebugMessengerCreateInfo(createInfo);

        if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
            throw std::runtime_error("failed to set up debug messenger!");
        }
    }

    bool checkValidationLayerSupport() {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : validationLayers) {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) {
                return false;
            }
        }

        return true;
    }

    std::vector<const char*> getRequiredExtensions() {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (enableValidationLayers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	    if (func != nullptr) {
	        func(instance, debugMessenger, pAllocator);
	    }
	}

	const VkInstance& getInstance() const { return instance; }

	bool validationLayersEnabled() { return enableValidationLayers; }
	const std::vector<const char*>& getValidationLayers() { return validationLayers; }

	VkPhysicalDevice pickPhysicalDevice(const VulkanPhysicalDeviceCriteria& criteria) {
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

        if (deviceCount == 0) {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        for (const auto& device : devices) {
            if (criteria.isDeviceSuitable(*this, device)) {
                physicalDevice = device;
                break;
            }
        }

        if (physicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error("failed to find a suitable GPU!");
        }

        return physicalDevice;
    }

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) const {
        SwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
        }

        return details;
    }

private:
	bool initialized;
	VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

	const std::vector<const char*> validationLayers = {
	    "VK_LAYER_LUNARG_standard_validation"
	};


	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	    if (func != nullptr) {
	        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	    } else {
	        return VK_ERROR_EXTENSION_NOT_PRESENT;
	    }
	}
};

class PhysicalDeviceCriteriaComposite : public VulkanPhysicalDeviceCriteria {
public:
	~PhysicalDeviceCriteriaComposite() {
		for (int f = 0; f < criteriaList.size(); f++) {
			delete criteriaList[f];
		}
	}
	void add(VulkanPhysicalDeviceCriteria* criteria) {
		criteriaList.push_back(criteria);
	}
	bool isDeviceSuitable(const VulkanInstance& instance, VkPhysicalDevice device) const {
		for (int f = 0; f < criteriaList.size(); f++) {
			if (!criteriaList[f]->isDeviceSuitable(instance, device)) {
				return false;
			}
		}

		return true;
	}
private:
	std::vector<VulkanPhysicalDeviceCriteria*> criteriaList;
};


class DeviceExtensionSupport : public VulkanPhysicalDeviceCriteria {
public:
	DeviceExtensionSupport(std::vector<const char*> deviceExtensions) : deviceExtensions(deviceExtensions) {}

	bool isDeviceSuitable(const VulkanInstance& instance, VkPhysicalDevice device) const {
		uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for (const auto& extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
	}
private:
	std::vector<const char*> deviceExtensions;
};

class DeviceSwapChainSupport : public VulkanPhysicalDeviceCriteria {
public:
    DeviceSwapChainSupport(const VkSurfaceKHR& surface) : surface(surface) {}
    bool isDeviceSuitable(const VulkanInstance& instance, VkPhysicalDevice device) const {
        SwapChainSupportDetails swapChainSupport = instance.querySwapChainSupport(device, surface);
        return !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }
private:
    VkSurfaceKHR surface;
};

class DeviceQueueFamilySupport : public VulkanPhysicalDeviceCriteria {
public:
	bool isDeviceSuitable(const VulkanInstance& instance, VkPhysicalDevice device) const {
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const VkQueueFamilyProperties& queueFamily : queueFamilies) {
        	if (isDeviceSuitable(instance, device, queueFamily, i)) {
        		return true;
        	}

        	i++;
        }

        return false;
	}

protected:
	virtual bool isDeviceSuitable(const VulkanInstance& instance, VkPhysicalDevice device, const VkQueueFamilyProperties& queueFamily, int index) const = 0;
};

class DeviceGraphicsSupport : public DeviceQueueFamilySupport {
protected:
	bool isDeviceSuitable(const VulkanInstance& instance, VkPhysicalDevice device, const VkQueueFamilyProperties& queueFamily, int index) const {
		return queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT;
	}
};

class DevicePresentSupport : public DeviceQueueFamilySupport {
public:
	DevicePresentSupport(const VkSurfaceKHR& surface) : surface(surface) {}
protected:
	bool isDeviceSuitable(const VulkanInstance& instance, VkPhysicalDevice device, const VkQueueFamilyProperties& queueFamily, int index) const {
		VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, index, surface, &presentSupport);
        return queueFamily.queueCount > 0 && presentSupport;
	}
private:
	VkSurfaceKHR surface;
};

class DeviceFeatureSupport : public VulkanPhysicalDeviceCriteria {
public:
	bool isDeviceSuitable(const VulkanInstance& instance, VkPhysicalDevice device) const {
		VkPhysicalDeviceFeatures supportedFeatures;
        vkGetPhysicalDeviceFeatures(device, &supportedFeatures);
        return isDeviceSuitable(supportedFeatures);
	}
protected:
	virtual bool isDeviceSuitable(const VkPhysicalDeviceFeatures& features) const = 0;
};


class DeviceSamplerAnisotropySupport : public DeviceFeatureSupport {
protected:
	bool isDeviceSuitable(const VkPhysicalDeviceFeatures& features) const {
        return features.samplerAnisotropy;
	}
};


class VulkanSurface : public VulkanComponent {
public:
	VulkanSurface() { 
		addType<VulkanSurface>();
	}

	virtual ~VulkanSurface() {}

	virtual const VkSurfaceKHR& getSurface() const = 0;
};


class GlfwSurface : public VulkanSurface {
public:
	GlfwSurface(GLFWwindow* window, Entity* instanceEntity = NULL) : VulkanSurface(), initialized(false), window(window), instanceEntity(instanceEntity) { addType<GlfwSurface>(); }
	virtual ~GlfwSurface() {
		vkDestroySurfaceKHR(instance, surface, nullptr);
	}

	void update() {
		if (initialized) {
			return;
		}
		initialized = true;

		if (!instanceEntity) {
			instanceEntity = &getEntity();
		}

		VulkanInstance* inst = instanceEntity->getComponent<VulkanInstance>();
		if (inst) {
			instance = inst->getInstance();
		}

		if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }
	}

	const VkSurfaceKHR& getSurface() const { return surface; }

private:
	bool initialized;
	Entity* instanceEntity;
	GLFWwindow* window;
	VkSurfaceKHR surface;
	VkInstance instance;
};

class VulkanQueue : public VulkanComponent {
public:
	VulkanQueue() { addType<VulkanQueue>(); }
	virtual ~VulkanQueue() {}

	void init(VkPhysicalDevice device) {
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

	void update();

	uint32_t getIndex() const { return index; }
	VkQueue getQueue() const { return queue; }

protected:
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

class VulkanDevice : public VulkanComponent {
public:
	VulkanDevice(Entity* instanceEntity) : instanceEntity(instanceEntity), instance(NULL), initialized(false) { addType<VulkanDevice>(); }
	virtual ~VulkanDevice() {
		vkDestroyDevice(device, nullptr);
	}

	void update() {
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
				std::vector<VulkanQueue*> queues = getEntity().getComponents<VulkanQueue>();

				std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

				std::set<uint32_t> uniqueQueueFamilies;// = {indices.graphicsFamily.value(), indices.presentFamily.value()};
				for (VulkanQueue* queue : queues) {
					queue->init(physicalDevice);
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

	VulkanPhysicalDeviceCriteria* createPhysicalCriteria() const { 
		PhysicalDeviceCriteriaComposite* composite = new PhysicalDeviceCriteriaComposite();
		composite->add(new DeviceSamplerAnisotropySupport());
		composite->add(new DeviceExtensionSupport(deviceExtensions));
		return composite; 
	}

	virtual const VkDevice& getDevice() const { return device; }
	virtual const VkPhysicalDevice& getPhysicalDevice() const { return physicalDevice; }

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

void VulkanQueue::update() {
	VulkanDevice* device = getEntity().getComponent<VulkanDevice>();
	if (device) {
		vkGetDeviceQueue(device->getDevice(), index, 0, &queue);
	}
}

}

#endif