#include "sandbox/input/interaction/ArcBall.h"
#include<cmath>
#include <glm/gtc/quaternion.hpp>

namespace sandbox {

void ArcBall::update() {
	if (!mouse) {
		mouse = input->getComponent<MouseInput>();
	}

	if (!transform) {
		transform = getEntity().getComponent<Transform>();
	}

	if (mouse) {
		if (mouse->getButtonState(0) && glm::length(mouse->getPosition()-mouse->getLastPosition()) > 0.0f) {
			glm::vec3 p1 = get3DPoint(mouse->getLastPosition());
			glm::vec3 p2 = get3DPoint(mouse->getPosition());
			glm::vec3 v1 = glm::normalize(p1);
			glm::vec3 v2 = glm::normalize(p2);
			glm::vec3 N = glm::normalize(glm::cross(v1,v2));
			float theta = std::acos(glm::dot(v1,v2));

			float c = std::sin(theta/2.0);
			glm::vec3 a = N*c;

			glm::quat Q(std::cos(theta/2.0), a.x, a.y, a.z);
			Q = Q/glm::length(Q);
			//glm::mat4 RotationMatrix = glm::quaternion::toMat4(Q);
			glm::mat4 rot = glm::mat4_cast(Q);
			//p1.z = std::sqrt(radius*radius - (p1.))
			//transform->setTransform(glm::translate(prevTransform,glm::vec3(mouse->getPosition()-mouse->getDragStartPosition(),0)));
			transform->setTransform(rot*transform->getTransform());
		}
	}
}

glm::vec3 ArcBall::get3DPoint(glm::vec2 point) {
	glm::vec3 p(point,0);//((point-glm::vec2(0.5))*2.0f,0);
	float mag = p.x*p.x + p.y*p.y;
	float r2 = radius*radius;
	if (mag <= r2/2.0) {
		p.z = std::sqrt(r2 - mag);
	}
	else {
		p.z = (r2/2.0)/std::sqrt(mag);
	}

	return p;
}

}