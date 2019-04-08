#ifndef SANDBOX_GRAPHICS_WINDOW_H_
#define SANDBOX_GRAPHICS_WINDOW_H_

#include "sandbox/SceneComponent.h"
#include "sandbox/interaction/events/ResizeEvent.h"

namespace sandbox {

class Window : public SceneComponent {
public:
	Window(SceneNode* eventNode);
	virtual ~Window() {}

	void updateModel();
	void render(const SceneContext& sceneContext);
	void finishRender(const SceneContext& sceneContext);

private:
	SceneNode* eventNode;
	ResizeEvent* resize;
};

}

#endif