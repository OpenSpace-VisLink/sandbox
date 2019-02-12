#include <iostream>

#include <nanogui/nanogui.h>
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
#include "sandbox/geometry/shapes/Quad.h"
#include "sandbox/geometry/MeshLoader.h"
#include "sandbox/graphics/MeshRenderer.h"
#include "sandbox/graphics/FloatDataRenderer.h"
#include "sandbox/graphics/shaders/MaterialShader.h"
#include "sandbox/graphics/shaders/Shader2D.h"
#include "sandbox/graphics/shaders/PointShader.h"
#include "sandbox/base/NodeRenderer.h"
#include "sandbox/base/Transform.h"
#include "sandbox/base/Camera.h"
#include "sandbox/data/FloatDataSet.h"
#include "sandbox/geometry/Material.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace sandbox;

class TestApp : public nanogui::Screen {
public:
	TestApp() : nanogui::Screen(Eigen::Vector2i(1024, 768), "Test App"), pointSize(4.0f) {
		using namespace nanogui;
		Window* window = new Window(this);
		window->setTitle("Window");
		window->setLayout(new GroupLayout());
		window->setPosition(Vector2i(15, 15));

		//Widget* panel = new Widget(window);
		//panel->setLayout(new BoxLayout(Orientation::Vertical, Alignment::Minimum, 0, 0));

		FloatDataSet* data = new FloatDataSet();
		data->addVariable("x");
		data->addVariable("y");
		data->addVariable("z");
		data->addVariable("force");

		data->addData(createPoint(-0.2,0.5,0.9,1));
		data->addData(createPoint(-0.3,0.2,-0.9,0.4));
		data->addData(createPoint(-0.1,0.1,-0.1,-0.4));
		data->addData(createPoint(0,0,0,1));

		new Label(window, "x-axis", "sans-bold");
		ComboBox* comboBox = new ComboBox(window, data->getVariables());
		comboBox->setFixedWidth(100);
		comboBox->setCallback([this](int index) {
			pointShader->setXDim(index);
    	});

		new Label(window, "y-axis", "sans-bold");
		comboBox = new ComboBox(window, data->getVariables());
		comboBox->setFixedWidth(100);
		comboBox->setSelectedIndex(1);
		comboBox->setCallback([this](int index) {
			pointShader->setYDim(index);
    	});

		Slider* slider = addVariableSlider(window, pointSize, "Point Size", [this](float value) { std::cout << value << std::endl; });
		slider->setRange(std::pair<float, float>(1,50));

		SceneNode* dataNode = new SceneNode();
		dataNode->addComponent(data);
		dataNode->addComponent(new FloatDataRenderer());
		scene.addNode(dataNode);

		SceneNode* geometryNode = new SceneNode();
		scene.addNode(geometryNode);

		obj = new SceneNode();
		obj->addComponent(new Mesh());
		obj->addComponent(new MeshLoader("data/monkey-head.obj"));
		obj->addComponent(new MeshRenderer());
		geometryNode->addNode(obj);

		graphicsNode = new SceneNode();
		graphicsNode->addComponent((new OpenGLCallback())->init(this));
		//graphicsNode->addComponent(new Shader2D());
		pointShader = new PointShader();
		graphicsNode->addComponent(pointShader);
		graphicsNode->addComponent(new NodeRenderer(dataNode));
		scene.addNode(graphicsNode);

		glm::vec3 pos = graphicsNode->getWorldPosition();
		std::cout << pos.x << " " << pos.y << " " << pos.z << std::endl;

	}

	void drawContents() {
		scene.updateModel();
		scene.updateSharedContext(context);
		scene.updateContext(context);
		graphicsNode->render(context);
	}

private:
	class OpenGLCallback : public RenderCallback<TestApp> {
		void renderCallback(const SceneContext& sceneContext, TestApp* app) {
			//std::cout << "Clear screen" << std::endl;
			glClearColor(1,1,1,1);
			int ps = app->pointSize;
			glPointSize(ps);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);
            glClearDepth(1.0f);
            glDepthFunc(GL_LEQUAL);
            glPatchParameteri(GL_PATCH_VERTICES, 3);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
		}
	};

	std::vector<float> createPoint(float a, float b, float c, float d) {
		std::vector<float> v;
		v.push_back(a);
		v.push_back(b);
		v.push_back(c);
		v.push_back(d);
		return v;
	}

	nanogui::Slider* addVariableSlider(Widget* parent, float& var, const std::string& fieldName, const std::function<void(float)>& lambda = [](float value) {}) {
		using namespace nanogui;
		Label* label = new Label(parent, fieldName);
		Slider* slider = new Slider(parent);
		slider->setFixedWidth(80);
		slider->setValue(var);
		slider->setCallback([&var, lambda](float value) {
			var = value;
			lambda(value);
		});
		return slider;
	}

	float pointSize;
	PointShader* pointShader;

	SceneContext context;
	SceneNode scene;
	SceneNode* graphicsNode;
	SceneNode* obj;
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


