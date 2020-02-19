#ifndef SANDBOX_GRAPHICS_COMPONENT_H_
#define SANDBOX_GRAPHICS_COMPONENT_H_

#include <typeinfo>
#include <vector>
#include "sandbox/Component.h"
#include "sandbox/graphics/GraphicsContext.h"

namespace sandbox {

class GraphicsComponent : public Component {
public:
	GraphicsComponent() {
		addType<GraphicsComponent>();
	}
	virtual ~GraphicsComponent() {}
	virtual void updateSharedContext(const GraphicsContext& graphicsContext) {}
	virtual void updateContext(const GraphicsContext& graphicsContext) {}
	virtual void use(const GraphicsContext& graphicsContext) {}
	virtual void release(const GraphicsContext& graphicsContext) {}
	virtual void startRender(const GraphicsContext& graphicsContext) {}
	virtual void finishRender(const GraphicsContext& graphicsContext) {}
};

}

#endif