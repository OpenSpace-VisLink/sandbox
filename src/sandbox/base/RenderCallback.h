#ifndef SANDBOX_GRAPHICS_RENDER_CALLBACK_H_
#define SANDBOX_GRAPHICS_RENDER_CALLBACK_H_

#include "sandbox/SceneComponent.h"

namespace sandbox {

template<typename T>
class RenderCallback : public SceneComponent {
public:
	RenderCallback() : parameter(nullptr) {
		addType< RenderCallback<T> >();
	}
	virtual ~RenderCallback() {}

	void render(const SceneContext& sceneContext) {
		renderCallback(sceneContext, parameter);
	}

	SceneComponent* init(T* parameter) { setParameter(parameter); return this; }
	void setParameter(T* parameter) { this->parameter = parameter; }

protected:
	virtual void renderCallback(const SceneContext& sceneContext, T* parameter) = 0;
	

private:
	T* parameter;
};

}

#endif