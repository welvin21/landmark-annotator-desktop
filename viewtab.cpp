#include "viewtab.h"

ViewTab::ViewTab(DesktopApp* parent)
{
    this->parent = parent;

    this->timer = new QTimer;
    QObject::connect(timer, &QTimer::timeout, [this]() {
        if (this->parent->deviceCount > 0) {
            switch (k4a_device_get_capture(this->parent->device, &this->parent->capture, K4A_WAIT_INFINITE)) {
            case K4A_WAIT_RESULT_SUCCEEDED:
                break;
            }

            if (this->parent->capture) {
                k4a_image_t k4aColorImage = k4a_capture_get_color_image(this->parent->capture);

                if (k4aColorImage != NULL) {
                    this->parent->currentColorImage = k4aColorImage;

                    int width = this->parent->ui.graphicsViewVideo->width(), height = this->parent->ui.graphicsViewVideo->height();
                    QImage qColorImage = (this->parent->getQColorImage()).scaled(width, height, Qt::KeepAspectRatio);
                    QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap::fromImage(qColorImage));
                    QGraphicsScene* scene = new QGraphicsScene;
                    scene->addItem(item);

                    this->parent->ui.graphicsViewVideo->setScene(scene);
                    this->parent->ui.graphicsViewVideo->show();

                    k4a_image_release(k4aColorImage);
                }

                k4a_image_t k4aDepthImage = k4a_capture_get_depth_image(this->parent->capture);

                if (k4aDepthImage != NULL) {
                    this->parent->currentDepthImage = k4aDepthImage;

                    int width = this->parent->ui.graphicsViewVideo2->width(), height = this->parent->ui.graphicsViewVideo2->height();
                    QImage qDepthImage = (this->parent->getQDepthImage()).scaled(width, height, Qt::KeepAspectRatio);
                    QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap::fromImage(qDepthImage));
                    QGraphicsScene* scene = new QGraphicsScene;
                    scene->addItem(item);

                    this->parent->ui.graphicsViewVideo2->setScene(scene);
                    this->parent->ui.graphicsViewVideo2->show();

                    k4a_image_release(k4aDepthImage);
                }

                k4a_image_t k4aIRImage = k4a_capture_get_ir_image(this->parent->capture);

                if (k4aIRImage != NULL) {
                    this->parent->currentIRImage = k4aIRImage;

                    int width = this->parent->ui.graphicsViewVideo3->width(), height = this->parent->ui.graphicsViewVideo3->height();
                    QImage qIRImage = (this->parent->getQIRImage()).scaled(width, height, Qt::KeepAspectRatio);
                    QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap::fromImage(qIRImage));
                    QGraphicsScene* scene = new QGraphicsScene;
                    scene->addItem(item);

                    this->parent->ui.graphicsViewVideo3->setScene(scene);
                    this->parent->ui.graphicsViewVideo3->show();

                    k4a_image_release(k4aIRImage);
                }

                k4a_capture_release(this->parent->capture);
            }
            else {
                qDebug() << "No capture found\n";
            }
        }
    });
}

