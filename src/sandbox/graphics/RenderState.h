#ifndef SANDBOX_GRAPHICS_RENDER_STATE_H_
#define SANDBOX_GRAPHICS_RENDER_STATE_H_

#include "glm/glm.hpp"
#include "sandbox/StateContainer.h"
#include "sandbox/graphics/GraphicsContext.h"
#include "sandbox/Entity.h"
//#include "sandbox/graphics/ShaderProgram.h"

namespace sandbox {

class RenderState : public StateContainerItem {
public:
	RenderState() {
		viewport.set(glm::ivec4(1.0));
		projectionMatrix.set(glm::mat4(1.0));
		viewMatrix.set(glm::mat4(1.0));
		modelMatrix.set(glm::mat4(1.0));
		//shaderProgram.set(NULL);
		entity.set(NULL);
	}

	virtual ~RenderState() {}

	StateContainerItemStack<glm::ivec4>& getViewport() { return viewport; }
	StateContainerItemStack<glm::mat4>& getProjectionMatrix() { return projectionMatrix; }
	StateContainerItemStack<glm::mat4>& getViewMatrix() { return viewMatrix; }
	StateContainerItemStack<glm::mat4>& getModelMatrix() { return modelMatrix; }
	//StateItemStack<ShaderProgram*>& getShaderProgram() { return shaderProgram; }
	StateContainerItemStack<const Entity*>& getEntity() { return entity; }

	static RenderState& get(const GraphicsContext& context) { return context.getRenderState()->getItem<RenderState>(); }

private:
	StateContainerItemStack<glm::ivec4> viewport;
	StateContainerItemStack<glm::mat4> projectionMatrix;
	StateContainerItemStack<glm::mat4> viewMatrix;
	StateContainerItemStack<glm::mat4> modelMatrix;
	//StateItemStack<ShaderProgram*> shaderProgram;
	StateContainerItemStack<const Entity*> entity;
};

}

#endif