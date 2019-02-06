#ifndef SANDBOX_GRAPHICS_SHADER_PROGRAM_H_
#define SANDBOX_GRAPHICS_SHADER_PROGRAM_H_

#include "sandbox/SceneComponent.h"
#include "OpenGL.h"
#include <vector>

namespace sandbox {

class ShaderProgram : public SceneComponent {
public:
	ShaderProgram();
	virtual ~ShaderProgram() {}

	void updateContext(const SceneContext& sceneContext);
	void use(const SceneContext& sceneContext);
	void render(const SceneContext& sceneContext) { use(sceneContext); }

protected:
	GLuint compileShader(const std::string& shaderText, GLuint shaderType) const;
	void linkShaderProgram(GLuint shaderProgram) const;

private:
	class ShaderProgramState : public ContextState {
	public:
	    virtual ~ShaderProgramState() {
	    	reset();
	    }

	    GLuint shaderProgram;

	    void reset() {
	    	for (int f = 0; f < shaders.size(); f++) {
	            glDetachShader(shaderProgram, shaders[f]);
	            glDeleteShader(shaders[f]);
	    	}

	    	shaders.clear();

	        glDeleteProgram(shaderProgram);
	    }

	    void addShader(GLuint shader) {
		    glAttachShader(shaderProgram, shader);
	    	shaders.push_back(shader);
	    }

	private:
	    std::vector<GLuint> shaders;
	};

	SceneContextHandler<ContextState,ShaderProgramState> contextHandler;
};

}

#endif