#ifndef SANDBOX_GRAPHICS_SHADERS_MATERIAL_SHADER_H_
#define SANDBOX_GRAPHICS_SHADERS_MATERIAL_SHADER_H_

#include "sandbox/graphics/render/ShaderProgram.h"
#include "sandbox/graphics/RenderState.h"
#include <iostream>

namespace sandbox {

class MaterialShader : public ShaderProgram {
public:
	MaterialShader();
	virtual ~MaterialShader();

	void setShaderParameters(const GraphicsContext& context, ShaderProgramState& state);

protected:
	void create(const GraphicsContext& context, ShaderProgramState& state);
};

class MaterialState : public StateContainerItem {
public:
	MaterialState() {
		entity.set(NULL);
	}

	virtual ~MaterialState() {}

	StateContainerItemStack<const Entity*>& getEntity() { return entity; }

	static MaterialState& get(const GraphicsContext& context) { return context.getRenderState()->getItem<MaterialState>(); }

private:
	StateContainerItemStack<const Entity*> entity;
};

class MaterialRenderer : public GraphicsComponent {
public:
	MaterialRenderer() {}
	virtual ~MaterialRenderer() {}

	void startRender(const GraphicsContext& context) {
		MaterialState& materialState = MaterialState::get(context);
		materialState.getEntity().push(&getEntity());
	}

	void finishRender(const GraphicsContext& context) {
		MaterialState& materialState = MaterialState::get(context);
		materialState.getEntity().pop();
	}
};




}

#endif