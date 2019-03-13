#include "sandbox/data/FloatDataSet.h"
#include <iostream>

namespace sandbox {

FloatDataSet::FloatDataSet() : currentVersion(-1) {
	addType<FloatDataSet>();
}

FloatDataSet::FloatDataSet(const std::vector<std::string>& variables) : currentVersion(-1) {
	addType<FloatDataSet>();
	setVariables(variables);
}

void FloatDataSet::updateModel() {
	if (currentVersion != getVersion()) {
		statistics = calculateStatistics();
		currentVersion = getVersion();
	}
}

void FloatDataSet::addData(const std::vector<float>& dataPoint) { 
	if (dataPoint.size() != variables.size()) {
		std::cout << "Data point size is not valid." << std::endl;
		return;
	}

	array.insert(array.end(), dataPoint.begin(), dataPoint.end());

	/*for (int f = 0; f < dataPoint.size(); f++) {
		
		if (numPoints == 0) {
			FloatDataSet::DataViewStatistics var;
			var.max = dataPoint[f];
			var.min = dataPoint[f];
			statistics.push_back(var);
		}
		else {
			if (statistics[f].max < dataPoint[f]) { statistics[f].max = dataPoint[f]; } 
			if (statistics[f].min > dataPoint[f]) { statistics[f].min = dataPoint[f]; } 
		}
	}*/

	points.push_back(points.size());

	updateVersion();
}

}