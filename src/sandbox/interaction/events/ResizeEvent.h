#ifndef SANDBOX_INTERACTION_EVENTS_RESIZE_EVENT_H_
#define SANDBOX_INTERACTION_EVENTS_RESIZE_EVENT_H_

#include "sandbox/interaction/Event.h"

namespace sandbox {

struct ResizeState {
	ResizeState() : width(0), height(0) {}
	unsigned int width;
	unsigned int height;
};

/*class ResizeEvent : public BaseEvent<ResizeState> {
public:
	ResizeEvent() {
		addType<ResizeEvent>();
	}

	virtual ~ResizeEvent() {}
};*/

typedef BaseEvent<ResizeState> ResizeEvent;
typedef StateEventHandler<ResizeState> ResizeEventHandler;

}

#endif