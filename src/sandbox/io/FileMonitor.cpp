#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <iostream>

void monitorSleep(int sleepMs)
{
#ifdef WIN32
    Sleep(sleepMs);
#else
    usleep(sleepMs * 1000);   // usleep takes sleep time in us (1 millionth of a second)
#endif
}

#include "sandbox/io/FileMonitor.h"

namespace sandbox {

FileMonitor::FileMonitor(int updateMilliseconds) : running(true), updateMilliseconds(updateMilliseconds) {
	addType<FileMonitor>();
	monitorThread = new std::thread(&FileMonitor::run, this);
}

FileMonitor::~FileMonitor() {
	running = false;
	if (monitorThread) {
		monitorThread->join();
	}

	delete monitorThread;
}
 
void FileMonitor::run() {
	while(running) {
		mutex.lock();

		for (int f = 0; f < files.size(); f++) {
			File* file = files[f];
			//std::cout << file->getPath() << std::endl;
			if (file->checkForUpdate()) {
				file->getEntity().incrementVersion();
				//std::cout << "Increment version." << std::endl;
			}
		}

		mutex.unlock();
		monitorSleep(updateMilliseconds);
	}
}

void FileMonitor::update() {
	mutex.lock();
	//std::cout << "Update FildMonitor" << std::endl;

	files.clear();
	for (int f = 0; f < getEntity().getChildren().size(); f++) {
		File* file = getEntity().getChildren()[f]->getComponent<File>();
		if (file) {
			files.push_back(file);
		}
	}

	mutex.unlock();
}

/*void FileMonitor::addFile(MonitoredFile* resource) {
	FileResource r(resource);
	mutex.lock();
	fileResources.push_back(r);
	mutex.unlock();
}

void FileMonitor::removeFile(MonitoredFile* file) {
	mutex.lock();
	for (int f = 0; f < fileResources.size(); f++) {
		if (file == fileResources[f].file) {
			fileResources.erase(fileResources.begin()+f);
			break;
		}
	}
	mutex.unlock();
}*/

}