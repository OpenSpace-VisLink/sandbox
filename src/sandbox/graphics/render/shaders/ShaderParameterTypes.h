#ifndef SANDBOX_GRAPHICS_SHADERS_SHADER_PARAMETER_TYPES_H_
#define SANDBOX_GRAPHICS_SHADERS_SHADER_PARAMETER_TYPES_H_

#include "sandbox/graphics/render/ShaderProgram.h"

namespace sandbox {
	
class Vector3ShaderParameter : public ShaderParameter {
public:
	Vector3ShaderParameter(const std::string& name) : ShaderParameter(name) {}
	void setParameter(const GraphicsContext& context, GLuint shaderProgram, GLuint location) const;
protected:
	virtual glm::vec3 getVector(const GraphicsContext& context) const = 0;
};

class Matrix4ShaderParameter : public ShaderParameter {
public:
	Matrix4ShaderParameter(const std::string& name) : ShaderParameter(name) {}
	void setParameter(const GraphicsContext& context, GLuint shaderProgram, GLuint location) const;
protected:
	virtual glm::mat4 getMatrix(const GraphicsContext& context) const = 0;
};

class Matrix3ShaderParameter : public ShaderParameter {
public:
	Matrix3ShaderParameter(const std::string& name) : ShaderParameter(name) {}
	void setParameter(const GraphicsContext& context, GLuint shaderProgram, GLuint location) const;
protected:
	virtual glm::mat3 getMatrix(const GraphicsContext& context) const = 0;
};

class ProjectionMatrixShaderParameter : public Matrix4ShaderParameter {
public:
	ProjectionMatrixShaderParameter(const std::string& name) : Matrix4ShaderParameter(name) {}
protected:
	virtual glm::mat4 getMatrix(const GraphicsContext& context) const;
};

class ViewMatrixShaderParameter : public Matrix4ShaderParameter {
public:
	ViewMatrixShaderParameter(const std::string& name) : Matrix4ShaderParameter(name) {}
protected:
	virtual glm::mat4 getMatrix(const GraphicsContext& context) const;
};

class ModelMatrixShaderParameter : public Matrix4ShaderParameter {
public:
	ModelMatrixShaderParameter(const std::string& name) : Matrix4ShaderParameter(name) {}
protected:
	virtual glm::mat4 getMatrix(const GraphicsContext& context) const;
};

class NormalMatrixShaderParameter : public Matrix3ShaderParameter {
public:
	NormalMatrixShaderParameter(const std::string& name) : Matrix3ShaderParameter(name) {}
protected:
	virtual glm::mat3 getMatrix(const GraphicsContext& context) const;
};

class EyePositionShaderParameter : public Vector3ShaderParameter {
public:
	EyePositionShaderParameter(const std::string& name) : Vector3ShaderParameter(name) {}
protected:
	virtual glm::vec3 getVector(const GraphicsContext& context) const;
};

}

#endif