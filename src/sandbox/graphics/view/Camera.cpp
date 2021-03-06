#include "sandbox/graphics/view/Camera.h"
#include "sandbox/graphics/RenderState.h"
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "sandbox/base/Transform.h"
#include "sandbox/graphics/render/shaders/ShaderParameterTypes.h"

namespace sandbox {

Camera::Camera() : projection(1.0f), view(1.0f) {
	addType<Camera>();
	shaderParameters.push_back(new ProjectionMatrixShaderParameter("ProjectionMatrix"));
	shaderParameters.push_back(new ViewMatrixShaderParameter("ViewMatrix"));
	shaderParameters.push_back(new ModelMatrixShaderParameter("ModelMatrix"));
	shaderParameters.push_back(new NormalMatrixShaderParameter("NormalMatrix"));
	shaderParameters.push_back(new EyePositionShaderParameter("eyePosition"));
}

Camera::~Camera() {
	for (int f = 0; f < shaderParameters.size(); f++) {
		delete shaderParameters[f];
	}
}

void Camera::update() {
		std::cout << "Update position." << std::endl;
		glm::vec3 pos = glm::vec3(0.0);
		Transform* transform = getEntity().getComponent<Transform>();
		if (transform) {
			pos = transform->getLocation();
		}

		// Camera matrix
		view = glm::lookAt(
		    //glm::vec3(4,3,3), // Camera is at (4,3,3), in World Space
		    //glm::vec3(0,0,3), // Camera is at (4,3,3), in World Space
		    pos,
		    glm::vec3(0,0,0), // and looks at the origin
		    glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
		    );

}

void Camera::startRender(const GraphicsContext& context) {
	RenderState& renderState = RenderState::get(context);

	glm::ivec4 viewport = renderState.getViewport().get();

	float aspectRatio = (float)viewport[2] / (float)viewport[3]; //1.0;//(float)1024  / (float)768;

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
	//glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates
	//projection = glm::ortho(-1.0f,1.0f,-1.0f,1.0f,0.0f,100.0f);

	renderState.getProjectionMatrix().push(projection);
	renderState.getViewMatrix().push(view);
	renderState.getModelMatrix().push(glm::mat4(1.0f));

	for (int f = 0; f < shaderParameters.size(); f++) {
		shaderParameters[f]->use(context);
	}
}

void Camera::finishRender(const GraphicsContext& context) {
	RenderState& renderState = RenderState::get(context);
	renderState.getProjectionMatrix().pop();
	renderState.getViewMatrix().pop();
	renderState.getModelMatrix().pop();

	for (int f = 0; f < shaderParameters.size(); f++) {
		shaderParameters[f]->release(context);
	}
}

}