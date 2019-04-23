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
#include "sandbox/data/KdTree.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <cstdlib>

#include <iostream>
#include <Eigen/Dense>
#include "src/LeastSquares.h" 
#include <algorithm>

using namespace sandbox;

float xFrequency = 1.0;

float function(float x, float y, float t) {
	return 0.5*std::cos(2.0f*3.1415*y)*std::cos(xFrequency*2.0f*3.1415*(x+t));
}

float fx(float x, float y) {
	return 0.5*std::cos(2.0f*3.1415*y)*std::sin(xFrequency*2.0f*3.1415*x)*-xFrequency*2.0f*3.1415;
}

float fy(float x, float y) {
	return 0.5*std::sin(2.0f*3.1415*y)*std::cos(xFrequency*2.0f*3.1415*x)*-xFrequency*3.1415;
}

class TestApp : public NanoguiScreen {
public:

	//TestApp() : NanoguiScreen(Eigen::Vector2i(1180, 980), "Test App") {
	TestApp() : NanoguiScreen(Eigen::Vector2i(1150, 600), "Test App") {
		using namespace nanogui;

		Grid* grid = new Grid(30, 30);
		Grid* estGrid = new Grid(30, 30);
		
		SceneNode* eventNode = new SceneNode(&scene);
			NanoguiResizeEvent* resize = new NanoguiResizeEvent(this);
			eventNode->addComponent(resize);
			ResizeEventHandler* resizeCallback = (new ResizeCallback())->init(this);
			resize->subscribe(resizeCallback);
			eventNode->addComponent(resizeCallback);
		SceneNode* geometryNode = new SceneNode(&scene);
			SceneNode* arrowNode = new SceneNode(geometryNode);
				arrowNode->addComponent(new Transform(glm::scale(glm::mat4(1.0f),glm::vec3(1.0f/1.25))*glm::translate(glm::mat4(1.0f),glm::vec3(0,2.0,0))));
				SceneNode* arrowTipNode = new SceneNode(arrowNode);
					glm::mat4 tipTrans = glm::translate(glm::mat4(1.0f),glm::vec3(0,0.25,0));
					tipTrans = glm::scale(tipTrans,glm::vec3(0.25f));
					arrowTipNode->addComponent(new Transform(tipTrans));
					arrowTipNode->addComponent(new Cylinder(20, 1.0f, 0.0f));
					arrowTipNode->addComponent(new MeshRenderer());
					Material* material = new Material();
					material->setColor(glm::vec4(1.0f, 0, 0, 1));
					arrowTipNode->addComponent(material);
				SceneNode* cylNode = new SceneNode(arrowNode);
					glm::mat4 cylTrans = glm::translate(glm::mat4(1.0f),glm::vec3(0,-1.0,0));
					cylTrans = glm::scale(cylTrans,glm::vec3(0.1f, 1.0f, 0.1f));
					cylNode->addComponent(new Transform(cylTrans));
					cylNode->addComponent(new Cylinder(20));
					cylNode->addComponent(new MeshRenderer());
					material = new Material();
					material->setColor(glm::vec4(1.0f, 0, 0, 1));
					cylNode->addComponent(material);
			SceneNode* gridNode = new SceneNode(geometryNode);
				gridNode->addComponent(grid);
				gridNode->addComponent(new SmoothNormals());
				gridNode->addComponent(new MeshRenderer());
				gridNode->addComponent(new Material());
			SceneNode* estGridNode = new SceneNode(geometryNode);
				estGridNode->addComponent(estGrid);
				estGridNode->addComponent(new SmoothNormals());
				estGridNode->addComponent(new MeshRenderer());
				//estGridNode->addComponent(new Material());
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
				functionViewNode->addComponent(new Transform(glm::translate(glm::mat4(1.0f),glm::vec3(0,2.5,5.0))));
				functionViewNode->addComponent(new Camera());
				functionViewNode->addComponent(new MaterialShader());
				SceneNode* functionNode = new SceneNode(functionViewNode);
					functionNode->addComponent(new Transform(glm::rotate(glm::mat4(1.0f), -3.1415f*1.0f/2.0f, glm::vec3(1.0f,0.0,0))*glm::scale(glm::mat4(1.0f), glm::vec3(3.0f,3.0f,1.0f*3.0f))));
					functionNode->addComponent(new NodeRenderer(gridNode));
				SceneNode* pointsNode = new SceneNode(functionViewNode);
					pointsNode->addComponent(new Transform(glm::rotate(glm::mat4(1.0f), -3.1415f*1.0f/2.0f, glm::vec3(1.0f,0.0,0))*glm::scale(glm::mat4(1.0f), glm::vec3(3.0f,3.0f,1.0f*3.0f))));
					//pointsNode->addComponent(new NodeRenderer(sampleNode));
			SceneNode* flatGraph = new SceneNode(graphicsNode);
				flatGraph->addComponent(new PercentViewport(glm::vec4(0.0, 0.5, 0.5, 0.5)));
				flatGraph->addComponent(new Transform(glm::translate(glm::mat4(1.0f),glm::vec3(0,0,3))));
				flatGraph->addComponent(new Camera());
				flatGraph->addComponent(new MaterialShader());
				SceneNode* flatPointsNode = new SceneNode(flatGraph);
					flatPointsNode->addComponent(new Transform(glm::rotate(glm::mat4(1.0f), 3.1415f*4.0f/2.0f, glm::vec3(1.0f,0.0,0))*glm::scale(glm::mat4(1.0f), glm::vec3(2.0f,2.0f,0.0f*2.0f))));
			functionViewNode = new SceneNode(graphicsNode);				
				//functionViewNode->addComponent(new PercentViewport(glm::vec4(0.5, 0.0, 0.5, 0.5)));
				functionViewNode->addComponent(new PercentViewport(glm::vec4(0.5, 0.0, 0.5, 1.0)));
				functionViewNode->addComponent(new Transform(glm::translate(glm::mat4(1.0f),glm::vec3(0,2.5,5.0))));
				functionViewNode->addComponent(new Camera());
				functionViewNode->addComponent(new MaterialShader());
				functionNode = new SceneNode(functionViewNode);
					functionNode->addComponent(new Transform(glm::rotate(glm::mat4(1.0f), -3.1415f/2.0f, glm::vec3(1.0f,0.0,0))*glm::scale(glm::mat4(1.0f), glm::vec3(3.0f))));
					functionNode->addComponent(new NodeRenderer(estGridNode));
				SceneNode* estPointsNode = new SceneNode(functionViewNode);
					estPointsNode->addComponent(new Transform(glm::rotate(glm::mat4(1.0f), -3.1415f*1.0f/2.0f, glm::vec3(1.0f,0.0,0))*glm::scale(glm::mat4(1.0f), glm::vec3(3.0f,3.0f,1.0f*3.0f))));
			SceneNode* estFlatGraph = new SceneNode(graphicsNode);
				estFlatGraph->addComponent(new PercentViewport(glm::vec4(0.5, 0.5, 0.5, 0.5)));
				estFlatGraph->addComponent(new Transform(glm::translate(glm::mat4(1.0f),glm::vec3(0,0,3))));
				estFlatGraph->addComponent(new Camera());
				estFlatGraph->addComponent(new MaterialShader());
				SceneNode* estFlatPointsNode = new SceneNode(estFlatGraph);
					estFlatPointsNode->addComponent(new Transform(glm::rotate(glm::mat4(1.0f), 3.1415f*4.0f/2.0f, glm::vec3(1.0f,0.0,0))*glm::scale(glm::mat4(1.0f), glm::vec3(2.0f,2.0f,0.0f*2.0f))*glm::rotate(glm::mat4(1.0f), 3.1415f*0.0f/2.0f, glm::vec3(0.0f,0.0f,1.0f))));


		float time = 0.25;

		for (int x = 0; x < grid->getWidth(); x++) {
			for (int y = 0; y < grid->getHeight(); y++) {
				grid->getNode(x,y).z = function(1.0f*x/(grid->getWidth()-1), 1.0f*y/(grid->getHeight()-1), time);
			}
		}

		int numSamples = 1000;
		std::vector<glm::vec4> samplePoints;

		/*std::vector<int> xDim;
		for (int f = 0; f < 10; f++) {
			cards.push_back(f);
		}
		std::random_shuffle(cards.begin(), cards.end());
		for (int f = 0; f < 10; f++) {
			std::cout << cards[f] << std::endl;
		}*/

		std::vector<float> xDim;
		std::vector<float> yDim;
		std::vector<float> tDim;

		int numSplits = 10;//grid->getWidth();
		float step = 1.0f/numSplits;

		for (int f = 0; f < numSamples/numSplits; f++) {
			for (int i = 0; i < numSplits; i++) {
				xDim.push_back(step*i + step*float(std::rand())/RAND_MAX);
				yDim.push_back(step*i + step*float(std::rand())/RAND_MAX);
				tDim.push_back(step*i + step*float(std::rand())/RAND_MAX);
			}
			std::random_shuffle(xDim.begin(), xDim.end());
			std::random_shuffle(yDim.begin(), yDim.end());
			std::random_shuffle(tDim.begin(), tDim.end());

			for (int i = 0; i < numSplits; i++) {
				float x = xDim[i];
				float y = yDim[i];
				float t = tDim[i];
				float z = function(x,y,t);
				samplePoints.push_back(glm::vec4(x,y,t,z));
			}
			/* random sampling
			float x = float(std::rand())/RAND_MAX;
			float y = float(std::rand())/RAND_MAX;
			float t = float(std::rand())/RAND_MAX;
			float z = function(x,y,t);
			samplePoints.push_back(glm::vec4(x,y,t,z));*/
		}


		for (int iteration = 0; iteration < 40; iteration++) {

			PointCollection pc(samplePoints);

			for (int x = 0; x < grid->getWidth(); x++) {
				for (int y = 0; y < grid->getHeight(); y++) {
					//estGrid->getNode(x,y).z = function(1.0f*x/(grid->getWidth()-1), 1.0f*y/(grid->getHeight()-1));
					float residual;
					estGrid->getNode(x,y).z = calculateFromSamples(pc, glm::vec3(1.0f*x/(grid->getWidth()-1), 1.0f*y/(grid->getHeight()-1), time), &residual);
					estGrid->getCoord(x,y).x = residual;
				}
			}

			glm::vec2 minMax;
			for (int f = 0; f < pc.getPoints().size(); f++) {
				if (f == 0) {
					minMax = glm::vec2(pc.getEstResiduals()[f]);
				}
				else {
					minMax.x = minMax.x < pc.getEstResiduals()[f] ? minMax.x : pc.getEstResiduals()[f];
					minMax.y = minMax.y > pc.getEstResiduals()[f] ? minMax.y : pc.getEstResiduals()[f];
				}

			}
			std::cout << minMax.x << ", "<< minMax.y << std::endl;

			for (int x = 0; x < grid->getWidth(); x++) {
				for (int y = 0; y < grid->getHeight(); y++) {
					estGrid->getCoord(x,y).x = (estGrid->getCoord(x,y).x - minMax.x)/(minMax.y-minMax.x);
				}
			}

			std::vector<float> splits;
			std::vector<int> splitSizes;
			for (int f = 0; f < numSplits*numSplits*numSplits; f++) {
				splits.push_back(0.0f);
				splitSizes.push_back(0);
			}

			for (int f = 0; f < pc.getPoints().size(); f++) {
				glm::vec4 splitIndex = pc.getActualPoints()[f];
				splitIndex = splitIndex / step;
				int index = (int(splitIndex.x)*numSplits+int(splitIndex.y))*numSplits+int(splitIndex.z);
				splits[index] += pc.getEstResiduals()[f];
				splitSizes[index]++;
			}

			float totalResidual = 0.0f;
			for (int f = 0; f < splits.size(); f++) {
				if (splitSizes[f] > 0) {
					splits[f] = splits[f]/splitSizes[f];
				}
				totalResidual += splits[f];
			}

			int numS = 0;
			for (int f = 0; f < splits.size(); f++) {
				splits[f] /= totalResidual;
				//std::cout << splits[f]*numSamples << std::endl;
				numS += std::round(splits[f]*numSamples);
			}

			//std::cout << numS << std::endl;

			/*for (int x = 0; x < numSplits; x++) {
				for (int y = 0; y < numSplits; y++) {
					for (int t = 0; t < numSplits; t++) {
						
					}
				}
			}*/

			for (int x = 0; x < numSplits; x++) {
				for (int y = 0; y < numSplits; y++) {
					for (int t = 0; t < numSplits; t++) {
						int index = (x*numSplits+y)*numSplits+t;
						for (int f = 0; f < std::round(1.0f*splits[index]*numSamples); f++) {
							float x2 = 1.0f*step*x + step*float(std::rand())/RAND_MAX;
							float y2 = 1.0f*step*y + step*float(std::rand())/RAND_MAX;
							float t2 = 1.0f*step*t + step*float(std::rand())/RAND_MAX;
							float z2 = function(x2,y2,t2);
							//std::cout << samplePoints.size() << " " << x << " " << x2 << " " << y2 << " " << t2 << " " << z2 << " " << splits[index] << " " << step << std::endl;
							samplePoints.push_back(glm::vec4(x2,y2,t2,z2));
						}
					}
				}
			}

			// vector estimate
			/*std::vector<glm::vec4> newPoints;
			for (int f = 0; f < pc.getPoints().size(); f++) {
				float randPercent = 2.0f*(float(std::rand())/RAND_MAX - 0.5);
				glm::vec4 newPoint = pc.getActualPoints()[f] + glm::vec4(pc.getEstGradients()[f]*randPercent,0.0);
				newPoint.w = function(newPoint.x, newPoint.y, newPoint.z);
				newPoints.push_back(newPoint);
			}

			for (int f = 0; f < newPoints.size(); f++) {
				samplePoints.push_back(newPoints[f]);
			}*/

			/*for (int f = 0; f < numSamples; f++) {
				float x = float(std::rand())/RAND_MAX;
				float y = float(std::rand())/RAND_MAX;
				float t = float(std::rand())/RAND_MAX;
				float z = function(x,y,t);
				samplePoints.push_back(glm::vec4(x,y,t,z));
			}*/

			/*float totalResidual = 0.0f;
			float dr = 1.0f/pc.getPoints().size();
			for (int f = 0; f < pc.getPoints().size(); f++) {
				totalResidual += (pc.getEstResiduals()[f]-minMax.x)/(pc.getEstResiduals()[f]-minMax.y - pc.getEstResiduals()[f]-minMax.x);
			}*/

			/*splits.clear();
			totalResidual = 0.0f;
			for (int x = 0; x < numSplits; x++) {
				for (int y = 0; y < numSplits; y++) {
					for (int t = 0; t < numSplits; t++) {
						float newX = 1.0f*step*x + 0.5*step;
						float newY = 1.0f*step*y + 0.5*step;
						float newT = 1.0f*step*t + 0.5*step;
						//estGrid->getNode(x,y).z = function(1.0f*x/(grid->getWidth()-1), 1.0f*y/(grid->getHeight()-1));

						int numNearest = 1;
						std::vector<float> point;
						point.push_back(newX);
						point.push_back(newY);
						point.push_back(newT);
						std::vector<KdTree<float>::KdValue> nearest = pc.getKdTree().getNearestSorted(point, numNearest);

						float residual = pc.getEstResiduals()[nearest[0].index];
						//std::cout << newX << " " << newY << " " << newT << " " << pc.getEstResiduals()[nearest[0].index] << std::endl;
						splits.push_back(residual);
						totalResidual += residual;
					}
				}
			}

			for (int f = 0; f < splits.size(); f++) {
				splits[f] /= totalResidual;
			}

			int splitIndex = 0;
			for (int x = 0; x < numSplits; x++) {
				for (int y = 0; y < numSplits; y++) {
					for (int t = 0; t < numSplits; t++) {
						for (int f = 0; f < int(splits[splitIndex]*numSamples); f++) {
							float newX = 1.0f*step*x + step*float(std::rand())/RAND_MAX;
							float newY = 1.0f*step*y + step*float(std::rand())/RAND_MAX;
							float newT = 1.0f*step*t + step*float(std::rand())/RAND_MAX;
							float newZ = function(newX, newY, newT);
							samplePoints.push_back(glm::vec4(newX, newY, newT, newZ));		
						}
						splitIndex++;
					}
				}
			}

			/*for (int f = 0; f < numSamples; f++) {
				float x = float(std::rand())/RAND_MAX;
				float y = float(std::rand())/RAND_MAX;
				float t = float(std::rand())/RAND_MAX;
				float z = function(x,y,t);
				samplePoints.push_back(glm::vec4(x,y,t,z));
			}*/
			std::cout << samplePoints.size() << std::endl;
		}

		
    	resizeEvent(Eigen::Vector2i(width(), height()));
	}

