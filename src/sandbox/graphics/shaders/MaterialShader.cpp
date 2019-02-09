#include "sandbox/graphics/shaders/MaterialShader.h"

#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "sandbox/graphics/RenderState.h"


namespace sandbox {

MaterialShader::MaterialShader() {
	addType<MaterialShader>();
}

MaterialShader::~MaterialShader() {}

void MaterialShader::create(const SceneContext& sceneContext, ShaderProgramState& state) {
	GLuint shader;

	std::string vertexShader =
		            "#version 330 \n"
		            "layout(location = 0) in vec3 position; "
		            "layout(location = 1) in vec3 normal; "
					"layout(location = 2) in vec2 coord; "
		            ""
		            "uniform float scale; "
		            "uniform mat4 ModelMatrix; "
		            "out vec3 pos; "
		            "out vec3 norm; "
		            ""
		            "void main() { "
		            "   pos = position.xyz; "
		            "   norm = normal.xyz; "
		            "   gl_Position = ModelMatrix*vec4(pos, 1.0); "
		            "}";

	state.addShader(compileShader(vertexShader, GL_VERTEX_SHADER));

    std::string fragmentShader =
            "#version 330 \n"
		    ""
		    "in vec3 pos; "
		    "in vec3 norm; "
		    ""
            "layout (location = 0) out vec4 colorOut;  "
            ""
            "void main() { colorOut = vec4(norm,1); }";
    state.addShader(compileShader(fragmentShader, GL_FRAGMENT_SHADER));
}

void MaterialShader::setShaderParameters(const SceneContext& sceneContext, ShaderProgramState& state) {
	RenderState& renderState = RenderState::get(sceneContext);
	GLint loc = glGetUniformLocation(state.shaderProgram, "ModelMatrix");
	//glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(glm::scale(glm::mat4(1.0f),glm::vec3(0.5f))));
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(renderState.getModelMatrix().get()));
	//GLint loc = glGetUniformLocation(state.shaderProgram, "scale");
    //glUniform1f(loc, 0.5);
}

}