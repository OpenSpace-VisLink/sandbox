#ifndef SANDBOX_GRAPHICS_BLEND_H_
#define SANDBOX_GRAPHICS_BLEND_H_

#include "sandbox/SceneComponent.h"
#include "OpenGL.h"

namespace sandbox {

class Blend : public SceneComponent {
public:
	Blend();
	virtual ~Blend() {}

	void render(const SceneContext& sceneContext);
	void finishRender(const SceneContext& sceneContext);

private:
};

}

#endif