#include "capturetab.h"

CaptureTab::CaptureTab(DesktopApp* parent)
{
    this->parent = parent;

    this->setDefaultCaptureMode();

    this->registerRadioButtonOnClicked(this->parent->ui.radioButton, &this->colorImage);
    this->registerRadioButtonOnClicked(this->parent->ui.radioButton2, &this->depthImage);
    this->registerRadioButtonOnClicked(this->parent->ui.radioButton3, &this->colorToDepthImage);
    this->registerRadioButtonOnClicked(this->parent->ui.radioButton4, &this->depthToColorImage);

    QObject::connect(this->parent->ui.saveButtonCaptureTab, &QPushButton::clicked, [this]() {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image File"), QString(), tr("Images (*.png)"));
        int width = this->parent->ui.graphicsViewImage->width(), height = this->parent->ui.graphicsViewImage->height();
        QImage image = this->parent->getQCurrentImage().scaled(width, height, Qt::KeepAspectRatio);

        if (!fileName.isEmpty())
        {
            image.save(fileName);
        }
    });

    QObject::connect(this->parent->ui.captureButton, &QPushButton::clicked, [this]() {
        this->colorImage = this->parent->getQColorImage();
        this->depthImage = this->parent->getQDepthImage();
        this->colorToDepthImage = this->parent->getQColorToDepthImage();
        this->depthToColorImage = this->parent->getQDepthToColorImage();

        QImage image;

        if (this->parent->ui.radioButton->isChecked()) {
            image = this->colorImage;
        }
        else if (this->parent->ui.radioButton2->isChecked()) {
            image = this->depthImage;
        }
        else if (this->parent->ui.radioButton3->isChecked()) {
            image = this->colorToDepthImage;
        }
        else {
            image = this->depthToColorImage;
        }

        int width = this->parent->ui.graphicsViewImage->width(), height = this->parent->ui.graphicsViewImage->height();
        this->parent->currentImage = image.copy();

        QImage imageScaled = image.scaled(width, height, Qt::KeepAspectRatio);

        // Deallocate heap memory used by previous GGraphicsScene object
        if (this->parent->ui.graphicsViewImage->scene()) {
            delete this->parent->ui.graphicsViewImage->scene();
        }

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
                    this->parent->colorImageQueue.push(k4aColorImage);

                    int width = this->parent->ui.graphicsViewVideo->width(), height = this->parent->ui.graphicsViewVideo->height();
                    QImage qColorImage = (this->parent->getQColorImage()).scaled(width, height, Qt::KeepAspectRatio);

                    // Deallocate heap memory used by previous GGraphicsScene object
                    if (this->parent->ui.graphicsViewVideo4->scene()) {
                        delete this->parent->ui.graphicsViewVideo4->scene();
                    }
                    
                    QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap::fromImage(qColorImage));
                    QGraphicsScene* scene = new QGraphicsScene;
                    scene->addItem(item);

                    this->parent->ui.graphicsViewVideo4->setScene(scene);
                    this->parent->ui.graphicsViewVideo4->show();

                    while (this->parent->colorImageQueue.size() > MAX_IMAGE_QUEUE_SIZE) {
                        k4a_image_release(this->parent->colorImageQueue.front());
                        this->parent->colorImageQueue.pop();
                    }
                }

                k4a_image_t k4aDepthImage = k4a_capture_get_depth_image(this->parent->capture);

                if (k4aDepthImage != NULL) {
                    this->parent->depthImageQueue.push(k4aDepthImage);

                    int width = this->parent->ui.graphicsViewVideo5->width(), height = this->parent->ui.graphicsViewVideo5->height();
                    QImage qDepthImage = (this->parent->getQDepthImage()).scaled(width, height, Qt::KeepAspectRatio);

                    // Deallocate heap memory used by previous GGraphicsScene object
                    if (this->parent->ui.graphicsViewVideo->scene()) {
                        delete this->parent->ui.graphicsViewVideo5->scene();
                    }

                    QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap::fromImage(qDepthImage));
                    QGraphicsScene* scene = new QGraphicsScene;
                    scene->addItem(item);

                    this->parent->ui.graphicsViewVideo5->setScene(scene);
                    this->parent->ui.graphicsViewVideo5->show();

                    while (this->parent->depthImageQueue.size() > MAX_IMAGE_QUEUE_SIZE) {
                        k4a_image_release(this->parent->depthImageQueue.front());
                        this->parent->depthImageQueue.pop();
                    }
                }

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

void CaptureTab::registerRadioButtonOnClicked(QRadioButton* radioButton, QImage* image) {
    QObject::connect(radioButton, &QRadioButton::clicked, [this, image]() {
        int width = this->parent->ui.graphicsViewImage->width(), height = this->parent->ui.graphicsViewImage->height();
        this->parent->currentImage = (*image).copy();

        QImage imageScaled = (*image).scaled(width, height, Qt::KeepAspectRatio);

        // Deallocate heap memory used by previous GGraphicsScene object
        if (this->parent->ui.graphicsViewImage->scene()) {
            delete this->parent->ui.graphicsViewImage->scene();
        }

        QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap::fromImage(imageScaled));
        QGraphicsScene* scene = new QGraphicsScene;
        scene->addItem(item);

        this->parent->ui.graphicsViewImage->setScene(scene);
        this->parent->ui.graphicsViewImage->show();
    });
}