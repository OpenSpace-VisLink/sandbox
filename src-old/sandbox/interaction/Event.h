#ifndef SANDBOX_INTERACTION_EVENT_H_
#define SANDBOX_INTERACTION_EVENT_H_

#include "sandbox/interaction/EventHandler.h"
#include "sandbox/SceneComponent.h"
#include <vector>
#include <algorithm>

namespace sandbox {

class Event : public SceneComponent {
public:
	Event() {
		addType<Event>();
	}
	
	virtual ~Event() {}

	void unsubscribe(EventHandler* handler) {
		std::vector<EventHandler*>::iterator it = std::find(handlers.begin(), handlers.end(), handler);
		if (it != handlers.end()) {
			handlers.erase(it);
		}
	}

protected:
	void subscribe(EventHandler* handler) {
		handlers.push_back(handler);
	}

	void notify() {
		for (int f = 0; f < handlers.size(); f++) {
			handlers[f]->onEvent();
		}
	}

private:
	std::vector<EventHandler*> handlers;
};

template<typename STATE>
class StateEvent : public Event {
public:
	StateEvent() {
		addType<StateEvent<STATE> >();
	}
	virtual ~StateEvent() {}

	virtual void onEvent(const STATE& state) {
		this->state = state;
		updateVersion();
		notify();
	}

	const STATE& getState() const { return state; }

protected:
	STATE state;
};

template<typename STATE>
class StateEventHandler : public EventHandler {
public:
	StateEventHandler() : event(NULL) {}
	virtual ~StateEventHandler() {}

	void onEvent() {
		if (event) {
			onEvent(event->getState());	
		}
	}

	virtual void onEvent(const STATE& state) = 0;

	void setEvent(StateEvent<STATE>* event) { this->event = event; }

private:
	StateEvent<STATE>* event;
};

template<typename STATE>
class BaseEvent : public StateEvent<STATE> {
public:
	BaseEvent() {
		SceneComponent::addType<BaseEvent<STATE> >();
	}
	virtual ~BaseEvent() {}

	void subscribe(StateEventHandler<STATE>* handler) {
		handler->setEvent(this);
		Event::subscribe(handler);
	}
};


template<typename STATE, typename PARAM>
class StateEventHandlerCallback : public StateEventHandler<STATE> {
public:
	StateEventHandlerCallback() : parameter(nullptr) {
	}
	virtual ~StateEventHandlerCallback() {}

	void onEvent(const STATE& state) {
		onEvent(state, parameter);
	}

	StateEventHandlerCallback<STATE, PARAM>* init(PARAM* parameter) { setParameter(parameter); return this; }
	void setParameter(PARAM* parameter) { this->parameter = parameter; }

protected:
	virtual void onEvent(const STATE& state, PARAM* parameter) = 0;

private:
	PARAM* parameter;
};

}

#endif