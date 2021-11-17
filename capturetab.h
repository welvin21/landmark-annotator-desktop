#ifndef CAPTURETAB_H
#define CAPTURETAB_H

#include <QtWidgets/QWidget>
#include "stdafx.h"
#include "annotatetab.h"
#include "desktopapp.h"
#include <k4a/k4a.hpp>
#include <opencv2/opencv.hpp>

class CaptureTab: public QWidget
{
    Q_OBJECT

public:
    CaptureTab(DesktopApp* parent);
    QTimer* timer;
    QImage getQCapturedColorImage();
    QImage getQCapturedDepthToColorImage();

private:
    DesktopApp* parent;
    QImage colorImage;
    QImage depthImage;
    QImage colorToDepthImage;
    QImage depthToColorImage;
    void setDefaultCaptureMode();
    void registerRadioButtonOnClicked(QRadioButton* radioButton, QImage* image);
    void drawGyroscopeData();
    void drawAccelerometerData();
};

#endif
