#include "sandbox/graphics/webgl/WebGLCanvas.h"
#include "sandbox/graphics/webgl/WebGLState.h"

namespace sandbox {

void WebGLCanvas::updateInstance(WebGLInstance* instance) {
	std::string canvasObject = ""
		"function WebGLCanvas(name) {"
			"this.canvas = document.querySelector(name);"
			"this.gl = this.canvas.getContext('experimental-webgl');"
			"if (!this.gl) {"
				"alert('Unable to initialize WebGL. Your browser may not support it.');"
			"}"
		"}"
		"console.log('obj');";
	instance->addFunction("WebGLCanvas", "constructor", canvasObject);

	std::string renderFunction = ""
		"WebGLCanvas.prototype.render = function() {"
			"this.gl.clearDepth(1.0);"
			"this.gl.enable(this.gl.DEPTH_TEST);"
			"this.gl.depthFunc(this.gl.LEQUAL);"
			"this.gl.clearColor(0.5, 0.5, 0.5, 1.0);"
			"this.gl.clear(this.gl.COLOR_BUFFER_BIT | this.gl.DEPTH_BUFFER_BIT);"
		"};"
		"WebGLCanvas.prototype.renderContext = function() {};"
		"console.log('render');";

	instance->addFunction("WebGLCanvas", "render", renderFunction);
	instance->addVariable("contexts", "var contexts = {};");
	instance->addCode(context + " = new WebGLCanvas('" + name + "');");

	//instance->addCode(context + ".render = " + renderFunction2);

	//instance->addCode(context + ".render();");
}

void WebGLCanvas::startRender(const GraphicsContext& context) {
	WebGLState& state = WebGLState::get(context);

	state.getContext().push(this->context);

	if (state.getRenderMode().get() == WEBGL_RENDER_CONTEXT) {
		std::stringstream& ss = state.getWriter();
		std::string renderFunction2 = ""
			"function() {"
				"this.gl.clearDepth(1.0);"
				"this.gl.enable(this.gl.DEPTH_TEST);"
				"this.gl.depthFunc(this.gl.LEQUAL);"
				"this.gl.clearColor(0.5, 0.0, 0.0, 1.0);"
				"this.gl.clear(this.gl.COLOR_BUFFER_BIT | this.gl.DEPTH_BUFFER_BIT);"
			"};";
		ss << this->context << ".render = " << renderFunction2;
		ss << this->context << ".render();";
	}

}

void WebGLCanvas::finishRender(const GraphicsContext& context) {
	WebGLState& state = WebGLState::get(context);
	state.getContext().pop();
	//std::stringstream& ss = state.getWriter() << "};"
}

}