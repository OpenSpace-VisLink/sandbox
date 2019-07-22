#ifndef SANDBOX_GEOMETRY_SHAPES_MESH_LOADER_H_
#define SANDBOX_GEOMETRY_SHAPES_MESH_LOADER_H_

#include "sandbox/geometry/Mesh.h"
#include "sandbox/Component.h"

namespace sandbox {

class MeshLoader : public Component {
public:
	MeshLoader();
	virtual ~MeshLoader() {}

	void update();
	virtual void load(Mesh* mesh) = 0;

private:
	bool isLoaded;
};

}

#endif