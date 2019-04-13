#ifndef SANDBOX_GEOMETRY_SHAPES_GRID_H_
#define SANDBOX_GEOMETRY_SHAPES_GRID_H_

#include "sandbox/geometry/Mesh.h"

namespace sandbox {

class Grid : public Mesh {
public:
	Grid(int width, int height);
	virtual ~Grid() {}

	glm::vec3& getNode(int x, int y) {
		return nodes[x*height + y];
	}

	glm::vec2& getCoord(int x, int y) {
		return coords[x*height + y];
	}

	int getWidth() const { return width; }
	int getHeight() const { return height; }

private:
	int width;
	int height;
};

}

#endif