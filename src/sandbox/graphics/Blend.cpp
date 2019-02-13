#include "sandbox/graphics/Blend.h"

namespace sandbox {

Blend::Blend() {
	addType<Blend>();
}

void Blend::render(const SceneContext& sceneContext) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Blend::finishRender(const SceneContext& sceneContext) {
	glDisable(GL_BLEND);
}

}