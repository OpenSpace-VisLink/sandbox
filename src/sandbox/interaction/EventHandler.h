#ifndef SANDBOX_INTERACTION_EVENT_HANDLER_H_
#define SANDBOX_INTERACTION_EVENT_HANDLER_H_

#include "sandbox/SceneComponent.h"

namespace sandbox {

class EventHandler : public SceneComponent {
public:
	virtual ~EventHandler() {}

	virtual void onEvent() = 0;
};

}

#endif