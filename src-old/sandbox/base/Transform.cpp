#include "sandbox/base/Transform.h"
#include "sandbox/graphics/RenderState.h"

namespace sandbox {


Transform::Transform() : transform(1.0f) {
	addType<Transform>();
}

Transform::Transform(const glm::mat4& transform) : transform(transform) {
	addType<Transform>();
}

Transform::~Transform() {

}

void Transform::render(const SceneContext& sceneContext) {
	RenderState& renderState = RenderState::get(sceneContext);
	renderState.getModelMatrix().push(renderState.getModelMatrix().get()*transform);
}

void Transform::finishRender(const SceneContext& sceneContext) {
	RenderState& renderState = RenderState::get(sceneContext);
	renderState.getModelMatrix().pop();
}

}