#include "sandbox/graphics/webgl/WebGLInstance.h"
#include "sandbox/graphics/webgl/WebGLComponent.h"

namespace sandbox {

void WebGLInstance::update() {
	uniqueCode.clear();
	this->impl = "";
	std::vector<WebGLComponent*> components = getEntity().getComponentsRecursive<WebGLComponent>();
	for (int f = 0; f < components.size(); f++) {
		components[f]->updateInstance(this);
	}
}

void WebGLInstance::addVariable(const std::string objectName, const std::string& impl) {
	std::string key = objectName;
	std::map<std::string, std::string>::iterator it = uniqueCode.find(key);
	if (it == uniqueCode.end()) {
		uniqueCode[key] = impl;
		this->impl += impl;
	}
}

void WebGLInstance::addFunction(const std::string objectName, const std::string& functionName, const std::string& impl) {
	std::string key = objectName + "." + functionName;
	std::map<std::string, std::string>::iterator it = uniqueCode.find(key);
	if (it == uniqueCode.end()) {
		uniqueCode[key] = impl;
		this->impl += impl;
	}
}

void WebGLInstance::addCode(const std::string& impl) {
	this->impl += impl;
}


const std::string& WebGLInstance::getImplementation() const {
	return impl;
}

}