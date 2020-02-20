#ifndef SANDBOX_GRAPHICS_WEBGL_WEBGLCODE_H_
#define SANDBOX_GRAPHICS_WEBGL_WEBGLCODE_H_

#include "sandbox/graphics/webgl/WebGLComponent.h"
#include "sandbox/graphics/webgl/WebGLState.h"

namespace sandbox {

class WebGLCode : public WebGLComponent {
public:
	WebGLCode(const std::string& renderContext, const std::string& renderScene) : renderContext(renderContext), renderScene(renderScene) { 
		addType<WebGLCode>();
	}
	virtual ~WebGLCode() {}

	void startRender(const GraphicsContext& context);

private:
	std::string renderContext;
	std::string renderScene;
};

}

#endif