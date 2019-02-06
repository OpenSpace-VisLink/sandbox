#ifndef SANDBOX_GRAPHICS_MESH_RENDERER_H_
#define SANDBOX_GRAPHICS_MESH_RENDERER_H_

#include "sandbox/SceneComponent.h"
#include "sandbox/geometry/Mesh.h"
#include "OpenGL.h"

namespace sandbox {

class MeshRenderer : public SceneComponent {
public:
	MeshRenderer();
	virtual ~MeshRenderer() {}

	void updateModel();
	void updateSharedContext(const SceneContext& sceneContext);
	void updateContext(const SceneContext& sceneContext);
	void render(const SceneContext& sceneContext);

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
	SceneContextHandler<MeshSharedState,MeshState> contextHandler;
};

}

#endif