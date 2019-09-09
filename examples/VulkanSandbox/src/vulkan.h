#ifndef SANDBOX_VULCAN_H_
#define SANDBOX_VULCAN_H_

#include "sandbox/Component.h"
#include "sandbox/graphics/GraphicsContext.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <set>
#include <fstream>
#include <chrono>

namespace sandbox {

class RenderObject : public Component {
public:
	RenderObject() { addType<RenderObject>(); }
	virtual ~RenderObject() {}

	virtual void startRender(const GraphicsContext& context) {}
	virtual void finishRender(const GraphicsContext& context) {}
};

class RenderNode : public RenderObject {
public:
	RenderNode(Entity* proxyEntity) : proxyEntity(proxyEntity) { addType<RenderNode>(); }
	virtual ~RenderNode() {}

	void render(const GraphicsContext& context) {
		startRender(context);
		finishRender(context);
	}

	void startRender(const GraphicsContext& context) {
		render(proxyEntity, context);
	}

	void finishRender(const GraphicsContext& context) {}

private:
	void render(Entity* entity, const GraphicsContext& context) {
		std::vector<RenderObject*> components = entity->getComponents<RenderObject>();
		for (int f = 0; f < components.size(); f++) {
			components[f]->startRender(context);
		}

		const std::vector<Entity*>& children = entity->getChildren();
		for (int f = 0; f < entity->getChildren().size(); f++) {
			render(children[f], context);
		}

		for (int f = components.size()-1; f >= 0; f--) {
			components[f]->finishRender(context);
		}
	}

	Entity* proxyEntity;
};

class GraphicsRenderer : public Component {
public:
	GraphicsRenderer(GraphicsContext* context = NULL) : context(context), node(NULL) {
		addType<GraphicsRenderer>();
		if (!this->context) {
			std::cout << context << std::endl;
			this->context = new GraphicsContext();
		}
	}
	virtual ~GraphicsRenderer() {
		if (node) {
			delete node;
		}
		delete context;
	}

	void update() {
		if (!node) {
			node = new RenderNode(&getEntity());
		}
	}

	void render() {
		node->render(*context);
	}

	GraphicsContext& getContext() { return *context; }

private:
	GraphicsContext* context;
	RenderNode* node;
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class VulkanDeviceState;

class VulkanRenderObject : public RenderObject {
public:
	VulkanRenderObject() { addType<VulkanRenderObject>(); }
	virtual ~VulkanRenderObject() {}

	void startRender(const GraphicsContext& context);
	void finishRender(const GraphicsContext& context);

protected:
	virtual void startRender(const GraphicsContext& context, VulkanDeviceState& state) {}
	virtual void finishRender(const GraphicsContext& context, VulkanDeviceState& state) {}
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


class VulkanDevice* device;
class VulkanDeviceState* state;


enum VulkanRenderMode {
	VULKAN_RENDER_NONE = 0,
	VULKAN_RENDER_CLEANUP_SHARED = 1,
	VULKAN_RENDER_CLEANUP = 2,
	VULKAN_RENDER_UPDATE_SHARED = 4,
	VULKAN_RENDER_UPDATE = 8,
	VULKAN_RENDER_OBJECT = 16,
	VULKAN_RENDER_COMMAND = 32
};


class VulkanDeviceRenderer : public GraphicsRenderer {
public:
	VulkanDeviceRenderer() : device(NULL), state(NULL) { addType<VulkanDeviceRenderer>(); }
	VulkanDeviceRenderer(GraphicsContext* context) : GraphicsRenderer(context), device(NULL), state(NULL) { addType<VulkanDeviceRenderer>(); }
	virtual ~VulkanDeviceRenderer() {}

	void update();
	void render(VulkanRenderMode renderMode);

private:
	VulkanDevice* device;
	VulkanDeviceState* state;
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


class VulkanSwapChainState : public StateContainerItem {
public:
	VulkanSwapChainState() {
		imageIndex = 0;
	}

	virtual ~VulkanSwapChainState() {}

	int getImageIndex() const { return imageIndex; }
	int setImageIndex(int imageIndex) { this->imageIndex = imageIndex; }

	static VulkanSwapChainState& get(const GraphicsContext& context) { return context.getRenderState()->getItem<VulkanSwapChainState>(); }

private:
	int imageIndex;
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

        for (int f = 0; f < imageCount; f++) {
        	GraphicsContext* context = new GraphicsContext(&sharedContext, new Context(), false);
        	VulkanSwapChainState::get(*context).setImageIndex(f);
        	VulkanDeviceRenderer* renderer = new VulkanDeviceRenderer(context);
	        getEntity().addComponent(renderer);
	        renderer->update();
	        if (f == 0) {
	        	renderer->render(VULKAN_RENDER_UPDATE_SHARED);
	        }
	        renderer->render(VULKAN_RENDER_UPDATE);
        }
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
    Context sharedContext;
};

class VulkanRenderPass : public VulkanRenderObject {
public:
	VulkanRenderPass() { addType<VulkanRenderPass>(); }
	virtual ~VulkanRenderPass() {}

