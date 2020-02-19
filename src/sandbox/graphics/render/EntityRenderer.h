#ifndef SANDBOX_GRAPHICS_ENTITY_RENDERER_H_
#define SANDBOX_GRAPHICS_ENTITY_RENDERER_H_

#include "sandbox/graphics/GraphicsComponent.h"
#include "sandbox/graphics/RenderState.h"

namespace sandbox {

class EntityRenderer : public GraphicsComponent {
public:
	EntityRenderer(Entity* entity) : entity(entity) {}
	virtual ~EntityRenderer() {}

	virtual void startRender(const GraphicsContext& context) {
		RenderState& renderState = RenderState::get(context);
		GraphicsContextRenderer* renderer = renderState.getRenderer().get();
		if (renderer) {
			renderer->startRender(*entity);
		}

		renderer->renderChildren(*entity);
	}

	virtual void finishRender(const GraphicsContext& context) {
		RenderState& renderState = RenderState::get(context);
		GraphicsContextRenderer* renderer = renderState.getRenderer().get();
		if (renderer) {
			renderer->finishRender(*entity);
		}
	}

private:
	Entity* entity;
};

}

#endif