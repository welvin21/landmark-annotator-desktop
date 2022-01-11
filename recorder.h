#ifndef RECORDER
#define RECORDER

#include "stdafx.h"
#include "desktopapp.h"

class Recorder {
private:
	bool isRecording;
	int counter;
	DesktopApp* parent;

public:
	QTimer* timer;
	Recorder(DesktopApp*);
	bool getRecordingStatus();
	void setRecordingStatus(bool);
};

#endif