	virtual VkRenderPass getRenderPass(const GraphicsContext& context) const = 0;
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


class VulkanDeviceState : public StateContainerItem {
public:
	VulkanDeviceState() {
		device = NULL;
		renderMode = VULKAN_RENDER_NONE;
		renderPass = NULL;
		//imageView = NULL;
		extent.width = 0;
		extent.height = 0;
	}

	virtual ~VulkanDeviceState() {}

	const VulkanDevice* getDevice() const { return device; }
	void setDevice(VulkanDevice* device) { this->device = device; }
	VulkanRenderMode getRenderMode() const { return renderMode; }
	void setRenderMode(VulkanRenderMode renderMode) { this->renderMode = renderMode; }
	VulkanRenderPass* getRenderPass() const { return renderPass; }
	void setRenderPass(VulkanRenderPass* renderPass) { this->renderPass = renderPass; }
	//VulkanImageView* getImageView() const { return imageView; }
	//void setImageView(VulkanImageView* renderPass) { this->imageView = imageView; }
	const VkExtent2D& getExtent() const { return extent; }
	void setExtent(VkExtent2D extent) { this->extent = extent; }

	static VulkanDeviceState& get(const GraphicsContext& context) { return context.getRenderState()->getItem<VulkanDeviceState>(); }

private:
	VulkanDevice* device;
	VulkanRenderMode renderMode;
	VulkanRenderPass* renderPass;
	//VulkanImageView* imageView;
	VkExtent2D extent;
};

class VulkanFramebuffer : public VulkanRenderObject {
public:
	VulkanFramebuffer() { addType<VulkanFramebuffer>(); }
	virtual ~VulkanFramebuffer() {}

	void finishRender(const GraphicsContext& context, VulkanDeviceState& state) {
		if (state.getRenderMode() == VULKAN_RENDER_CLEANUP) {
			vkDestroyFramebuffer(state.getDevice()->getDevice(), getFramebuffer(context), nullptr);
		}
	}

	virtual VkFramebuffer getFramebuffer(const GraphicsContext& context) const = 0;
};

class VulkanSwapChainFramebuffer : public VulkanFramebuffer {
public:
	VulkanSwapChainFramebuffer() { addType<VulkanSwapChainFramebuffer>(); }
	virtual ~VulkanSwapChainFramebuffer() {}

