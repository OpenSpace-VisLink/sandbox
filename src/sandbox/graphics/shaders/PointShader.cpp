#include "sandbox/graphics/shaders/PointShader.h"

#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "sandbox/graphics/RenderState.h"


namespace sandbox {

PointShader::PointShader() : xDim(0), yDim(1) {
	addType<PointShader>();
}

PointShader::~PointShader() {}

void PointShader::create(const SceneContext& sceneContext, ShaderProgramState& state) {
	GLuint shader;

	std::string vertexShader =
		            "#version 330 \n"
		            "layout(location = 0) in vec4 data[16]; "
		            ""
		            "uniform float scale; "
		            "uniform int dataIndex; "
		            "uniform int xDim; "
		            "uniform int yDim; "
		            "uniform mat4 ModelMatrix; "
		            "out vec2 pos; "
		            ""
		            "void main() { "
		            "   pos = vec2(data[dataIndex][xDim], data[dataIndex][yDim]); "
		            "   gl_Position = ModelMatrix*vec4(pos, 0.0, 1.0); "
		            "}";

	state.addShader(compileShader(vertexShader, GL_VERTEX_SHADER));

    std::string fragmentShader =
            "#version 330 \n"
		    ""
		    "in vec2 pos; "
		    ""
            "layout (location = 0) out vec4 colorOut;  "
            ""
            "void main() { colorOut = vec4(pos,0,1); }";
    state.addShader(compileShader(fragmentShader, GL_FRAGMENT_SHADER));
}

void PointShader::setShaderParameters(const SceneContext& sceneContext, ShaderProgramState& state) {
	RenderState& renderState = RenderState::get(sceneContext);
	GLint loc = glGetUniformLocation(state.shaderProgram, "ModelMatrix");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(renderState.getModelMatrix().get()));
	loc = glGetUniformLocation(state.shaderProgram, "dataIndex");
    glUniform1i(loc, 0);
	loc = glGetUniformLocation(state.shaderProgram, "xDim");
    glUniform1i(loc, xDim);
	loc = glGetUniformLocation(state.shaderProgram, "yDim");
    glUniform1i(loc, yDim);
}

}