#ifndef SANDBOX_GRAPHICS_CONTEXT_H_
#define SANDBOX_GRAPHICS_CONTEXT_H_

#include "sandbox/Context.h"
#include "sandbox/StateContainer.h"

namespace sandbox {

class GraphicsContext {
public:
	GraphicsContext() : sharedContext(new Context()), context(new Context()), displayContext(new Context()), renderState(new StateContainer()), deleteSharedContext(true), deleteDisplayContext(true), deleteContext(true) {}
	GraphicsContext(Context* sharedContext, Context* context, bool deleteSharedContext = true, bool deleteContext = true) : sharedContext(sharedContext), displayContext(new Context), context(context), renderState(new StateContainer()), deleteSharedContext(deleteSharedContext), deleteContext(deleteContext), deleteDisplayContext(true) {}
	GraphicsContext(Context* sharedContext, Context* displayContext, Context* context, bool deleteSharedContext = true, bool deleteDisplayContext = true, bool deleteContext = true) : sharedContext(sharedContext), displayContext(displayContext), context(context), renderState(new StateContainer()), deleteSharedContext(deleteSharedContext), deleteContext(deleteContext), deleteDisplayContext(deleteDisplayContext) {}
	virtual ~GraphicsContext() {
		if (deleteSharedContext) {
			delete sharedContext;
		}
		if (deleteContext) {
			delete context;	
		}
		if (deleteDisplayContext) {
			delete displayContext;	
		}
		delete renderState;
	}

	Context* getSharedContext() const { return sharedContext; }
	Context* getDisplayContext() const { return displayContext; }
	Context* getContext() const { return context; }
	StateContainer* getRenderState() const { return renderState; }

private:
	Context* sharedContext;
	Context* displayContext;
	Context* context;
	StateContainer* renderState;
	bool deleteSharedContext;
	bool deleteDisplayContext;
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

template<typename SharedStateType, typename DisplayStateType, typename StateType>
class DisplayContextHandler : GraphicsContextHandler<SharedStateType, StateType> {
public:
	DisplayStateType* getDisplayState(const GraphicsContext& GraphicsContext) const {
		ContextState* state = GraphicsContext.getDisplayContext()->getState(this);
		if (!state) {
			state = new DisplayStateType();
			GraphicsContext.getDisplayContext()->setState(this, state);
		}

		return static_cast<DisplayStateType*>(state);
	}
};

}

#endif