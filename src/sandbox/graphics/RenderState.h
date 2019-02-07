#ifndef SANDBOX_GRAPHICS_RENDER_STATE_H_
#define SANDBOX_GRAPHICS_RENDER_STATE_H_

#include "glm/glm.hpp"
#include "sandbox/SceneState.h"
#include "sandbox/SceneContext.h"

namespace sandbox {

class RenderState : public SceneStateItem {
public:
	RenderState() {
		projectionMatrix.set(glm::mat4(1.0));
		viewMatrix.set(glm::mat4(1.0));
		modelMatrix.set(glm::mat4(1.0));
	}

	virtual ~RenderState() {}

	StateItemStack<glm::mat4>& getProjectionMatrix() { return projectionMatrix; }
	StateItemStack<glm::mat4>& getViewMatrix() { return viewMatrix; }
	StateItemStack<glm::mat4>& getModelMatrix() { return modelMatrix; }

	static RenderState& get(const SceneContext& sceneContext) { return sceneContext.getRenderState().getItem<RenderState>(); }

private:
	StateItemStack<glm::mat4> projectionMatrix;
	StateItemStack<glm::mat4> viewMatrix;
	StateItemStack<glm::mat4> modelMatrix;
};

}

#endif