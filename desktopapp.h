#ifndef DESKTOPAPP_H
#define DESKTOPAPP_H

#define MAX_IMAGE_QUEUE_SIZE 10
#define MAX_GYROSCOPE_QUEUE_SIZE 30
#define MAX_ACCELEROMETER_QUEUE_SIZE 30

#include <QtWidgets/QWidget>
#include "ui_desktopapp.h"
#include <k4a/k4a.hpp>
#include "stdafx.h"
#include "patient.h"

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
    k4a_imu_sample_t imuSample;
    
    ViewTab* viewTab;
    CaptureTab* captureTab;
    AnnotateTab* annotateTab;

    std::queue<k4a_image_t> colorImageQueue;
    std::queue<k4a_image_t> depthImageQueue;
    std::queue<k4a_image_t> irImageQueue;
    std::deque<k4a_float3_t> gyroSampleQueue;
    std::deque<k4a_float3_t> accSampleQueue;
    QImage currentCapturedImage;

    Patient patient;

    void setTextOnGraphicsViews(char* text);
    QImage getQColorImage();
    QImage getQDepthImage();
    QImage getQIRImage();
    QImage getQDepthToColorImage();
    QImage getQColorToDepthImage();
    k4a_result_t copyk4aImage(k4a_image_t* src, k4a_image_t* target);
    k4a_result_t alignk4APointCloud(k4a_image_t* k4aDepthImage, k4a_image_t* target);
};

#endif
