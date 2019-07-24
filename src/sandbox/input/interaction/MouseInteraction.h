#ifndef SANDBOX_INPUT_INTERACTION_MOUSE_INTERACTION_H_
#define SANDBOX_INPUT_INTERACTION_MOUSE_INTERACTION_H_

#include "sandbox/Entity.h"
#include "sandbox/base/Transform.h"
#include "sandbox/input/MouseInput.h"
#include "sandbox/input/interaction/ArcBall.h"
#include "sandbox/input/interaction/MouseZoom.h"
#include "sandbox/input/interaction/MouseTranslate.h"

namespace sandbox {

class MouseInteraction : public EntityNode {
public:
	MouseInteraction(Entity* input) : EntityNode() { init(input); }
	MouseInteraction(Entity* parent, Entity* input) : EntityNode(parent) { init(input); }

	void init(Entity* input) {
		add(this, input);
	}

	static void add(Entity* entity, Entity* input) {
		entity->addComponent(new Transform());
		entity->addComponent(new ArcBall(input));
		entity->addComponent(new MouseZoom(input));
		entity->addComponent(new MouseTranslate(input));
	}

};


}

#endif