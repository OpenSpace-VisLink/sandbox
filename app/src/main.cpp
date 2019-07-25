#include <iostream>

#include <nanogui/nanogui.h>
#include <nanogui/opengl.h>
#include <nanogui/window.h>
#include <nanogui/layout.h>
#include <nanogui/slider.h>
#include <nanogui/textbox.h>
#include <nanogui/glcanvas.h>
#include <nanogui/opengl.h>
#include <nanogui/glutil.h>
#include "sandbox/base/EntityComponent.h"
#include "sandbox/base/Object.h"
#include "sandbox/base/Transform.h"
#include "sandbox/geometry/loaders/ShapeLoader.h"
#include "sandbox/graphics/GraphicsContextRenderer.h"
#include "sandbox/graphics/RenderCallback.h"
#include "sandbox/graphics/render/EntityRenderer.h"
#include "sandbox/graphics/render/MeshRenderer.h"
#include "sandbox/graphics/render/shaders/MaterialShader.h"
#include "sandbox/graphics/render/shaders/BasicShader.h"
#include "sandbox/graphics/view/Camera.h"
#include "sandbox/input/MouseInput.h"
#include "sandbox/input/NanoGUIMouseInput.h"
#include "sandbox/input/interaction/MouseInteraction.h"
#include "sandbox/input/interaction/VirtualCursor.h"
//#include "sandbox/input/interaction/ArcBall.h"
//#include "sandbox/input/interaction/MouseZoom.h"
//#include "sandbox/input/interaction/MouseTranslate.h"
#include "sandbox/io/File.h"
#include "sandbox/io/FileMonitor.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace sandbox;

class TestApp : public nanogui::Screen {
friend class NanoGUIMouseInput<TestApp>;
public:


	TestApp() : nanogui::Screen(Eigen::Vector2i(1024, 768), "Test App"), r(0), g(0), b(0), a(1) {
		using namespace nanogui;
		Window* window = new Window(this);
		window->setTitle("Window");
		window->setLayout(new GroupLayout());
		window->setPosition(Vector2i(15, 15));

		Widget* panel = new Widget(window);
		panel->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 0));

		addVariableSlider(panel, r, "Red");
		addVariableSlider(panel, g, "Green");
		addVariableSlider(panel, b, "Blue");

		files.addComponent(new FileMonitor(1000));
		EntityNode* defaultVsh = new EntityNode(&files);
			defaultVsh->addComponent(new File("app/src/shaders/default.vsh"));
		EntityNode* defaultFsh = new EntityNode(&files);
			defaultFsh->addComponent(new File("app/src/shaders/default.fsh"));
		EntityNode* textFile = new EntityNode(&files);
			//textFile->addComponent(new File("CMakeLists.txt"));
			//imageFile->addComponent(new File());

		EntityNode* quad = new EntityNode(&objects);
			quad->addComponent(new sandbox::Object<Mesh>());
			quad->addComponent(new ShapeLoader(SHAPE_QUAD));
			quad->addComponent(new MeshRenderer());
		EntityNode* cylindar = new EntityNode(&objects);
			cylindar->addComponent(new sandbox::Object<Mesh>());
			cylindar->addComponent(new ShapeLoader(SHAPE_CYLINDAR, 20));
			cylindar->addComponent(new MeshRenderer());

		EntityNode* defaultShader = new EntityNode(&shaders);
			defaultShader->addComponent(new BasicShader());
			defaultShader->addComponent(new EntityComponent(defaultVsh));
			defaultShader->addComponent(new EntityComponent(defaultFsh));
		EntityNode* materialShader = new EntityNode(&shaders);
			materialShader->addComponent(new MaterialShader());

		input.addComponent(new NanoGUIMouseInput<TestApp>(this));
		VirtualCursor* vc = new VirtualCursor(&input);
		input.addComponent(vc);


		EntityNode* view = new EntityNode(&scene);
			//view->addComponent(new EntityComponent(textFile));
			view->addComponent(new Transform(glm::translate(glm::mat4(1.0f),glm::vec3(0,0,3))));
			view->addComponent(new Camera());
			//view->addComponent(new EntityRenderer(defaultShader));
			view->addComponent(new EntityRenderer(materialShader));
			EntityNode* cursor = new EntityNode(view);
				cursor->addComponent(new EntityRenderer(vc->getVirtualCursor()));
				EntityNode* cursorModel = new EntityNode(cursor);
					glm::mat4 cursorTransform = glm::translate(glm::mat4(1.0),glm::vec3(0,0,0.25));
					cursorTransform = glm::rotate(cursorTransform, 3.14159f/2.0f,glm::vec3(1.0,0,0));
					cursorTransform = glm::scale(cursorTransform,glm::vec3(0.1,0.5,0.1));
					cursorModel->addComponent(new Transform(cursorTransform));
					cursorModel->addComponent(new EntityRenderer(defaultShader));
					//cursorModel->addComponent(new EntityRenderer(cylindar));
			EntityNode* world = new EntityNode(view);
				world->addComponent(new MouseInteraction(&input));
				world->addComponent(new EntityRenderer(quad));


		renderer.addComponent(new GraphicsContextRenderer());
		renderer.addComponent((new OpenGLCallback())->init(this));
		renderer.addChild(new EntityReference(&objects));
		renderer.addChild(new EntityReference(&shaders));
		renderer.addChild(new EntityReference(&scene));

		files.update();
		objects.update();
	}

	void drawContents() {
		input.update();
		/*MouseInput* mouse = input.getComponent<MouseInput>();
		if (mouse) {
			if (mouse->isDragging()) {
				std::cout << mouse->getButtonState(0) << " " << mouse->getPosition().x << " " << mouse->getPosition().y << std::endl;
			}
		}*/
		renderer.update();
		renderer.getComponent<GraphicsContextRenderer>()->render();
	}


private:
	class OpenGLCallback : public RenderCallback<TestApp> {
		void renderCallback(const GraphicsContext& sceneContext, TestApp* app) {
			//std::cout << "Clear screen" << std::endl;
			glClearColor(app->r,app->g,app->b,1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);
            glClearDepth(1.0f);
            glDepthFunc(GL_LEQUAL);
            glPatchParameteri(GL_PATCH_VERTICES, 3);
            //glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
		}
	};

	void addVariableSlider(Widget* parent, float& var, const std::string& fieldName, const std::function<void(float)>& lambda = [](float value) {}) {
		using namespace nanogui;
		Label* label = new Label(parent, fieldName);
		Slider* slider = new Slider(parent);
		slider->setFixedWidth(80);
		slider->setValue(var);
		slider->setCallback([&var, lambda](float value) {
			var = value;
			lambda(value);
		});
	}

	float r, g, b, a;

	EntityNode renderer;
	EntityNode objects;
	EntityNode shaders;
	EntityNode scene;
	EntityNode files;
	EntityNode input;
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



