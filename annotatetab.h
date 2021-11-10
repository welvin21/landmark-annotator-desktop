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
    DesktopApp* getParent();
    QImage* getImage();
    QImage* getAnnotatedColorImage();
    QImage* getAnnotatedDepthToColorImage();
    QPointF* getAnnotations();
    std::vector<QPointF> getConvexHull();
    void setAnnotationsText();
    void recopyAnnotatedImage();
    DragAndDropGraphicsScene* getColorScene();
    DragAndDropGraphicsScene* getDepthToColorScene();

private:
    QImage colorImage;
    QImage annotatedColorImage;
    QImage depthToColorImage;
    QImage annotatedDepthToColorImage;
    QPointF annotations[NUM_ANNOTATIONS];
    DesktopApp* parent;
    DragAndDropGraphicsScene* colorScene;
    DragAndDropGraphicsScene* depthToColorScene;
    void drawAnnotations();
    QJsonDocument getAnnotationsJson();
};

// Helper function for calculating convex hull given a set of points
int orientation(QPointF p, QPointF q, QPointF r);
#endif
