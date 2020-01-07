#ifndef SANDBOX_GRAPHICS_VULKAN_IMAGE_VULKAN_EXTERNAL_IMAGE_H_
#define SANDBOX_GRAPHICS_VULKAN_IMAGE_VULKAN_EXTERNAL_IMAGE_H_

#include "sandbox/Component.h"
#include "sandbox/graphics/vulkan/VulkanDeviceRenderer.h"
#include "sandbox/graphics/vulkan/buffer/VulkanBuffer.h"
#include "sandbox/graphics/vulkan/image/VulkanImage.h"

#include <sandbox/image/Image.h>

namespace sandbox {


class VulkanExternalImage : public VulkanImage {
public:
	VulkanExternalImage(bool external = true) : image(NULL), external(external) { addType<VulkanExternalImage>(); }
	virtual ~VulkanExternalImage() {}

	void update() {
		if (!image) {
			image = getEntity().getComponent<Image>();
		}
	}

	VkImage getImage(const GraphicsContext& context) const { return contextHandler.getSharedState(context)->image; }
    int getExternalHandle(const GraphicsContext& context) const { return contextHandler.getSharedState(context)->externalHandle; }

protected:
	void startRender(const GraphicsContext& context, VulkanDeviceState& state) {
		if (!image) { return; }

		if (state.getRenderMode().get() == VULKAN_RENDER_UPDATE_SHARED) {
			ImageState* imageState = contextHandler.getSharedState(context);
			int texWidth, texHeight, texChannels;
	        texHeight = image->getHeight();
	        texWidth = image->getWidth();
	        texChannels = image->getComponents();
	        std::cout << "blah " << texChannels << std::endl;
	        const unsigned char* pixels = image->getData();
	        VkDeviceSize imageSize = texWidth * texHeight * texChannels; 

	        if (!pixels) {
	            throw std::runtime_error("failed to load texture image!");
	        }

	        VulkanBuffer* stagingBuffer = new VulkanBuffer(state.getDevice(), imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	        //VkBuffer stagingBuffer;
	        //VkDeviceMemory stagingBufferMemory;
	        //createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	        stagingBuffer->update(pixels, imageSize);
	        /*void* data;
	        vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
	            memcpy(data, pixels, static_cast<size_t>(imageSize));
	        vkUnmapMemory(device, stagingBufferMemory);*/

	        //stbi_image_free(pixels);

	        //createImage(state.getDevice(), texWidth, texHeight, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, imageState->image, imageState->imageMemory, external);

            if (external) {
                createImage(state.getDevice(), texWidth, texHeight, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, imageState->image, imageState->imageMemory, external);

                VkMemoryGetFdInfoKHR memoryGet;
                memoryGet.sType = VK_STRUCTURE_TYPE_MEMORY_GET_FD_INFO_KHR;
                memoryGet.pNext = NULL;
                memoryGet.memory = imageState->imageMemory;
                memoryGet.handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT;
#ifdef WIN32
#else
                if (state.getDevice()->getInstance().GetMemoryFdKHR(state.getDevice()->getDevice(), &memoryGet, &imageState->externalHandle) != VK_SUCCESS) {
                    throw std::runtime_error("failed to vkGetMemoryFdKHR!");
                }
                std::cout << "handle memory " << imageState->externalHandle << std::endl;
                if (state.getDevice()->getInstance().GetMemoryFdKHR(state.getDevice()->getDevice(), &memoryGet, &imageState->externalHandle) != VK_SUCCESS) {
                    throw std::runtime_error("failed to vkGetMemoryFdKHR!");
                }
                std::cout << "handle memory " << imageState->externalHandle << std::endl;
#endif

                transitionImageLayout(imageState->image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, state.getCommandPool().get(), context);
                copyBufferToImage(stagingBuffer->getBuffer(), imageState->image, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), state.getCommandPool().get(), context);
                transitionImageLayout(imageState->image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, state.getCommandPool().get(), context);

            }
            else {
                createImage(state.getDevice(), texWidth, texHeight, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, imageState->image, imageState->imageMemory, external);

                transitionImageLayout(imageState->image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, state.getCommandPool().get(), context);
                copyBufferToImage(stagingBuffer->getBuffer(), imageState->image, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), state.getCommandPool().get(), context);
                transitionImageLayout(imageState->image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, state.getCommandPool().get(), context);
            }

	        //vkDestroyBuffer(state.getDevice()->getDevice(), stagingBuffer, nullptr);
	        //vkFreeMemory(state.getDevice()->getDevice(), stagingBufferMemory, nullptr);
	        delete stagingBuffer;


		}

        //ImageState* imageState = contextHandler.getSharedState(context);
        //glDrawVkImageNV((GLuint64)imageState->image, 0, 0,0, 100,100,0,0,0,100,100);
	}

	void finishRender(const GraphicsContext& context, VulkanDeviceState& state) {
		if (!image) { return; }

		if (state.getRenderMode().get() == VULKAN_RENDER_CLEANUP_SHARED) {
			ImageState* imageState = contextHandler.getSharedState(context);
			vkDestroyImage(state.getDevice()->getDevice(), imageState->image, nullptr);
			vkFreeMemory(state.getDevice()->getDevice(), imageState->imageMemory, nullptr);
		}
	}

    static void createImage(const VulkanDevice* device, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory, bool external = true) {
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


private:
	struct ImageState : public ContextState {
		VkImage image;
    	VkDeviceMemory imageMemory;
        int externalHandle;
	};
    
    /*external
            // Setup the command buffers used to transition the image between GL and VK
            transitionCmdBuf = context.createCommandBuffer();
            transitionCmdBuf.begin(vk::CommandBufferBeginInfo{});
            context.setImageLayout(transitionCmdBuf, texture.image, vk::ImageAspectFlagBits::eColor, vk::ImageLayout::eUndefined,
                                   vk::ImageLayout::eColorAttachmentOptimal);
            transitionCmdBuf.end();
            */

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

    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, VulkanCommandPool* commandPool, const GraphicsContext& context) {
        VkCommandBuffer commandBuffer = commandPool->beginSingleTimeCommands(context);

        VkBufferImageCopy region = {};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = {0, 0, 0};
        region.imageExtent = {
            width,
            height,
            1
        };

        vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        commandPool->endSingleTimeCommands(context, commandBuffer);
    }

	GraphicsContextHandler<ImageState,ContextState> contextHandler;
	sandbox::Image* image;
    bool external;

};

}

#endif