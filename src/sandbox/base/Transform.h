#ifndef SANDBOX_BASE_TRANSFORM_COMPONENT_H_
#define SANDBOX_BASE_TRANSFORM_COMPONENT_H_

#include "glm/glm.hpp"
#include "sandbox/SceneComponent.h"

namespace sandbox {

class Transform : public SceneComponent {
public:
	Transform();
	Transform(const glm::mat4& transform);
	virtual ~Transform();

	void render(const SceneContext& sceneContext);
	void finishRender(const SceneContext& sceneContext);

	const glm::mat4& getTransform() const { return transform; }
	void setTransform(const glm::mat4& transform) { this->transform = transform; }

private:
	glm::mat4 transform;
};

}

#endif