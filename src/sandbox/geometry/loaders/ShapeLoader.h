#ifndef SANDBOX_GEOMETRY_LOADERS_SHAPE_LOADER_H_
#define SANDBOX_GEOMETRY_LOADERS_SHAPE_LOADER_H_

#include "sandbox/geometry/MeshLoader.h"

namespace sandbox {

enum Shape {
	SHAPE_QUAD,
	SHAPE_CYLINDAR,
	SHAPE_CIRCLE
};

class ShapeLoader : public MeshLoader {
public:
	ShapeLoader(Shape shape, int resolution = 0);
	virtual ~ShapeLoader() {}

	void load(Mesh* mesh);

private:
	Shape shape;
	int resolution;
};

}

#endif