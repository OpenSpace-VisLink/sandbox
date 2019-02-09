#ifndef SANDBOX_GRAPHICS_NODE_RENDERER_H_
#define SANDBOX_GRAPHICS_NODE_RENDERER_H_

#include "sandbox/SceneComponent.h"

namespace sandbox {

class NodeRenderer : public SceneComponent {
public:
	NodeRenderer(SceneNode* node) : node(node) {}
	virtual ~NodeRenderer() {}

	virtual void render(const SceneContext& sceneContext) {
		node->render(sceneContext);
	}

private:
	SceneNode* node;
};

}

#endif