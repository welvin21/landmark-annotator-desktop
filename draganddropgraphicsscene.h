#ifndef DRAGANDDROPGRAPHICSSCENE_H
#define DRAGANDDROPGRAPHICSSCENE_H
#define NUM_ANNOTATIONS 6

#include "stdafx.h"

// Forward Declaration
class AnnotateTab;

class DragAndDropGraphicsScene : public QGraphicsScene {
public:
	DragAndDropGraphicsScene(AnnotateTab* annotateTab);
	void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
	void dropEvent(QGraphicsSceneDragDropEvent* event) override;
	void dragEnterEvent(QGraphicsSceneDragDropEvent* event) override;
	void dragMoveEvent(QGraphicsSceneDragDropEvent* event) override;

private:
	AnnotateTab* annotateTab;
	bool isPoint;
	int pointIndex;
};

#endif