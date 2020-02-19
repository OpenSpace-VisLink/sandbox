#ifndef SANDBOX_GRAPHICS_RENDER_TEXTURE_H_
#define SANDBOX_GRAPHICS_RENDER_TEXTURE_H_

#include "sandbox/graphics/GraphicsComponent.h"
#include "sandbox/image/Image.h"
#include "OpenGL.h"

namespace sandbox {

class Texture : public GraphicsComponent {
public:
	Texture();
	virtual ~Texture() {}

	void update();
	void updateSharedContext(const GraphicsContext& context);
	void updateContext(const GraphicsContext& context);
	void startRender(const GraphicsContext& context);
	void finishRender(const GraphicsContext& context);
	void use(const GraphicsContext& context);
	void release(const GraphicsContext& context);

	int getWidth() { return image->getWidth(); }
	int getHeight() { return image->getHeight(); }

	GLuint getTarget(const GraphicsContext& context);
	GLuint getId(const GraphicsContext& context);

	virtual GLuint getFormat() const;
	virtual GLuint getInternalFormat() const;
	virtual GLuint getType() const;
	virtual GLuint getAttachmentType() const;

private:
	class TextureSharedState : public ContextState {
	public:
	    virtual ~TextureSharedState() {
	    	reset();
	    }

	    void reset() {
	    	if (initialized) {
	    		glDeleteTextures(1, &texture);
	    	}
	    }

		GLuint texture;
		int width, height;
		GLuint target;
		long textureVersion;
	};

	Image* image;
	GraphicsContextHandler<TextureSharedState,ContextState> contextHandler;
	long textureVersion;
	GLuint format;
	GLuint internalFormat;
};

}

#endif