#include "sandbox/graphics/webgl/WebGLCode.h"

namespace sandbox {

void WebGLCode::startRender(const GraphicsContext& context) {
	WebGLState& state = WebGLState::get(context);
	std::stringstream& ss = state.getWriter();

	if (state.getRenderMode().get() == WEBGL_RENDER_UPDATE) {
		ss << renderContext;
	}
	else if (state.getRenderMode().get() == WEBGL_RENDER_SCENE) {
		ss << renderScene;
	}
}

}