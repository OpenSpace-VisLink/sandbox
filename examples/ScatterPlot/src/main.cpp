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
#include "sandbox/base/Image.h"
#include "sandbox/base/RenderCallback.h"
#include "sandbox/geometry/shapes/Quad.h"
#include "sandbox/geometry/MeshLoader.h"
#include "sandbox/graphics/Blend.h"
#include "sandbox/graphics/MeshRenderer.h"
#include "sandbox/graphics/FloatDataRenderer.h"
#include "sandbox/graphics/Texture.h"
#include "sandbox/graphics/shaders/MaterialShader.h"
#include "sandbox/graphics/shaders/Shader2D.h"
#include "sandbox/graphics/shaders/PointShader.h"
#include "sandbox/base/NodeRenderer.h"
#include "sandbox/base/Transform.h"
#include "sandbox/base/Camera.h"
#include "sandbox/data/CSVLoader.h"
#include "sandbox/data/FloatDataSet.h"
#include "sandbox/data/QueryableDataView.h"
#include "sandbox/data/KdTree.h"
#include "sandbox/geometry/Material.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace sandbox;

class TestApp : public nanogui::Screen {
public:
	TestApp() : nanogui::Screen(Eigen::Vector2i(1024, 768), "Test App"), pointSize(2.0f), xDimension(0), yDimension(1), backgroundDimension(-1) {
		using namespace nanogui;
		Window* window = new Window(this);
		window->setTitle("");
		window->setLayout(new GroupLayout());
		window->setPosition(Vector2i(1, 1));
		window->setFixedWidth(180);


		SceneNode* textures = new SceneNode();
		SceneNode* texture = new SceneNode();
		//texture->addComponent(new Image("data/test.png"));
		texture->addComponent(new Image(500,500,4));
		texture->addComponent(new Texture());
		textures->addNode(texture);
		scene.addNode(textures);

		image = texture->getComponent<Image>();
		for (int x = 0; x < image->getWidth(); x++) {
			for (int y = 0; y < image->getHeight(); y++) {
				//std::cout << x << ", " << y << std::endl;
				image->setPixelValue(x,y,0,255);
				image->setPixelValue(x,y,1,255);
				image->setPixelValue(x,y,2,255);
				image->setPixelValue(x,y,3,255);
			}
		}

		data = new FloatDataSet();

		SceneNode* dataNode = new SceneNode();
		dataNode->addComponent(data);
		//dataNode->addComponent(new CSVLoader("examples/ScatterPlot/data/cars.csv"));
		//dataNode->addComponent(new CSVLoader("/home/dan/src/cinema_quest_jay/data/full_5000.cdb/data.csv"));
		dataNode->addComponent(new CSVLoader("/home/dan/src/cinema_quest/data/discrete2.cdb/data2.csv"));
		scene.addNode(dataNode);
		dataNode->updateModel();

		SceneNode* dataRenderNode = new SceneNode();
		FloatQueryableDataView* view = new FloatQueryableDataView(dataNode);
		view->updateModel();
		view->addQuery(new DimensionCompareFilter<float>(2,21599.9,DimensionCompareFilter<float>::GreaterThan));
		view->addQuery(new DimensionCompareFilter<float>(9,7.0,DimensionCompareFilter<float>::LessThanEqual));
		ApplyLogScale<float>* applyLogScale = new ApplyLogScale<float>();
		for (int f = 0; f < data->getVariables().size(); f++) {
			if (data->getVariables()[f].rfind("input_", 0) == 0) {
				applyLogScale->addDimension(f);
			}
		}
		view->addQuery(applyLogScale);
		view->addQuery(new AddColumn<float>());
		GroupByDimensions<float>* groupBySim = new GroupByDimensions<float>(new MeanAggregator<float>());
		groupBySim->addDimension(0);
		view->addQuery(groupBySim);
		GroupByDimensions<float>* groupByDimensions = new GroupByDimensions<float>(new GroupAggregator<float>());
		groupByDimensions->addDimension(4);
		groupByDimensions->addDimension(5);
		groupByDimensions->addDimension(6);
		view->addQuery(groupByDimensions);
		dataRenderNode->addComponent(view);
		dataRenderNode->addComponent(new FloatDataRenderer());
		dataNode->addNode(dataRenderNode);
		dataNode->updateModel();

		data = view;
 
		std::vector<unsigned int> dimensions;
		dimensions.push_back(0);
		dimensions.push_back(1);
		kdTree = new KdTree<float>(dimensions, *data, new NormalizedEuclideanDistance<float>(*data));

		SceneNode* geometryNode = new SceneNode();
		scene.addNode(geometryNode);

		SceneNode* quad = new SceneNode();
		quad->addComponent(new Quad());
		quad->addComponent(new MeshRenderer());
		Material* quadMaterial = new Material();
		quadMaterial->setTexture(texture);
		//quad->addComponent(quadMaterial);
		geometryNode->addNode(quad);

		graphicsNode = new SceneNode();
		graphicsNode->addComponent((new OpenGLCallback())->init(this));
		scene.addNode(graphicsNode);

		//float pixelWidth = 2.0f/width();
		//float pixelHeight = 2.0f/height();
		//std::cout << pixelHeight*50 << std::endl;
		scatterPlotNode = new SceneNode();
		//glm::mat4 scatterPlotTransform = glm::translate(glm::mat4(1.0),glm::vec3(pixelWidth*85, pixelHeight*10.0, 0.0));
		//scatterPlotNode->addComponent(new Transform(scatterPlotTransform));
		//scatterPlotNode->addComponent(new Transform(glm::scale(scatterPlotTransform, glm::vec3(1.0f-pixelWidth*105, 1.0f-pixelHeight*30, 0.0))));
		scatterPlotNode->addComponent(new Transform());
		Shader2D* scatterPlotBackground = new Shader2D();
		//scatterPlotBackground->setTexture(texture);
		scatterPlotBackground->setColor(glm::vec4(1.0));
		scatterPlotNode->addComponent(scatterPlotBackground);
		scatterPlotNode->addComponent(new NodeRenderer(quad));
		graphicsNode->addNode(scatterPlotNode);

		pointNode = new SceneNode();
		pointNode->addComponent(new Transform(glm::scale(glm::mat4(1.0), glm::vec3(0.90f, 0.90, 1.0))));
		scatterPlotNode->addNode(pointNode);

		scatterPlotBackgroundMap = new SceneNode();
		scatterPlotBackgroundMap->setVisible(false);
		Shader2D* scatterPlotBackgroundShader = new Shader2D();
		scatterPlotBackgroundShader->setTexture(texture);
		scatterPlotBackgroundMap->addComponent(scatterPlotBackgroundShader);
		scatterPlotBackgroundMap->addComponent(new NodeRenderer(quad));
		pointNode->addNode(scatterPlotBackgroundMap);

		pointShader = new PointShader();
		points = new SceneNode();
		points->addComponent(pointShader);
		points->addComponent(new NodeRenderer(dataNode));
		pointNode->addNode(points);

		SceneNode* selectNode = new SceneNode();
		selectNode->addComponent(new Shader2D());
		graphicsNode->addNode(selectNode);

		selectQuad = new SceneNode();
		selectQuad->addComponent(new Blend());
		selectQuad->addComponent(new Transform(glm::scale(glm::mat4(1.0f), glm::vec3(0.0f))));
		selectQuad->addComponent(new NodeRenderer(quad));
		selectNode->addNode(selectQuad);

		glm::vec3 pos = graphicsNode->getWorldPosition();
		std::cout << pos.x << " " << pos.y << " " << pos.z << std::endl;

		pointShader->setXRange(glm::vec2(data->getMin(0), data->getMax(0)));
		pointShader->setYRange(glm::vec2(data->getMin(1), data->getMax(1)));
		pointShader->setColorRange(glm::vec2(data->getMin(2), data->getMax(2)));

		//new Label(window, "x-axis", "sans-bold");
		xAxisWindow = new Window(this);
		xAxisWindow->setTitle("");
		xAxisWindow->setLayout(new BoxLayout(Orientation::Vertical, Alignment::Minimum, 0, 0));
		ComboBox* comboBox = new ComboBox(xAxisWindow, data->getVariables());
		comboBox->setFixedWidth(150);
		comboBox->setCallback([this](int index) {
			xDimension = index;
			pointShader->setXDim(index);
			pointShader->setXRange(glm::vec2(data->getMin(index), data->getMax(index)));
			regenerateBackground();
    	});
    	//comboBox->popup()->setLayout(new GroupLayout(10));
    	//comboBox->popup()->setLayout(new BoxLayout(Orientation::Vertical, Alignment::Minimum, 0, 0));
		//comboBox->popup()->setAnchorHeight(comboBox->popup()->anchorHeight() + comboBox->popup()->height());
		performLayout();
		comboBox->popup()->setAnchorHeight(comboBox->popup()->height() - 20);
		xAxisWindow->setPosition(Vector2i(width()/2, height()-xAxisWindow->height()-5));

		//new Label(window, "y-axis", "sans-bold");
		yAxisWindow = new Window(this);
		yAxisWindow->setTitle("");
		yAxisWindow->setLayout(new BoxLayout(Orientation::Vertical, Alignment::Minimum, 0, 0));
		comboBox = new ComboBox(yAxisWindow, data->getVariables());
		comboBox->setFixedWidth(150);
		comboBox->setSelectedIndex(1);
		comboBox->setCallback([this](int index) {
			yDimension = index;
			pointShader->setYDim(index);
			pointShader->setYRange(glm::vec2(data->getMin(index), data->getMax(index)));
			regenerateBackground();
    	});

    	performLayout();
		comboBox->popup()->setAnchorHeight(comboBox->popup()->height()/2);

		Slider* slider = addVariableSlider(window, pointSize, "Point Size", [this](float value) { 
			std::cout << value << std::endl; 
			points->setVisible(value >= 1.0f);
		});
		slider->setRange(std::pair<float, float>(0,10));
		
		new Label(window, "Point Color");
		std::vector<std::string> colorVariables = data->getVariables();
		colorVariables.insert(colorVariables.begin(),"None");
		comboBox = new ComboBox(window, colorVariables);
		comboBox->setFixedWidth(125);
		comboBox->setSelectedIndex(0);
		//comboBox->setSide(Popup::Left);
		comboBox->setCallback([this, dataNode](int index) {
			pointShader->setHasColorGradient(index > 0);
			FloatDataRenderer* dataRenderer = dataNode->getComponent<FloatDataRenderer>();
			//dataRenderer->sortByVariable(index-1);
			if (index > 0) {
				pointShader->setColorDim(index-1);
				pointShader->setColorRange(glm::vec2(data->getMin(index-1), data->getMax(index-1)));
			}
    	});
		ColorPicker* pointColor = new ColorPicker(window, Color(0.0f, 0.0f, 0.5f, 1.0f));
		pointColor->setCallback([this](const Color& color) {
			pointShader->setColor(glm::vec4(color[0],color[1],color[2],color[3]));
    	});
    	pointShader->setColor(glm::vec4(pointColor->color()[0],pointColor->color()[1],pointColor->color()[2],pointColor->color()[3]));

    	new Label(window, "Background Color");
		comboBox = new ComboBox(window, colorVariables);
		comboBox->setFixedWidth(125);
		comboBox->setSelectedIndex(0);
		//comboBox->setSide(Popup::Left);
		comboBox->setCallback([this, dataNode](int index) {
			backgroundDimension = index -1;
			regenerateBackground();
    	});
    	backgroundColor = glm::vec4(0.25,0.25,0.25,0.25);
		pointColor = new ColorPicker(window, Color(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a));
		pointColor->setCallback([this](const Color& color) {
			backgroundColor = glm::vec4(color[0],color[1],color[2],color[3]);
    	});
    	pointShader->setColor(glm::vec4(pointColor->color()[0],pointColor->color()[1],pointColor->color()[2],pointColor->color()[3]));


    	resizeEvent(Eigen::Vector2i(width(), height()));
	}

