#ifndef SANDBOX_GRAPHICS_WEBGL_WEBGLSTATE_H_
#define SANDBOX_GRAPHICS_WEBGL_WEBGLSTATE_H_

#include "sandbox/Component.h"
#include "sandbox/graphics/GraphicsContext.h"
#include <sstream>

namespace sandbox {

enum WebGLRenderMode {
	WEBGL_RENDER_NONE,
	WEBGL_RENDER_CONTEXT,
	WEBGL_RENDER_SCENE
};

class WebGLState : public StateContainerItem {
public:
	WebGLState() {
		context.set("");
		contextWriter.str("");
		renderMode.set(WEBGL_RENDER_NONE);
	}

	virtual ~WebGLState() {}

	StateContainerItemStack<std::string>& getContext() { return context; }
	void resetWriter() { contextWriter.str(""); }
	std::stringstream& getWriter() { return contextWriter; }
	StateContainerItemStack<WebGLRenderMode>& getRenderMode() { return renderMode; }

	static WebGLState& get(const GraphicsContext& context) { return context.getRenderState()->getItem<WebGLState>(); }

private:
	StateContainerItemStack<std::string> context;
	std::stringstream contextWriter;
	StateContainerItemStack<WebGLRenderMode> renderMode;
};

}

#endif
