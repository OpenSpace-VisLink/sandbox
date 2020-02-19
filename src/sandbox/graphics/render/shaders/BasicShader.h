#ifndef SANDBOX_GRAPHICS_SHADERS_BASIC_SHADER_H_
#define SANDBOX_GRAPHICS_SHADERS_BASIC_SHADER_H_

#include "sandbox/graphics/render/ShaderProgram.h"
#include "sandbox/graphics/render/Texture.h"

namespace sandbox {

class BasicShader : public ShaderProgram {
public:
	BasicShader();
	virtual ~BasicShader();

	void setShaderParameters(const GraphicsContext& context, ShaderProgramState& state);

protected:
	void create(const GraphicsContext& context, ShaderProgramState& state);

private:
	Texture* texture;
};

}

#endif