	class PointCollection : public KdSearchable<float> {
	public:

		class ObjectiveFunction {
		public:
			virtual ~ObjectiveFunction() {}
			virtual float getValue(int sampleNum, const glm::vec4& point) const {
				return point.w;
			}
		};

		class GradObjectiveFunction : public ObjectiveFunction {
		public:
			GradObjectiveFunction(int dim, PointCollection* pc) : dim(dim), pc(pc) {}
			virtual ~GradObjectiveFunction() {}
			virtual float getValue(int sampleNum, const glm::vec4& point) const {
				return pc->estGradients[sampleNum][dim];
			}
		private:
			int dim;
			PointCollection* pc;
		};

		const glm::vec3 calculateGradient(int numNearest, glm::vec4 estPoint, float value, bool pointIsSample, const ObjectiveFunction* objFunction, float* residual = NULL) const {
			int startNearest = pointIsSample ? 1 : 0;

			Eigen::MatrixXf A = Eigen::MatrixXf(numNearest-startNearest, 3);
			Eigen::VectorXf b = Eigen::VectorXf(numNearest-startNearest);

			std::vector<float> point;
			point.push_back(estPoint[0]);
			point.push_back(estPoint[1]);
			point.push_back(estPoint[2]);
			//point.push_back(estPoint[3]);

			std::vector<KdTree<float>::KdValue> nearest = kdTree->getNearestSorted(point, numNearest);

			for (int f = startNearest; f < nearest.size(); f++) {
				glm::vec3 diff = glm::vec3(points[nearest[f].index]-estPoint);
				glm::vec3 dir = normalize(diff);
				float dirDeriv = (objFunction->getValue(nearest[f].index, points[nearest[f].index])-value)/(glm::length(diff));

				Eigen::VectorXf d(3);
				d[0] = dir[0];
				d[1] = dir[1];
				d[2] = dir[2];

				b[f-startNearest] = dirDeriv;
				A.block(f-startNearest, 0, 1, 3) = d.transpose();
			}

			Eigen::VectorXf sol = calculateLeastSquares(A,b);
			if (residual) {
				*residual = (A * sol - b).norm();
			}
			
			return glm::vec3(sol[0], sol[1], sol[2]);
		}

