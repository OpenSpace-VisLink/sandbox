#ifndef SANDBOX_GRAPHICS_VULKAN_DEVICE_RENDERER_H_
#define SANDBOX_GRAPHICS_VULKAN_DEVICE_RENDERER_H_

#include "sandbox/Component.h"
#include "sandbox/graphics/vulkan/VulkanDevice.h"

namespace sandbox {

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

enum VulkanRenderMode {
	VULKAN_RENDER_NONE = 0,
	VULKAN_RENDER_CLEANUP_SHARED = 1,
	VULKAN_RENDER_CLEANUP = 2,
	VULKAN_RENDER_UPDATE_SHARED = 4,
	VULKAN_RENDER_UPDATE = 8,
	VULKAN_RENDER_OBJECT = 16,
	VULKAN_RENDER_COMMAND = 32,
	VULKAN_RENDER_CLEANUP_DISPLAY = 64,
	VULKAN_RENDER_UPDATE_DISPLAY = 128,
	VULKAN_RENDER_CLEANUP_ALL = VULKAN_RENDER_CLEANUP_SHARED | VULKAN_RENDER_CLEANUP | VULKAN_RENDER_CLEANUP_DISPLAY,
	VULKAN_RENDER_UPDATE_ALL = VULKAN_RENDER_UPDATE_SHARED | VULKAN_RENDER_UPDATE | VULKAN_RENDER_UPDATE_DISPLAY,
	VULKAN_RENDER_RENDER_ALL = VULKAN_RENDER_COMMAND | VULKAN_RENDER_OBJECT,
	VULKAN_RENDER_SHARED_ONLY = VULKAN_RENDER_UPDATE_SHARED | VULKAN_RENDER_CLEANUP_SHARED,
	VULKAN_RENDER_ALL = VULKAN_RENDER_CLEANUP_SHARED | VULKAN_RENDER_CLEANUP | VULKAN_RENDER_UPDATE_SHARED | VULKAN_RENDER_UPDATE | VULKAN_RENDER_OBJECT | VULKAN_RENDER_COMMAND
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


}

#endif