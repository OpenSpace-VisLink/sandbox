#ifndef SANDBOX_DATA_FLOAT_DATASET_H_
#define SANDBOX_DATA_FLOAT_DATASET_H_

#include <vector>
#include <string>
#include "sandbox/SceneComponent.h"

namespace sandbox {

class FloatDataSet : public SceneComponent {
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