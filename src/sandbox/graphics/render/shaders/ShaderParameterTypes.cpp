#include "sandbox/graphics/render/shaders/ShaderParameterTypes.h"
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "sandbox/graphics/RenderState.h"

namespace sandbox {

void Matrix4ShaderParameter::setParameter(const GraphicsContext& context, GLuint shaderProgram, GLuint location) const {
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(getMatrix(context)));
}

void Matrix3ShaderParameter::setParameter(const GraphicsContext& context, GLuint shaderProgram, GLuint location) const {
	glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(getMatrix(context)));
}

void Vector3ShaderParameter::setParameter(const GraphicsContext& context, GLuint shaderProgram, GLuint location) const {
	glm::vec3 vec = getVector(context);
	glUniform3f(location, vec.x, vec.y, vec.z);
}

glm::mat4 ProjectionMatrixShaderParameter::getMatrix(const GraphicsContext& context) const {
	return RenderState::get(context).getProjectionMatrix().get();
}

glm::mat4 ViewMatrixShaderParameter::getMatrix(const GraphicsContext& context) const {
	return RenderState::get(context).getViewMatrix().get();
}

glm::mat4 ModelMatrixShaderParameter::getMatrix(const GraphicsContext& context) const {
	return RenderState::get(context).getModelMatrix().get();
}

glm::mat3 NormalMatrixShaderParameter::getMatrix(const GraphicsContext& context) const {
	RenderState& renderState = RenderState::get(context);
	glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(renderState.getViewMatrix().get()*renderState.getModelMatrix().get())));
	return normalMatrix;
}

glm::vec3 EyePositionShaderParameter::getVector(const GraphicsContext& context) const {
	return glm::column(RenderState::get(context).getViewMatrix().get(), 3);
}

}