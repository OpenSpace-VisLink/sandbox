#ifndef SANDBOX_GEOMETRY_MESH_COMPONENT_H_
#define SANDBOX_GEOMETRY_MESH_COMPONENT_H_

#include <vector>
#include "glm/glm.hpp"
#include "sandbox/SceneComponent.h"

namespace sandbox {

class Mesh : public SceneComponent {
public:
	Mesh();
	virtual ~Mesh() {}

	const std::vector<unsigned int>& getIndices() const { return indices; }
	const std::vector<glm::vec3>& getNodes() const { return nodes; }
	const std::vector<glm::vec3>& getNormals() const { return normals; }
	const std::vector<glm::vec2>& getCoords() const { return coords; }

	virtual void setIndices(const std::vector<unsigned int>& indices) { if (!readOnly) { this->indices = indices;} }
	virtual void setNodes(const std::vector<glm::vec3>& nodes) { if (!readOnly) {this->nodes = nodes; } }
	virtual void setNormals(const std::vector<glm::vec3>& normals) { if (!readOnly) {this->normals = normals;} }
	virtual void setCoords(const std::vector<glm::vec2>& coords) { if (!readOnly) {this->coords = coords;} }

protected:
	std::vector<unsigned int> indices;
    std::vector<glm::vec3> nodes;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> coords;
	bool readOnly;
};

}

#endif