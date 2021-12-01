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
    int* getScalingFactor();
    std::map<std::string, QVector3D>* getAnnotations3D();
    QVector3D query3DPoint(int x, int y);

private:
    QImage colorImage;
    QImage annotatedColorImage;
    QImage depthToColorImage;
    QImage annotatedDepthToColorImage;
    std::map<std::string, QPointF> annotations;
    std::map<std::string, QVector3D> annotations3D;
    DesktopApp* parent;
    DragAndDropGraphicsScene* colorScene;
    DragAndDropGraphicsScene* depthToColorScene;
    int scalingFactor;
    void drawAnnotations();
    QJsonDocument getAnnotationsJson();
};

// Helper functions
QPointF getRandomPoint(int maxWidth, int maxHeight);
#endif
