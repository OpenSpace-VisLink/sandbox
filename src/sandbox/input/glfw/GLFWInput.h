#ifndef SANDBOX_INPUT_GLFW_GLFW_INPUT_H_
#define SANDBOX_INPUT_GLFW_GLFW_INPUT_H_

#include "glm/glm.hpp"
#include "sandbox/Component.h"
#include <iostream>

#include <GLFW/glfw3.h>

namespace sandbox {

class GLFWMouseInput : public MouseInput {
public:
	GLFWMouseInput() {
		addType<GLFWMouseInput>();
		buttons[0] = false;
		buttons[1] = false;
		buttons[2] = false;
	}
	bool getButtonState(int button) const { return button >= 0 && button < 3 ? buttons[button] : false; }
	glm::vec2 getPosition() const {
		return position;
	}

	bool buttons[3];
	glm::vec2 position;
};

class GLFWInput : public Component {
public:
	GLFWInput(GLFWwindow* window) {
		addType<GLFWInput>();
		glfwSetWindowUserPointer(window, this);
		glfwGetWindowSize(window, &width, &height);
    	glfwSetMouseButtonCallback(window, GLFWInput::glfw_mouse_button_callback);
    	glfwSetCursorPosCallback(window, GLFWInput::glfw_cursor_position_callback);
		glfwSetWindowSizeCallback(window, glfw_size_callback);
	}

	void afterAdd() {
		GLFWMouseInput* currentMouseInput = getEntity().getComponent<GLFWMouseInput>();
		if (!currentMouseInput) {
    		mouseInput = new GLFWMouseInput();
			getEntity().addComponent(mouseInput);
		}
		else {
			mouseInput = currentMouseInput;
		}
	}

private:
    void cursorPositionCallback(GLFWwindow* window, float xpos, float ypos) {
    	mouseInput->position = glm::vec2(xpos/width, 1.0-ypos/height);
    }
  	void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
  		bool* buttonPtr = NULL;
	    std::string buttonStr;
	    if (button == GLFW_MOUSE_BUTTON_LEFT) {
	        buttonPtr = &mouseInput->buttons[0];
	    }
	    else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
	        buttonPtr = &mouseInput->buttons[1];
	    }
	    else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
	        buttonPtr = &mouseInput->buttons[2];
	    }

	    if (buttonPtr) {
	    	*buttonPtr = action == GLFW_PRESS;
	    }
  	}

  	void sizeCallback(GLFWwindow* window, int width, int height) {
  		this->width = width;
  		this->height = height;
  	}

  	static void glfw_cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    	((GLFWInput*)(glfwGetWindowUserPointer(window)))->cursorPositionCallback(window, (float)xpos, (float)ypos);
	}

	static void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
		((GLFWInput*)(glfwGetWindowUserPointer(window)))->mouseButtonCallback(window, button, action, mods);
	}

	static void glfw_size_callback(GLFWwindow* window, int width, int height) {
		((GLFWInput*)(glfwGetWindowUserPointer(window)))->sizeCallback(window, width, height);
	}

	GLFWMouseInput* mouseInput;
	int width, height;

};


}

#endif