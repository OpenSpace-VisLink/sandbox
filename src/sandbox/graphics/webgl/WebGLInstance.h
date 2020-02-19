#ifndef SANDBOX_GRAPHICS_WEBGL_WEBGLINSTANCE_H_
#define SANDBOX_GRAPHICS_WEBGL_WEBGLINSTANCE_H_

#include "sandbox/Component.h"

namespace sandbox {

class WebGLInstance : public Component {
public:
	WebGLInstance() { addType<WebGLInstance>(); }
	virtual ~WebGLInstance() {}

	void update();

	void addVariable(const std::string objectName, const std::string& impl);
	void addFunction(const std::string objectName, const std::string& functionName, const std::string& impl);
	void addCode(const std::string& impl);

	const std::string& getImplementation() const;

private:
	std::map<std::string, std::string> uniqueCode;
	std::string impl;
};




}

#endif