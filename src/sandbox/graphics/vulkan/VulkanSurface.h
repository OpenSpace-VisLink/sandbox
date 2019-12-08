#ifndef SANDBOX_GRAPHICS_VULKAN_SURFACE_H_
#define SANDBOX_GRAPHICS_VULKAN_SURFACE_H_

#include "sandbox/Component.h"
#include "sandbox/graphics/vulkan/Vulkan.h"

namespace sandbox {

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
		std::cout << width << " -- " << height <<std::endl; 
	}

private:
	bool initialized;
	Entity* instanceEntity;
	GLFWwindow* window;
	VkSurfaceKHR surface;
	VkInstance instance;
};

}

#endif