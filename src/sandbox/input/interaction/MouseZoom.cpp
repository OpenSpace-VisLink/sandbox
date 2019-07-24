#include "sandbox/input/interaction/MouseZoom.h"
#include <cmath>
#include <glm/gtc/quaternion.hpp>
#include <iostream>

namespace sandbox {

void MouseZoom::update() {
	if (!mouse) {
		mouse = input->getComponent<MouseInput>();
	}

	if (!transform) {
		transform = getEntity().getComponent<Transform>();
	}

	if (mouse) {
		if (mouse->getButtonState(1) && glm::length(mouse->getPosition().y-mouse->getLastPosition().y) > 0.0f) {
			glm::vec2 p1 = mouse->getLastPosition();
			glm::vec2 p2 = mouse->getPosition();
			bool zoomIn = p2.y-p1.y > 0.0f;
			float scaleRate = 1.0+1.0*std::abs(p2.y-p1.y);
			scaleRate = zoomIn ? scaleRate : 1.0f/scaleRate;
			glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(scaleRate));
			transform->setTransform(transform->getTransform()*scale);
		}
	}
}

}