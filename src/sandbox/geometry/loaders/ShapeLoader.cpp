#include "sandbox/geometry/loaders/ShapeLoader.h"
#include<iostream>

namespace sandbox {

ShapeLoader::ShapeLoader(Shape shape, int resolution) : shape(shape), resolution(resolution) {
	addType<ShapeLoader>();
}

void ShapeLoader::load(Mesh* mesh) {
	//readOnly = true;
	std::cout << "Update mesh" << std::endl;

	std::vector<unsigned int> indices;
    std::vector<glm::vec3> nodes;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> coords;

	if (shape == SHAPE_QUAD) {
		nodes.push_back(glm::vec3(-0.5, -0.5, 0));
		nodes.push_back(glm::vec3(-0.5, 0.5, 0));
		nodes.push_back(glm::vec3(0.5, 0.5, 0));
		nodes.push_back(glm::vec3(0.5, -0.5, 0));

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
		indices.push_back(2);
	}
	else if (shape == SHAPE_CYLINDAR) {
		int numSegments = resolution > 3 ? resolution : 3;
		for (int f = 0; f < numSegments+1; f++) {
			float x = std::cos(-2.0f*3.14159*f/(numSegments));
			float y = std::sin(-2.0f*3.14159*f/(numSegments));
			float bottomRadius = 0.5;
			float topRadius = 0.5;
			nodes.push_back(glm::vec3(bottomRadius*x, -0.5, bottomRadius*y));
			nodes.push_back(glm::vec3(topRadius*x, 0.5, topRadius*y));
			normals.push_back(glm::vec3(x, 0, y));
			normals.push_back(glm::vec3(x, 0, y));
			coords.push_back(glm::vec2(1.0f*f/numSegments, 1));
			coords.push_back(glm::vec2(1.0f*f/numSegments, 0));

			if (f > 0) {
				indices.push_back((f-1)*2+0);
				indices.push_back((f-1)*2+2);
				indices.push_back((f-1)*2+1);
				indices.push_back((f-1)*2+2);
				indices.push_back((f-1)*2+3);
				indices.push_back((f-1)*2+1);
			}
		}
	}
	else if (shape == SHAPE_SPHERE) {
		float dx = (1.0f/(resolution-1));
		float dy = dx;
		for (int j = 0; j < resolution; j++) {
			for (int i = 0; i < resolution; i++) {
				glm::vec3 gridPoint = glm::vec3(-0.5 + dx*i, 0.5 - dy*j, 0.0);
				glm::vec3 latLong = gridPoint*glm::vec3(2.0)*glm::vec3(3.14159, 3.14159/2.0f, 0.0f);
				//glm::vec3 spherePoint(std::cos(latLong.x)*std::sin(latLong.y), std::sin(latLong.x), std::cos(latLong.x)*std::cos(latLong.y));
				//glm::vec3 spherePoint(std::sin(latLong.x), std::cos(latLong.x)*std::sin(latLong.y), std::cos(latLong.x)*std::cos(latLong.y));
				glm::vec3 spherePoint(std::cos(latLong.y)*std::sin(latLong.x), std::sin(latLong.y), std::cos(latLong.x)*std::cos(latLong.y));
				nodes.push_back(spherePoint*0.5f);
				normals.push_back(spherePoint);
				coords.push_back(glm::vec2(dx*i, dy*j));
			}
		}

		for (int i = 0; i < resolution-1; i++) {
			for (int j = 0; j < resolution-1; j++) {
				indices.push_back(i*resolution+j);
				indices.push_back((i+1)*resolution+j);
				indices.push_back((i)*resolution+j+1);
				indices.push_back((i)*resolution+j+1);
				indices.push_back((i+1)*resolution+j);
				indices.push_back((i+1)*resolution+j+1);
			}
		}
	}
	/*else if (shape == SHAPE_TWIST) {
		float dx = (1.0f/(resolution-1));
		float dy = dx;
		for (int j = 0; j < resolution; j++) {
			for (int i = 0; i < resolution; i++) {
				glm::vec3 gridPoint = glm::vec3(-0.5 + dx*i, 0.5 - dy*j, 0.0);
				glm::vec3 latLong = gridPoint*glm::vec3(2.0)*glm::vec3(3.14159, 3.14159/2.0f, 0.0f);
				glm::vec3 spherePoint(std::cos(latLong.x)*std::sin(latLong.y), std::sin(latLong.y), std::cos(latLong.x)*std::cos(latLong.y));
				nodes.push_back(spherePoint);
				normals.push_back(glm::vec3(0, 0, 1.0f));
				coords.push_back(glm::vec2(dx*i, dy*j));
			}
		}

		for (int i = 0; i < resolution-1; i++) {
			for (int j = 0; j < resolution-1; j++) {
				indices.push_back(i*resolution+j);
				indices.push_back((i+1)*resolution+j);
				indices.push_back((i)*resolution+j+1);
				indices.push_back((i)*resolution+j+1);
				indices.push_back((i+1)*resolution+j);
				indices.push_back((i+1)*resolution+j+1);
			}
		}
	}*/
	else if (shape == SHAPE_GRID) {
		float dx = (1.0f/(resolution-1));
		float dy = dx;
		for (int j = 0; j < resolution; j++) {
			for (int i = 0; i < resolution; i++) {
				nodes.push_back(glm::vec3(-0.5 + dx*i, 0.5 - dy*j, 0.0));
				normals.push_back(glm::vec3(0, 0, 1.0f));
				coords.push_back(glm::vec2(dx*i, dy*j));
			}
		}

		for (int i = 0; i < resolution-1; i++) {
			for (int j = 0; j < resolution-1; j++) {
				indices.push_back(i*resolution+j);
				indices.push_back((i+1)*resolution+j);
				indices.push_back((i)*resolution+j+1);
				indices.push_back((i)*resolution+j+1);
				indices.push_back((i+1)*resolution+j);
				indices.push_back((i+1)*resolution+j+1);
			}
		}
	}
	else if (shape == SHAPE_CIRCLE) {
		nodes.push_back(glm::vec3(0.0f));
		normals.push_back(glm::vec3(glm::vec3(0, 0, 1)));
		coords.push_back(glm::vec2(0.5f));
		indices.push_back(0);
		int numSegments = resolution > 3 ? resolution : 3;
		for (int f = 0; f < numSegments; f++) {
			float x = std::cos(2.0f*3.14159*f/(numSegments))*0.5;
			float y = std::sin(2.0f*3.14159*f/(numSegments))*0.5;
			nodes.push_back(glm::vec3(x,y,0.0f));
			normals.push_back(glm::vec3(glm::vec3(0, 0, 1)));
			glm::vec2 uv(x,y);
			uv = uv/2.0f + glm::vec2(1.0f)/2.0f;
			coords.push_back(uv);
			indices.push_back(f+1);
		}

		indices.push_back(1);
	}

	mesh->setIndices(indices);
	mesh->setNodes(nodes);
	mesh->setNormals(normals);
	mesh->setCoords(coords);
}

}