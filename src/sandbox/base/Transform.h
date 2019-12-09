#ifndef SANDBOX_BASE_TRANSFORM_H_
#define SANDBOX_BASE_TRANSFORM_H_

#include "glm/glm.hpp"
#include "sandbox/graphics/RenderObject.h"
#include <iostream>

namespace sandbox {

class TransformState : public StateContainerItem {
public:
	TransformState() {
		calculateTransform = false;
		transform.set(glm::mat4(1.0f));
	}

	virtual ~TransformState() {}

	StateContainerItemStack<glm::mat4>& getTransform() { return transform; }

	static TransformState& get(const GraphicsContext& context) { return context.getRenderState()->getItem<TransformState>(); }

	bool calculateTransform;

private:
	StateContainerItemStack<glm::mat4> transform;

};

class Transform : public RenderObject {
public:
	Transform();
	Transform(const glm::mat4& transform);
	virtual ~Transform();

	const glm::mat4& getTransform() const { return transform; }
	void setTransform(const glm::mat4& transform) { this->transform = transform; getEntity().incrementVersion(); }

	glm::vec3 getLocation() const;

	virtual void startRender(const GraphicsContext& context) {
		TransformState& state = TransformState::get(context);
		if (state.calculateTransform) {
			state.getTransform().push(state.getTransform().get()*transform);
			std::cout << "begin Transform " << std::endl;
		}
	}

	virtual void finishRender(const GraphicsContext& context) {
		TransformState& state = TransformState::get(context);
		if (state.calculateTransform) {
			TransformState::get(context).getTransform().pop();
			std::cout << "end Transform " << std::endl;
		}
	}

private:
	glm::mat4 transform;
};

}

#endif