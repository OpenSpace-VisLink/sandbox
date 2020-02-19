#ifndef SANDBOX_INPUT_INTERACTION_MOUSE_TRANSLATE_H_
#define SANDBOX_INPUT_INTERACTION_MOUSE_TRANSLATE_H_

#include "glm/glm.hpp"
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "sandbox/base/Transform.h"
#include "sandbox/Component.h"
#include "sandbox/input/MouseInput.h"

namespace sandbox {

class MouseTranslate : public Component {
public:
	MouseTranslate(Entity* input) : input(input), mouse(NULL), transform(NULL) { addType<MouseTranslate>(); }
	virtual ~MouseTranslate() {}

	void update();

	bool forceUpdate() {
		return true;
	}

private:
	Entity* input;
	MouseInput* mouse;
	Transform* transform;
};


}

#endif