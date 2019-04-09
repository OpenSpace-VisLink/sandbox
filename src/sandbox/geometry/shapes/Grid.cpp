#include "sandbox/geometry/shapes/Grid.h"

namespace sandbox {

Grid::Grid(int width, int height) : width(width), height(height) {

	addType<Grid>();

	float dx = 1.0f/float(width-1);
	float dy = 1.0f/float(height-1);

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			glm::vec3 point(-0.5f + dx*x, -0.5f + dy*y, 0.0f);
			nodes.push_back(point);
			normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
			//normals.push_back(point);
			coords.push_back(glm::vec2(point)+glm::vec2(0.5));
		}
	}

	for (int x = 0; x < width-1 ; x++) {
		for (int y = 0; y < height-1; y++) {
			indices.push_back(x*height+y);
			indices.push_back((x+1)*height+y);
			indices.push_back(x*height+y+1);
			indices.push_back((x+1)*height+y);
			indices.push_back((x+1)*height+y+1);
			indices.push_back(x*height+y+1);
		}
	}

	/*nodes.push_back(glm::vec3(-1, -1, 0));
	nodes.push_back(glm::vec3(-1, 1, 0));
	nodes.push_back(glm::vec3(1, 1, 0));
	nodes.push_back(glm::vec3(1, -1, 0));

	normals.push_back(glm::vec3(0, 0, 1));
	normals.push_back(glm::vec3(0, 0, 1));
	normals.push_back(glm::vec3(0, 0, 1));
	normals.push_back(glm::vec3(0, 0, 1));

	coords.push_back(glm::vec2(0, 1));
	coords.push_back(glm::vec2(0, 0));
	coords.push_back(glm::vec2(1, 0));
	coords.push_back(glm::vec2(1, 1));

	indices.push_back(0);
	indices.push_back(2);
	indices.push_back(1);
	indices.push_back(0);
	indices.push_back(3);
	indices.push_back(2);*/
}

}