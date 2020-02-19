#ifndef SANDBOX_INPUT_INTERACTION_ARCBALL_H_
#define SANDBOX_INPUT_INTERACTION_ARCBALL_H_

#include "glm/glm.hpp"
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "sandbox/base/Transform.h"
#include "sandbox/Component.h"
#include "sandbox/input/MouseInput.h"

namespace sandbox {

class ArcBall : public Component {
public:
	ArcBall(Entity* input, float radius = 1.0f) : input(input), radius(radius), mouse(NULL), transform(NULL) { addType<ArcBall>(); }
	virtual ~ArcBall() {}

	void update();

	bool forceUpdate() {
		return true;
	}

private:
	glm::vec3 get3DPoint(glm::vec2 point);

	Entity* input;
	MouseInput* mouse;
	Transform* transform;
	float radius;
};


}

#endif