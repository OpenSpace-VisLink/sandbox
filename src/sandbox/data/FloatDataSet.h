#ifndef SANDBOX_DATA_FLOAT_DATASET_H_
#define SANDBOX_DATA_FLOAT_DATASET_H_

#include <vector>
#include <string>
#include "sandbox/SceneComponent.h"
#include "sandbox/data/DataView.h"
#include <cmath>

namespace sandbox {

class FloatDataSet : public FloatDataView {
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

	const FloatDataView::DataViewStatistics& getStatistics(unsigned int dimension) const { return statistics[dimension]; }

private:
	
	std::vector<std::string> variables;
	std::vector<FloatDataView::DataViewStatistics> statistics;
	std::vector<float> array;
	std::vector<unsigned int> points;
	unsigned int numPoints;
};

}

#endif