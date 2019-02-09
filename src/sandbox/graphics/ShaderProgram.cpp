#include "sandbox/graphics/ShaderProgram.h"
#include <iostream>

#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "sandbox/graphics/RenderState.h"

namespace sandbox {

ShaderProgram::ShaderProgram() {
	addType<ShaderProgram>();
}

void ShaderProgram::updateContext(const SceneContext& sceneContext) {
	ShaderProgramState& state = *contextHandler.getState(sceneContext);

	if (!state.initialized) {
        std::cout << "INitialize shader context" << std::endl;
        state.shaderProgram = glCreateProgram();

		//create(context, state);
		GLuint shader;

		std::string vertexShader =
			            "#version 330 \n"
			            "layout(location = 0) in vec3 position; "
			            "layout(location = 1) in vec3 normal; "
						"layout(location = 2) in vec2 coord; "
			            ""
			            "uniform float scale; "
			            "uniform mat4 ModelMatrix; "
			            "out vec2 pos; "
			            ""
			            "void main() { "
			            "   pos = position.xy; "
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
        // finish create

	    linkShaderProgram(state.shaderProgram);
	    state.initialized = true;
	}
}

void ShaderProgram::use(const SceneContext& sceneContext) {
	ShaderProgramState& state = *contextHandler.getState(sceneContext);

	if (state.initialized) {
		glUseProgram(state.shaderProgram);
		setShaderParameters(sceneContext);
	}
}

void ShaderProgram::release(const SceneContext& sceneContext) {
	ShaderProgramState& state = *contextHandler.getState(sceneContext);

	if (state.initialized) {
		glUseProgram(0);
	}
}


void ShaderProgram::render(const SceneContext& sceneContext) {
	ShaderProgramState& state = *contextHandler.getState(sceneContext);

	if (state.initialized) {
		RenderState& renderState = RenderState::get(sceneContext);
		renderState.getShaderProgram().push(this);
	}
}

void ShaderProgram::finishRender(const SceneContext& sceneContext) {
	ShaderProgramState& state = *contextHandler.getState(sceneContext);

	if (state.initialized) {
		RenderState& renderState = RenderState::get(sceneContext);
	    renderState.getShaderProgram().pop();
	}
}

void ShaderProgram::setShaderParameters(const SceneContext& sceneContext) {
	ShaderProgramState& state = *contextHandler.getState(sceneContext);

	if (state.initialized) {
		RenderState& renderState = RenderState::get(sceneContext);
		GLint loc = glGetUniformLocation(state.shaderProgram, "ModelMatrix");
	    //glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(glm::scale(glm::mat4(1.0f),glm::vec3(0.5f))));
	    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(renderState.getModelMatrix().get()));
	    //GLint loc = glGetUniformLocation(state.shaderProgram, "scale");
    	//glUniform1f(loc, 0.5);
	}
}

GLuint ShaderProgram::compileShader(const std::string& shaderText, GLuint shaderType) const {
    const char* source = shaderText.c_str();
    int length = (int)shaderText.size();
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &source, &length);
    glCompileShader(shader);
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if(status == GL_FALSE) {
        GLint length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> log(length);
        glGetShaderInfoLog(shader, length, &length, &log[0]);
        std::cerr << &log[0];
    }

    return shader;
}

/// links shader program
void ShaderProgram::linkShaderProgram(GLuint shaderProgram) const {
    glLinkProgram(shaderProgram);
    GLint status;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
    if(status == GL_FALSE) {
        GLint length;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> log(length);
        glGetProgramInfoLog(shaderProgram, length, &length, &log[0]);
        std::cerr << "Error compiling program: " << &log[0] << std::endl;
    }
}


}