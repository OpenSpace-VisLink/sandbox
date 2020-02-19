#ifndef SANDBOX_CONTEXT_H_
#define SANDBOX_CONTEXT_H_

#include <map>

namespace sandbox {

class ContextState {
public: 
	ContextState() : initialized(false) {}
	virtual ~ContextState() {}
	bool initialized;
};

class Context {
public:
	virtual ~Context() {
		for (std::map<const void*, ContextState*>::iterator it = states.begin(); it != states.end(); it++) {
			delete it->second;
		}
	}

	ContextState* getState(const void* ptr) const {
		std::map<const void*, ContextState*>::const_iterator it = states.find(ptr);
		if (it != states.end()) {
			return it->second;
		}

		return NULL;
	}

	void setState(const void* ptr, ContextState* state) {
		states[ptr] = state;
	}

	void deleteState(const void* ptr) {
		std::map<const void*, ContextState*>::iterator it = states.find(ptr);
		if (it != states.end()) {
			delete it->second;
			states.erase(it);
		}
	}

private:
	std::map<const void*, ContextState*> states;
};

}

#endif