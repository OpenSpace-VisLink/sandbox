#ifndef SANDBOX_INPUT_MOUSE_INPUT_H_
#define SANDBOX_INPUT_MOUSE_INPUT_H_

#include "glm/glm.hpp"
#include "sandbox/Component.h"

namespace sandbox {

class MouseInput : public Component {
public:
	MouseInput() /*:dragging(false)*/ { addType<MouseInput>(); }
	virtual ~MouseInput() {} 

	virtual bool getButtonState(int button) const = 0;
	virtual glm::vec2 getPosition() const = 0;
	//virtual bool isDragging() const = 0;

	//glm::vec2 getDragStartPosition() const { return dragStartPosition; }
	//glm::vec2 getDragEndPosition() const { return dragEndPosition; }
	glm::vec2 getLastPosition() const { return lastPosition; }

	void update() {
//		bool newDrag = isDragging();
//		if (!dragging && newDrag) {
//			dragStartPosition = getPosition();
//		}
//		if (dragging && !newDrag) {
//			dragEndPosition = getPosition();
//		}
//		dragging = newDrag;
		lastPosition = pos;
		pos = getPosition();
	}

	bool forceUpdate() { return true; }

private:
	//bool dragging;
	//glm::vec2 dragStartPosition;
	//glm::vec2 dragEndPosition;
	glm::vec2 pos;
	glm::vec2 lastPosition;
};


}

#endif