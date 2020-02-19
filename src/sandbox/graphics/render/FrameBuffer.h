#ifndef SANDBOX_GRAPHICS_RENDER_FRAME_BUFFER_H_
#define SANDBOX_GRAPHICS_RENDER_FRAME_BUFFER_H_

#include "sandbox/graphics/GraphicsComponent.h"
#include "sandbox/graphics/render/Texture.h"
#include "OpenGL.h"
#include <vector>

namespace sandbox {

class FrameBuffer : public GraphicsComponent {
public:
	FrameBuffer();
	virtual ~FrameBuffer() {}

	void update();

	void updateContext(const GraphicsContext& context);
	void use(const GraphicsContext& context);
	void release(const GraphicsContext& context);
	void startRender(const GraphicsContext& context);
	void finishRender(const GraphicsContext& context);

protected:

	class FrameBufferState : public ContextState {
	public:
		FrameBufferState() : inUse(false), version(0), numAttachments(0), numColorAttachments(0) {}
	    virtual ~FrameBufferState() {
	    	reset();
	    }

	    void reset() {
	    	if (initialized) {
	    		glDeleteFramebuffers(1, &frameBuffer);
	    		drawBuffers.clear();
	    		numAttachments = 0;
	    		numColorAttachments = 0;
	    	}

	    	initialized = false;
	    }

	    GLuint frameBuffer;
		std::vector<GLuint> drawBuffers;
	    int width, height;
	    int version;
	    int numAttachments;
	    int numColorAttachments;

	    bool inUse;
	    GLint drawFboId;
		GLint readFboId;
		GLint oldViewport[4];
		GLint oldScissorBox[4];
	};

	//GLuint compileShader(const std::string& shaderText, GLuint shaderType) const;
	//void linkShaderProgram(GLuint shaderProgram) const;

private:
	void attach(const GraphicsContext& context, FrameBufferState& state, Texture& texture);

	GraphicsContextHandler<ContextState,FrameBufferState> contextHandler;
	int version;
	std::vector<Texture*> textures;
};

}

#endif