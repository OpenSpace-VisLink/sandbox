#include "sandbox/input/interaction/VirtualCursor.h"
#include <cmath>
#include <glm/gtc/quaternion.hpp>

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
		float scaleFactor = 2.0f;
		
		if (mouse->getButtonState(1) && glm::length(mouse->getPosition()-mouse->getLastPosition()) > 0.0f) {
			glm::vec2 p1 = mouse->getLastPosition();
			glm::vec2 p2 = mouse->getPosition();
			glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(p2.x-p1.x,0.0,(p1.y-p2.y))*scaleFactor);
			transform->setTransform(translate*transform->getTransform());
		} else if (glm::length(mouse->getPosition()-mouse->getLastPosition()) > 0.0f) {
			glm::vec2 p1 = mouse->getLastPosition();
			glm::vec2 p2 = mouse->getPosition();
			glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(p2-p1,0.0)*scaleFactor);
			transform->setTransform(translate*transform->getTransform());
		}
		
	}
}

}