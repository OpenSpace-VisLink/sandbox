#ifndef SANDBOX_GRAPHICS_VULKAN_SWAP_CHAIN_H_
#define SANDBOX_GRAPHICS_VULKAN_SWAP_CHAIN_H_

#include "glm/glm.hpp"
#include "sandbox/Component.h"
#include "sandbox/graphics/vulkan/VulkanDevice.h"
#include "sandbox/graphics/vulkan/VulkanDeviceRenderer.h"
#include "sandbox/graphics/vulkan/render/VulkanCommandPool.h"

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
    virtual const std::string& getName() const = 0;
    virtual VkResult acquireNextImage(VkSemaphore semaphore, uint32_t* index) = 0;
    virtual VkResult queuePresent(VkQueue queue, VkPresentInfoKHR& presentInfo) = 0;
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

class VulkanBasicSwapChainBase : public VulkanSwapChain {
public:
	VulkanBasicSwapChainBase(const std::string& name) : name(name), initialized(false) { addType<VulkanBasicSwapChainBase>(); }
	virtual ~VulkanBasicSwapChainBase() {
		cleanup();
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

        //vkDestroySwapchainKHR(getDevice().getDevice(), swapChain, nullptr);
            //std::cout << "Destroy swap chain." << std::endl;

        destroyImages();
    }

    virtual void createImages() = 0;
    virtual void destroyImages() = 0;

	void update() {
		if (initialized) {
			cleanup();
			//return;
		}

		createImages();
        createImageViews();
        createDepthResources();

        int imageCount = swapChainImages.size();
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
	        }
        }

        initialized = true;
	}


    void createDepthResources() {
        VkFormat depthFormat = getDevice().findDepthFormat();

        createImage(&getDevice(), getExtent().width, getExtent().height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory);
        depthImageView = createImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
    }

    bool hasStencilComponent(VkFormat format) {
        return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
    }

    void createImageViews() {
        swapChainImageViews.resize(swapChainImages.size());

        for (size_t i = 0; i < swapChainImages.size(); i++) {
            swapChainImageViews[i] = createImageView(swapChainImages[i], getImageFormat(), VK_IMAGE_ASPECT_COLOR_BIT);
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

    static void createImage(const VulkanDevice* device, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory, bool external = false) {
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

        VkExportMemoryAllocateInfo exportMemAlloc;
        exportMemAlloc.sType = VK_STRUCTURE_TYPE_EXPORT_MEMORY_ALLOCATE_INFO;
        exportMemAlloc.pNext = NULL;
        exportMemAlloc.handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT;

        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        if (external) {
            allocInfo.pNext = &exportMemAlloc;
        }
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = device->findMemoryType(memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(device->getDevice(), &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate image memory!");
        }

        vkBindImageMemory(device->getDevice(), image, imageMemory, 0);

    }


    const std::vector<VkImage>&  getImages() const { return swapChainImages; }
    const std::vector<VkImageView>& getImageViews() const { return swapChainImageViews; }
    VkImageView getDepthImageView() const { return depthImageView; }
    const std::string& getName() const { return name; }

protected:
    bool initialized;
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;
    Context swapChainContext;
    std::string name;

    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;
};

class VulkanBasicSwapChain : public VulkanBasicSwapChainBase {
public:
    VulkanBasicSwapChain(const std::string& name, Entity* surfaceEntity) : VulkanBasicSwapChainBase(name), surfaceEntity(surfaceEntity) { addType<VulkanBasicSwapChain>(); }

    void createImages() {
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
    }

    void destroyImages() {
        vkDestroySwapchainKHR(getDevice().getDevice(), swapChain, nullptr);
            std::cout << "Destroy swap chain." << std::endl;
    }

    VkResult acquireNextImage(VkSemaphore semaphore, uint32_t* imageIndex) {
        return vkAcquireNextImageKHR(getDevice().getDevice(), swapChain, UINT64_MAX, semaphore, VK_NULL_HANDLE, imageIndex);
    }

    VkResult queuePresent(VkQueue queue, VkPresentInfoKHR& presentInfo) {
        VkSwapchainKHR swapChains[] = {swapChain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        return vkQueuePresentKHR(queue, &presentInfo);
    }

    VkFormat getImageFormat() const { return swapChainImageFormat; }
    VkExtent2D getExtent() const { return swapChainExtent; }

    VulkanPhysicalDeviceCriteria* createPhysicalCriteria() const { 
        VulkanSurface* surface = surfaceEntity->getComponent<VulkanSurface>();
        if (surface) {
            return new DeviceSwapChainSupport(surface->getSurface()); 
        }

        return NULL;
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

            actualExtent.width = glm::max(capabilities.minImageExtent.width, glm::min(capabilities.maxImageExtent.width, actualExtent.width));
            actualExtent.height = glm::max(capabilities.minImageExtent.height, glm::min(capabilities.maxImageExtent.height, actualExtent.height));

            return actualExtent;
        }
    }

private:
    Entity* surfaceEntity;
    VulkanSurface* surface;
    VkSwapchainKHR swapChain;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
};

class VulkanImageTransition : public VulkanRenderObject {
public:
    VulkanImageTransition() { addType<VulkanImageTransition>(); }

protected:
    virtual void startRender(const GraphicsContext& context, VulkanDeviceState& state);

    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VulkanCommandPool* commandPool, const GraphicsContext& context) {
        VkCommandBuffer commandBuffer = commandPool->beginSingleTimeCommands(context);

        VkImageMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        }  else {
            throw std::invalid_argument("unsupported layout transition!");
        }

        vkCmdPipelineBarrier(
            commandBuffer,
            sourceStage, destinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );

        commandPool->endSingleTimeCommands(context, commandBuffer);
    }
};

