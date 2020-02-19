#ifndef SANDBOX_INPUT_CURSOR_INPUT_H_
#define SANDBOX_INPUT_CURSOR_INPUT_H_

#include "glm/glm.hpp"
#include "sandbox/Component.h"

namespace sandbox {

class CursorInput : public Component {
public:
	CursorInput() /*:dragging(false)*/ { addType<CursorInput>(); }
	virtual ~CursorInput() {} 

	virtual bool getButtonState(int button) const = 0;
	virtual glm::mat4 getTransform() const = 0;
	//virtual glm::vec3 getPosition() const = 0;
};

}

#endif