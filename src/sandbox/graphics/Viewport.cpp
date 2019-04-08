#include "sandbox/graphics/Viewport.h"
#include "sandbox/graphics/RenderState.h"

namespace sandbox {

Viewport::Viewport() {
	addType<Viewport>();
}

void Viewport::render(const SceneContext& sceneContext) {
	RenderState& renderState = RenderState::get(sceneContext);
	glm::ivec4 viewport = calculateViewport(renderState.getViewport().get());
	renderState.getViewport().push(viewport);
	glViewport((GLint)viewport[0], (GLint)viewport[1], (GLsizei)viewport[2], (GLsizei)viewport[3]);
}

void Viewport::finishRender(const SceneContext& sceneContext) {
	RenderState& renderState = RenderState::get(sceneContext);
	renderState.getViewport().pop();
}

glm::ivec4 FixedViewport::calculateViewport(const glm::ivec4& currentViewport) {
	return viewport;
}

glm::ivec4 PercentViewport::calculateViewport(const glm::ivec4& currentViewport) {
	return glm::ivec4(currentViewport[0]+currentViewport[2]*viewport[0],
		currentViewport[1]+currentViewport[3]*viewport[1],
		currentViewport[2]*viewport[2],
		currentViewport[3]*viewport[3]);
}

}