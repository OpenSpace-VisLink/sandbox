#include "sandbox/geometry/shapes/Cylinder.h"
#include <cmath>

namespace sandbox {

Cylinder::Cylinder(int numSegments, float bottomRadius, float topRadius) {
	//readOnly = true;

	addType<Cylinder>();

	for (int f = 0; f < numSegments+1; f++) {
		float x = std::cos(-2.0f*3.14159*f/(numSegments));
		float y = std::sin(-2.0f*3.14159*f/(numSegments));
		nodes.push_back(glm::vec3(bottomRadius*x, -1, bottomRadius*y));
		nodes.push_back(glm::vec3(topRadius*x, 1, topRadius*y));
		normals.push_back(glm::vec3(x, 0, y));
		normals.push_back(glm::vec3(x, 0, y));
		coords.push_back(glm::vec2(1.0f*f/numSegments, 0));
		coords.push_back(glm::vec2(1.0f*f/numSegments, 1));

		if (f > 0) {
			indices.push_back((f-1)*2+0);
			indices.push_back((f-1)*2+2);
			indices.push_back((f-1)*2+1);
			indices.push_back((f-1)*2+2);
			indices.push_back((f-1)*2+3);
			indices.push_back((f-1)*2+1);
		}
/*		float x = std::cos(-2.0f*3.14159*f/(numSegments));
		float y = std::sin(-2.0f*3.14159*f/(numSegments));
		float nextX = std::cos(-2.0f*3.14159*(f+1)/(numSegments));
		float nextY = std::sin(-2.0f*3.14159*(f+1)/(numSegments));
		nodes.push_back(glm::vec3(bottomRadius*x, -1, bottomRadius*y));
		nodes.push_back(glm::vec3(topRadius*x, 1, topRadius*y));
		nodes.push_back(glm::vec3(topRadius*nextX, 1, topRadius*nextY));
		nodes.push_back(glm::vec3(bottomRadius*nextX, -1, bottomRadius*nextY));
		
		normals.push_back(glm::vec3(0, 0, 1));
		normals.push_back(glm::vec3(0, 0, 1));
		normals.push_back(glm::vec3(0, 0, 1));
		normals.push_back(glm::vec3(0, 0, 1));

		coords.push_back(glm::vec2(0, 1));
		coords.push_back(glm::vec2(0, 0));
		coords.push_back(glm::vec2(1, 0));
		coords.push_back(glm::vec2(1, 1));

		indices.push_back(f*4+0);
		indices.push_back(f*4+2);
		indices.push_back(f*4+1);
		indices.push_back(f*4+0);
		indices.push_back(f*4+3);
		indices.push_back(f*4+2);*/
	}


}

}