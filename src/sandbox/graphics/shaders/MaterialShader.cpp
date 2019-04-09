#include "sandbox/graphics/shaders/MaterialShader.h"

#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "sandbox/graphics/RenderState.h"
#include "sandbox/geometry/Material.h"
#include "sandbox/graphics/Texture.h"
#include <iostream>


namespace sandbox {

MaterialShader::MaterialShader() {
	addType<MaterialShader>();
}

MaterialShader::~MaterialShader() {}

void MaterialShader::create(const SceneContext& sceneContext, ShaderProgramState& state) {
	GLuint shader;

	std::string vertexShader =
		            "#version 330 \n"
		            "layout(location = 0) in vec3 position; "
		            "layout(location = 1) in vec3 normal; "
					"layout(location = 2) in vec2 coord; "
		            ""
		            "uniform mat4 ProjectionMatrix; "
		            "uniform mat4 ViewMatrix; "
		            "uniform mat4 ModelMatrix; "
		            "uniform mat3 NormalMatrix; "
		            ""
		            "out vec3 pos; "
		            "out vec3 norm; "
		            "out vec2 uv; "
		            ""
		            "void main() { "
		            "   pos = (ModelMatrix*vec4(position,1.0)).xyz; "
		            "   norm = NormalMatrix*normal.xyz; "
		            "   uv = coord; "
		            "   gl_Position = ProjectionMatrix*ViewMatrix*ModelMatrix*vec4(pos, 1.0); "
		            "}";

	state.addShader(compileShader(vertexShader, GL_VERTEX_SHADER));

    std::string fragmentShader =
            "#version 330 \n"
            ""
		    "uniform bool hasColor; "
		    "uniform bool hasTexture; "
            "uniform sampler2D tex; "
		    "uniform bool hasMaterial; "
		    "uniform vec4 color; "
		    "uniform vec3 eyePosition; "
		    "uniform vec3 ambient; "
            "uniform vec3 diffuse; "
            "uniform vec3 specular; "
            "uniform float shininess; "
		    ""
		    "in vec3 pos; "
		    "in vec3 norm; "
		    "in vec2 uv; "
		    ""
            "layout (location = 0) out vec4 colorOut;  "
            ""
            "void main() { "
            "  vec3 n = normalize(norm); "
            "  if (!hasMaterial) { colorOut = vec4(n,1); return; } "
            "  vec4 lightPos = vec4(-1,1,2.0,0); "
            ""
            "  vec3 lightDir = lightPos.xyz; "
            "  if (lightPos.w > 0.001) { lightDir = lightPos.xyz - pos; } "
            "  lightDir = normalize(lightDir); "
            "  float intensity = max(dot(n,lightDir), 0.0); "
            "  vec3 spec = vec3(0.0); "
            "  if (intensity > 0.0) { "
            "    vec3 h = normalize(lightDir + normalize(eyePosition/1.0-pos.xyz)); "
            "    float intSpec = max(dot(h,n), 0.0); "
            "    float powTerm = pow(intSpec, shininess); "
            "    spec = specular*powTerm; "
            "  } "
            "  colorOut = vec4(max(intensity*diffuse + spec, ambient).xyz, 1.0);"
            ""
            //"  colorOut = vec4(norm,1); "
            "  if (hasColor) { colorOut = vec4(colorOut.xyz*color.xyz,1.0); } "
            "  if (hasTexture) { "
            "    vec4 texColor = texture(tex, uv);"
            "	 colorOut = vec4(colorOut.xyz*texColor.xyz,texColor.a); "
            "  } "
            "}";

    state.addShader(compileShader(fragmentShader, GL_FRAGMENT_SHADER));
}

void MaterialShader::setShaderParameters(const SceneContext& sceneContext, ShaderProgramState& state) {
	RenderState& renderState = RenderState::get(sceneContext);
	GLint loc = glGetUniformLocation(state.shaderProgram, "ProjectionMatrix");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(renderState.getProjectionMatrix().get()));
	loc = glGetUniformLocation(state.shaderProgram, "ViewMatrix");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(renderState.getViewMatrix().get()));
	loc = glGetUniformLocation(state.shaderProgram, "ModelMatrix");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(renderState.getModelMatrix().get()));

	glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(renderState.getViewMatrix().get()*renderState.getModelMatrix().get())));
	loc = glGetUniformLocation(state.shaderProgram, "NormalMatrix");
	glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	glm::vec3 eyePosition = glm::column(renderState.getViewMatrix().get(), 3);
    loc = glGetUniformLocation(state.shaderProgram, "eyePosition");
    glUniform3f(loc, eyePosition.x, eyePosition.y, eyePosition.z);

	bool hasMaterial = false;
	bool hasColor = false;
	bool hasTexture = false;
	glm::vec4 color;
	const SceneNode* node = renderState.getSceneNode().get();
	Material* material = node->getComponent<Material>();
	if (material) {
		hasColor = material->hasColor();
		color = material->getColor();

		if (material->getTexture()) {
			Texture* texture = material->getTexture()->getComponent<Texture>();
	    	if (texture) {
		    		hasTexture = true;
	    			glActiveTexture(GL_TEXTURE0+0);
					glBindTexture(texture->getTarget(sceneContext), texture->getId(sceneContext));

					loc = glGetUniformLocation(state.shaderProgram, "tex");
					glUniform1i(loc, 0);
	    	}
	    }

		glm::vec3 ambient = material->getAmbient();
		loc = glGetUniformLocation(state.shaderProgram, "ambient");
		glUniform3f(loc, ambient.r, ambient.g, ambient.b);

		glm::vec3 diffuse = material->getDiffuse();
		loc = glGetUniformLocation(state.shaderProgram, "diffuse");
		glUniform3f(loc, diffuse.r, diffuse.g, diffuse.b);

		glm::vec3 specular = material->getSpecular();
		loc = glGetUniformLocation(state.shaderProgram, "specular");
		glUniform3f(loc, specular.r, specular.g, specular.b);

		loc = glGetUniformLocation(state.shaderProgram, "shininess");
		glUniform1f(loc, material->getShininess());

		hasMaterial = true;
	}


	loc = glGetUniformLocation(state.shaderProgram, "hasColor");
	glUniform1i(loc, hasColor);
	loc = glGetUniformLocation(state.shaderProgram, "hasMaterial");
	glUniform1i(loc, hasMaterial);
	loc = glGetUniformLocation(state.shaderProgram, "color");
	glUniform4f(loc, color.r, color.g, color.b, color.a);
    loc = glGetUniformLocation(state.shaderProgram, "hasTexture");
	glUniform1i(loc, hasTexture);
}

}