#include "sandbox/base/Camera.h"
#include "sandbox/graphics/RenderState.h"
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

namespace sandbox {

Camera::Camera() : projection(1.0f), view(1.0f) {
	addType<Camera>();
}

Camera::~Camera() {
}

void Camera::updateModel() {
		// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
		projection = glm::perspective(glm::radians(45.0f), (float)1024  / (float)768, 0.1f, 100.0f);
		  
		// Or, for an ortho camera :
		//projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates
		  
		glm::vec3 pos = getSceneNode().getWorldPosition();

		// Camera matrix
		view = glm::lookAt(
		    //glm::vec3(4,3,3), // Camera is at (4,3,3), in World Space
		    pos,
		    glm::vec3(0,0,0), // and looks at the origin
		    glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
		    );

}

void Camera::render(const SceneContext& sceneContext) {
	RenderState& renderState = RenderState::get(sceneContext);
	renderState.getProjectionMatrix().push(projection);
	renderState.getViewMatrix().push(view);
	renderState.getModelMatrix().push(glm::mat4(1.0f));
}

void Camera::finishRender(const SceneContext& sceneContext) {
	RenderState& renderState = RenderState::get(sceneContext);
	renderState.getProjectionMatrix().pop();
	renderState.getViewMatrix().pop();
	renderState.getModelMatrix().pop();
}

}