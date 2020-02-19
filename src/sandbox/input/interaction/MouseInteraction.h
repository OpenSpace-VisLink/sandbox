#ifndef SANDBOX_INPUT_INTERACTION_MOUSE_INTERACTION_H_
#define SANDBOX_INPUT_INTERACTION_MOUSE_INTERACTION_H_

#include "sandbox/Entity.h"
#include "sandbox/base/Transform.h"
#include "sandbox/input/MouseInput.h"
#include "sandbox/input/interaction/ArcBall.h"
#include "sandbox/input/interaction/MouseZoom.h"
#include "sandbox/input/interaction/MouseTranslate.h"

namespace sandbox {

class MouseInteraction : public Component {
public:
	MouseInteraction(Entity* input) : input(input) {}

	bool beforeAdd() { return false; }

	void afterAdd() {
		getEntity().addComponent(new Transform());
		getEntity().addComponent(new ArcBall(input));
		getEntity().addComponent(new MouseZoom(input));
		getEntity().addComponent(new MouseTranslate(input));
	}

private:
	Entity* input;
};


}

#endif