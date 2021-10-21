#ifndef ANNOTATETAB_H
#define ANNOTATETAB_H
#define NUM_ANNOTATIONS 6

#include <QtWidgets/QWidget>
#include "stdafx.h"
#include "desktopapp.h"
#include <k4a/k4a.hpp>
#include <opencv2/opencv.hpp>

class AnnotateTab : public QWidget
{
    Q_OBJECT

public:
    AnnotateTab(DesktopApp* parent);

    void reloadCurrentImage();
    DesktopApp* getParent();
    QImage* getImage();
    QImage* getAnnotatedImage();
    QPointF* getAnnotations();
    std::vector<QPointF> getConvexHull();
    void setAnnotationsText();
    void recopyAnnotatedImage();

private:
    QImage image;
    QImage annotatedImage;
    QPointF annotations[NUM_ANNOTATIONS];
    DesktopApp* parent;
    void drawAnnotations();
};

// Helper function for calculating convex hull given a set of points
int orientation(QPointF p, QPointF q, QPointF r);
#endif
