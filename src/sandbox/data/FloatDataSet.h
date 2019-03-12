#ifndef SANDBOX_DATA_FLOAT_DATASET_H_
#define SANDBOX_DATA_FLOAT_DATASET_H_

#include <vector>
#include <string>
#include "sandbox/SceneComponent.h"
#include "sandbox/data/DataView.h"
#include <cmath>

namespace sandbox {

class FloatDataSet : public SceneComponent, public DataView<float> {
public:
	FloatDataSet();
	FloatDataSet(const std::vector<std::string>& variables);
	virtual ~FloatDataSet() {}

	void addVariable(const std::string& variable) { variables.push_back(variable); }

	void setVariables(const std::vector<std::string>& variables) { this->variables = variables; }
	const std::vector<std::string>& getVariables() const { return variables; }
	void addData(const std::vector<float>& dataPoint);
	const std::vector<float>& getArray() const { return array; }
	const std::vector<unsigned int>& getPoints() const { return points; }

	float getMin(unsigned int dimension) const { return statistics[dimension].min; }
	float getMax(unsigned int dimension) const { return statistics[dimension].max; }

	unsigned int getSize() const { return numPoints; }
	float getDimension(unsigned int index, unsigned int dimension) const {
		return array[variables.size()*index + dimension];
	}

	float getDistance(const std::vector<float>& point, unsigned int index, const std::vector<unsigned int>& dimensions) const {
		float dist = 0.0f;
		for (int f = 0; f < dimensions.size(); f++) {
			dist += std::pow((array[variables.size()*index + dimensions[f]] - point[f])/(getMax(dimensions[f]) - getMin(dimensions[f])), 2.0f);
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
	std::vector<unsigned int> points;
	unsigned int numPoints;
};

}

#endif