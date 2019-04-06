#include "sandbox/geometry/algorithms/SmoothNormals.h"
#include <iostream>

namespace sandbox {

SmoothNormals::SmoothNormals() : mesh(NULL) {
	addType<SmoothNormals>();
}

void SmoothNormals::updateModel() {
	if (!mesh) {
		mesh = getSceneNode().getComponent<Mesh>();
		std::cout << "Calculate smooth normals." << std::endl;

		const std::vector<unsigned int>& inds = mesh->getIndices();
		const std::vector<glm::vec3>& positions = mesh->getNodes();

		std::vector<glm::vec3> norms(positions.size());
		std::vector<int> numNorms(positions.size());

		for (int f = 0; f < inds.size(); f+=3) {
			const glm::vec3& a = positions[inds[f]];
			const glm::vec3& b = positions[inds[f+1]];
			const glm::vec3& c = positions[inds[f+2]];
			glm::vec3 n = glm::normalize(glm::cross(b-a, c-a));
			norms[inds[f]] += n;
			norms[inds[f+1]] += n;
			norms[inds[f+2]] += n;
			numNorms[inds[f]]++;
			numNorms[inds[f+1]]++;
			numNorms[inds[f+2]]++;
		}

		for (int f = 0; f < norms.size(); f++) {
			if (numNorms[f] > 0) {
				norms[f] = norms[f]/(float(numNorms[f]));
			}
		}

		mesh->setNormals(norms);
	}
}

}