	~TestApp() {
		delete kdTree;
	}

	bool resizeEvent(const Eigen::Vector2i& size) {
		float pixelWidth = 2.0f/size[0];
		float pixelHeight = 2.0f/size[1];
		Transform* transform = scatterPlotNode->getComponent<Transform>();
		glm::mat4 scatterPlotTransform = glm::translate(glm::mat4(1.0),glm::vec3(pixelWidth*85, pixelHeight*10.0, 0.0));
		transform->setTransform(glm::scale(scatterPlotTransform, glm::vec3(1.0f-pixelWidth*105, 1.0f-pixelHeight*30, 1.0)));
		xAxisWindow->setPosition(Eigen::Vector2i(width()/2, height()-35));
		yAxisWindow->setPosition(Eigen::Vector2i(5, height()/2));
		return true;
	}

	void drawContents() {
		handleSelect();
		scene.updateModel();
		scene.updateSharedContext(context);
		scene.updateContext(context);
		graphicsNode->render(context);
	}

private:
	void regenerateBackground() {
		if (backgroundDimension >= 0) {
			scatterPlotBackgroundMap->setVisible(true);
			std::vector<unsigned int> dimensions;
			dimensions.push_back(xDimension);
			dimensions.push_back(yDimension);
			delete kdTree;
			kdTree = new KdTree<float>(dimensions, *data, new NormalizedEuclideanDistance<float>(*data));

			std::vector<float> point;
			point.push_back(0.0f);
			point.push_back(0.0f);
			for (int x = 0; x < image->getWidth(); x++) {
				for (int y = 0; y < image->getHeight(); y++) {
					/*image->setPixelValue(x,y,0,255.0*(1.0f*x/image->getWidth()));
					image->setPixelValue(x,y,1,255.0*(1.0f - 1.0f*y/image->getHeight()));
					image->setPixelValue(x,y,2,0);
					image->setPixelValue(x,y,3,255);*/
					image->setPixelValue(x,y,0,255);
					image->setPixelValue(x,y,1,255);
					image->setPixelValue(x,y,2,255);
					image->setPixelValue(x,y,3,255);

					//if (std::pow(1.0f*x/image->getWidth() - sX, 2.0f) + std::pow(1.0-1.0f*y/image->getWidth() - sY, 2.0f) < 0.01) {
						point[0] = data->getMin(xDimension) + 1.0f*x/image->getWidth()*(data->getMax(xDimension)-data->getMin(xDimension));
						point[1] = data->getMin(yDimension) + (1.0-1.0f*y/image->getHeight())*(data->getMax(yDimension)-data->getMin(yDimension));
						std::vector<KdTree<float>::KdValue> values = kdTree->getNearestSorted(point, 10);
						
						float totalInverseDistance = 0.0f;
						for (int f = 0; f < values.size(); f++) {
							totalInverseDistance += std::pow(1.0f/values[f].distance,2.0);
						}
						unsigned int zDimension = backgroundDimension;

						float estimate = 0.0;
						for (int f = 0; f < values.size(); f++) {
							float weight = std::pow(1.0f/values[f].distance,2.0)/totalInverseDistance;
							estimate += weight*data->getDimension(values[f].index, zDimension);
						}

						estimate = (estimate - data->getMin(zDimension))/(data->getMax(zDimension)-data->getMin(zDimension));
						float binSize = (1.0/6.0);
						estimate = std::floor(estimate/binSize)*binSize;

						glm::vec4 color = glm::vec4(1.0)*(1-estimate) + estimate*backgroundColor;

						//std::cout << values[0].distance << std::endl;
						//image->setPixelValue(x,y,0,255.0*(1.0-values[0].distance*10.0f));
						image->setPixelValue(x,y,0,255.0*color.r);
						image->setPixelValue(x,y,1,255.0*color.g);
						image->setPixelValue(x,y,2,255.0*color.b);
						image->setPixelValue(x,y,3,255);
					//}
				}
			}
		}
		else {
			scatterPlotBackgroundMap->setVisible(false);
			/*for (int x = 0; x < image->getWidth(); x++) {
				for (int y = 0; y < image->getHeight(); y++) {
					//std::cout << x << ", " << y << std::endl;
					image->setPixelValue(x,y,0,255);
					image->setPixelValue(x,y,1,255);
					image->setPixelValue(x,y,2,255);
					image->setPixelValue(x,y,3,255);
				}
			}*/
		}
	}

