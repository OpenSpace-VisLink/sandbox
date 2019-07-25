#ifndef SANDBOX_INPUT_NANOGUI_MOUSE_INPUT_H_
#define SANDBOX_INPUT_NANOGUI_MOUSE_INPUT_H_

#include "glm/glm.hpp"
#include "sandbox/input/MouseInput.h"
#include <nanogui/nanogui.h>
#include <iostream>

namespace sandbox {

template<typename T>
class NanoGUIMouseInput : public MouseInput {
public:
	NanoGUIMouseInput(T* app) : app(app) {
	}
	bool getButtonState(int button) const { return app->mMouseState & (1 << button) && !app->mDragActive; }
	glm::vec2 getPosition() const { 
		float x = 2.0f*app->mousePos()[0]/app->width()-1.0;
		float y = 1.0-2.0f*app->mousePos()[1]/app->height();
		//x = (1.0+x)/2.0;
		//y = (1.0+y)/2.0;
		return glm::vec2(x, y);
	}
	bool isDragging() const { return app->mMouseState || app->mDragActive; }

private:
	T* app;


};


}

#endif