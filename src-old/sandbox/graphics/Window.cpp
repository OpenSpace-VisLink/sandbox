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
		glm::ivec4 viewport = glm::ivec4(0, 0, resize->getState().width, resize->getState().height);
		renderState.getViewport().push(viewport);
		glViewport((GLint)viewport[0], (GLint)viewport[1], (GLsizei)viewport[2], (GLsizei)viewport[3]);
		// glViewport((GLint)rect.getX(), (GLint)rect.getY(), (GLsizei)rect.getWidth(), (GLsizei)rect.getHeight());
	}
}

void Window::finishRender(const SceneContext& sceneContext) {
	if (resize) {
		RenderState& renderState = RenderState::get(sceneContext);
		renderState.getViewport().pop();	
	}
}

}
