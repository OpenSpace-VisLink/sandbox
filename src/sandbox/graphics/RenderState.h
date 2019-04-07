#ifndef SANDBOX_GRAPHICS_RENDER_STATE_H_
#define SANDBOX_GRAPHICS_RENDER_STATE_H_

#include "glm/glm.hpp"
#include "sandbox/SceneState.h"
#include "sandbox/SceneContext.h"
#include "sandbox/graphics/ShaderProgram.h"

namespace sandbox {

class RenderState : public SceneStateItem {
public:
	RenderState() {
		viewportSize.set(glm::ivec2(1.0));
		projectionMatrix.set(glm::mat4(1.0));
		viewMatrix.set(glm::mat4(1.0));
		modelMatrix.set(glm::mat4(1.0));
		shaderProgram.set(NULL);
		sceneNode.set(NULL);
	}

	virtual ~RenderState() {}

	StateItemStack<glm::ivec2>& getViewportSize() { return viewportSize; }
	StateItemStack<glm::mat4>& getProjectionMatrix() { return projectionMatrix; }
	StateItemStack<glm::mat4>& getViewMatrix() { return viewMatrix; }
	StateItemStack<glm::mat4>& getModelMatrix() { return modelMatrix; }
	StateItemStack<ShaderProgram*>& getShaderProgram() { return shaderProgram; }
	StateItemStack<const SceneNode*>& getSceneNode() { return sceneNode; }

	static RenderState& get(const SceneContext& sceneContext) { return sceneContext.getRenderState()->getItem<RenderState>(); }

private:
	StateItemStack<glm::ivec2> viewportSize;
	StateItemStack<glm::mat4> projectionMatrix;
	StateItemStack<glm::mat4> viewMatrix;
	StateItemStack<glm::mat4> modelMatrix;
	StateItemStack<ShaderProgram*> shaderProgram;
	StateItemStack<const SceneNode*> sceneNode;
};

}

#endif