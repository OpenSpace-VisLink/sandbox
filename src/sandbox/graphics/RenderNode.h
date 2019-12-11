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

class RenderContextStateModifier {
public:
	struct State { virtual ~State() {} };

	virtual ~RenderContextStateModifier() {}
	virtual bool startRender(Entity* entity, const GraphicsContext& context, State*& modifyState) const {}
	virtual void finishRender(Entity* entity, const GraphicsContext& context, State*& modifyState) const {}
};

struct PathModifier {
	PathModifier(Entity* entity, const RenderContextStateModifier& modifier) : entity(entity), modifier(modifier) {}
	Entity* entity;
	const RenderContextStateModifier& modifier;
};

class RenderNode : public RenderObject {
public:

	RenderNode(Entity* proxyEntity, RenderAction action = RENDER_ACTION_RENDER, const RenderContextStateModifier* modifier = NULL) : proxyEntity(proxyEntity), action(action), modifier(modifier) { addType<RenderNode>(); }
	virtual ~RenderNode() {
		if (modifier) {
			delete modifier;
		}
	}

	void render(const GraphicsContext& context) {
		startRender(context);
		finishRender(context);
	}

	void startRender(const GraphicsContext& context) {
		startRender(context, this->modifier, NULL);
	}

	void startRender(const GraphicsContext& context, const RenderContextStateModifier* modifier, RenderContextStateModifier::State* modifyState) {
		bool modified = modifier && modifier->startRender(proxyEntity, context, modifyState);
		render(proxyEntity, context, modifier, modifyState);
		if (modified) {
			modifier->finishRender(proxyEntity, context, modifyState);
		}
	}

	void finishRender(const GraphicsContext& context) {}

	bool isNode() { return true; }

private:
	void render(Entity* entity, const GraphicsContext& context, const RenderContextStateModifier* modifier, RenderContextStateModifier::State* modifyState) {
		std::vector<RenderObject*> components = entity->getComponents<RenderObject>();
		
		if (action == RENDER_ACTION_RENDER || action == RENDER_ACTION_START) {
			for (int f = 0; f < components.size(); f++) {
				if (modifier && components[f]->isNode()) {
					static_cast<RenderNode*>(components[f])->startRender(context, modifier, modifyState);
				}
				else {
					components[f]->startRender(context);	
				}
			}

			const std::vector<Entity*>& children = entity->getChildren();
			for (int f = 0; f < entity->getChildren().size(); f++) {
				bool modified = modifier && modifier->startRender(children[f], context, modifyState);
				render(children[f], context, modifier, modifyState);
				if (modified) {
					modifier->finishRender(children[f], context, modifyState);
				}
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
	const RenderContextStateModifier* modifier;
};

}

#endif