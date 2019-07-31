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
	void startRender(const GraphicsContext& sceneContext);

private:
	class MeshSharedState : public ContextState {
	public:
		MeshSharedState() : version(0) {}
	    virtual ~MeshSharedState() {
	    	reset();
	    }

	    void reset() {
	    	if (initialized) {
		        glDeleteBuffers(1, &vbo);
		        glDeleteBuffers(1, &elementBuffer);
	    	}
	    }

	    GLuint vbo;
	    GLuint elementBuffer;
	    int version;
	};

	class MeshState : public ContextState {
	public:
		MeshState() : version(0) {}
	    virtual ~MeshState() {
	    	reset();
	    }

	    void reset() {
	    	if (initialized) {
		        glDeleteVertexArrays(1, &vao);
	    	}
	    }

	    GLuint vao;
	    int version;
	};

	Mesh* mesh;
	GraphicsContextHandler<MeshSharedState,MeshState> contextHandler;
	GLuint renderType;
	int version;
};

}

#endif