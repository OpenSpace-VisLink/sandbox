#ifndef SANDBOX_GRAPHICS_VULKAN_SWAP_CHAIN_H_
#define SANDBOX_GRAPHICS_VULKAN_SWAP_CHAIN_H_

#include "sandbox/Component.h"
#include "sandbox/graphics/vulkan/VulkanDevice.h"
#include "sandbox/graphics/vulkan/VulkanDeviceRenderer.h"

namespace sandbox {

class VulkanSwapChain : public VulkanDeviceComponent {
public:
	VulkanSwapChain() { addType<VulkanSwapChain>(); }
	virtual ~VulkanSwapChain() {}

	virtual VkFormat getImageFormat() const = 0;
    virtual const std::vector<VkImage>&  getImages() const = 0;
    virtual const std::vector<VkImageView>& getImageViews() const = 0;
    virtual VkImageView getDepthImageView() const = 0;
	virtual VkExtent2D getExtent() const = 0;
    virtual VkSwapchainKHR getSwapChain() const = 0;
    virtual const std::string& getName() const = 0;
};


class VulkanSwapChainState : public StateContainerItem {
public:
	VulkanSwapChainState() {
		imageIndex = 0;
		numImages = 0;
		swapChain = NULL;
	}

	virtual ~VulkanSwapChainState() {}

	int getImageIndex() const { return imageIndex; }
	void setImageIndex(int imageIndex) { this->imageIndex = imageIndex; }
	int getNumImages() const { return numImages; }
	void setNumImages(int numImages) { this->numImages = numImages; }
	VulkanSwapChain* getSwapChain() const { return swapChain; }
	void setSwapChain(VulkanSwapChain* swapChain) { this->swapChain = swapChain; }

	static VulkanSwapChainState& get(const GraphicsContext& context) { return context.getRenderState()->getItem<VulkanSwapChainState>(); }


private:
	int imageIndex;
	int numImages;
	VulkanSwapChain* swapChain;
};

class VulkanBasicSwapChain : public VulkanSwapChain {
public:
	VulkanBasicSwapChain(const std::string& name, Entity* surfaceEntity) : name(name), surfaceEntity(surfaceEntity), initialized(false) { addType<VulkanBasicSwapChain>(); }
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
		std::vector<VulkanDeviceRenderer*> renderers = getEntity().getComponentsRecursive<VulkanDeviceRenderer>();
		for (int f = 0; f < renderers.size(); f++) {
			renderers[f]->render(VULKAN_RENDER_CLEANUP);	
		}
		renderers[0]->render(VULKAN_RENDER_CLEANUP_DISPLAY);
		

        vkDestroyImageView(getDevice().getDevice(), depthImageView, nullptr);
        vkDestroyImage(getDevice().getDevice(), depthImage, nullptr);
        vkFreeMemory(getDevice().getDevice(), depthImageMemory, nullptr);

		for (auto imageView : swapChainImageViews) {
            vkDestroyImageView(getDevice().getDevice(), imageView, nullptr);
			std::cout << "Destroy image view." << std::endl;
        }

        vkDestroySwapchainKHR(getDevice().getDevice(), swapChain, nullptr);
			std::cout << "Destroy swap chain." << std::endl;
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
		if (initialized) {
			cleanup();
			//return;
		}

		surface = surfaceEntity->getComponent<VulkanSurface>();

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
        createDepthResources();


        if (!initialized) {
        	Context* shared = &swapChainContext;
        	SharedContext* sharedContextComponent = getEntity().getComponentRecursive<SharedContext>(false);
        	if (sharedContextComponent) {
        		std::cout << "Use shared" << std::endl;
        		shared = &sharedContextComponent->getContext();
        	}
        	for (int f = 0; f < imageCount; f++) {
	        	GraphicsContext* context = new GraphicsContext(shared, &swapChainContext, new Context(), false, false);
		        VulkanSwapChainState::get(*context).setSwapChain(this);
		        VulkanSwapChainState::get(*context).setNumImages(imageCount);
	        	VulkanSwapChainState::get(*context).setImageIndex(f);
	        	VulkanDeviceRenderer* renderer = new VulkanDeviceRenderer(context);
		        getEntity().addComponent(renderer);
		        renderer->update();
		        /*if (f == 0) {
		        	renderer->render(VULKAN_RENDER_UPDATE_SHARED);
		        }
		        renderer->render(VULKAN_RENDER_UPDATE);*/
	        }
        }

        initialized = true;
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
        if (false) {//capabilities.currentExtent.width != UINT32_MAX) {
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


    void createDepthResources() {
        VkFormat depthFormat = getDevice().findDepthFormat();

        createImage(&getDevice(), swapChainExtent.width, swapChainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory);
        depthImageView = createImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
    }

    bool hasStencilComponent(VkFormat format) {
        return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
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
            swapChainImageViews[i] = createImageView(swapChainImages[i], swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
        }
    }


    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) {
        VkImageViewCreateInfo viewInfo = {};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = aspectFlags;
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

    void createImage(const VulkanDevice* device, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) {
        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateImage(device->getDevice(), &imageInfo, nullptr, &image) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image!");
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(device->getDevice(), image, &memRequirements);

        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = device->findMemoryType(memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(device->getDevice(), &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate image memory!");
        }

        vkBindImageMemory(device->getDevice(), image, imageMemory, 0);
    }


    VkFormat getImageFormat() const { return swapChainImageFormat; }
    VkSwapchainKHR getSwapChain() const { return swapChain; }
    const std::vector<VkImage>&  getImages() const { return swapChainImages; }
    const std::vector<VkImageView>& getImageViews() const { return swapChainImageViews; }
    VkImageView getDepthImageView() const { return depthImageView; }
    VkExtent2D getExtent() const { return swapChainExtent; }
    //Context* getSharedContext() { return &sharedContext; };
    const std::string& getName() const { return name; }

//private:
    bool initialized;
	Entity* surfaceEntity;
	VulkanSurface* surface;
	VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;
    Context swapChainContext;
    std::string name;

    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;
};

}

#endif