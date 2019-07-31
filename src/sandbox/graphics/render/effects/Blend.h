#ifndef SANDBOX_GRAPHICS_BLEND_H_
#define SANDBOX_GRAPHICS_BLEND_H_

#include "sandbox/graphics/GraphicsComponent.h"
#include "OpenGL.h"

namespace sandbox {

class Blend : public GraphicsComponent {
public:
	Blend();
	virtual ~Blend() {}

	void startRender(const GraphicsContext& context);
	void finishRender(const GraphicsContext& context);

private:
};

}

#endif