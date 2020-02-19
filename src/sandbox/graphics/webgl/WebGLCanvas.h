#ifndef SANDBOX_GRAPHICS_WEBGL_WEBGLCANVAS_H_
#define SANDBOX_GRAPHICS_WEBGL_WEBGLCANVAS_H_

#include "sandbox/graphics/webgl/WebGLComponent.h"
#include "sandbox/graphics/webgl/WebGLState.h"
#include "sandbox/graphics/RenderNode.h"
#include "sandbox/graphics/GraphicsRenderer.h"

namespace sandbox {

class WebGLCanvas : public WebGLComponent {
public:
	WebGLCanvas(const std::string& name) : name(name) { 
		addType<WebGLCanvas>();
		context = "contexts['" + name + "']";
	}
	virtual ~WebGLCanvas() {}

	void updateInstance(WebGLInstance* instance);

	void startRender(const GraphicsContext& context);
	void finishRender(const GraphicsContext& context);

private:
	std::string name;
	std::string context;
};


class WebGLGraphicsRenderer : public GraphicsRenderer {
public:
	WebGLGraphicsRenderer(GraphicsContext* context = NULL) : GraphicsRenderer(context) {
		addType<WebGLGraphicsRenderer>();
	}

	std::string render(WebGLRenderMode renderMode) {
		WebGLState& state = WebGLState::get(getContext());
		state.resetWriter();
		state.getRenderMode().push(renderMode);
		GraphicsRenderer::render();
		state.getRenderMode().pop();
		return state.getWriter().str();
	}
};

}

#endif