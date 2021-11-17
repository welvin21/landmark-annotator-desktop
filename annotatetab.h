#ifndef ANNOTATETAB_H
#define ANNOTATETAB_H
#define NUM_ANNOTATIONS 6

#include <QtWidgets/QWidget>
#include "stdafx.h"
#include "desktopapp.h"
#include <k4a/k4a.hpp>
#include <opencv2/opencv.hpp>
#include "draganddropgraphicsscene.h"
#include "capturetab.h"

enum ImageType {Color, DepthToColor};

class AnnotateTab : public QWidget
{
    Q_OBJECT

public:
    AnnotateTab(DesktopApp* parent);

    void reloadCurrentImage();
    QImage* getImage();
    QImage* getAnnotatedColorImage();
    QImage* getAnnotatedDepthToColorImage();
    std::map<std::string, QPointF>* getAnnotations();
    void setAnnotationsText();
    void recopyAnnotatedImage();
    DragAndDropGraphicsScene* getColorScene();
    DragAndDropGraphicsScene* getDepthToColorScene();

private:
    QImage colorImage;
    QImage annotatedColorImage;
    QImage depthToColorImage;
    QImage annotatedDepthToColorImage;
    std::map<std::string, QPointF> annotations;
    DesktopApp* parent;
    DragAndDropGraphicsScene* colorScene;
    DragAndDropGraphicsScene* depthToColorScene;
    void drawAnnotations();
    QJsonDocument getAnnotationsJson();
};

// Helper functions
QPointF getRandomPoint(int maxWidth, int maxHeight);
#endif
