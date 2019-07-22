#ifndef SANDBOX_BASE_TRANSFORM_H_
#define SANDBOX_BASE_TRANSFORM_H_

#include "glm/glm.hpp"
#include "sandbox/Component.h"

namespace sandbox {

class Transform : public Component {
public:
	Transform();
	Transform(const glm::mat4& transform);
	virtual ~Transform();

	const glm::mat4& getTransform() const { return transform; }
	void setTransform(const glm::mat4& transform) { this->transform = transform; }

private:
	glm::mat4 transform;
};

}

#endif