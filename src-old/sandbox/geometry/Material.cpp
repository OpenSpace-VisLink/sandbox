#include "sandbox/geometry/Material.h"

namespace sandbox {

Material::Material() : colorExists(false), ambient(0.1), diffuse(0.5), specular(0.2), shininess(1.0), textureNode(nullptr) {
	addType<Material>();
	//setColor(glm::vec4(0.0, 0.0, 1.0f, 1.0f));
}

}