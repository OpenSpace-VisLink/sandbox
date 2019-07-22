#ifndef SANDBOX_GRAPHICS_MESH_RENDERER_H_
#define SANDBOX_GRAPHICS_MESH_RENDERER_H_

#include "sandbox/graphics/GraphicsComponent.h"
#include "sandbox/geometry/Mesh.h"
#include "OpenGL.h"

namespace sandbox {

class MeshRenderer : public GraphicsComponent {
public:
	MeshRenderer(GLuint renderType = GL_TRIANGLES);
	virtual ~MeshRenderer() {}

	void update();
	void updateSharedContext(const GraphicsContext& sceneContext);
	void updateContext(const GraphicsContext& sceneContext);
	void render(const GraphicsContext& sceneContext);

private:
	class MeshSharedState : public ContextState {
	public:
	    virtual ~MeshSharedState() {
	    	if (initialized) {
		        glDeleteBuffers(1, &vbo);
		        glDeleteBuffers(1, &elementBuffer);
	    	}
	    }

	    GLuint vbo;
	    GLuint elementBuffer;
	};

	class MeshState : public ContextState {
	public:
	    virtual ~MeshState() {
	    	if (initialized) {
	        	glDeleteVertexArrays(1, &vao);
	        }
	    }

	    GLuint vao;
	};

	Mesh* mesh;
	GraphicsContextHandler<MeshSharedState,MeshState> contextHandler;
	GLuint renderType;
};

}

#endif