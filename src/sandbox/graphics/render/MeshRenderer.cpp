#include "sandbox/graphics/render/MeshRenderer.h"
#include "sandbox/graphics/RenderState.h"
#include "sandbox/base/Object.h"

namespace sandbox {

MeshRenderer::MeshRenderer(GLuint renderType) : mesh(nullptr), renderType(renderType), version(-1) {
	addType<MeshRenderer>();
}

void MeshRenderer::update() {
	std::cout << "mesh renderer item" << std::endl;
	if (!mesh) {
		mesh = &getEntity().getComponent< Object<Mesh> >()->get();
	}

	version++;
}

void MeshRenderer::updateSharedContext(const GraphicsContext& context) {
	MeshSharedState& state = *contextHandler.getSharedState(context);

	if (state.initialized && state.version != version) {
        state.reset();
        state.initialized = false;
        state.version = version;
    }

	if (mesh && !state.initialized) {
	    std::cout << "INitialize mesh shared context " << std::endl;
	    glGenBuffers(1, &state.elementBuffer);
	    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, state.elementBuffer);
	    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->getIndices().size() * sizeof(unsigned int), &mesh->getIndices()[0], GL_STATIC_DRAW);

		glGenBuffers(1, &state.vbo);
	    glBindBuffer(GL_ARRAY_BUFFER, state.vbo);
	    glBufferData(GL_ARRAY_BUFFER, 3*sizeof(float)*(mesh->getNodes().size() + mesh->getNormals().size()) + 2*sizeof(float)*mesh->getCoords().size(), 0, GL_DYNAMIC_DRAW);
	    glBufferSubData(GL_ARRAY_BUFFER, 0, 3*sizeof(float)*(mesh->getNodes().size()), &mesh->getNodes()[0]);
	    glBufferSubData(GL_ARRAY_BUFFER, 3*sizeof(float)*mesh->getNodes().size(), 3*sizeof(float)*mesh->getNormals().size(), &mesh->getNormals()[0]);
	    glBufferSubData(GL_ARRAY_BUFFER, 3*sizeof(float)*mesh->getNodes().size()+3*sizeof(float)*mesh->getNormals().size(), 2*sizeof(float)*mesh->getCoords().size(), &mesh->getCoords()[0]);
	    state.initialized = true;
	    //state.version = version;
	}
	else if (!mesh && state.initialized) {
		glDeleteBuffers(1, &state.vbo);
	    glDeleteBuffers(1, &state.elementBuffer);
	    state.initialized = false;
	}

	/*if (!sharedState.initialized) {

	    // Allocate space and send Vertex Data
	    glGenBuffers(1, &sharedState.vbo);
	    glBindBuffer(GL_ARRAY_BUFFER, sharedState.vbo);
	    glBufferData(GL_ARRAY_BUFFER, 3*sizeof(float)*(mesh.nodes.size() + mesh.normals.size()) + 2*sizeof(float)*mesh.coords.size(), 0, GL_DYNAMIC_DRAW);
	    glBufferSubData(GL_ARRAY_BUFFER, 0, 3*sizeof(float)*(mesh.nodes.size()), &mesh.nodes[0]);
	    glBufferSubData(GL_ARRAY_BUFFER, 3*sizeof(float)*mesh.nodes.size(), 3*sizeof(float)*mesh.normals.size(), &mesh.normals[0]);
	    glBufferSubData(GL_ARRAY_BUFFER, 3*sizeof(float)*mesh.nodes.size()+3*sizeof(float)*mesh.normals.size(), 2*sizeof(float)*mesh.coords.size(), &mesh.coords[0]);
	    //glBufferSubData(GL_ARRAY_BUFFER, 3*sizeof(float)*(mesh.nodes.size() + mesh.normals.size()), 3*sizeof(float)*mesh.colors.size(), &mesh.colors[0]);

	}

	if (!sharedState.initialized || sharedState.version != version) {
		glBindBuffer(GL_ARRAY_BUFFER, sharedState.vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 3*sizeof(float)*(mesh.nodes.size()), &mesh.nodes[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 3*sizeof(float)*mesh.nodes.size(), 3*sizeof(float)*mesh.normals.size(), &mesh.normals[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 3*sizeof(float)*mesh.nodes.size()+3*sizeof(float)*mesh.normals.size(), 2*sizeof(float)*mesh.coords.size(), &mesh.coords[0]);
		//glBufferSubData(GL_ARRAY_BUFFER, 3*sizeof(float)*(mesh.nodes.size() + mesh.normals.size()), 3*sizeof(float)*mesh.colors.size(), &mesh.colors[0]);
		sharedState.version = version;
	}*/
}

void MeshRenderer::updateContext(const GraphicsContext& context) {
	MeshSharedState& sharedState = *contextHandler.getSharedState(context);
	MeshState& state = *contextHandler.getState(context);

	if (state.initialized && state.version != version) {
        state.reset();
        state.initialized = false;
        state.version = version;
    }

	if (mesh && !state.initialized) {
        std::cout << "INitialize mesh context" << std::endl;
	    glGenVertexArrays(1, &state.vao);
	    glBindVertexArray(state.vao);
	    glBindBuffer(GL_ARRAY_BUFFER, sharedState.vbo);
	    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sharedState.elementBuffer);
	    glEnableVertexAttribArray(0);
	    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (char*)0);
	    glEnableVertexAttribArray(1);
	    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (char*)0 + 3*sizeof(float)*mesh->getNodes().size());
	    glEnableVertexAttribArray(2);
	    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), (char*)0 + 3*sizeof(float)*mesh->getNodes().size() + 3*sizeof(float)*mesh->getNormals().size());
	    glBindVertexArray(0);
	    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	    glBindBuffer(GL_ARRAY_BUFFER, 0);
	    state.initialized = true;
	}
	else if (!mesh && state.initialized) {
		glDeleteVertexArrays(1, &state.vao);
		state.initialized = false;
	}
}

void MeshRenderer::startRender(const GraphicsContext& context) {
	RenderState& renderState = RenderState::get(context);
	ShaderProgram* shader = renderState.getShaderProgram().get();
	if (!shader) {
		return;
	}

	MeshState& state = *contextHandler.getState(context);

	if (state.initialized) {
		//std::cout << "Render Mesh" << state.vao << " " << mesh->getIndices().size() << std::endl;
	    glBindVertexArray(state.vao);

	    RenderState& renderState = RenderState::get(context);
	    ShaderProgram* shader = renderState.getShaderProgram().get();
	    if (shader) {
	    	shader->use(context);
	    }

	    //glDrawElements(GL_PATCHES, mesh.indices.size(), GL_UNSIGNED_INT, (void*)0);
	    //glDrawElements(GL_TRIANGLES, mesh->getIndices().size(), GL_UNSIGNED_INT, (void*)0);
		glDrawElementsInstancedBaseVertex(renderType,//GL_TRIANGLES,
				mesh->getIndices().size(),
				GL_UNSIGNED_INT,
				(void*)(sizeof(unsigned int) * 0),
				1, //numInstances,
				0);
	    glBindVertexArray(0);

	    if (shader) {
	    	shader->release(context);
	    }
	}
}

}