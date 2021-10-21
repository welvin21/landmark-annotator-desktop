#include "capturetab.h"

CaptureTab::CaptureTab(DesktopApp* parent)
{
    this->parent = parent;

    this->setDefaultCaptureMode();

    QObject::connect(this->parent->ui.saveButton, &QPushButton::clicked, [this]() {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image File"), QString(), tr("Images (*.png)"));
        int width = this->parent->ui.graphicsViewImage->width(), height = this->parent->ui.graphicsViewImage->height();
        QImage image = this->parent->getQCurrentImage().scaled(width, height, Qt::KeepAspectRatio);

        if (!fileName.isEmpty())
        {
            image.save(fileName);
        }
    });

    QObject::connect(this->parent->ui.captureButton, &QPushButton::clicked, [this]() {
        QImage image;

        if (this->parent->ui.radioButton->isChecked()) {
            image = this->parent->getQColorImage();
        }
        else if (this->parent->ui.radioButton2->isChecked()) {
            image = this->parent->getQDepthImage();
        }
        else if (this->parent->ui.radioButton3->isChecked()) {
            image = this->parent->getQIRImage();
        }
        else {
            image = this->parent->getQAlignmentImage();
        }

        int width = this->parent->ui.graphicsViewImage->width(), height = this->parent->ui.graphicsViewImage->height();
        this->parent->currentImage = image.copy();

        QImage imageScaled = image.scaled(width, height, Qt::KeepAspectRatio);

        QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap::fromImage(imageScaled));
        QGraphicsScene* scene = new QGraphicsScene;
        scene->addItem(item);

        this->parent->ui.graphicsViewImage->setScene(scene);
        this->parent->ui.graphicsViewImage->show();
    });

    QObject::connect(this->parent->ui.annotateButtonCaptureTab, &QPushButton::clicked, [this]() {
        // Move to annotate tab whose index is 2
        this->parent->annotateTab->reloadCurrentImage();
        this->parent->ui.tabWidget->setCurrentIndex(2);
    });

    this->timer = new QTimer;
    QObject::connect(timer, &QTimer::timeout, [this]() {
        if (this->parent->deviceCount > 0) {
            switch (k4a_device_get_capture(this->parent->device, &this->parent->capture, K4A_WAIT_INFINITE)) {
                case K4A_WAIT_RESULT_SUCCEEDED:
                    break;
            }

            if (this->parent->capture) {
                // For every k4a_image_t object under this block of code
                // It should be released using the k4a_image_release() function for memory deallocation
                // We should find a way to copy the corresponding object so that they can be stored
                // as a member variable in this->parent object

                k4a_image_t k4aColorImage = k4a_capture_get_color_image(this->parent->capture);

                if (k4aColorImage != NULL) {
                    this->parent->currentColorImage = k4aColorImage;

                    int width = this->parent->ui.graphicsViewVideo->width(), height = this->parent->ui.graphicsViewVideo->height();
                    QImage qColorImage = (this->parent->getQColorImage()).scaled(width, height, Qt::KeepAspectRatio);
                    QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap::fromImage(qColorImage));
                    QGraphicsScene* scene = new QGraphicsScene;
                    scene->addItem(item);

                    this->parent->ui.graphicsViewVideo4->setScene(scene);
                    this->parent->ui.graphicsViewVideo4->show();
                }

                k4a_image_t k4aDepthImage = k4a_capture_get_depth_image(this->parent->capture);

                if (k4aDepthImage != NULL) this->parent->currentDepthImage = k4aDepthImage;

                k4a_image_t k4aIRImage = k4a_capture_get_ir_image(this->parent->capture);

                if (k4aIRImage != NULL) this->parent->currentIRImage = k4aIRImage;

                k4a_capture_release(this->parent->capture);
            }
            else {
                qDebug() << "No capture found\n";
            }
        }
    });

}

void CaptureTab::setDefaultCaptureMode() {
    parent->ui.radioButton->setChecked(true);
    parent->ui.radioButton2->setChecked(false);
    parent->ui.radioButton3->setChecked(false);
    parent->ui.radioButton4->setChecked(false);
}
