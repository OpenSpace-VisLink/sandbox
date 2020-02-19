#ifndef SANDBOX_INPUT_INTERACTION_TOUCH_TRANSLATE_H_
#define SANDBOX_INPUT_INTERACTION_TOUCH_TRANSLATE_H_

#include "glm/glm.hpp"
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "sandbox/base/Transform.h"
#include "sandbox/Component.h"
#include "sandbox/input/TouchInput.h"

namespace sandbox {

class TouchTranslate : public Component {
public:
	TouchTranslate(Entity* input) : input(input), touch(NULL), transform(NULL) { addType<TouchTranslate>(); }
	virtual ~TouchTranslate() {}

	void update() {
		if (!touch) {
			touch = input->getComponent<TouchInput>();
		}

		if (!transform) {
			transform = getEntity().getComponent<Transform>();
		}

		if (touch) {
			//std::cout << touch->getTouchState(0) << " " << touch->getPosition(0).x << " " << touch->getPosition(0).y << std::endl;
			if (touch->getTouchState(0) && glm::length(touch->getPosition(0)-touch->getLastPosition(0)) > 0.0f && !touch->getTouchState(1)) {
				glm::vec2 p1 = touch->getLastPosition(0);
				glm::vec2 p2 = touch->getPosition(0);
				glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(p2-p1,0.0));
				transform->setTransform(translate*transform->getTransform());
			}
		}
	}

	bool forceUpdate() {
		return true;
	}

private:
	Entity* input;
	TouchInput* touch;
	Transform* transform;
};


}

#endif