		PointCollection(const std::vector<glm::vec4>& points) : points(points) {
			for (int f = 0; f < points.size(); f++) {
				indices.push_back(f);
			}

			std::vector<unsigned int> dimensions;
			dimensions.push_back(0);
			dimensions.push_back(1);
			dimensions.push_back(2);
			//dimensions.push_back(3);
			kdTree = new KdTree<float>(dimensions, *this, new EuclideanDistance<float>());

			int numNearest = 11;
//			Eigen::MatrixXf A = Eigen::MatrixXf(numNearest-1, 3);
//			Eigen::VectorXf b = Eigen::VectorXf(numNearest-1);

			ObjectiveFunction fun;

			for (int sampleNum = 0; sampleNum < points.size(); sampleNum++) {	
				float residual;
				glm::vec3 gradient = calculateGradient(numNearest, points[sampleNum], points[sampleNum].w, true, &fun, &residual);
				estGradients.push_back(gradient);
				estResiduals.push_back(residual);
			
				/*std::vector<float> point;
				point.push_back(points[sampleNum][0]);
				point.push_back(points[sampleNum][1]);
				point.push_back(points[sampleNum][2]);
				point.push_back(points[sampleNum][3]);
				std::vector<KdTree<float>::KdValue> nearest = kdTree->getNearestSorted(point, numNearest);

				for (int f = 1; f < nearest.size(); f++) {
					glm::vec4 diff = points[nearest[f].index]-points[sampleNum];
					glm::vec3 dir = normalize(glm::vec3(diff));
					float dirDeriv = diff.w/(glm::length(glm::vec3(diff)));

					Eigen::VectorXf d(3);
					d[0] = dir[0];
					d[1] = dir[1];
					d[2] = dir[2];

					b[f-1] = dirDeriv;
					A.block(f-1, 0, 1, 3) = d.transpose();
				}


				Eigen::VectorXf sol = calculateLeastSquares(A,b);
				float residual = (A * sol - b).norm();
				estGradients.push_back(glm::vec3(sol[0], sol[1], sol[2]));
				estResiduals.push_back(residual);*/
			}

			// calculate Hessians
			std::vector<GradObjectiveFunction> dimFunctions;
			dimFunctions.push_back(GradObjectiveFunction(0, this));
			dimFunctions.push_back(GradObjectiveFunction(1, this));
			dimFunctions.push_back(GradObjectiveFunction(2, this));
			for (int sampleNum = 0; sampleNum < points.size(); sampleNum++) {
				glm::mat3 hessian(1.0f);
				for (int f = 0; f < 3; f++) {
					glm::vec3 gradient = calculateGradient(numNearest, points[sampleNum], estGradients[sampleNum][f], true, &(dimFunctions[f]));
					hessian = glm::row(hessian, f, gradient);
					//std::cout << "Grad: " << f << " "  << gradient[0] << " " << gradient[1] << " " << gradient[2] << std::endl;
					//std::cout << "Hessian: " << f << " "  << glm::row(hessian, f)[0] << " " << glm::row(hessian, f)[1] << " " << glm::row(hessian, f)[2] << std::endl;
				}

				estHessians.push_back(hessian);
			}

			dimensions.clear();
			dimensions.push_back(0);
			dimensions.push_back(1);
			dimensions.push_back(2);
			queryKdTree = new KdTree<float>(dimensions, *this, new EuclideanDistance<float>());

		}
		~PointCollection() {
			delete kdTree;
			delete queryKdTree;
		}

