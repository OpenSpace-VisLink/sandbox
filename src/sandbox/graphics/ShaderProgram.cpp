#include "sandbox/graphics/ShaderProgram.h"
#include <iostream>

#include "sandbox/graphics/RenderState.h"

namespace sandbox {

ShaderProgram::ShaderProgram() : forceShader(false) {
	addType<ShaderProgram>();
}

void ShaderProgram::updateContext(const SceneContext& sceneContext) {
	ShaderProgramState& state = *contextHandler.getState(sceneContext);

	if (!state.initialized) {
        std::cout << "INitialize shader context" << std::endl;
        state.shaderProgram = glCreateProgram();

        create(sceneContext, state);

	    linkShaderProgram(state.shaderProgram);
	    state.initialized = true;
	}
}

void ShaderProgram::use(const SceneContext& sceneContext) {
	ShaderProgramState& state = *contextHandler.getState(sceneContext);

	if (state.initialized) {
		glUseProgram(state.shaderProgram);
		setShaderParameters(sceneContext, state);
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
		renderState.getShaderProgram().push(this, forceShader);
	}
}

void ShaderProgram::finishRender(const SceneContext& sceneContext) {
	ShaderProgramState& state = *contextHandler.getState(sceneContext);

	if (state.initialized) {
		RenderState& renderState = RenderState::get(sceneContext);
	    renderState.getShaderProgram().pop();
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