#include "recorder.h"

Recorder::Recorder(DesktopApp* parent) {
	this->isRecording = false;
	this->counter = 0;
	this->parent = parent;
    this->timer = new QTimer;

	QObject::connect(this->timer, &QTimer::timeout, [this]() {
		this->counter += 1;

		if (this->counter == MAX_RECORDING_SECONDS) {
			// Stop recording here
			this->counter = 0;
			this->parent->ui.saveVideoButton->click();
		}
	});
}

bool Recorder::getRecordingStatus() {
	return this->isRecording;
}

void Recorder::setRecordingStatus(bool recordingStatus) {
	this->isRecording = recordingStatus;
}

void Recorder::prepareRecorder() {
	// Initialize output filename
	QString dateTimeString = Helper::getCurrentDateTimeString();
	this->outputFilename = this->parent->savePath.absolutePath() + "/recording_" + dateTimeString + ".mp4";

	// Initialize opencv VideoWriter
	cv::Size size(
		COLOR_IMAGE_WIDTH,
		COLOR_IMAGE_HEIGHT
	);

	this->videoWriter = new cv::VideoWriter(
		this->outputFilename.toStdString(),
		cv::VideoWriter::fourcc('H', '2', '6', '4'),
		VIDEOWRITER_FPS,
		size
	);
}

cv::VideoWriter* Recorder::getVideoWriter() { return this->videoWriter; }

QString Recorder::getOutputFilename() { return this->outputFilename; }