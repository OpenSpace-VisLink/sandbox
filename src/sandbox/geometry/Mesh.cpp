#include "sandbox/geometry/Mesh.h"

namespace sandbox {

Mesh::Mesh() : readOnly(false) {
	addType<Mesh>();
}

/*std::vector<glm::vec3> Mesh::calculateNormals(const std::vector<unsigned int>& inds, const std::vector<glm::vec3>& positions) {
	std::vector<glm::vec3> norms(positions.size());
	std::vector<int> numNorms(positions.size());

	for (int f = 0; f < indices.size(); f+=3) {
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

	return norms;
}*/

}