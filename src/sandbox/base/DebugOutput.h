#ifndef SANDBOX_BASE_DEBUG_OUTPUT_H_
#define SANDBOX_BASE_DEBUG_OUTPUT_H_

#include <iostream>
#include "glm/glm.hpp"
#include "sandbox/graphics/GraphicsComponent.h"

namespace sandbox {

class DebugOutput : public GraphicsComponent {
public:
	DebugOutput(const std::string& name) : name(name) { addType<DebugOutput>(); }
	virtual ~DebugOutput() {}

	void afterAdd() { printMessage("afterAdd()"); }
	void prepareUpdate() { printMessage("prepareUpdate()"); }
	void update() { printMessage("update()"); }
	void updateSharedContext(const GraphicsContext& graphicsContext) { printMessage("updateSharedContext()"); }
	void updateContext(const GraphicsContext& graphicsContext) { printMessage("updateContext()"); }
	void use(const GraphicsContext& graphicsContext) { printMessage("use()"); }
	void release(const GraphicsContext& graphicsContext) { printMessage("release()"); }
	void startRender(const GraphicsContext& graphicsContext) { printMessage("startRender()"); }
	void finishRender(const GraphicsContext& graphicsContext) { printMessage("finishRender()"); }

	void printMessage(const std::string& message) {
		std::cout << name << ": " << message << std::endl;
	}

private:
	std::string name;
};

}

#endif