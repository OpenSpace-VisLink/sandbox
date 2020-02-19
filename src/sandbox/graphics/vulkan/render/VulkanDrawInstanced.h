#ifndef SANDBOX_GRAPHICS_VULKAN_RENDER_VULKAN_DRAW_INSTANCED_H_
#define SANDBOX_GRAPHICS_VULKAN_RENDER_VULKAN_DRAW_INSTANCED_H_

#include "sandbox/graphics/vulkan/VulkanDeviceRenderer.h"
#include "sandbox/graphics/vulkan/VulkanDeviceState.h"
#include "sandbox/graphics/vulkan/buffer/VulkanBufferTypes.h"

namespace sandbox {

class VulkanDrawInstanced : public VulkanRenderObject {
public:
	VulkanDrawInstanced() : initialized(false), numInstances(1) { addType<VulkanDrawInstanced>(); }
	VulkanDrawInstanced(int numInstances) : initialized(true), numInstances(numInstances) { addType<VulkanDrawInstanced>(); }

	virtual ~VulkanDrawInstanced() {}

	void update() {
		if (!initialized) {
			VulkanDeviceBuffer* buffer = getEntity().getComponent<VulkanDeviceBuffer>();
			if (buffer) {
				numInstances = buffer->getNumValues();
			}
			initialized = true;
		}
	}

	void startRender(const GraphicsContext& context, VulkanDeviceState& state) {
		if ((state.getRenderMode().get() & VULKAN_RENDER_COMMAND) == VULKAN_RENDER_COMMAND) {
			state.getNumInstances().push(numInstances);
		}
	}
	void finishRender(const GraphicsContext& context, VulkanDeviceState& state) {
		if ((state.getRenderMode().get() & VULKAN_RENDER_COMMAND) == VULKAN_RENDER_COMMAND) {
			state.getNumInstances().pop();
		}
	}

private:
	int numInstances;
	bool initialized;
};


}

#endif