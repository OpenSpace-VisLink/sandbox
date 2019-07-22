#ifndef SANDBOX_GEOMETRY_MESH_LOADER_COMPONENT_H_
#define SANDBOX_GEOMETRY_MESH_LOADER_COMPONENT_H_

#include "sandbox/SceneComponent.h"
#include <string>
#include "sandbox/geometry/Mesh.h"

namespace sandbox {

class MeshLoader : public SceneComponent {
public:
	MeshLoader(const std::string& path);
	virtual ~MeshLoader() {}

	void updateModel();
	virtual void load(const std::string& path, Mesh* mesh);

private:
	std::string path;
	bool isLoaded;
};

}

#endif