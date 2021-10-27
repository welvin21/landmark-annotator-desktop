#ifndef DESKTOPAPP_H
#define DESKTOPAPP_H

#define MAX_IMAGE_QUEUE_SIZE 10

#include <QtWidgets/QWidget>
#include "ui_desktopapp.h"
#include <k4a/k4a.hpp>
#include <queue>

// Forward declaration to break circular dependency
// Since DesktopApp have member variables of type <View | Capture | Annotate>Tab
// And each tab classes has member variable of type DesktopApp
class ViewTab;
class CaptureTab;
class AnnotateTab;

class DesktopApp : public QWidget
{
    Q_OBJECT

public:
    DesktopApp(QWidget *parent = Q_NULLPTR);

    Ui::DesktopAppClass ui;
    uint32_t deviceCount = 0;
    k4a_device_t device = NULL;
    k4a_device_configuration_t deviceConfig = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
    k4a_capture_t capture = NULL;
    
    ViewTab* viewTab;
    CaptureTab* captureTab;
    AnnotateTab* annotateTab;

    std::queue<k4a_image_t> colorImageQueue;
    std::queue<k4a_image_t> depthImageQueue;
    std::queue<k4a_image_t> irImageQueue;
    QImage currentImage;

    void setTextOnGraphicsViews(char* text);
    QImage getQColorImage();
    QImage getQDepthImage();
    QImage getQIRImage();
    QImage getQAlignmentImage();
    QImage getQCurrentImage();
};

#endif
