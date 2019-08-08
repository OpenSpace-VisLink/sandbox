#include "sandbox/graphics/render/shaders/BasicShader.h"

#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "sandbox/graphics/RenderState.h"
//#include "sandbox/geometry/Material.h"
//#include "sandbox/graphics/Texture.h"
#include <iostream>
#include <fstream>
#include <sstream>

#include "sandbox/io/File.h"
#include "sandbox/base/EntityComponent.h"


namespace sandbox {

BasicShader::BasicShader() : texture(NULL) {
	addType<BasicShader>();
}

BasicShader::~BasicShader() {}

void BasicShader::create(const GraphicsContext& context, ShaderProgramState& state) {

	std::vector<EntityComponent*> entities = getEntity().getComponents<EntityComponent>();
	for (int f = 0; f < entities.size(); f++) {
		File* file = entities[f]->getEntityReference()->getComponent<File>();
		if (file) {
			std::string text = "";
			std::ifstream fIn;
			std::string path = file->getPath();
			fIn.open(path.c_str(),std::ios::in);

			if (fIn) {
				std::stringstream ss;
				ss << fIn.rdbuf();
				fIn.close();

				text =  ss.str();
			}

			GLuint shaderType = 0;
			if (path.find(".vsh") == path.size() - 4) {
				shaderType = GL_VERTEX_SHADER;
			}
			else if (path.find(".fsh") == path.size() - 4) {
				shaderType = GL_FRAGMENT_SHADER;
			}

			state.addShader(compileShader(text, shaderType));
		}

		Texture* tex = entities[f]->getEntityReference()->getComponent<Texture>();
		if (tex) {
			std::cout << tex << std::endl;
			texture = tex;
		}
	}

	/*std::string text = "";

	std::ifstream fIn;
	fIn.open(path.c_str(),std::ios::in);

	if (fIn) {
		std::stringstream ss;
		ss << fIn.rdbuf();
		fIn.close();

		text =  ss.str();
	}

	GLuint shaderType = 0;
	if (path.find(".vsh") == path.size() - 4) {
		shaderType = GL_VERTEX_SHADER;
	}
	else if (path.find(".fsh") == path.size() - 4) {
		shaderType = GL_FRAGMENT_SHADER;
	}*/


	/*std::string text = "";

	std::ifstream fIn;
	fIn.open(path.c_str(),std::ios::in);

	if (fIn) {
		std::stringstream ss;
		ss << fIn.rdbuf();
		fIn.close();

		text =  ss.str();
	}*/

	/*std::string vertexShader =
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
		            "   gl_Position = ProjectionMatrix*ViewMatrix*ModelMatrix*vec4(position, 1.0); "
		            "}";

	state.addShader(compileShader(vertexShader, GL_VERTEX_SHADER));

    std::string fragmentShader =
            "#version 330 \n"
            ""
		    "in vec3 pos; "
		    "in vec3 norm; "
		    "in vec2 uv; "
		    ""
            "layout (location = 0) out vec4 colorOut;  "
            ""
            "void main() { "
            "  vec3 n = normalize(norm); "
            "  colorOut = vec4(n,1); return; "
            ""
            "}";

    state.addShader(compileShader(fragmentShader, GL_FRAGMENT_SHADER));*/
}

void BasicShader::setShaderParameters(const GraphicsContext& context, ShaderProgramState& state) {
	RenderState& renderState = RenderState::get(context);
	GLint loc;

	Texture* tex = texture;

	/*std::vector<Texture*> textureQueue = renderState.getTexture().getQueue();
	if (textureQueue.size() > 1) {
		//tex = textureQueue[0];
		std::cout << textureQueue.size() << " " << textureQueue[1] << std::endl;
	}*/

	Texture* currentTexture = renderState.getTexture().get();
	if (currentTexture) {
		tex = currentTexture;
	}

	if (tex) {
		glActiveTexture(GL_TEXTURE0+0);
		glBindTexture(tex->getTarget(context), tex->getId(context));
		loc = glGetUniformLocation(state.shaderProgram, "tex");
		glUniform1i(loc, 0);
	}
}

}