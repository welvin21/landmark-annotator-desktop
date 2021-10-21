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

private:
    DesktopApp* parent;
    void setDefaultCaptureMode();
};

#endif