	class OpenGLCallback : public RenderCallback<TestApp> {
		void renderCallback(const SceneContext& sceneContext, TestApp* app) {
			//std::cout << "Clear screen" << std::endl;
			glClearColor(0.75,0.75,0.75,1);
			int ps = app->pointSize;
			glPointSize(ps);
            glClearDepth(1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            glDepthFunc(GL_LEQUAL);
            glPatchParameteri(GL_PATCH_VERTICES, 3);
            glEnable(GL_CULL_FACE);
            glDisable(GL_BLEND);
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

	void handleSelect() {
		Transform* transform = selectQuad->getComponent<Transform>();
		if (transform) {
			if (mMouseState && !mDragActive) {
				//std::cout << 1.0f*mousePos()[0]/width() << " " << 1.0f*mousePos()[1]/height() << std::endl;
				float x = 2.0f*mousePos()[0]/width()-1.0;
				float y = 1.0-2.0f*mousePos()[1]/height();

				std::cout << std::endl << x << " " << y << std::endl;

				glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));//glm::scale(glm::mat4(1.0f), glm::vec3(0.25));
				//glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.25));
				transform->setTransform(glm::scale(trans, glm::vec3(0.25)));


				Transform* transform = scatterPlotNode->getComponent<Transform>();
				trans = transform->getTransform();
				Transform* pointTrans = pointNode->getComponent<Transform>();
				glm::vec4 p(x,y,0,1.0);
				glm::vec2 newP = glm::inverse(trans*pointTrans->getTransform())*p; //
				std::cout << std::endl << newP.x << " " << newP.y << std::endl;
				float sX = (1.0+newP.x)/2.0;
				float sY = (1.0+newP.y)/2.0;
				std::cout << std::endl << sX << " " << sY << std::endl;

				std::vector<float> point;

				point.push_back(data->getMin(xDimension) + sX*(data->getMax(xDimension)-data->getMin(xDimension)));
				point.push_back(data->getMin(yDimension) + sY*(data->getMax(yDimension)-data->getMin(yDimension)));
				//point.push_back(sY);
				/*point.push_back(2.0f);
				point.push_back(5.67804f);
				point.push_back(6.29711);*/
				std::cout << point[0] << " " << point[1] << std::endl;
				std::vector<KdTree<float>::KdValue> values = kdTree->getNearestSorted(point, 10);
			}
			else {
				transform->setTransform(glm::scale(glm::mat4(1.0f), glm::vec3(0.0f)));
			}
		}
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
	SceneNode* selectQuad;
	SceneNode* scatterPlotNode;
	nanogui::Window* xAxisWindow;
	nanogui::Window* yAxisWindow;

	SceneNode* pointNode;
	SceneNode* points;
	SceneContext context;
	SceneNode scene;
	SceneNode* graphicsNode;
	KdTree<float>* kdTree;
	FloatDataView* data;
	int xDimension, yDimension, backgroundDimension;
	Image* image;
	glm::vec4 backgroundColor;
	SceneNode* scatterPlotBackgroundMap;
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


