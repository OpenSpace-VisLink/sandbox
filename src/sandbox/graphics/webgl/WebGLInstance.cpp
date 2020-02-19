#include "sandbox/graphics/webgl/WebGLInstance.h"
#include "sandbox/graphics/webgl/WebGLComponent.h"

namespace sandbox {

void WebGLInstance::update() {
	std::vector<WebGLComponent*> components = getEntity().getComponentsRecursive<WebGLComponent>();
	for (int f = 0; f < components.size(); f++) {
		components[f]->updateInstance(this);

	}
}


void WebGLInstance::addFunction(const std::string objectName, const std::string& functionName, const std::string& impl) {
	std::string key = objectName + "." + functionName;
	std::map<std::string, std::string>::iterator it = functions.find(key);
	if (it == functions.end()) {
		functions[key] = impl;
	}
}

const std::string& WebGLInstance::getImplementation() const {
	return impl;
}

}