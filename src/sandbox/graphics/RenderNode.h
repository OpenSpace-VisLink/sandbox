#ifndef SANDBOX_GRAPHICS_RENDER_NODE_H_
#define SANDBOX_GRAPHICS_RENDER_NODE_H_

#include "sandbox/Component.h"
#include "sandbox/graphics/GraphicsContext.h"
#include "sandbox/graphics/RenderObject.h"

namespace sandbox {

enum RenderAction {
	RENDER_ACTION_RENDER,
	RENDER_ACTION_START,
	RENDER_ACTION_END
};

class RenderNode : public RenderObject {
public:

	RenderNode(Entity* proxyEntity, RenderAction action = RENDER_ACTION_RENDER) : proxyEntity(proxyEntity), action(action) { addType<RenderNode>(); }
	virtual ~RenderNode() {}

	void render(const GraphicsContext& context) {
		startRender(context);
		finishRender(context);
	}

	void startRender(const GraphicsContext& context) {
		render(proxyEntity, context);
	}

	void finishRender(const GraphicsContext& context) {}

private:
	void render(Entity* entity, const GraphicsContext& context) {
		std::vector<RenderObject*> components = entity->getComponents<RenderObject>();
		
		if (action == RENDER_ACTION_RENDER || action == RENDER_ACTION_START) {
			for (int f = 0; f < components.size(); f++) {
				components[f]->startRender(context);
			}

			const std::vector<Entity*>& children = entity->getChildren();
			for (int f = 0; f < entity->getChildren().size(); f++) {
				render(children[f], context);
			}

		}

		if (action == RENDER_ACTION_RENDER || action == RENDER_ACTION_END) {
			for (int f = components.size()-1; f >= 0; f--) {
				components[f]->finishRender(context);
			}	
		}
	}

	Entity* proxyEntity;
	RenderAction action;
};

}

#endif