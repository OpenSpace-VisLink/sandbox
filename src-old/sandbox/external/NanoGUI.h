#ifndef SANDBOX_EXTERNAL_NANOGUI_H_
#define SANDBOX_EXTERNAL_NANOGUI_H_


#include <nanogui/nanogui.h>
#include "sandbox/interaction/events/ResizeEvent.h"

namespace sandbox {

class NanoguiScreen : public nanogui::Screen {
	friend class NanoguiResizeEvent;
public:
	NanoguiScreen(Eigen::Vector2i size, const std::string& name) : nanogui::Screen(size, name), resize(NULL) {
	}

	bool resizeEvent(const Eigen::Vector2i& size) {
		if (resize) {
			ResizeState state;
			state.width = size[0];
			state.height = size[1];
			resize->onEvent(state);
		}

		return true;
	}

private:
	ResizeEvent* resize;

};

class NanoguiResizeEvent : public ResizeEvent {
public:
	NanoguiResizeEvent(NanoguiScreen* screen) : screen(screen) {
		screen->resize = this;
	}
	~NanoguiResizeEvent() {
		screen->resize = NULL;
	}

private:
	NanoguiScreen* screen;
};

}

#endif