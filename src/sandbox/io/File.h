#ifndef SANDBOX_IO_FILE_H_
#define SANDBOX_IO_FILE_H_

#include <string>
#include "sandbox/Component.h"

namespace sandbox {

class File : public Component {
public:
	File(const std::string& path) : path(path) {
		addType<File>();
		lastModifiedTime = getFileModifiedTime();
	}
	virtual ~File() {}

	const std::string& getPath() const { return path; }
	long long getFileModifiedTime() const;
	bool checkForUpdate();

private:
	std::string path;
	long long lastModifiedTime;
};

}

#endif