#include "sandbox/input/interaction/MouseTranslate.h"
#include <cmath>
#include <glm/gtc/quaternion.hpp>
#include <iostream>

namespace sandbox {

void MouseTranslate::update() {
	if (!mouse) {
		mouse = input->getComponent<MouseInput>();
	}

	if (!transform) {
		transform = getEntity().getComponent<Transform>();
	}

	if (mouse) {
		if (mouse->getButtonState(2) && glm::length(mouse->getPosition()-mouse->getLastPosition()) > 0.0f) {
			glm::vec2 p1 = mouse->getLastPosition();
			glm::vec2 p2 = mouse->getPosition();
			glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(p2-p1,0.0));
			transform->setTransform(translate*transform->getTransform());
		}
	}
}

}