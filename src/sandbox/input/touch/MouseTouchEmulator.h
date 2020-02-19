#ifndef SANDBOX_INPUT_TOUCH_MOUSE_TOUCH_EMULATOR_H_
#define SANDBOX_INPUT_TOUCH_MOUSE_TOUCH_EMULATOR_H_

#include "glm/glm.hpp"
#include "sandbox/input/MouseInput.h"
#include "sandbox/input/TouchInput.h"
#include <vector>

namespace sandbox {

class MouseTouchEmulator : public TouchInput {
public:
	MouseTouchEmulator() : TouchInput(3), mouse(NULL) {
	}
	virtual ~MouseTouchEmulator() {} 

	virtual bool getTouchState(int id) const { return mouse ? mouse->getButtonState(id) : false; }
	virtual glm::vec2 getPosition(int id) const { return mouse ? mouse->getPosition() : glm::vec2(0.0f); }

	void update() {
		if (!mouse) {
			mouse = getEntity().getComponent<MouseInput>();
		}

		if (mouse) {
			TouchInput::update();
		}
	}

private:
	MouseInput* mouse;
};


}

#endif