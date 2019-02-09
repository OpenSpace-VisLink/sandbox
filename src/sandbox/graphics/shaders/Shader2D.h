#ifndef SANDBOX_GRAPHICS_SHADERS_SHADER2D_H_
#define SANDBOX_GRAPHICS_SHADERS_SHADER2D_H_

#include "sandbox/graphics/ShaderProgram.h"

namespace sandbox {

class Shader2D : public ShaderProgram {
public:
	Shader2D();
	virtual ~Shader2D();

	void setShaderParameters(const SceneContext& sceneContext, ShaderProgramState& state);

protected:
	void create(const SceneContext& sceneContext, ShaderProgramState& state);
};

}

#endif