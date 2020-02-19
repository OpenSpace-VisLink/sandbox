#ifndef SANDBOX_GRAPHICS_GRAPHICS_RENDERER_H_
#define SANDBOX_GRAPHICS_GRAPHICS_RENDERER_H_

#include "sandbox/Component.h"
#include "sandbox/graphics/GraphicsContext.h"

namespace sandbox {

class GraphicsRenderer : public Component {
public:
	GraphicsRenderer(GraphicsContext* context = NULL) : context(context), node(NULL) {
		addType<GraphicsRenderer>();
	}
	virtual ~GraphicsRenderer() {
		if (node) {
			delete node;
		}
		delete context;
	}

	void update() {
		if (!node) {
			node = new RenderNode(&getEntity());
		}

		if (!context) {
			SharedContext* sharedContext = getEntity().getComponentRecursive<SharedContext>(false);
			if (sharedContext) {
				this->context = new GraphicsContext(&sharedContext->getContext(), new Context(), false);
			}
			else {
				this->context = new GraphicsContext();
			}
		}
	}

	void render() {
		node->render(*context);
	}

	GraphicsContext& getContext() { return *context; }

private:
	GraphicsContext* context;
	RenderNode* node;
};

}

#endif