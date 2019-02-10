#ifndef SANDBOX_GEOMETRY_MATERIAL_H_
#define SANDBOX_GEOMETRY_MATERIAL_H_

#include "sandbox/SceneComponent.h"
#include "glm/glm.hpp"

namespace sandbox {

class Material : public SceneComponent {
public:
	Material();
	virtual ~Material() {}

	// Light properties
	const glm::vec3& getAmbient() { return ambient; }
	const glm::vec3& getDiffuse() { return diffuse; }
	const glm::vec3& getSpecular() { return specular; }
	float getShininess() { return shininess; }
	void setAmbient(const glm::vec3& ambient) { this->ambient = ambient; }
	void setDiffuse(const glm::vec3& diffuse) { this->diffuse = diffuse; }
	void setSpecular(const glm::vec3& specular) { this->specular = specular; }
	void setShininess(float shininess) { this->shininess = shininess; }

	// color proporties
	bool hasColor() { return colorExists; }
	const glm::vec4& getColor() { return color; }
	void setColor(const glm::vec4& color) { this-> color = color; colorExists = true; }
	void removeColor() { bool colorExists = false; }

protected:
	bool colorExists;
	glm::vec4 color;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
};

}

#endif