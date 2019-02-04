#ifndef SANDBOX_SCENE_CONTEXT_H_
#define SANDBOX_SCENE_CONTEXT_H_

#include "sandbox/Context.h"

namespace sandbox {

class SceneContext {
public:
	SceneContext(Context* sharedContext, Context* contex) : sharedContext(sharedContext), context(context) {}
	virtual ~SceneContext() {
		delete sharedContext;
		delete context; 
	}

	Context* getSharedContext() const { return sharedContext; }
	Context* getContext() const { return context; }

private:
	Context* sharedContext;
	Context* context;
};

template<typename SharedStateType, typename StateType>
class SceneContextHandler {
public:
	SharedStateType* getSharedState(const SceneContext& sceneContext) const {
		ContextState* state = sceneContext.getSharedContext()->getState(this);
		if (!state) {
			state = new SharedStateType();
			sceneContext.getSharedContext()->setState(this, state);
		}

		return static_cast<SharedStateType*>(state);
	}

	StateType* getState(const SceneContext& sceneContext) const {
		ContextState* state = sceneContext.getContext()->getState(this);
		if (!state) {
			state = new StateType();
			sceneContext.getContext()->setState(this, state);
		}

		return static_cast<StateType*>(state);
	}
};

}

#endif