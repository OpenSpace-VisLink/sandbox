#ifndef SANDBOX_GRAPHICS_SHADERS_POINTSHADER_H_
#define SANDBOX_GRAPHICS_SHADERS_POINTSHADER_H_

#include "sandbox/graphics/ShaderProgram.h"
#include "glm/glm.hpp"

namespace sandbox {

class PointShader : public ShaderProgram {
public:
	PointShader();
	virtual ~PointShader();

	void setShaderParameters(const SceneContext& sceneContext, ShaderProgramState& state);

	void setXDim(int dim) { xDim = dim; }
	void setYDim(int dim) { yDim = dim; }
	void setXRange(const glm::vec2& range ) { xRange = range; }
	void setYRange(const glm::vec2& range ) { yRange = range; }

protected:
	void create(const SceneContext& sceneContext, ShaderProgramState& state);

private:
	int xDim;
	int yDim;
	glm::vec2 xRange;
	glm::vec2 yRange;
};

}

#endif