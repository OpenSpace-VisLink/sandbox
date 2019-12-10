#ifndef SANDBOX_INPUT_INTERACTION_TOUCH_SCALE_H_
#define SANDBOX_INPUT_INTERACTION_TOUCH_SCALE_H_

#include "glm/glm.hpp"
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "sandbox/base/Transform.h"
#include "sandbox/Component.h"
#include "sandbox/input/TouchInput.h"

namespace sandbox {

class TouchScale : public Component {
public:
	TouchScale(Entity* input) : input(input), touch(NULL), transform(NULL) { addType<TouchTranslate>(); }
	virtual ~TouchScale() {}

	void update() {
		if (!touch) {
			touch = input->getComponent<TouchInput>();
		}

		if (!transform) {
			transform = getEntity().getComponent<Transform>();
		}

		if (touch) {
			//std::cout << touch->getTouchState(0) << " " << touch->getPosition(0).x << " " << touch->getPosition(0).y << std::endl;
			if (touch->getTouchState(0) && touch->getTouchState(1) && (checkChanged(0) || checkChanged(1))) {
				float dist1 = glm::length(touch->getLastPosition(1)-touch->getLastPosition(0));
				float dist2 = glm::length(touch->getPosition(1)-touch->getPosition(0));
				glm::mat4 translate = glm::scale(glm::mat4(1.0f), glm::vec3(dist2/dist1));
				transform->setTransform(translate*transform->getTransform());
			}
		}
	}

	bool forceUpdate() {
		return true;
	}

private:
	bool checkChanged(int index) {
		return touch->getTouchState(index) && glm::length(touch->getPosition(index)-touch->getLastPosition(index)) > 0.0f;
	}

	Entity* input;
	TouchInput* touch;
	Transform* transform;
};


}

#endif