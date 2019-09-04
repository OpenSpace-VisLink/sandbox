#ifndef SANDBOX_VULCAN_H_
#define SANDBOX_VULCAN_H_

#include "sandbox/Component.h"
#include "sandbox/graphics/GraphicsContext.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <set>
#include <fstream>

namespace sandbox {

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
	virtual void getFramebufferSize(int &width, int &height) const = 0;
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

	virtual void getFramebufferSize(int &width, int &height) const {
		glfwGetFramebufferSize(window, &width, &height);
	}

private:
	bool initialized;
	Entity* instanceEntity;
	GLFWwindow* window;
	VkSurfaceKHR surface;
	VkInstance instance;
};

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

	VulkanPhysicalDeviceCriteria* createPhysicalCriteria() const { 
		PhysicalDeviceCriteriaComposite* composite = new PhysicalDeviceCriteriaComposite();
		composite->add(new DeviceSamplerAnisotropySupport());
		composite->add(new DeviceExtensionSupport(deviceExtensions));
		return composite; 
	}

	virtual const VkDevice& getDevice() const { return device; }
	virtual const VkPhysicalDevice& getPhysicalDevice() const { return physicalDevice; }
	virtual const VulkanInstance& getInstance() const { return *vulkanInstance; }

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


void VulkanQueue::initDeviceComponent() {
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

void VulkanQueue::update() {
	vkGetDeviceQueue(getDevice().getDevice(), index, 0, &queue);
}

class VulkanSwapChain : public VulkanDeviceComponent {
public:
	VulkanSwapChain() { addType<VulkanSwapChain>(); }
	virtual ~VulkanSwapChain() {}

	virtual VkFormat getImageFormat() const = 0;
	virtual const std::vector<VkImageView>& getImageViews() const = 0;
	virtual VkExtent2D getExtent() const = 0;
};

class VulkanBasicSwapChain : public VulkanSwapChain {
public:
	VulkanBasicSwapChain(Entity* surfaceEntity) : surfaceEntity(surfaceEntity) { addType<VulkanBasicSwapChain>(); }
	virtual ~VulkanBasicSwapChain() {
		cleanup();
	}

	void afterAdd() {
		/*getEntity().addComponent(new Transform());
		getEntity().addComponent(new ArcBall(input));
		getEntity().addComponent(new MouseZoom(input));
		getEntity().addComponent(new MouseTranslate(input));*/
	}

	void cleanup() {
        /*for (auto framebuffer : swapChainFramebuffers) {
            vkDestroyFramebuffer(device, framebuffer, nullptr);
        }

        vkFreeCommandBuffers(device, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

        vkDestroyPipeline(device, graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
        
        vkDestroyRenderPass(device, renderPass, nullptr);

        for (auto imageView : swapChainImageViews) {
            vkDestroyImageView(device, imageView, nullptr);
        }

        vkDestroySwapchainKHR(device, swapChain, nullptr);

        for (size_t i = 0; i < swapChainImages.size(); i++) {
            vkDestroyBuffer(device, uniformBuffers[i], nullptr);
            vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
        }

        vkDestroyDescriptorPool(device, descriptorPool, nullptr);
        */
    }

	void update() {
		VulkanSurface* surface = surfaceEntity->getComponent<VulkanSurface>();

        SwapChainSupportDetails swapChainSupport = getDevice().getInstance().querySwapChainSupport(getDevice().getPhysicalDevice(), surface->getSurface());

        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }
        std::cout << "image count " << imageCount << " " << swapChainSupport.capabilities.maxImageCount<< std::endl;

        VkSwapchainCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface->getSurface();

        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        VulkanGraphicsQueue* graphicsQueue = getDevice().getEntity().getComponentRecursive<VulkanGraphicsQueue>();
        VulkanPresentQueue* presentQueue = getDevice().getEntity().getComponentRecursive<VulkanPresentQueue>();

        uint32_t queueFamilyIndices[] = {graphicsQueue->getIndex(), presentQueue->getIndex()};

        if (graphicsQueue->getIndex() != presentQueue->getIndex()) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;

        if (vkCreateSwapchainKHR(getDevice().getDevice(), &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
            throw std::runtime_error("failed to create swap chain!");
        }

        vkGetSwapchainImagesKHR(getDevice().getDevice(), swapChain, &imageCount, nullptr);
        swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(getDevice().getDevice(), swapChain, &imageCount, swapChainImages.data());

        swapChainImageFormat = surfaceFormat.format;
        swapChainExtent = extent;
        createImageViews();
	}

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
        if (capabilities.currentExtent.width != UINT32_MAX) {
            return capabilities.currentExtent;
        } else {
            int width, height;
            surface->getFramebufferSize(width, height);

            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
            actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

            return actualExtent;
        }
    }

    VulkanPhysicalDeviceCriteria* createPhysicalCriteria() const { 
    	VulkanSurface* surface = surfaceEntity->getComponent<VulkanSurface>();
		if (surface) {
			return new DeviceSwapChainSupport(surface->getSurface()); 
		}

		return NULL;
    }

    void createImageViews() {
        swapChainImageViews.resize(swapChainImages.size());

        for (size_t i = 0; i < swapChainImages.size(); i++) {
            swapChainImageViews[i] = createImageView(swapChainImages[i], swapChainImageFormat);
        }
    }


    VkImageView createImageView(VkImage image, VkFormat format) {
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
        if (vkCreateImageView(getDevice().getDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture image view!");
        }

        return imageView;
    }

    VkFormat getImageFormat() const { return swapChainImageFormat; }
    const std::vector<VkImageView>& getImageViews() const { return swapChainImageViews; }
    VkExtent2D getExtent() const { return swapChainExtent; }

//private:
	Entity* surfaceEntity;
	VulkanSurface* surface;
	VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;
};

class VulkanRenderPass : public VulkanDeviceComponent {
public:
	VulkanRenderPass() { addType<VulkanRenderPass>(); }
	virtual ~VulkanRenderPass() {}

	virtual VkRenderPass getRenderPass() const = 0;
};

class VulkanBasicRenderPass : public VulkanRenderPass {
public:
	VulkanBasicRenderPass() { addType<VulkanBasicRenderPass>(); }
	virtual ~VulkanBasicRenderPass() {
		vkDestroyRenderPass(getDevice().getDevice(), renderPass, nullptr);
	}

	void update() {
		VkAttachmentDescription colorAttachment = {};
        colorAttachment.format = getEntity().getComponent<VulkanSwapChain>()->getImageFormat();
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef = {};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        if (vkCreateRenderPass(getDevice().getDevice(), &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
        }
	}

	VkRenderPass getRenderPass() const {
		return renderPass;
	}

private:
	VkRenderPass renderPass;
};

class VulkanSwapChainFramebufferGroup : public VulkanDeviceComponent {
public:
	VulkanSwapChainFramebufferGroup() { addType<VulkanSwapChainFramebufferGroup>(); }
	virtual ~VulkanSwapChainFramebufferGroup() {
		for (auto framebuffer : swapChainFramebuffers) {
            vkDestroyFramebuffer(getDevice().getDevice(), framebuffer, nullptr);
        }
	}

	void update() {
		VulkanSwapChain* swapChain = getEntity().getComponent<VulkanSwapChain>();
        swapChainFramebuffers.resize(swapChain->getImageViews().size());

        for (size_t i = 0; i < swapChain->getImageViews().size(); i++) {
            VkImageView attachments[] = {
                swapChain->getImageViews()[i]
            };

            VkFramebufferCreateInfo framebufferInfo = {};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = getEntity().getComponent<VulkanRenderPass>()->getRenderPass();
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = swapChain->getExtent().width;
            framebufferInfo.height = swapChain->getExtent().height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(getDevice().getDevice(), &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
	}

	std::vector<VkFramebuffer> getFramebuffers() const {
		return swapChainFramebuffers;
	}

private:
	std::vector<VkFramebuffer> swapChainFramebuffers;
};


class VulkanCommandPool : public VulkanDeviceComponent {
public:
	VulkanCommandPool(VulkanQueue* queue) : queue(queue) { addType<VulkanCommandPool>(); }
	virtual ~VulkanCommandPool() {
		vkDestroyCommandPool(getDevice().getDevice(), commandPool, nullptr);
	}

	void update() {
        VkCommandPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queue->getIndex();

        if (vkCreateCommandPool(getDevice().getDevice(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics command pool!");
        }
	}

	VkCommandPool getCommandPool() const {
		return commandPool;
	}

private:
	VkCommandPool commandPool;
	VulkanQueue* queue;
};

class RenderObject : public Component {
public:
	RenderObject() { addType<RenderObject>(); }
	virtual ~RenderObject() {}

	virtual void cleanup(const GraphicsContext& context) {}
	virtual void update(const GraphicsContext& context) {}
	virtual void startRender(const GraphicsContext& context) {}
	virtual void finishRender(const GraphicsContext& context) {}
};

class RenderNode : public RenderObject {
public:
	RenderNode(Entity* proxyEntity) : proxyEntity(proxyEntity) { addType<RenderNode>(); }
	virtual ~RenderNode() {}

	void update(const GraphicsContext& context) {
		update(proxyEntity, context);
	}

	void render(const GraphicsContext& context) {
		startRender(context);
		finishRender(context);
	}

	void startRender(const GraphicsContext& context) {
		render(proxyEntity, context);
	}

	void finishRender(const GraphicsContext& context) {}

private:
	void update(Entity* entity, const GraphicsContext& context) {
		std::vector<RenderObject*> components = entity->getComponents<RenderObject>();

		for (int f = 0; f < components.size(); f++) {
			components[f]->update(context);
		}

		const std::vector<Entity*>& children = entity->getChildren();
		for (int f = 0; f < children.size(); f++) {
			update(children[f], context);
		}
	}

	void render(Entity* entity, const GraphicsContext& context) {
		std::vector<RenderObject*> components = entity->getComponents<RenderObject>();
		for (int f = 0; f < components.size(); f++) {
			components[f]->update(context);
		}

		const std::vector<Entity*>& children = entity->getChildren();
		for (int f = 0; f < entity->getChildren().size(); f++) {
			update(children[f], context);
		}

		for (int f = components.size()-1; f >= 0; f--) {
			components[f]->finishRender(context);
		}
	}

	Entity* proxyEntity;
};

class GraphicsRenderer : public Component {
public:
	GraphicsRenderer() : node(NULL) { addType<GraphicsRenderer>(); }
	virtual ~GraphicsRenderer() {
		if (node) {
			delete node;
		}
	}

	void update() {
		if (!node) {
			node = new RenderNode(&getEntity());
		}

		node->update(context);
	}

	void render() {
		node->render(context);
	}

	GraphicsContext& getContext() { return context; }

private:
	GraphicsContext context;
	RenderNode* node;
};

enum VulkanRenderMode {
	VULKAN_RENDER_NONE,
	VULKAN_RENDER_OBJECT,
	VULKAN_RENDER_COMMAND
};

class VulkanDeviceState : public StateContainerItem {
public:
	VulkanDeviceState() {
		device = NULL;
		renderMode = VULKAN_RENDER_NONE;
	}

	virtual ~VulkanDeviceState() {}

	const VulkanDevice* getDevice() const { return device; }
	void setDevice(VulkanDevice* device) { this->device = device; }
	VulkanRenderMode getRenderMode() const { return renderMode; }
	void setRenderMode(VulkanRenderMode renderMode) { this->renderMode = renderMode; }

	static VulkanDeviceState& get(const GraphicsContext& context) { return context.getRenderState()->getItem<VulkanDeviceState>(); }

private:
	VulkanDevice* device;
	VulkanRenderMode renderMode;
};

class VulkanDeviceRenderer : public GraphicsRenderer {
public:
	VulkanDeviceRenderer() : device(NULL), state(NULL) { addType<VulkanDeviceRenderer>(); }
	virtual ~VulkanDeviceRenderer() {}

	void update() {
		if (!state) {
			state = &VulkanDeviceState::get(getContext());
		}

		if (!device) {
			device = getEntity().getComponentRecursive<VulkanDevice>(false);
			if (device) {
				state->setDevice(device);
			}
		}
		
		GraphicsRenderer::update();
	}

	void render() {
		state->setRenderMode(VULKAN_RENDER_OBJECT);
		GraphicsRenderer::render();
		state->setRenderMode(VULKAN_RENDER_COMMAND);
		GraphicsRenderer::render();
		state->setRenderMode(VULKAN_RENDER_NONE);
	}

private:
	VulkanDevice* device;
	VulkanDeviceState* state;
};

class VulkanRenderObject : public RenderObject {
public:
	VulkanRenderObject() { addType<VulkanRenderObject>(); }
	virtual ~VulkanRenderObject() {}

	void cleanup(const GraphicsContext& context) {
		cleanup(context, VulkanDeviceState::get(context));
	}
	void update(const GraphicsContext& context) {
		update(context, VulkanDeviceState::get(context));
	}
	void startRender(const GraphicsContext& context) {
		VulkanDeviceState& state = VulkanDeviceState::get(context);
		switch (state.getRenderMode()) {
			case VULKAN_RENDER_OBJECT:
				updateObject(context, state);
				break;
			case VULKAN_RENDER_COMMAND:
				startRenderCommand(context, state);
				break;
			default:
				break;
		}

	}
	void finishRender(const GraphicsContext& context) {
		VulkanDeviceState& state = VulkanDeviceState::get(context);
		if (state.getRenderMode() == VULKAN_RENDER_COMMAND) {
			finishRenderCommand(context, state);
		}
	}

protected:
	virtual void cleanup(const GraphicsContext& context, VulkanDeviceState& state) {}
	virtual void update(const GraphicsContext& context, VulkanDeviceState& state) {}
	virtual void startRenderCommand(const GraphicsContext& context, VulkanDeviceState& state) {}
	virtual void finishRenderCommand(const GraphicsContext& context, VulkanDeviceState& state) {}
	virtual void updateObject(const GraphicsContext& context, VulkanDeviceState& state) {}
};

class VulkanShaderModule : public VulkanRenderObject {
public:
	VulkanShaderModule(const std::string& filename, VkShaderStageFlagBits shaderStage) : shaderStage(shaderStage) { 
		addType<VulkanShaderModule>(); 
		code = readFile(filename);

	}
	virtual ~VulkanShaderModule() {}

	VkShaderStageFlagBits getShaderStage() const { return shaderStage; }
	VkShaderModule getShaderModule(const GraphicsContext& context) const { return contextHandler.getState(context)->shaderModule; }

protected:
	void cleanup(const GraphicsContext& context, VulkanDeviceState& state) {
		vkDestroyShaderModule(state.getDevice()->getDevice(), contextHandler.getState(context)->shaderModule, nullptr);
	}

	void update(const GraphicsContext& context, VulkanDeviceState& state) {
		VkShaderModuleCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        if (vkCreateShaderModule(state.getDevice()->getDevice(), &createInfo, nullptr, &contextHandler.getState(context)->shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("failed to create shader module!");
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
	GraphicsContextHandler<ContextState,ShaderModuleState> contextHandler;
};

class VulkanGraphicsPipeline : public VulkanRenderObject {
public:
	VulkanGraphicsPipeline() { addType<VulkanGraphicsPipeline>(); }
	virtual ~VulkanGraphicsPipeline() {}

	void update(const GraphicsContext& context) {
	}

protected:
	void startRenderCommand(const GraphicsContext& context, VulkanDeviceState& state) {
	}
	void finishRenderCommand(const GraphicsContext& context, VulkanDeviceState& state) {
	}
};

}

#endif