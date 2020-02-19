#ifndef SANDBOX_GEOMETRY_MESH_H_
#define SANDBOX_GEOMETRY_MESH_H_

#include <vector>
#include "glm/glm.hpp"
#include "sandbox/Component.h"

namespace sandbox {

class Mesh : public Component {
public:
	Mesh();
	virtual ~Mesh() {}

	enum MeshComponent {
		MESH_INDICES = 0,
		MESH_NODES = 1,
		MESH_NORMALS = 2,
		MESH_COORDS = 3,
		MESH_COMPONENT_NUM
	};

	const std::vector<unsigned int>& getIndices() const { return indices; }
	const std::vector<glm::vec3>& getNodes() const { return nodes; }
	const std::vector<glm::vec3>& getNormals() const { return normals; }
	const std::vector<glm::vec2>& getCoords() const { return coords; }

	virtual void setIndices(const std::vector<unsigned int>& indices) { this->indices = indices; incrementVersion(MESH_INDICES); }
	virtual void setNodes(const std::vector<glm::vec3>& nodes) { this->nodes = nodes; incrementVersion(MESH_NODES); }
	virtual void setNormals(const std::vector<glm::vec3>& normals) { this->normals = normals; incrementVersion(MESH_NORMALS); }
	virtual void setCoords(const std::vector<glm::vec2>& coords) { this->coords = coords; incrementVersion(MESH_COORDS); }

	void setNode(int index, glm::vec3 node) {
		nodes[index] = node;
		incrementVersion(MESH_NODES);
	}

	int getComponentVersion(MeshComponent component) const { return componentVersions[component]; }

protected:
	void incrementVersion(MeshComponent component);

	std::vector<unsigned int> indices;
    std::vector<glm::vec3> nodes;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> coords;
	int componentVersions[MESH_COMPONENT_NUM];
};

}

#endif