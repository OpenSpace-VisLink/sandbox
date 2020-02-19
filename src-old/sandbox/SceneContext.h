#ifndef SANDBOX_SCENE_CONTEXT_H_
#define SANDBOX_SCENE_CONTEXT_H_

#include "sandbox/Context.h"
#include "sandbox/SceneState.h"

namespace sandbox {

class SceneContext {
public:
	SceneContext() : sharedContext(new Context()), context(new Context()), renderState(new SceneState()) {}
	SceneContext(Context* sharedContext, Context* contex) : sharedContext(sharedContext), context(context), renderState(new SceneState()) {}
	virtual ~SceneContext() {
		delete sharedContext;
		delete context;
		delete renderState;
	}

	Context* getSharedContext() const { return sharedContext; }
	Context* getContext() const { return context; }
	SceneState* getRenderState() const { return renderState; }

private:
	Context* sharedContext;
	Context* context;
	SceneState* renderState;
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