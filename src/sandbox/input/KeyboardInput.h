#ifndef SANDBOX_INPUT_KEYBOARD_INPUT_H_
#define SANDBOX_INPUT_KEYBOARD_INPUT_H_

#include "glm/glm.hpp"
#include "sandbox/Component.h"

namespace sandbox {

class KeyboardInput : public Component {
public:
	KeyboardInput() { addType<KeyboardInput>(); }
	virtual ~KeyboardInput() {} 

	virtual bool getKeyState(char key) const = 0;
	virtual bool getLastKeyState(char key) const = 0;

private:
	bool a;
};


}

#endif