#ifndef SANDBOX_GRAPHICS_SHADERS_POINTSHADER_H_
#define SANDBOX_GRAPHICS_SHADERS_POINTSHADER_H_

#include "sandbox/graphics/ShaderProgram.h"

namespace sandbox {

class PointShader : public ShaderProgram {
public:
	PointShader();
	virtual ~PointShader();

	void setShaderParameters(const SceneContext& sceneContext, ShaderProgramState& state);

	void setXDim(int dim) { xDim = dim; }
	void setYDim(int dim) { yDim = dim; }

protected:
	void create(const SceneContext& sceneContext, ShaderProgramState& state);

private:
	int xDim;
	int yDim;
};

}

#endif