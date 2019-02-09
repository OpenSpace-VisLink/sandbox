#ifndef SANDBOX_BASE_CAMERA_H_
#define SANDBOX_BASE_CAMERA_H_

#include "glm/glm.hpp"
#include "sandbox/SceneComponent.h"

namespace sandbox {

class Camera : public SceneComponent {
public:
	Camera();
	virtual ~Camera();

	void updateModel();
	void render(const SceneContext& sceneContext);
	void finishRender(const SceneContext& sceneContext);

private:
	glm::mat4 projection;
	glm::mat4 view;
};

}

#endif
