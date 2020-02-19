#ifndef SANDBOX_GEOMETRY_ALGORITHMS_SMOOTH_NORMALS_COMPONENT_H_
#define SANDBOX_GEOMETRY_ALGORITHMS_SMOOTH_NORMALS_COMPONENT_H_

#include "sandbox/SceneComponent.h"
#include "sandbox/geometry/Mesh.h"

namespace sandbox {

class SmoothNormals : public SceneComponent {
public:
	SmoothNormals();
	virtual ~SmoothNormals() {}

	void updateModel();

private:
	Mesh* mesh;
};

}

#endif