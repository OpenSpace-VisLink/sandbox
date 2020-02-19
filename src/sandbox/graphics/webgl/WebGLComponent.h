#ifndef SANDBOX_GRAPHICS_WEBGL_WEBGLCOMPONENT_H_
#define SANDBOX_GRAPHICS_WEBGL_WEBGLCOMPONENT_H_

#include "sandbox/Component.h"
#include "sandbox/graphics/webgl/WebGLInstance.h"

namespace sandbox {

class WebGLInstance;

class WebGLComponent : public Component {
public:
	WebGLComponent() { addType<WebGLComponent>(); }
	virtual ~WebGLComponent() {}

	virtual void updateInstance(WebGLInstance* instance) {}
};




}

#endif