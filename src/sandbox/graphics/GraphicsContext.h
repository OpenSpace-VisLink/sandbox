#ifndef SANDBOX_GRAPHICS_CONTEXT_H_
#define SANDBOX_GRAPHICS_CONTEXT_H_

#include "sandbox/Context.h"
#include "sandbox/StateContainer.h"

namespace sandbox {

class GraphicsContext {
public:
	GraphicsContext() : sharedContext(new Context()), context(new Context()), renderState(new StateContainer()), deleteSharedContext(true), deleteContext(true) {}
	GraphicsContext(Context* sharedContext, Context* context, bool deleteSharedContext = true, bool deleteContext = true) : sharedContext(sharedContext), context(context), renderState(new StateContainer()), deleteSharedContext(deleteSharedContext), deleteContext(deleteContext) {}
	virtual ~GraphicsContext() {
		if (deleteSharedContext) {
			delete sharedContext;
		}
		if (deleteContext) {
			delete context;	
		}
		delete renderState;
	}

	Context* getSharedContext() const { return sharedContext; }
	Context* getContext() const { return context; }
	StateContainer* getRenderState() const { return renderState; }

private:
	Context* sharedContext;
	Context* context;
	StateContainer* renderState;
	bool deleteSharedContext;
	bool deleteContext;
};

template<typename SharedStateType, typename StateType>
class GraphicsContextHandler {
public:
	SharedStateType* getSharedState(const GraphicsContext& GraphicsContext) const {
		ContextState* state = GraphicsContext.getSharedContext()->getState(this);
		if (!state) {
			state = new SharedStateType();
			GraphicsContext.getSharedContext()->setState(this, state);
		}

		return static_cast<SharedStateType*>(state);
	}

	StateType* getState(const GraphicsContext& GraphicsContext) const {
		ContextState* state = GraphicsContext.getContext()->getState(this);
		if (!state) {
			state = new StateType();
			GraphicsContext.getContext()->setState(this, state);
		}

		return static_cast<StateType*>(state);
	}
};

}

#endif