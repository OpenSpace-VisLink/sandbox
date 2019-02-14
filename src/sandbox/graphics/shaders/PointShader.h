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
	void setColorDim(int dim) { colorDim = dim; }
	void setXRange(const glm::vec2& range ) { xRange = range; }
	void setYRange(const glm::vec2& range ) { yRange = range; }
	void setColorRange(const glm::vec2& range ) { colorRange = range; }
	void setColor(const glm::vec4& color) { this-> color = color; }

protected:
	void create(const SceneContext& sceneContext, ShaderProgramState& state);

private:
	int xDim;
	int yDim;
	int colorDim;
	glm::vec2 xRange;
	glm::vec2 yRange;
	glm::vec2 colorRange;
	glm::vec4 color;
};

}

#endif