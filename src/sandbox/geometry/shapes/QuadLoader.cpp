#include "sandbox/geometry/shapes/QuadLoader.h"
#include<iostream>

namespace sandbox {

QuadLoader::QuadLoader() {
	addType<QuadLoader>();
}

void QuadLoader::load(Mesh* mesh) {
	//readOnly = true;
	std::cout << "Update mesh" << std::endl;

	std::vector<unsigned int> indices;
    std::vector<glm::vec3> nodes;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> coords;

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

	mesh->setIndices(indices);
	mesh->setNodes(nodes);
	mesh->setNormals(normals);
	mesh->setCoords(coords);
}

}