#include "sandbox/graphics/render/ShaderProgram.h"
#include <iostream>
#include <sstream>
#include <set>

#include "sandbox/graphics/RenderState.h"

namespace sandbox {

ShaderProgram::ShaderProgram() : forceShader(false), version(-1) {
	addType<ShaderProgram>();
}

void ShaderProgram::updateContext(const GraphicsContext& sceneContext) {
    //std::cout << "updateContext" << std::endl;
	ShaderProgramState& state = *contextHandler.getState(sceneContext);

    if (state.initialized && state.version != version) {
        state.reset();
        state.initialized = false;
        state.version = version;
    }

	if (!state.initialized) {
        std::cout << "INitialize shader context" << std::endl;
        state.shaderProgram = glCreateProgram();

        create(sceneContext, state);

	    if (linkShaderProgram(state.shaderProgram)) {
            GLint count;
            GLchar name[50];
            GLsizei length;
            GLint size;
            GLenum type;

            glGetProgramiv(state.shaderProgram, GL_ACTIVE_UNIFORMS, &count);
            for (int f = 0; f < count; f++) {
                glGetActiveUniform(state.shaderProgram, f, 50, &length, &size, &type, name);

                if (size > 1) {
                    for (int i = 0; i < size; i++) {
                        std::string n(name);
                        std::stringstream ss;
                        ss << n.substr(0, n.length()-2) << i << "]";
                        n = ss.str();
                        GLint loc = glGetUniformLocation(state.shaderProgram, n.c_str());
                        state.uniformMap[n] = loc;
                        std::cout << n << " " << loc << std::endl;
                    }
                }
                else {
                    GLint loc = glGetUniformLocation(state.shaderProgram, name);
                    state.uniformMap[name] = loc;
                    std::cout << name << " " << loc << std::endl;
                }
            }
        }
	    state.initialized = true;
        state.version = version;
	}
}

void ShaderProgram::use(const GraphicsContext& sceneContext) {
	ShaderProgramState& state = *contextHandler.getState(sceneContext);

	if (state.initialized) {
		glUseProgram(state.shaderProgram);
		setShaderParameters(sceneContext, state);
	}
}

void ShaderProgram::release(const GraphicsContext& sceneContext) {
	ShaderProgramState& state = *contextHandler.getState(sceneContext);

	if (state.initialized) {
		glUseProgram(0);
	}
}


void ShaderProgram::startRender(const GraphicsContext& sceneContext) {
	ShaderProgramState& state = *contextHandler.getState(sceneContext);

	if (state.initialized) {
		RenderState& renderState = RenderState::get(sceneContext);
		renderState.getShaderProgram().push(this, forceShader);
	}
}

void ShaderProgram::finishRender(const GraphicsContext& sceneContext) {
	ShaderProgramState& state = *contextHandler.getState(sceneContext);

	if (state.initialized) {
		RenderState& renderState = RenderState::get(sceneContext);
	    renderState.getShaderProgram().pop();
	}
}

void ShaderProgram::setShaderParameters(const GraphicsContext& context, ShaderProgram::ShaderProgramState& state) {
    std::set<std::string> keys;
    RenderState& renderState = RenderState::get(context);
    const std::vector<ShaderParameter*>& shaderParameters = renderState.getShaderParameter().getQueue();

    for (int f = shaderParameters.size()-1; f >= 0; f--) {
        ShaderParameter* param = shaderParameters[f];
        if (param != NULL && keys.find(param->getName()) == keys.end()) {
            std::map<std::string, GLint>::iterator it = state.uniformMap.find(param->getName());
            if (it != state.uniformMap.end()) {
                // Set parameter
                param->setParameter(context, state.shaderProgram, it->second);
                keys.insert(param->getName());
            }
        }
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
bool ShaderProgram::linkShaderProgram(GLuint shaderProgram) const {
    glLinkProgram(shaderProgram);
    GLint status;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
    if(status == GL_FALSE) {
        GLint length;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> log(length);
        glGetProgramInfoLog(shaderProgram, length, &length, &log[0]);
        std::cerr << "Error compiling program: " << &log[0] << std::endl;
        return false;
    }

    return true;
}

void ShaderParameter::startRender(const GraphicsContext& context) {
    use(context);
}
void ShaderParameter::finishRender(const GraphicsContext& context) {
    release(context);
}
void ShaderParameter::use(const GraphicsContext& context) {
    RenderState& renderState = RenderState::get(context);
    renderState.getShaderParameter().push(this);
}
void ShaderParameter::release(const GraphicsContext& context) {
    RenderState::get(context).getShaderParameter().pop();
}
}