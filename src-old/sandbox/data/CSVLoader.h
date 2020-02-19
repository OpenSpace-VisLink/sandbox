#ifndef SANDBOX_DATA_CSVLOADER_H_
#define SANDBOX_DATA_CSVLOADER_H_

#include "sandbox/SceneComponent.h"
#include <string>

namespace sandbox {

class CSVLoader : public SceneComponent {
public:
	CSVLoader(const std::string& path);
	virtual ~CSVLoader() {}

	void updateModel();

private:
	bool isLoaded;
	std::string path;
};

}

#endif