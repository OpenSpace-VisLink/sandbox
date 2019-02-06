#include "sandbox/graphics/ShaderProgram.h"
#include <iostream>

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
			            "void main() { "
			            "   vec2 pos = position.xy; "
			            "   gl_Position = vec4(pos, 0.0, 1.0); "
			            "}";

		state.addShader(compileShader(vertexShader, GL_VERTEX_SHADER));

	    std::string fragmentShader =
	            "#version 330 \n"
	            "layout (location = 0) out vec4 colorOut;  "
	            ""
	            "void main() { colorOut = vec4(1,0,0,1); }";
	    state.addShader(compileShader(fragmentShader, GL_FRAGMENT_SHADER));
        // finish create

	    linkShaderProgram(state.shaderProgram);
	    state.initialized = true;
	}
}

void ShaderProgram::use(const SceneContext& sceneContext) {
	ShaderProgramState& state = *contextHandler.getState(sceneContext);

	if (state.initialized) {
		//std::cout << "Use Shader" << state.shaderProgram << std::endl;
		glUseProgram(state.shaderProgram);
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