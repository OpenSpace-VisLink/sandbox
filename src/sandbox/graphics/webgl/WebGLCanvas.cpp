#include "sandbox/graphics/webgl/WebGLCanvas.h"
#include "sandbox/graphics/webgl/WebGLState.h"

namespace sandbox {

void WebGLCanvas::updateInstance(WebGLInstance* instance) {
	std::string canvasObject = ""
		"function WebGLCanvas(name) {"
			"this.canvas = document.querySelector(name);"
			"this.gl = this.canvas.getContext('experimental-webgl');"
			"this.initialized = false;"
			"if (!this.gl) {"
				"alert('Unable to initialize WebGL. Your browser may not support it.');"
			"}"
		"}"
		"console.log('obj');";
	instance->addFunction("WebGLCanvas", "constructor", canvasObject);

	std::string renderFunction = ""
		"WebGLCanvas.prototype.renderScene = function(gl) {};"
		"WebGLCanvas.prototype.renderUpdate = function(gl) {};"
		"WebGLCanvas.prototype.render = function() {"
			"if (!this.initialized) { this.renderUpdate(this.gl); this.initialized = true; }"
			"this.renderScene(this.gl);"
		"};"
		"console.log('render2');";

	instance->addFunction("WebGLCanvas", "render", renderFunction);
	instance->addVariable("contexts", "var contexts = {};");
	instance->addCode(context + " = new WebGLCanvas('" + name + "');");

	//instance->addCode(context + ".render = " + renderFunction2);

	//instance->addCode(context + ".render();");
}

void WebGLCanvas::startRender(const GraphicsContext& context) {
	WebGLState& state = WebGLState::get(context);
	state.getContext().push(this->context);
	std::stringstream& ss = state.getWriter();

	if (state.getRenderMode().get() == WEBGL_RENDER_UPDATE) {
		/*std::string renderUpdate = ""
			"function(gl) {"
				"gl.clearDepth(1.0);"
				"gl.enable(gl.DEPTH_TEST);"
				"gl.depthFunc(gl.LEQUAL);"
				"gl.clearColor(0.5, 0.0, 0.0, 1.0);"
				"gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);";*/
		ss << this->context << ".renderUpdate = function(gl) {";
	}
	else if (state.getRenderMode().get() == WEBGL_RENDER_SCENE) {
		ss << this->context << ".renderScene = function(gl) {";
	}
	else if (state.getRenderMode().get() == WEBGL_RENDER) {
		ss << this->context << ".render();";
	}
	else if (state.getRenderMode().get() == WEBGL_UPDATE) {
		ss << this->context << ".renderUpdate();";
	}

}

void WebGLCanvas::finishRender(const GraphicsContext& context) {
	WebGLState& state = WebGLState::get(context);
	if (state.getRenderMode().get() == WEBGL_RENDER_UPDATE || state.getRenderMode().get() == WEBGL_RENDER_SCENE) {
		std::stringstream& ss = state.getWriter();
		ss << "};";
	}
	state.getContext().pop();
	//std::stringstream& ss = state.getWriter() << "};"
}

}