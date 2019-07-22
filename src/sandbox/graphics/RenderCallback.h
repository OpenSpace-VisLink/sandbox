#ifndef SANDBOX_GRAPHICS_RENDER_CALLBACK_H_
#define SANDBOX_GRAPHICS_RENDER_CALLBACK_H_

#include "sandbox/graphics/GraphicsComponent.h"

namespace sandbox {

template<typename T>
class RenderCallback : public GraphicsComponent {
public:
	RenderCallback() : parameter(nullptr) {
		addType< RenderCallback<T> >();
	}
	virtual ~RenderCallback() {}

	void startRender(const GraphicsContext& context) {
		renderCallback(context, parameter);
	}

	Component* init(T* parameter) { setParameter(parameter); return this; }
	void setParameter(T* parameter) { this->parameter = parameter; }

protected:
	virtual void renderCallback(const GraphicsContext& context, T* parameter) = 0;
	

private:
	T* parameter;
};

}

#endif