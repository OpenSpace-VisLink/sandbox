#ifndef SANDBOX_BASE_CAMERA_H_
#define SANDBOX_BASE_CAMERA_H_

#include "glm/glm.hpp"
#include "sandbox/graphics/GraphicsComponent.h"

namespace sandbox {

class Camera : public GraphicsComponent {
public:
	Camera();
	virtual ~Camera();

	void update();
	void startRender(const GraphicsContext& context);
	void finishRender(const GraphicsContext& context);

private:
	glm::mat4 projection;
	glm::mat4 view;
};

}

#endif