class VulkanOffscreenRenderer : public VulkanBasicSwapChainBase {
public:
    VulkanOffscreenRenderer(const std::string& name, VkFormat imageFormat, int width, int height, int imageCount) : VulkanBasicSwapChainBase(name), swapChainImageFormat(imageFormat), imageCount(imageCount), currentImage(0) { 
        addType<VulkanOffscreenRenderer>();
        swapChainExtent.width = width;
        swapChainExtent.height = height;
    }
    
    void createImages() {
        for (int f = 0; f < imageCount; f++) {
            VkImage image;
            VkDeviceMemory memory;
            VulkanBasicSwapChainBase::createImage(&getDevice(), swapChainExtent.width, swapChainExtent.height, swapChainImageFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image, memory, true);
            
            VkMemoryGetFdInfoKHR memoryGet;
            memoryGet.sType = VK_STRUCTURE_TYPE_MEMORY_GET_FD_INFO_KHR;
            memoryGet.pNext = NULL;
            memoryGet.memory = memory;
            memoryGet.handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT;
            int externalHandle;
            if (getDevice().getInstance().GetMemoryFdKHR(getDevice().getDevice(), &memoryGet, &externalHandle) != VK_SUCCESS) {
                throw std::runtime_error("failed to vkGetMemoryFdKHR!");
            }
            std::cout << "handle memory " << externalHandle << std::endl;

            swapChainImages.push_back(image);
            imageMemory.push_back(memory);
            externalHandles.push_back(externalHandle);
        }

    }

    void destroyImages() {
        for (int f = 0; f < swapChainImages.size(); f++) {
            vkDestroyImage(getDevice().getDevice(), swapChainImages[f], nullptr);
            vkFreeMemory(getDevice().getDevice(), imageMemory[f], nullptr);
        }

        swapChainImages.clear();
        imageMemory.clear();
        externalHandles.clear();
    }

    VkResult acquireNextImage(VkSemaphore semaphore, uint32_t* imageIndex) {
        currentImage = (currentImage + 1) % imageCount;
        *imageIndex = currentImage;
        return VK_SUCCESS;
    }

    VkResult queuePresent(VkQueue queue, VkPresentInfoKHR& presentInfo) {
        return VK_SUCCESS;
    }

    VkFormat getImageFormat() const { return swapChainImageFormat; }
    VkExtent2D getExtent() const { return swapChainExtent; }
    int getCurrentImage() { return currentImage; }

public:
    std::vector<VkDeviceMemory> imageMemory;
    std::vector<int> externalHandles;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    int imageCount;
    int currentImage;
};

}

#endif