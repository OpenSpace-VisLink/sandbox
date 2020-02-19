#ifndef SANDBOX_GEOMETRY_SHAPES_SPHERE_H_
#define SANDBOX_GEOMETRY_SHAPES_SPHERE_H_

#include "sandbox/geometry/Mesh.h"

namespace sandbox {

class Sphere : public Mesh {
public:
	Sphere(int numSegments);
	virtual ~Sphere() {}
};

}

#endif