#include "sandbox/graphics/shaders/Shader2D.h"

#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "sandbox/graphics/RenderState.h"
#include "sandbox/geometry/Material.h"
#include "sandbox/graphics/Texture.h"


namespace sandbox {

Shader2D::Shader2D() : hasColor(false), color(0.0) {
	addType<Shader2D>();
}

Shader2D::~Shader2D() {}

void Shader2D::create(const SceneContext& sceneContext, ShaderProgramState& state) {
	GLuint shader;

	std::string vertexShader =
		            "#version 330 \n"
		            "layout(location = 0) in vec3 position; "
		            "layout(location = 1) in vec3 normal; "
					"layout(location = 2) in vec2 coord; "
		            ""
		            "uniform float scale; "
		            "uniform mat4 ModelMatrix; "
		            "out vec2 pos; "
		            "out vec2 uv; "
		            ""
		            "void main() { "
		            "   pos = position.xy; "
		            "   uv = coord; "
		            "   gl_Position = ModelMatrix*vec4(pos, 0.0, 1.0); "
		            "}";

	state.addShader(compileShader(vertexShader, GL_VERTEX_SHADER));

    std::string fragmentShader =
            "#version 330 \n"
		    ""
		    "uniform bool hasTexture; "
            "uniform sampler2D tex; "
		    "uniform bool hasColor; "
		    "uniform vec4 color; "
		    "in vec2 pos; "
		    "in vec2 uv; "
		    ""
            "layout (location = 0) out vec4 colorOut;  "
            ""
            "void main() { "
            "	if (hasColor) { "
            "		colorOut = vec4(color); "
            "	} "
            "	else if (hasTexture) { "
            "    vec4 texColor = texture(tex, uv);"
            "	 colorOut = texture(tex, uv); "
            "	} "
            "	else { "
            "		colorOut = vec4(pos,0,0.5); "
            "	} "
            "}";
    state.addShader(compileShader(fragmentShader, GL_FRAGMENT_SHADER));
}

void Shader2D::setShaderParameters(const SceneContext& sceneContext, ShaderProgramState& state) {
	RenderState& renderState = RenderState::get(sceneContext);
	GLint loc = glGetUniformLocation(state.shaderProgram, "ModelMatrix");
	//glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(glm::scale(glm::mat4(1.0f),glm::vec3(0.5f))));
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(renderState.getModelMatrix().get()));
    loc = glGetUniformLocation(state.shaderProgram, "hasColor");
	glUniform1i(loc, hasColor);
	loc = glGetUniformLocation(state.shaderProgram, "color");
	glUniform4f(loc, color.r, color.g, color.b, color.a);

	const SceneNode* node = renderState.getSceneNode().get();
	bool hasTexture = false;
	Material* material = node->getComponent<Material>();
	if (material) {
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
	}

	loc = glGetUniformLocation(state.shaderProgram, "hasTexture");
	glUniform1i(loc, hasTexture);
}

}