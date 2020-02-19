#ifndef SANDBOX_GRAPHICS_SHADERS_MATERIAL_SHADER_H_
#define SANDBOX_GRAPHICS_SHADERS_MATERIAL_SHADER_H_

#include "sandbox/graphics/ShaderProgram.h"

namespace sandbox {

class MaterialShader : public ShaderProgram {
public:
	MaterialShader();
	virtual ~MaterialShader();

	void setShaderParameters(const SceneContext& sceneContext, ShaderProgramState& state);

protected:
	void create(const SceneContext& sceneContext, ShaderProgramState& state);
};

}

#endif