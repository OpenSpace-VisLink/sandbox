#ifndef SANDBOX_INPUT_TOUCH_INPUT_H_
#define SANDBOX_INPUT_TOUCH_INPUT_H_

#include "glm/glm.hpp"
#include "sandbox/Component.h"
#include <vector>

namespace sandbox {

class TouchInput : public Component {
public:
	TouchInput(int maxTouchPoints) : maxTouchPoints(maxTouchPoints) {
		addType<TouchInput>();
		pos.resize(maxTouchPoints);
		lastPosition.resize(maxTouchPoints);
		state.resize(maxTouchPoints);
	}
	virtual ~TouchInput() {} 

	virtual bool getTouchState(int id) const = 0;
	virtual glm::vec2 getPosition(int id) const = 0;
	
	glm::vec2 getLastPosition(int id) const { return lastPosition[id]; }

	void update() {
		for (int f = 0; f < maxTouchPoints; f++) {
			lastPosition[f] = pos[f];
			pos[f] = getPosition(f);
			bool newState = getTouchState(f);
			if (newState != state[f]) {
				lastPosition[f] = pos[f];
			}
			state[f] = getTouchState(f);
		}
	}

	bool forceUpdate() { return true; }

private:
	std::vector<glm::vec2> pos;
	std::vector<glm::vec2> lastPosition;
	std::vector<bool> state;
	int maxTouchPoints;
};


}

#endif