	void startRender(const GraphicsContext& context, VulkanDeviceState& state) {
		if (state.getRenderMode() == VULKAN_RENDER_UPDATE) {
			VulkanSwapChain* swapChain = getEntity().getComponent<VulkanSwapChain>();

	        VkImageView attachments[] = {
	            //swapChain->getImageViews()[0]
	            //state.getImageView()->getImageView(context)
	            swapChain->getImageViews()[VulkanSwapChainState::get(context).getImageIndex()]
	        };

	        VkFramebufferCreateInfo framebufferInfo = {};
	        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	        framebufferInfo.renderPass = state.getRenderPass()->getRenderPass(context);
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
	if (!state) {
		state = &VulkanDeviceState::get(getContext());
	}

	if (!device) {
		device = getEntity().getComponentRecursive<VulkanDevice>(false);
		if (device) {
			state->setDevice(device);
			state->setRenderPass(getEntity().getComponent<VulkanRenderPass>());
			state->setExtent(getEntity().getComponent<VulkanSwapChain>()->getExtent());
		}
	}
	
	GraphicsRenderer::update();
}

inline void VulkanDeviceRenderer::render(VulkanRenderMode renderMode) {
	state->setRenderMode(renderMode);
	GraphicsRenderer::render();
}


inline void VulkanRenderObject::startRender(const GraphicsContext& context) {
	VulkanDeviceState& state = VulkanDeviceState::get(context);
	/*switch (state.getRenderMode()) {
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
		if (state.getRenderMode() == VULKAN_RENDER_UPDATE_SHARED) {
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
		if (state.getRenderMode() == VULKAN_RENDER_CLEANUP_SHARED) {
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

class VulkanBasicRenderPass : public VulkanRenderPass {
public:
	VulkanBasicRenderPass() { addType<VulkanBasicRenderPass>(); }
	virtual ~VulkanBasicRenderPass() {}

	void startRender(const GraphicsContext& context, VulkanDeviceState& state) {
		if (state.getRenderMode() == VULKAN_RENDER_UPDATE_SHARED) {
			RenderPassState* sharedState = contextHandler.getSharedState(context);
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

	        if (vkCreateRenderPass(state.getDevice()->getDevice(), &renderPassInfo, nullptr, &sharedState->renderPass) != VK_SUCCESS) {
	            throw std::runtime_error("failed to create render pass!");
	        }
		}
	}

	void finishRender(const GraphicsContext& context, VulkanDeviceState& state) {
		if (state.getRenderMode() == VULKAN_RENDER_CLEANUP_SHARED) {
			vkDestroyRenderPass(state.getDevice()->getDevice(), contextHandler.getSharedState(context)->renderPass, nullptr);
		}
	}

	VkRenderPass getRenderPass(const GraphicsContext& context) const {
		return contextHandler.getSharedState(context)->renderPass;
	}

private:
	struct RenderPassState : public ContextState {
		VkRenderPass renderPass;
	};

	
	GraphicsContextHandler<RenderPassState,ContextState> contextHandler;
};

class VulkanGraphicsPipeline : public VulkanRenderObject {
public:
	VulkanGraphicsPipeline() { addType<VulkanGraphicsPipeline>(); }
	virtual ~VulkanGraphicsPipeline() {}

	VkPipeline getGraphicsPipeline(const GraphicsContext& context) const { return contextHandler.getSharedState(context)->graphicsPipeline; }
	VkPipelineLayout getPipelineLayout(const GraphicsContext& context) const { return contextHandler.getSharedState(context)->pipelineLayout; }

protected:
	void startRender(const GraphicsContext& context, VulkanDeviceState& state) {
		if (state.getRenderMode() == VULKAN_RENDER_UPDATE_SHARED) {
			GraphicsPipelineState* pipelineState = contextHandler.getSharedState(context);
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
	        //pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;

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
	        pipelineInfo.renderPass = state.getRenderPass()->getRenderPass(context);
	        pipelineInfo.subpass = 0;
	        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	        if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipelineState->graphicsPipeline) != VK_SUCCESS) {
	            throw std::runtime_error("failed to create graphics pipeline!");
	        }			
		}
	}
	void finishRender(const GraphicsContext& context, VulkanDeviceState& state) {
		if (state.getRenderMode() == VULKAN_RENDER_CLEANUP_SHARED) {
			GraphicsPipelineState* pipelineState = contextHandler.getSharedState(context);
			vkDestroyPipeline(state.getDevice()->getDevice(), pipelineState->graphicsPipeline, nullptr);
	        vkDestroyPipelineLayout(state.getDevice()->getDevice(), pipelineState->pipelineLayout, nullptr);
		}
	}

private:
	struct GraphicsPipelineState : public ContextState {
		VkPipeline graphicsPipeline;
		VkPipelineLayout pipelineLayout;
	};

	GraphicsContextHandler<GraphicsPipelineState,ContextState> contextHandler;
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

	void update(void* newData, size_t size) {
        void* data;
        vkMapMemory(device->getDevice(), bufferMemory, 0, size, 0, &data);
            memcpy(data, newData, size);
        vkUnmapMemory(device->getDevice(), bufferMemory);
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

class VulkanUniformBuffer : public VulkanRenderObject {
public:
	VulkanUniformBuffer() { addType<VulkanUniformBuffer>(); }
	virtual ~VulkanUniformBuffer() {}

	void startRender(const GraphicsContext& context, VulkanDeviceState& state) {
		UniformBufferState* uboState = contextHandler.getState(context);
		if (state.getRenderMode() == VULKAN_RENDER_UPDATE) {
			int bufferSize = getBufferSize();
			uboState->buffer = new VulkanBuffer(state.getDevice(), bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		}
		else if (state.getRenderMode() == VULKAN_RENDER_OBJECT) {
			updateBuffer(context, state, uboState->buffer);
		}
	}
	void finishRender(const GraphicsContext& context, VulkanDeviceState& state) {
		if (state.getRenderMode() == VULKAN_RENDER_CLEANUP) {
			delete contextHandler.getState(context)->buffer;
		}
	}

	VkBuffer getBuffer(const GraphicsContext& context) const { return contextHandler.getState(context)->buffer->getBuffer(); }

protected:
	virtual size_t getBufferSize() const = 0;
	virtual void updateBuffer(const GraphicsContext& context, VulkanDeviceState& state, VulkanBuffer* buffer) = 0;
	/*virtual size_t getBufferSize() const { return sizeof(UniformBufferObject); }

	virtual void updateBuffer(const GraphicsContext& context, VulkanDeviceState& state, VulkanBuffer* buffer) {
		static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        UniformBufferObject ubo = {};
        ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.proj = glm::perspective(glm::radians(45.0f), (float) state.getExtent().width / (float) state.getExtent().height, 0.1f, 10.0f);
        ubo.proj[1][1] *= -1;

        buffer->update(&ubo, sizeof(ubo));
	}*/

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
        value.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        value.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        value.proj = glm::perspective(glm::radians(45.0f), (float) state.getExtent().width / (float) state.getExtent().height, 0.1f, 10.0f);
        value.proj[1][1] *= -1;
		VulkanUniformBufferValue<UniformBufferObject>::updateBuffer(context, state, buffer);
	}
};

}

#endif