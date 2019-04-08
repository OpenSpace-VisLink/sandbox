#include "sandbox/graphics/Window.h"
#include "sandbox/graphics/RenderState.h"

namespace sandbox {

Window::Window(SceneNode* eventNode) : eventNode(eventNode), resize(NULL) {
	addType<Window>();
}


void Window::updateModel() {
	if (!resize) {
		resize = eventNode->getComponent<ResizeEvent>();
	}
}

void Window::render(const SceneContext& sceneContext) {
	if (resize) {
		RenderState& renderState = RenderState::get(sceneContext);
		renderState.getViewportSize().push(glm::ivec2(resize->getState().width, resize->getState().height));
	}
}

void Window::finishRender(const SceneContext& sceneContext) {
	if (resize) {
		RenderState& renderState = RenderState::get(sceneContext);
		renderState.getViewportSize().pop();	
	}
}

}
