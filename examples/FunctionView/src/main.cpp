#include <iostream>

#include "sandbox/external/NanoGUI.h"
#include <nanogui/opengl.h>
#include <nanogui/window.h>
#include <nanogui/layout.h>
#include <nanogui/slider.h>
#include <nanogui/combobox.h>
#include <nanogui/textbox.h>
#include <nanogui/glcanvas.h>
#include <nanogui/opengl.h>
#include <nanogui/glutil.h>
#include "sandbox/SceneNode.h"
#include "sandbox/base/RenderCallback.h"
#include "sandbox/base/Transform.h"
#include "sandbox/base/Camera.h"
#include "sandbox/base/NodeRenderer.h"
#include "sandbox/geometry/algorithms/SmoothNormals.h"
#include "sandbox/geometry/shapes/Grid.h"
#include "sandbox/geometry/Material.h"
#include "sandbox/graphics/MeshRenderer.h"
#include "sandbox/graphics/shaders/MaterialShader.h"
#include "sandbox/graphics/Viewport.h"
#include "sandbox/graphics/Window.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace sandbox;

class TestApp : public NanoguiScreen {
public:
	TestApp() : NanoguiScreen(Eigen::Vector2i(1024, 768), "Test App") {
		using namespace nanogui;

		Grid* grid = new Grid(30, 30);
		
		SceneNode* eventNode = new SceneNode(&scene);
			NanoguiResizeEvent* resize = new NanoguiResizeEvent(this);
			eventNode->addComponent(resize);
			ResizeEventHandler* resizeCallback = (new ResizeCallback())->init(this);
			resize->subscribe(resizeCallback);
			eventNode->addComponent(resizeCallback);
		SceneNode* geometryNode = new SceneNode(&scene);
			SceneNode* gridNode = new SceneNode(geometryNode);
				gridNode->addComponent(grid);
				gridNode->addComponent(new SmoothNormals());
				gridNode->addComponent(new MeshRenderer());
				//gridNode->addComponent(new Material());
		graphicsNode = new SceneNode(&scene);
			graphicsNode->addComponent(new sandbox::Window(eventNode));
			graphicsNode->addComponent((new OpenGLCallback())->init(this));
			SceneNode* functionViewNode = new SceneNode(graphicsNode);				
				functionViewNode->addComponent(new PercentViewport(glm::vec4(0.0, 0.0, 0.5, 1.0)));
				functionViewNode->addComponent(new Transform(glm::translate(glm::mat4(1.0f),glm::vec3(4,1,3))));
				functionViewNode->addComponent(new Camera());
				SceneNode* functionNode = new SceneNode(functionViewNode);
					functionNode->addComponent(new MaterialShader());
					functionNode->addComponent(new Transform(glm::rotate(glm::mat4(1.0f), -3.1415f/4.0f, glm::vec3(1.0f,0.0,0))*glm::scale(glm::mat4(1.0f), glm::vec3(1.5f,1.5,1.5))));
					functionNode->addComponent(new NodeRenderer(gridNode));
			functionViewNode = new SceneNode(graphicsNode);				
				functionViewNode->addComponent(new PercentViewport(glm::vec4(0.5, 0.0, 0.5, 1.0)));
				functionViewNode->addComponent(new Transform(glm::translate(glm::mat4(1.0f),glm::vec3(4,1,3))));
				functionViewNode->addComponent(new Camera());
				functionNode = new SceneNode(functionViewNode);
					functionNode->addComponent(new MaterialShader());
					functionNode->addComponent(new Transform(glm::rotate(glm::mat4(1.0f), -3.1415f/4.0f, glm::vec3(1.0f,0.0,0))*glm::scale(glm::mat4(1.0f), glm::vec3(1.0f,1.0,1.0))));
					functionNode->addComponent(new NodeRenderer(gridNode));


		for (int x = 0; x < grid->getWidth(); x++) {
			for (int y = 0; y < grid->getHeight(); y++) {
				//grid->getNode(x,y).z = 0.5*std::sin(2.0f*3.1415*y/grid->getHeight())*std::sin(2.0f*3.1415*x/grid->getWidth());
				grid->getNode(x,y).z = 0.5*std::cos(2.0f*3.1415*y/grid->getHeight())*std::cos(2.0f*3.1415*x/grid->getWidth());//*std::sin(2.0f*3.1415*x/grid->getWidth());
			}
		}
		

    	resizeEvent(Eigen::Vector2i(width(), height()));
	}

	~TestApp() {
	}


	void drawContents() {
		scene.updateModel();
		scene.updateSharedContext(context);
		scene.updateContext(context);
		graphicsNode->render(context);
	}

private:

	class ResizeCallback : public StateEventHandlerCallback<ResizeState, TestApp> {
		void onEvent(const ResizeState& state, TestApp* app) {
			std::cout << state.width << " " << state.height << " " << app << std::endl;
		}
	};

	class OpenGLCallback : public RenderCallback<TestApp> {
		void renderCallback(const SceneContext& sceneContext, TestApp* app) {
			//std::cout << "Clear screen" << std::endl;
			glClearColor(0.75,0.75,0.75,1);
            //glClearDepth(1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            //glDepthFunc(GL_LEQUAL);
            glPatchParameteri(GL_PATCH_VERTICES, 3);
            glEnable(GL_CULL_FACE);
            glDisable(GL_BLEND);
            glCullFace(GL_BACK);
            glDisable(GL_CULL_FACE);
		}
	};

	SceneContext context;
	SceneNode scene;
	SceneNode* graphicsNode;
};

int main(int argc, char**argv) {
	nanogui::init();

	nanogui::Screen* screen = new TestApp();

	screen->performLayout();
	screen->drawAll();
	screen->setVisible(true);

	nanogui::mainloop();
	nanogui::shutdown();

	return 0;
}


