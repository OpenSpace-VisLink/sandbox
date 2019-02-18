#ifndef SANDBOX_DATA_FLOAT_DATASET_H_
#define SANDBOX_DATA_FLOAT_DATASET_H_

#include <vector>
#include <string>
#include "sandbox/SceneComponent.h"
#include "sandbox/data/KdTree.h"
#include <cmath>

namespace sandbox {

class FloatDataSet : public SceneComponent, public KdSearchable<float> {
public:
	FloatDataSet();
	FloatDataSet(const std::vector<std::string>& variables);
	virtual ~FloatDataSet() {}

	void addVariable(const std::string& variable) { variables.push_back(variable); }

	void setVariables(const std::vector<std::string>& variables) { this->variables = variables; }
	const std::vector<std::string>& getVariables() const { return variables; }
	void addData(const std::vector<float>& dataPoint);
	const std::vector<float>& getArray() const { return array; }
	unsigned int getNumPoints() const { return numPoints; }

	float getMin(int index) const { return statistics[index].min; }
	float getMax(int index) const { return statistics[index].max; }

	unsigned int getSize() const { return numPoints; }
	float getDimension(unsigned int index, unsigned int dimension) const {
		return array[variables.size()*index + dimension];
	}

	float getDistance(std::vector<float> point, unsigned int index, std::vector<unsigned int> dimensions) const {
		float dist = 0.0f;
		for (int f = 0; f < dimensions.size(); f++) {
			dist += std::pow(array[variables.size()*index + dimensions[f] - point[f]], 2.0f);
		}

		return std::sqrt(dist);
	}

private:
	struct stats {
		float min;
		float max;
	};
	
	std::vector<std::string> variables;
	std::vector<stats> statistics;
	std::vector<float> array;
	unsigned int numPoints;
};

}

#endif