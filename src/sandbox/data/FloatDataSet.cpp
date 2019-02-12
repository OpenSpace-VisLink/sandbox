#include "sandbox/data/FloatDataSet.h"
#include <iostream>

namespace sandbox {

FloatDataSet::FloatDataSet() : numPoints(0) {
	addType<FloatDataSet>();
}

FloatDataSet::FloatDataSet(const std::vector<std::string>& variables) : numPoints(0) {
	addType<FloatDataSet>();
	setVariables(variables);
}


void FloatDataSet::addData(const std::vector<float>& dataPoint) { 
	if (dataPoint.size() != variables.size()) {
		std::cout << "Data point size is not valid." << std::endl;
		return;
	}

	array.insert(array.end(), dataPoint.begin(), dataPoint.end()); 
	numPoints++;
}

}