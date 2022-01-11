#include "recorder.h"

Recorder::Recorder(DesktopApp* parent) {
	this->isRecording = false;
	this->counter = 0;
	this->parent = parent;
    this->timer = new QTimer;

	QObject::connect(this->timer, &QTimer::timeout, [this]() {
		this->counter += 1;

		if (this->counter == 5) {
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