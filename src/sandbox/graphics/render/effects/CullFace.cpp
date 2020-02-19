#include "sandbox/graphics/render/effects/CullFace.h"

namespace sandbox {

CullFace::CullFace(GLuint cullFace) : cullFace(cullFace) {
	addType<CullFace>();
}

void CullFace::startRender(const GraphicsContext& context) {
	//glDepthFunc(GL_GEQUAL);
	glEnable(GL_CULL_FACE);
    glCullFace(cullFace);
}

void CullFace::finishRender(const GraphicsContext& context) {
	glDisable(GL_CULL_FACE);
	//glDepthFunc(GL_LEQUAL);
}


}