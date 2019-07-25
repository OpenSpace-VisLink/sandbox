#ifndef SANDBOX_INPUT_INTERACTION_MOUSE_ZOOM_H_
#define SANDBOX_INPUT_INTERACTION_MOUSE_ZOOM_H_

#include "glm/glm.hpp"
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "sandbox/base/Transform.h"
#include "sandbox/Component.h"
#include "sandbox/input/MouseInput.h"
#include "sandbox/input/CursorInput.h"

namespace sandbox {

class VirtualCursor : public CursorInput {
public:
	VirtualCursor(Entity* input) : input(input), mouse(NULL), transform(NULL), initialized(false) { addType<VirtualCursor>(); }
	virtual ~VirtualCursor() {}

	virtual bool getButtonState(int button) const { return mouse->getButtonState(button); }
	virtual glm::mat4 getTransform() const = 0;

	void prepareUpdate();

	void update();

	bool forceUpdate() {
		return true;
	}

private:
	Entity* virtualCursorNode;
	Entity* input;
	MouseInput* mouse;
	Transform* transform;
	bool initialized;
};


}

#endif