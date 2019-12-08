#ifndef SANDBOX_GRAPHICS_RENDER_OBJECT_H_
#define SANDBOX_GRAPHICS_RENDER_OBJECT_H_

#include "sandbox/Component.h"
#include "sandbox/graphics/GraphicsContext.h"

namespace sandbox {

class RenderObject : public Component {
public:
	RenderObject() { addType<RenderObject>(); }
	virtual ~RenderObject() {}

	virtual void startRender(const GraphicsContext& context) {}
	virtual void finishRender(const GraphicsContext& context) {}
};

}

#endif