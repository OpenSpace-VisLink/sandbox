#ifndef SANDBOX_IO_FILE_MONITOR_H_
#define SANDBOX_IO_FILE_MONITOR_H_

#include <thread>
#include <mutex>
#include "sandbox/Component.h"
#include "sandbox/io/File.h"

namespace sandbox {

class FileMonitor : public Component {
public:
	FileMonitor(int updateMilliseconds = 500);
	virtual ~FileMonitor();

	void run();
	void update();
	bool forceUpdate() { return true; }

private:
	std::thread* monitorThread;
	std::mutex mutex;
	bool running;
	std::vector<File*> files;
	std::vector<File*> changedFiles;
	int updateMilliseconds;
};

}

#endif