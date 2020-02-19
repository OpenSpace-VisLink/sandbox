#ifndef SANDBOX_GRAPHICS_VIEWPORT_H_
#define SANDBOX_GRAPHICS_VIEWPORT_H_

#include "sandbox/SceneComponent.h"
#include "glm/glm.hpp"

namespace sandbox {

class Viewport : public SceneComponent {
public:
	Viewport();
	virtual ~Viewport() {}

	void render(const SceneContext& sceneContext);
	void finishRender(const SceneContext& sceneContext);

protected:
	virtual glm::ivec4 calculateViewport(const glm::ivec4& currentViewport) = 0;
};

class FixedViewport : public Viewport {
public:
	FixedViewport(const glm::ivec4& viewport) : viewport(viewport) {}

protected:
	glm::ivec4 calculateViewport(const glm::ivec4& currentViewport);

private:
	glm::ivec4 viewport;
};

class PercentViewport : public Viewport {
public:
	PercentViewport(const glm::vec4& viewport) : viewport(viewport) {}

protected:
	glm::ivec4 calculateViewport(const glm::ivec4& currentViewport);

private:
	glm::vec4 viewport;
};

}

#endif