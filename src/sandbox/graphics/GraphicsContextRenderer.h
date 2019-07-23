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

	void update(const Entity& entity);
	void render(const Entity& entity);
	void startRender(const Entity& entity);
	void renderChildren(const Entity& entity);
	void finishRender(const Entity& entity);
	GraphicsContext context;
};

}

#endif