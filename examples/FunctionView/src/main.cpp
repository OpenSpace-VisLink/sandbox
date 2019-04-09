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
#include "sandbox/geometry/shapes/Cylinder.h"
#include "sandbox/geometry/shapes/Grid.h"
#include "sandbox/geometry/Material.h"
#include "sandbox/graphics/MeshRenderer.h"
#include "sandbox/graphics/shaders/MaterialShader.h"
#include "sandbox/graphics/Viewport.h"
#include "sandbox/graphics/Window.h"
#include "sandbox/graphics/RenderState.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdlib>

using namespace sandbox;

float function(float x, float y) {
	return 0.5*std::cos(2.0f*3.1415*y)*std::cos(2.0f*3.1415*x);
}

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
			SceneNode* arrowNode = new SceneNode(geometryNode);
				//functionNode->addComponent(new Transform(glm::translate(glm::mat4(1.0f),glm::vec3(0.5,0.0,0.0))));
				//arrowNode->addComponent(new Transform(glm::translate(glm::mat4(1.0f),glm::vec3(-0.75,0,0))));
				arrowNode->addComponent(new Transform(glm::translate(glm::mat4(1.0f),glm::vec3(0,2.0,0))));
				arrowNode->addComponent(new Cylinder(20, 1.0f, 0.0f));
				arrowNode->addComponent(new MeshRenderer());
				arrowNode->addComponent(new Material());
				//arrowNode->addComponent((new TestCallback("arrow"))->init(this));
				SceneNode* cylNode = new SceneNode(arrowNode);
					glm::mat4 cylTrans = glm::translate(glm::mat4(1.0f),glm::vec3(0,-1.0,0));
					cylTrans = glm::scale(cylTrans,glm::vec3(0.5f, 1.0f, 0.5f));
					cylNode->addComponent(new Transform(cylTrans));
					cylNode->addComponent(new Cylinder(20));
					//cylNode->addComponent((new TestCallback("cyl"))->init(this));
					cylNode->addComponent(new MeshRenderer());
					cylNode->addComponent(new Material());
			SceneNode* gridNode = new SceneNode(geometryNode);
				gridNode->addComponent(grid);
				gridNode->addComponent(new SmoothNormals());
				gridNode->addComponent(new MeshRenderer());
				//gridNode->addComponent(new Material());
			SceneNode* sampleNode = new SceneNode(geometryNode);
				Mesh* sampleMesh = new Mesh();
				sampleNode->addComponent(sampleMesh);
				sampleNode->addComponent(new MeshRenderer(GL_POINTS));
				Material* sampleMaterial = new Material();
				sampleMaterial->setColor(glm::vec4(1.0f,0,0,1));
				sampleNode->addComponent(sampleMaterial);
		graphicsNode = new SceneNode(&scene);
			graphicsNode->addComponent(new sandbox::Window(eventNode));
			graphicsNode->addComponent((new OpenGLCallback())->init(this));
			SceneNode* functionViewNode = new SceneNode(graphicsNode);				
				functionViewNode->addComponent(new PercentViewport(glm::vec4(0.0, 0.0, 0.5, 1.0)));
				functionViewNode->addComponent(new Transform(glm::translate(glm::mat4(1.0f),glm::vec3(4,3,3))));
				functionViewNode->addComponent(new Camera());
				functionViewNode->addComponent(new MaterialShader());
				SceneNode* functionNode = new SceneNode(functionViewNode);
					functionNode->addComponent(new Transform(glm::rotate(glm::mat4(1.0f), -3.1415f/2.0f, glm::vec3(1.0f,0.0,0))*glm::scale(glm::mat4(1.0f), glm::vec3(2.0f))));
					functionNode->addComponent(new NodeRenderer(gridNode));
				SceneNode* pointsNode = new SceneNode(functionViewNode);
					pointsNode->addComponent(new Transform(glm::rotate(glm::mat4(1.0f), -3.1415f/2.0f, glm::vec3(1.0f,0.0,0))*glm::scale(glm::mat4(1.0f), glm::vec3(2.0f))));
					pointsNode->addComponent(new NodeRenderer(sampleNode));
			functionViewNode = new SceneNode(graphicsNode);				
				functionViewNode->addComponent(new PercentViewport(glm::vec4(0.5, 0.5, 0.5, 0.5)));
				functionViewNode->addComponent(new Transform(glm::translate(glm::mat4(1.0f),glm::vec3(0,0,3))));
				functionViewNode->addComponent(new Camera());
				functionNode = new SceneNode(functionViewNode);
					functionNode->addComponent(new Transform(glm::scale(glm::mat4(1.0f),glm::vec3(0.5f))));
					//functionNode->addComponent(new Transform(glm::translate(glm::mat4(1.0f),glm::vec3(0.5,0.0,0.0))));
					functionNode->addComponent(new MaterialShader());
					functionNode->addComponent(new NodeRenderer(arrowNode));
			functionViewNode = new SceneNode(graphicsNode);				
				functionViewNode->addComponent(new PercentViewport(glm::vec4(0.5, 0.0, 0.5, 0.5)));
				functionViewNode->addComponent(new Transform(glm::translate(glm::mat4(1.0f),glm::vec3(4,1,3))));
				functionViewNode->addComponent(new Camera());
				functionNode = new SceneNode(functionViewNode);
					functionNode->addComponent(new MaterialShader());
					functionNode->addComponent(new Transform(glm::rotate(glm::mat4(1.0f), -3.1415f/2.0f, glm::vec3(1.0f,0.0,0))*glm::scale(glm::mat4(1.0f), glm::vec3(1.5f,1.5,1.5))));
					functionNode->addComponent(new NodeRenderer(gridNode));


		for (int x = 0; x < grid->getWidth(); x++) {
			for (int y = 0; y < grid->getHeight(); y++) {
				//grid->getNode(x,y).z = 0.5*std::sin(2.0f*3.1415*y/grid->getHeight())*std::sin(2.0f*3.1415*x/grid->getWidth());
				grid->getNode(x,y).z = function(1.0f*x/grid->getWidth(), 1.0f*y/grid->getHeight());//*std::sin(2.0f*3.1415*x/grid->getWidth());
			}
		}

		std::vector<glm::vec3> samplePoints;
		std::vector<glm::vec3> sampleNormals;
		std::vector<unsigned int> sampleIndices;
		for (int f = 0; f < 100; f++) {
			float x = float(std::rand())/RAND_MAX;
			float y = float(std::rand())/RAND_MAX;
			float z = function(x, y);
			samplePoints.push_back(glm::vec3(x-0.5, y-0.5, z));
			sampleNormals.push_back(glm::vec3(0.0f,0.0f,1.0f));
			sampleIndices.push_back(f);
		}
		sampleMesh->setNodes(samplePoints);
		sampleMesh->setNormals(sampleNormals);
		sampleMesh->setIndices(sampleIndices);

		for (int f = 0; f < samplePoints.size(); f++) {
			SceneNode* pointNode = new SceneNode(pointsNode);
			pointNode->addComponent(new Transform(glm::translate(glm::mat4(1.0f),samplePoints[f])*glm::scale(glm::mat4(1.0f),glm::vec3(0.02,0.075,0.02)*0.5f)));
			pointNode->addComponent(new NodeRenderer(arrowNode));
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
			glPointSize(10);
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

	class TestCallback : public RenderCallback<TestApp> {
	public:
		TestCallback(std::string name) : name(name) {}
		void renderCallback(const SceneContext& sceneContext, TestApp* app) {
			RenderState& renderState = RenderState::get(sceneContext);
			glm::mat4 model = renderState.getModelMatrix().get();
			glm::vec3 v(1.0);
			//glm::vec3 v(0.0f);
			v = model*glm::vec4(v, 1.0);
			std::cout << name << ": " << v.x << " " << v.y << " " << v.z << std::endl;
		}
		std::string name;
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


