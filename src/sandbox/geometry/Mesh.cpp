#include "sandbox/geometry/Mesh.h"

namespace sandbox {

Mesh::Mesh() {
	addType<Mesh>();

	for (int f = 0; f < MESH_COMPONENT_NUM; f++) {
		componentVersions[f] = 0;
	}
}


void Mesh::incrementVersion(Mesh::MeshComponent component) {
	componentVersions[component]++;
	getEntity().incrementVersion();
}

}