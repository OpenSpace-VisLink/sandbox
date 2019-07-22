#ifndef SANDBOX_GRAPHICS_CONEXT_RENDERER_H_
#define SANDBOX_GRAPHICS_CONEXT_RENDERER_H_

#include "sandbox/graphics/GraphicsComponent.h"

namespace sandbox {

class GraphicsContextRenderer : public Component {
public:
	GraphicsContextRenderer() {
		addType<GraphicsContextRenderer>();
	}
	virtual ~GraphicsContextRenderer() {}

	void update();
	void render();

private:
	void renderRecursive(const Entity& entity);
	GraphicsContext context;
};

}

#endif