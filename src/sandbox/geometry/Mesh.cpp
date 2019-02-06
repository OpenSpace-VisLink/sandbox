#include "sandbox/geometry/Mesh.h"

namespace sandbox {

Mesh::Mesh() : readOnly(false) {
	addType<Mesh>();
}

}