#ifndef RECORDER
#define RECORDER

#define MAX_RECORDING_SECONDS 10
#define VIDEOWRITER_FPS 15
#define COLOR_IMAGE_WIDTH 1280
#define COLOR_IMAGE_HEIGHT 720 

#include "stdafx.h"
#include "desktopapp.h"
#include "helper.h"

class Recorder {
private:
	bool isRecording;
	int counter;
	DesktopApp* parent;
	cv::VideoWriter* videoWriter;
	QString outputFilename;

public:
	QTimer* timer;
	Recorder(DesktopApp*);
	bool getRecordingStatus();
	void setRecordingStatus(bool);
	void prepareRecorder();
	cv::VideoWriter* getVideoWriter();
	QString getOutputFilename();
};

#endif
