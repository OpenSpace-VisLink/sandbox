#include "sandbox/io/File.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

namespace sandbox {

long long File::getFileModifiedTime() const {
#if defined(WIN32)
	return 0;
#else
	struct stat attr;
	stat(getPath().c_str(), &attr);
	//printf("Last modified time: %s", ctime(&attr.st_mtime));
	return attr.st_mtime;
#endif
}

bool File::checkForUpdate() {
	long long modifiedTime = getFileModifiedTime(); 
	if (modifiedTime != lastModifiedTime) {
		lastModifiedTime = modifiedTime;
		return true;
	}

	return false;
}

}