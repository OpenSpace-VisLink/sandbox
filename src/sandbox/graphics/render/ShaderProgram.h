#ifndef SANDBOX_GRAPHICS_SHADER_PROGRAM_H_
#define SANDBOX_GRAPHICS_SHADER_PROGRAM_H_

#include "sandbox/graphics/GraphicsComponent.h"
#include "OpenGL.h"
#include <vector>

namespace sandbox {

class ShaderProgram : public GraphicsComponent {
public:
	ShaderProgram();
	virtual ~ShaderProgram() {}

	void update() {
		version++;
	}

	void updateContext(const GraphicsContext& context);
	void use(const GraphicsContext& context);
	void release(const GraphicsContext& context);
	void startRender(const GraphicsContext& context);
	void finishRender(const GraphicsContext& context);
	bool setForceShader(bool forceShader) { this->forceShader = forceShader; }

protected:

	class ShaderProgramState : public ContextState {
	public:
		ShaderProgramState() : version(0) {}
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
	        initialized = false;
	    }

	    void addShader(GLuint shader) {
		    glAttachShader(shaderProgram, shader);
	    	shaders.push_back(shader);
	    }

	    int version;

	private:
	    std::vector<GLuint> shaders;
	};

	virtual void setShaderParameters(const GraphicsContext& context, ShaderProgramState& state) = 0;
	virtual void create(const GraphicsContext& context, ShaderProgramState& state) = 0;
	GLuint compileShader(const std::string& shaderText, GLuint shaderType) const;
	void linkShaderProgram(GLuint shaderProgram) const;

private:
	GraphicsContextHandler<ContextState,ShaderProgramState> contextHandler;
	bool forceShader;
	int version;
};

}

#endif