#include "sandbox/geometry/shapes/Quad.h"

namespace sandbox {

Quad::Quad() {
	readOnly = true;

	addType<Quad>();

	nodes.push_back(glm::vec3(-1, -1, 0));
	nodes.push_back(glm::vec3(-1, 1, 0));
	nodes.push_back(glm::vec3(1, 1, 0));
	nodes.push_back(glm::vec3(1, -1, 0));

	normals.push_back(glm::vec3(0, 0, 1));
	normals.push_back(glm::vec3(0, 0, 1));
	normals.push_back(glm::vec3(0, 0, 1));
	normals.push_back(glm::vec3(0, 0, 1));

	coords.push_back(glm::vec2(0, 1));
	coords.push_back(glm::vec2(0, 0));
	coords.push_back(glm::vec2(1, 0));
	coords.push_back(glm::vec2(1, 1));

	indices.push_back(0);
	indices.push_back(2);
	indices.push_back(1);
	indices.push_back(0);
	indices.push_back(3);
	indices.push_back(2);
}

}