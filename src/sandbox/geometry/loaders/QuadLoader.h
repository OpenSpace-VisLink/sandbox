#ifndef SANDBOX_GEOMETRY_LOADERS_QUAD_LOADER_H_
#define SANDBOX_GEOMETRY_LOADERS_QUAD_LOADER_H_

#include "sandbox/geometry/MeshLoader.h"

namespace sandbox {

class QuadLoader : public MeshLoader {
public:
	QuadLoader();
	virtual ~QuadLoader() {}

	void load(Mesh* mesh);
};

}

#endif