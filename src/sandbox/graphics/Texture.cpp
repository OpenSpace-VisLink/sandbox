#include "sandbox/graphics/Texture.h"
#include <iostream>
#include "sandbox/graphics/RenderState.h"

namespace sandbox {

Texture::Texture() : image(nullptr) {
	addType<Texture>();
}

void Texture::updateModel() {
	if (!image) {
		image = getSceneNode().getComponent<Image>();
	}
}

void Texture::updateSharedContext(const SceneContext& sceneContext) {
	TextureSharedState& state = *contextHandler.getSharedState(sceneContext);

	if (image && !state.initialized) {
	    std::cout << "INitialize texture shared context " << std::endl;
	    GLuint format = getFormat();
		GLuint internalFormat = getInternalFormat();
		GLuint type = getType();

		state.target = GL_TEXTURE_2D;
		glGenTextures(1, &state.texture);
		glBindTexture(state.target, state.texture);
		glTexImage2D(state.target, 0, internalFormat, image->getWidth(), image->getHeight(), 0, format, type, image->getData());
	    state.initialized = true;
	}
	else if (!image && state.initialized) {
		glDeleteTextures(1, &state.texture);
	    state.initialized = false;
	}
}

void Texture::updateContext(const SceneContext& sceneContext) {
	TextureSharedState& sharedState = *contextHandler.getSharedState(sceneContext);
	ContextState& state = *contextHandler.getState(sceneContext);

	if (image && !state.initialized) {
        std::cout << "INitialize texture context" << std::endl;
        glBindTexture(sharedState.target, sharedState.texture);
		glTexParameteri(sharedState.target, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(sharedState.target, GL_TEXTURE_MAX_LEVEL, 0);
		glTexParameteri(sharedState.target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(sharedState.target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(sharedState.target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(sharedState.target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(sharedState.target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	    state.initialized = true;
	}
	else if (!image && state.initialized) {
		state.initialized = false;
	}
}

GLuint Texture::getTarget(const SceneContext& sceneContext) {
	TextureSharedState& sharedState = *contextHandler.getSharedState(sceneContext);
	return sharedState.target;
}

GLuint Texture::getId(const SceneContext& sceneContext) {
	TextureSharedState& sharedState = *contextHandler.getSharedState(sceneContext);
	return sharedState.texture;

}

/*
void SBTexture::renderSpecific(Context& sharedContext, Context& context, SBTextureSharedState& sharedState, ContextState& state) const {
	glActiveTexture(GL_TEXTURE0+0);
	glBindTexture(sharedState.target, sharedState.texture);

	GLuint shaderProgram = ShaderProgram::getCurrentProgramId(sharedContext, context);
	GLint loc = glGetUniformLocation(shaderProgram, "tex");
	glUniform1i(loc, 0);

}
*/

void Texture::render(const SceneContext& sceneContext) {
	/*MeshState& state = *contextHandler.getState(sceneContext);

	if (state.initialized) {
		//std::cout << "Render Mesh" << state.vao << " " << mesh->getIndices().size() << std::endl;
	    glBindVertexArray(state.vao);

	    RenderState& renderState = RenderState::get(sceneContext);
	    ShaderProgram* shader = renderState.getShaderProgram().get();
	    if (shader) {
	    	shader->use(sceneContext);
	    }

	    //glDrawElements(GL_PATCHES, mesh.indices.size(), GL_UNSIGNED_INT, (void*)0);
	    //glDrawElements(GL_TRIANGLES, mesh->getIndices().size(), GL_UNSIGNED_INT, (void*)0);
		glDrawElementsInstancedBaseVertex(GL_TRIANGLES,
				mesh->getIndices().size(),
				GL_UNSIGNED_INT,
				(void*)(sizeof(unsigned int) * 0),
				1, //numInstances,
				0);
	    glBindVertexArray(0);

	    if (shader) {
	    	shader->release(sceneContext);
	    }
	}*/
}

GLuint Texture::getFormat() const {
	return GL_RGBA;
}

GLuint Texture::getInternalFormat() const {
	return GL_RGBA;
}

GLuint Texture::getType() const {
	return GL_UNSIGNED_BYTE;
}

}