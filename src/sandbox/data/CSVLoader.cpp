#include "sandbox/data/CSVLoader.h"
#include "sandbox/data/FloatDataSet.h"
#include <fstream>
#include <iostream>
#include <sstream>

namespace sandbox {

CSVLoader::CSVLoader(const std::string& path) : path(path), isLoaded(false) {
	addType<CSVLoader>();
}

void CSVLoader::updateModel() {
	if (!isLoaded) {
		FloatDataSet* floatDataSet = getSceneNode().getComponent<FloatDataSet>();
		if (floatDataSet) {
			std::ifstream csvFile;
			csvFile.open(path.c_str());
			std::string line;
			int lineNum = 0;

			std::vector<std::string> headers;
			std::vector<std::string> variables;

			while (std::getline(csvFile, line,'\n')) {

				std::stringstream lineStream(line);

				std::vector<float> row;
				std::string value;
				int cellNum = 0;

				while(std::getline(lineStream, value, ',')) {
					if (lineNum == 0) {
						headers.push_back(value);
					}
					else {
						try {
							float val = std::stof(value);
							row.push_back(val);
							if (lineNum == 1) {
								variables.push_back(headers[cellNum]);
							}
						}
						catch (...) {
						}
					}

					cellNum++;
				}

				if (lineNum == 1) {
					floatDataSet->setVariables(variables);
				}

				if (lineNum > 0) {
					floatDataSet->addData(row);
				}

				lineNum++;
			}

			isLoaded = true;
		}

	}
}

}