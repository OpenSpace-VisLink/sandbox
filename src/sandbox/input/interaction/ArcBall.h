#ifndef SANDBOX_INPUT_INTERACTION_ARCBALL_H_
#define SANDBOX_INPUT_INTERACTION_ARCBALL_H_

#include "glm/glm.hpp"
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "sandbox/base/Transform.h"
#include "sandbox/Component.h"
#include "sandbox/input/MouseInput.h"

namespace sandbox {

class ArcBall : public Component {
public:
	ArcBall(Entity* input) : input(input), mouse(NULL), transform(NULL), dragging(false) { addType<ArcBall>(); }
	virtual ~ArcBall() {}

	void update() {
		if (!mouse) {
			mouse = input->getComponent<MouseInput>();
		}

		if (!transform) {
			transform = getEntity().getComponent<Transform>();
		}

		if (mouse) {
			bool drag = mouse->isDragging();
			if (drag && !dragging) {
				prevTransform = transform->getTransform();
			}
			dragging = drag;
			if (drag) {
				transform->setTransform(glm::translate(prevTransform,glm::vec3(mouse->getPosition()-mouse->getDragStartPosition(),0)));
			}
		}
	}

	bool forceUpdate() {
		return true;
	}

private:
	Entity* input;
	MouseInput* mouse;
	Transform* transform;
	bool dragging;
	glm::mat4 prevTransform;
};


}

#endif