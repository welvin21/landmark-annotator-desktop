#include "desktopapp.h"
#include "stdafx.h"
#include "viewtab.h"
#include "capturetab.h"
#include "annotatetab.h"
#include <opencv2/opencv.hpp>

DesktopApp::DesktopApp(QWidget* parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    // Setup azure kinect device

    uint32_t count = k4a_device_get_installed_count();
    this->deviceCount = count;

    if (count == 0) {
        this->setTextOnGraphicsViews("No kinect device found");

        return;
    }

    if (K4A_FAILED(k4a_device_open(K4A_DEVICE_DEFAULT, &this->device))) {
        this->setTextOnGraphicsViews("Can't connect to kinect device");

        return;
    }

    deviceConfig.camera_fps = K4A_FRAMES_PER_SECOND_30;
    deviceConfig.color_format = K4A_IMAGE_FORMAT_COLOR_BGRA32;
    deviceConfig.color_resolution = K4A_COLOR_RESOLUTION_720P;
    deviceConfig.depth_mode = K4A_DEPTH_MODE_NFOV_UNBINNED;

    if (K4A_FAILED(k4a_device_start_cameras(device, &this->deviceConfig))) {
        this->setTextOnGraphicsViews("Failed to start cameras");

        k4a_device_close(device);
       return;
    }

    this->viewTab = new ViewTab(this);
    this->captureTab = new CaptureTab(this);
    this->annotateTab = new AnnotateTab(this);

    if (this->ui.tabWidget->currentIndex() == 0) viewTab->timer->start(1000 / 30);
    if (this->ui.tabWidget->currentIndex() == 1) captureTab->timer->start(1000 / 30);

    QObject::connect(ui.tabWidget, &QTabWidget::currentChanged, [this]() {
        switch (this->ui.tabWidget->currentIndex()) {
            case 0:
                this->captureTab->timer->stop();
                this->viewTab->timer->start(1000 / 30);
                break;
            case 1:
                this->viewTab->timer->stop();
                this->captureTab->timer->start(1000 / 30);
                break;
            case 2:
                this->viewTab->timer->stop();
                this->captureTab->timer->stop();
            default:
                break;
        }
    });

}

void DesktopApp::setTextOnGraphicsViews(char* text) {
    QGraphicsTextItem* graphicsText = new QGraphicsTextItem;
    graphicsText->setPlainText(text);
    
    QGraphicsScene* scene = new QGraphicsScene;
    scene->addItem(graphicsText);

    ui.graphicsViewVideo->setScene(scene);
    ui.graphicsViewVideo2->setScene(scene);
    ui.graphicsViewVideo3->setScene(scene);
    ui.graphicsViewVideo4->setScene(scene);
    ui.graphicsViewImage->setScene(scene);
    ui.graphicsViewAnnotation->setScene(scene);
}

QImage DesktopApp::getQColorImage() {
    k4a_image_t k4aColorImage = this->colorImageQueue.back();

    cv::Mat matColorImage = cv::Mat(k4a_image_get_height_pixels(k4aColorImage), k4a_image_get_width_pixels(k4aColorImage), CV_8UC4, k4a_image_get_buffer(k4aColorImage));
            
    cv::Mat temp;

    cvtColor(matColorImage, temp, cv::COLOR_BGR2RGB);

    QImage qImage((const uchar*)temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
    qImage.bits();

    return qImage;
}

QImage DesktopApp::getQDepthImage() {
    k4a_image_t k4aDepthImage = this->depthImageQueue.back();

    double min, max;
    cv::Mat matDepthImageRaw = cv::Mat(k4a_image_get_height_pixels(k4aDepthImage), k4a_image_get_width_pixels(k4aDepthImage), CV_16U, k4a_image_get_buffer(k4aDepthImage), cv::Mat::AUTO_STEP);

    cv::minMaxIdx(matDepthImageRaw, &min, &max);
    cv::Mat matDepthImage;
    cv::convertScaleAbs(matDepthImageRaw, matDepthImage, 255 / max);

    cv::Mat temp;
    cvtColor(matDepthImage, temp, cv::COLOR_GRAY2RGB);

    QImage qImage((const uchar*)temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
    qImage.bits();

    return qImage;

}

QImage DesktopApp::getQIRImage() {
    k4a_image_t k4aIRImage = this->irImageQueue.back();

    double min, max;
    cv::Mat matIRImageRaw = cv::Mat(k4a_image_get_height_pixels(k4aIRImage), k4a_image_get_width_pixels(k4aIRImage), CV_16U, k4a_image_get_buffer(k4aIRImage), cv::Mat::AUTO_STEP);

    cv::minMaxIdx(matIRImageRaw, &min, &max);
    cv::Mat matIRImage;
    cv::convertScaleAbs(matIRImageRaw, matIRImage, 255 / max);

    cv::Mat temp;
    cvtColor(matIRImage, temp, cv::COLOR_GRAY2RGB);

    QImage qImage((const uchar*)temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
    qImage.bits();

    return qImage;
}

QImage DesktopApp::getQAlignmentImage() {
    k4a_image_t k4aDepthImage = this->depthImageQueue.back(), k4aColorImage = this->colorImageQueue.back();

    QImage qEmptyImage;

    if (k4aDepthImage != NULL) {
        k4a_calibration_t calibration;
        if (k4a_device_get_calibration(this->device, this->deviceConfig.depth_mode, this->deviceConfig.color_resolution, &calibration) != K4A_RESULT_SUCCEEDED) {
            return qEmptyImage;
        }

        k4a_transformation_t transformationHandle = k4a_transformation_create(&calibration);
        k4a_image_t alignmentImage;

        if (k4a_image_create(K4A_IMAGE_FORMAT_DEPTH16,
            k4a_image_get_width_pixels(k4aColorImage),
            k4a_image_get_height_pixels(k4aColorImage),
            k4a_image_get_width_pixels(k4aColorImage) * (int)sizeof(uint16_t),
            &alignmentImage) != K4A_RESULT_SUCCEEDED) return qEmptyImage;

        if (k4a_transformation_depth_image_to_color_camera(transformationHandle, k4aDepthImage, alignmentImage) != K4A_WAIT_RESULT_SUCCEEDED) {
            return qEmptyImage;
        }

        double min, max;
        cv::Mat matAlignmentImageRaw = cv::Mat(k4a_image_get_height_pixels(alignmentImage), k4a_image_get_width_pixels(alignmentImage), CV_16U, k4a_image_get_buffer(alignmentImage), cv::Mat::AUTO_STEP);

        cv::minMaxIdx(matAlignmentImageRaw, &min, &max);
        cv::Mat matAlignmentImage;
        cv::convertScaleAbs(matAlignmentImageRaw, matAlignmentImage, 255 / max);

        cv::Mat temp;
        cv::applyColorMap(matAlignmentImage, temp, cv:: COLORMAP_HSV);
        
        QImage qImage((const uchar*)temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
        qImage.bits();

        return qImage;
    }

    return qEmptyImage;
}

QImage DesktopApp::getQCurrentImage() {
    return this->currentImage;
}