		const std::vector<unsigned int>& getPoints() const { return indices; }
		float getDimension(unsigned int index, unsigned int dimension) const { return points[index][dimension]; }
		const std::vector<glm::vec4>& getActualPoints() const { return points; }
		const std::vector<glm::vec3>& getEstGradients() const { return estGradients; }
		const std::vector<float>& getEstResiduals() const { return estResiduals; }
		const std::vector<glm::mat3>& getEstHessians() const { return estHessians; }
		const KdTree<float>& getKdTree() const { return *queryKdTree; }
	private:
		std::vector<glm::vec4> points;
		std::vector<unsigned int> indices;
		std::vector<glm::vec3> estGradients;
		std::vector<float> estResiduals;
		std::vector<glm::mat3> estHessians;
		KdTree<float>* kdTree;
		KdTree<float>* queryKdTree;
	};


	float calculateFromSamples(const PointCollection& pc, const glm::vec3& pos, float* residual) {
		//return 0.0f;
				/*if (true) {//recalculateZ_FirstHalf) { // nearest directional deriv
					numAlgorithms++;
					glm::vec3 diff = samplePoints[sampleNum]-samplePoints[nearest[0].index];
					glm::vec2 dir = normalize(glm::vec2(diff));
					float dirDeriv = glm::dot(estGradients[nearest[0].index],dir);
					zEstimate = samplePoints[nearest[0].index].z + dirDeriv*glm::length(glm::vec2(diff));

					finalEstimate += zEstimate;
				}*/
		int numNearest = 10;

		std::vector<float> point;
		point.push_back(pos[0]);
		point.push_back(pos[1]);
		point.push_back(pos[2]);

		std::vector<KdTree<float>::KdValue> nearest = pc.getKdTree().getNearestSorted(point, numNearest);

		glm::vec3 diff = pos-glm::vec3(pc.getActualPoints()[nearest[0].index]);
		glm::vec3 dir = normalize(diff);
		float dirDeriv = glm::dot(pc.getEstGradients()[nearest[0].index],dir);

		*residual = pc.getEstResiduals()[nearest[0].index];

		//return pc.getActualPoints()[nearest[0].index].w + dirDeriv*glm::length(diff);

		glm::vec3 gradient = pc.getEstGradients()[nearest[0].index];
		glm::vec4 newPoint = pc.getActualPoints()[nearest[0].index];

		float integrationSteps = 1.0f;
		glm::vec3 intDiff = diff/integrationSteps;
		PointCollection::ObjectiveFunction fun;

		for (int f = 0; f < integrationSteps; f++) {
			//float hessCalc =  0.5*glm::dot(diff, pc.getEstHessians()[nearest[0].index]*diff)
			newPoint += glm::vec4(intDiff, glm::dot(gradient, intDiff));

			gradient = pc.calculateGradient(5, newPoint, newPoint.w, false, &fun);
		}

		//return newPoint.w;


		//return pc.getActualPoints()[nearest[0].index].w;

		//return pc.getActualPoints()[nearest[0].index].w + glm::dot(pc.getEstGradients()[nearest[0].index],diff);



		//return f(x+dx) ~= f(x) + df(x)*dx + (1/2)*dx*H(x)*dx
		return pc.getActualPoints()[nearest[0].index].w + glm::dot(pc.getEstGradients()[nearest[0].index],diff)
		 + 0.5*glm::dot(diff, pc.getEstHessians()[nearest[0].index]*diff);


		// inverse weighted
		float totalWeight = 0.0f;
		for (int f = 0; f < nearest.size(); f++) {
			totalWeight += 1.0f/nearest[f].distance;
		}

		float value = 0.0f;
		for (int f = 0; f < nearest.size(); f++) {
			value += pc.getActualPoints()[nearest[f].index].w * ((1.0f/nearest[f].distance)/totalWeight);
		}		
		return value;
	}

	glm::quat rotationBetweenVectors(glm::vec3 start, glm::vec3 dest){
		start = normalize(start);
		dest = normalize(dest);

		float cosTheta = dot(start, dest);
		glm::vec3 rotationAxis;

		if (cosTheta < -1 + 0.001f){
			// special case when vectors in opposite directions:
			// there is no "ideal" rotation axis
			// So guess one; any will do as long as it's perpendicular to start
			rotationAxis = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), start);
			if (glm::length(rotationAxis) < 0.01 ) // bad luck, they were parallel, try again!
				rotationAxis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), start);

			rotationAxis = glm::normalize(rotationAxis);
			return glm::angleAxis(glm::radians(180.0f), rotationAxis);
		}

		rotationAxis = glm::cross(start, dest);

		float s = sqrt( (1+cosTheta)*2 );
		float invs = 1 / s;

		return glm::quat(
			s * 0.5f, 
			rotationAxis.x * invs,
			rotationAxis.y * invs,
			rotationAxis.z * invs
		);

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


