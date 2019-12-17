#ifndef SANDBOX_INPUT_TOUCH_TUIO_TOUCH_INPUT_H_
#define SANDBOX_INPUT_TOUCH_TUIO_TOUCH_INPUT_H_

#include "sandbox/input/TouchInput.h"

#include  <TUIO/TuioClient.h>


namespace sandbox {

#define TUIO_PORT  3333

class TUIOTouchInput : public TouchInput {
public:
	TUIOTouchInput(int port = TUIO_PORT, double  xScaleFactor = 1.0, double  yScaleFactor=1.0 ) : TouchInput(10), xScale(xScaleFactor), yScale(yScaleFactor), tuioClient(NULL) {
		tuioClient = new TUIO::TuioClient(port);
		tuioClient->connect();

		cursorPositions.resize(10);

		if (!tuioClient->isConnected())
		{  
			std::cout << "TUIOTouchInput: Cannot connect on port " << port << "." << std::endl;
		}
		else {
			std::cout << "Connected!" << std::endl;
		}
	}
	virtual ~TUIOTouchInput() {} 

	virtual bool getTouchState(int id) const { return cursorsDown.find(id) != cursorsDown.end(); }
	virtual glm::vec2 getPosition(int id) const { return cursorPositions[id]; }

	void update() {
		using namespace TUIO;

		// Send out events for TUIO "cursors" by polling the TuioClient for the current state
		std::list<TuioCursor*> cursorList = tuioClient->getTuioCursors();
		tuioClient->lockCursorList();

		// Send "button" up events for cursors that were down last frame, but are now up.
		std::vector<int> downLast;
		for (std::set<int>::iterator downLast_it = cursorsDown.begin(); downLast_it!= cursorsDown.end(); ++downLast_it ) {
			downLast.push_back(*downLast_it);
		}

		for (int i=0;i<downLast.size();i++) {
			bool stillDown = false;
			for (std::list<TuioCursor*>::iterator iter = cursorList.begin(); iter!=cursorList.end(); iter++) {
				TuioCursor *tcur = (*iter);
				if (tcur->getCursorID() == downLast[i]) {
					stillDown = true;
				}
			}
			if (!stillDown) {
				cursorsDown.erase(downLast[i]);
			}
		}

		// Send "button" down events for cursors that are new and updated positions for all cursors
		for (std::list<TuioCursor*>::iterator iter = cursorList.begin(); iter!=cursorList.end(); iter++) {
			TuioCursor *tcur = (*iter);

			if (cursorsDown.find(tcur->getCursorID()) == cursorsDown.end()) {
	            cursorPositions[tcur->getCursorID()] = glm::vec2((float)xScale*tcur->getX(), (float)yScale*(1.0f-tcur->getY()));
				cursorsDown.insert(tcur->getCursorID());
			}
			else if (tcur->getMotionSpeed() > 0.0) {
	            cursorPositions[tcur->getCursorID()] = glm::vec2((float)xScale*tcur->getX(), (float)yScale*(1.0f-tcur->getY()));
			}

			//std::cout << tcur->getCursorID() << " " << cursorPositions[tcur->getCursorID()].x << " " << cursorPositions[tcur->getCursorID()].y << std::endl;
		}
	    
		tuioClient->unlockCursorList();


		TouchInput::update();
	}

private:
	std::set<int> cursorsDown;
	std::vector<glm::vec2> cursorPositions;
	TUIO::TuioClient *tuioClient;
	double xScale;
	double yScale;
};


}

#endif