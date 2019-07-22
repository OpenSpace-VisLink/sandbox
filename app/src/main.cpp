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
#include "sandbox/base/Object.h"
#include "sandbox/geometry/shapes/QuadLoader.h"
#include "sandbox/graphics/GraphicsContextRenderer.h"
#include "sandbox/graphics/RenderCallback.h"
#include "sandbox/graphics/render/EntityRenderer.h"
#include "sandbox/graphics/render/MeshRenderer.h"
#include "sandbox/graphics/render/shaders/MaterialShader.h"
#include "sandbox/graphics/view/Camera.h"

using namespace sandbox;

class TestApp : public nanogui::Screen {
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

		EntityNode* quad = new EntityNode(&objects);
			quad->addComponent(new sandbox::Object<Mesh>());
			quad->addComponent(new QuadLoader());
			quad->addComponent(new MeshRenderer());

		EntityNode* shader = new EntityNode(&shaders);
			shader->addComponent(new MaterialShader());

		EntityNode* view = new EntityNode(&scene);
			view->addComponent(new Camera());
			//view->addComponent(new MaterialShader());
			view->addComponent(new EntityRenderer(shader));
			view->addComponent(new EntityRenderer(quad));

		renderer.addComponent(new GraphicsContextRenderer());
		renderer.addComponent((new OpenGLCallback())->init(this));
		renderer.addChild(new EntityReference(&objects));
		renderer.addChild(new EntityReference(&shaders));
		renderer.addChild(new EntityReference(&scene));
	}

	void drawContents() {
		objects.update();
		renderer.update();
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
            glEnable(GL_CULL_FACE);
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


