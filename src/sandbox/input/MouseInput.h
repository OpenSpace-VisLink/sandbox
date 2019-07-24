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

template<typename T>
class NanoGUIMouseInput : public MouseInput {
public:
	NanoGUIMouseInput(T* app) : app(app) {}
	bool getButtonState(int button) const { return app->mMouseState & (1 << button) && !app->mDragActive; }
	glm::vec2 getPosition() const { 
		float x = 2.0f*app->mousePos()[0]/app->width()-1.0;
		float y = 1.0-2.0f*app->mousePos()[1]/app->height();
		x = (1.0+x)/2.0;
		y = (1.0+y)/2.0;
		return glm::vec2(x, y);
	}
	bool isDragging() const { return app->mMouseState || app->mDragActive; }

private:
	T* app;

};

}

#endif