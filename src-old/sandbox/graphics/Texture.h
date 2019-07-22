#ifndef SANDBOX_GRAPHICS_TEXTURE_H_
#define SANDBOX_GRAPHICS_TEXTURE_H_

#include "sandbox/SceneComponent.h"
#include "sandbox/base/Image.h"
#include "OpenGL.h"

namespace sandbox {

class Texture : public SceneComponent {
public:
	Texture();
	virtual ~Texture() {}

	void updateModel();
	void updateSharedContext(const SceneContext& sceneContext);
	void updateContext(const SceneContext& sceneContext);
	void render(const SceneContext& sceneContext);

	GLuint getTarget(const SceneContext& sceneContext);
	GLuint getId(const SceneContext& sceneContext);

	virtual GLuint getFormat() const;
	virtual GLuint getInternalFormat() const;
	virtual GLuint getType() const;

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
	SceneContextHandler<TextureSharedState,ContextState> contextHandler;
};

}

#endif