#ifndef SANDBOX_GRAPHICS_VULKAN_DEVICE_STATE_H_
#define SANDBOX_GRAPHICS_VULKAN_DEVICE_STATE_H_

#include "sandbox/Component.h"
#include "sandbox/graphics/GraphicsContext.h"
#include "sandbox/graphics/vulkan/Vulkan.h"
#include "sandbox/graphics/vulkan/render/VulkanRenderPass.h"
#include "sandbox/graphics/vulkan/render/VulkanCommandPool.h"
#include "sandbox/graphics/vulkan/render/VulkanCommandBuffer.h"
#include "sandbox/graphics/vulkan/render/VulkanGraphicsPipeline.h"

namespace sandbox {

class VulkanDeviceState : public StateContainerItem {
public:
	VulkanDeviceState() {
		device = NULL;
		renderMode.set(VULKAN_RENDER_NONE);
		renderPass.set(NULL);
		//imageView = NULL;
		extent.width = 0;
		extent.height = 0;
		commandPool.set(NULL);
		imageFormat.set(VK_FORMAT_UNDEFINED);
		commandBuffer.set(NULL);
		graphicsPipeline.set(NULL);
		numInstances.set(1);
	}

	virtual ~VulkanDeviceState() {}

	const VulkanDevice* getDevice() const { return device; }
	void setDevice(VulkanDevice* device) { this->device = device; }
	StateContainerItemStack<VulkanRenderMode>& getRenderMode() { return renderMode; }
	//VulkanRenderPass* getRenderPass() const { return renderPass; }
	//void setRenderPass(VulkanRenderPass* renderPass) { this->renderPass = renderPass; }
	//VulkanImageView* getImageView() const { return imageView; }
	//void setImageView(VulkanImageView* renderPass) { this->imageView = imageView; }
	const VkExtent2D& getExtent() const { return extent; }
	void setExtent(VkExtent2D extent) { this->extent = extent; }
	StateContainerItemStack<VulkanCommandPool*>& getCommandPool() { return commandPool; }
	StateContainerItemStack<VkFormat>& getImageFormat() { return imageFormat; }
	StateContainerItemStack<VulkanRenderPass*>& getRenderPass() { return renderPass; }
	StateContainerItemStack<VulkanCommandBuffer*>& getCommandBuffer() { return commandBuffer; }
	StateContainerItemStack<VulkanGraphicsPipeline*>& getGraphicsPipeline() { return graphicsPipeline; }
	StateContainerValueItemStack<int>& getNumInstances() { return numInstances; }

	static VulkanDeviceState& get(const GraphicsContext& context) { return context.getRenderState()->getItem<VulkanDeviceState>(); }

private:
	VulkanDevice* device;
	StateContainerItemStack<VulkanRenderPass*> renderPass;
	//VulkanImageView* imageView;
	VkExtent2D extent;
	StateContainerItemStack<VulkanCommandPool*> commandPool;
	StateContainerItemStack<VulkanRenderMode> renderMode;
	StateContainerItemStack<VkFormat> imageFormat;
	StateContainerItemStack<VulkanCommandBuffer*> commandBuffer;
	StateContainerItemStack<VulkanGraphicsPipeline*> graphicsPipeline;
	StateContainerValueItemStack<int> numInstances;

};

}

#endif