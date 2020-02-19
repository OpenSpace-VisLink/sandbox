#ifndef SANDBOX_GRAPHICS_CULL_FACE_H_
#define SANDBOX_GRAPHICS_CULL_FACE_H_

#include "sandbox/graphics/GraphicsComponent.h"
#include "OpenGL.h"

namespace sandbox {

class CullFace : public GraphicsComponent {
public:
	CullFace(GLuint cullFace);
	virtual ~CullFace() {}

	void startRender(const GraphicsContext& context);
	void finishRender(const GraphicsContext& context);

private:
	GLuint cullFace;
};

}

#endif