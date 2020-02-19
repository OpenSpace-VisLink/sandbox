#include "sandbox/base/Transform.h"

namespace sandbox {


Transform::Transform() : transform(1.0f) {
	addType<Transform>();
}

Transform::Transform(const glm::mat4& transform) : transform(transform) {
	addType<Transform>();
}

Transform::~Transform() {

}

glm::vec3 Transform::getLocation() const {
	glm::vec4 position(0.0f, 0.0f, 0.0f, 1.0f);
	for (const Entity* node = &getEntity(); node != NULL; node = node->getParent() ) {
		std::vector<Transform*> transforms = node->getComponents<Transform>();
		for (int f = transforms.size()-1; f >=0; f--) {
			position = transforms[f]->getTransform()*position;
		}
	}
	return position;
}

/*void Transform::render(const SceneContext& sceneContext) {
	RenderState& renderState = RenderState::get(sceneContext);
	renderState.getModelMatrix().push(renderState.getModelMatrix().get()*transform);
}

void Transform::finishRender(const SceneContext& sceneContext) {
	RenderState& renderState = RenderState::get(sceneContext);
	renderState.getModelMatrix().pop();
}*/

}