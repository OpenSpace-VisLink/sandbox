#include "sandbox/graphics/render/effects/Blend.h"

namespace sandbox {

Blend::Blend() {
	addType<Blend>();
}

void Blend::startRender(const GraphicsContext& context) {
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_DST_COLOR, GL_ZERO);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
}

void Blend::finishRender(const GraphicsContext& context) {
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}

}