#include "sandbox/input/interaction/VirtualCursor.h"
#include <cmath>
#include <glm/gtc/quaternion.hpp>
#include <iostream>

namespace sandbox {

void VirtualCursor::afterAdd() {
	virtualCursorNode = new EntityNode(&getEntity());
	transform = new Transform();
	virtualCursorNode->addComponent(transform);
}

void VirtualCursor::update() {
	if (!mouse) {
		mouse = input->getComponent<MouseInput>();
	}

	if (mouse) {
		//if (mouse->getButtonState(2) && glm::length(mouse->getPosition()-mouse->getLastPosition()) > 0.0f) {
			glm::vec2 p1 = mouse->getLastPosition();
			glm::vec2 p2 = mouse->getPosition();
			glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(p2-p1,0.0));
			transform->setTransform(translate*transform->getTransform());
		//}
	}
}

}