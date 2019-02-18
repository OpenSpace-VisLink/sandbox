#ifndef SANDBOX_GRAPHICS_SHADERS_SHADER2D_H_
#define SANDBOX_GRAPHICS_SHADERS_SHADER2D_H_

#include "sandbox/graphics/ShaderProgram.h"

namespace sandbox {

class Shader2D : public ShaderProgram {
public:
	Shader2D();
	virtual ~Shader2D();

	void setShaderParameters(const SceneContext& sceneContext, ShaderProgramState& state);
	void setColor(const glm::vec4& color) { this-> color = color; hasColor = true; }
	void setTexture(SceneNode* textureNode) { this->textureNode = textureNode; }

protected:
	void create(const SceneContext& sceneContext, ShaderProgramState& state);

private:
	SceneNode* textureNode;
	glm::vec4 color;
	bool hasColor;
};

}

#endif