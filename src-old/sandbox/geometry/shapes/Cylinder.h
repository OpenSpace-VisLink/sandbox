#ifndef SANDBOX_GEOMETRY_SHAPES_CYLINDER_H_
#define SANDBOX_GEOMETRY_SHAPES_CYLINDER_H_

#include "sandbox/geometry/Mesh.h"

namespace sandbox {

class Cylinder : public Mesh {
public:
	Cylinder(int numSegments, float bottomRadius = 1.0f, float topRadius = 1.0f);
	virtual ~Cylinder() {}
};

}

#endif