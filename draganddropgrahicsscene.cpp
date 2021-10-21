#include "draganddropgraphicsscene.h";
#include "annotatetab.h";

DragAndDropGraphicsScene::DragAndDropGraphicsScene( AnnotateTab* annotateTab) {
	this->annotateTab = annotateTab;
	this->annotatedImage = this->annotateTab->getImage()->copy();

	// Draw annotations if any
	QPainter painter(&this->annotatedImage);
	painter.setPen(QPen(Qt::red, 5, Qt::SolidLine, Qt::RoundCap));

	for (int i = 0; i < NUM_ANNOTATIONS; ++i) {
		if (!this->annotateTab->getAnnotations()[i].isNull()) {
			painter.drawPoint(this->annotateTab->getAnnotations()[i].x(), this->annotateTab->getAnnotations()[i].y());
		}
	}

	// Draw lines between convex hull if any
	std::vector<QPointF> convexHullPoints = this->annotateTab->getConvexHull();
	painter.setPen(QPen(Qt::white, 1, Qt::DashDotLine, Qt::RoundCap));

	int j;
	for (int i = 0; i < convexHullPoints.size() - 1; ++i) {
		j = i + 1;
		painter.drawLine(convexHullPoints[i].x(), convexHullPoints[i].y(), convexHullPoints[j].x(), convexHullPoints[j].y());
	}

	painter.end();

	this->addPixmap(QPixmap::fromImage(this->annotatedImage));
	this->annotateTab->setAnnotationsText();
}

void DragAndDropGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
	float x = event->scenePos().x(), y = event->scenePos().y();
	this->isPoint = false;
	this->pointIndex = -1;

	for (int i = 0; i < NUM_ANNOTATIONS; ++i) {
		if (abs(this->annotateTab->getAnnotations()[i].x() - x) <= 5 && abs(this->annotateTab->getAnnotations()[i].y() - y) <= 5) {
			this->isPoint = true;
			this->pointIndex = i;
		}
	}

	if (event->button() == Qt::LeftButton && this->isPoint) {
		QDrag* drag = new QDrag(this);
		QPixmap* point = new QPixmap(5, 5);
		point->fill(Qt::red);

		QMimeData* mimeData = new QMimeData;
		mimeData->setText("annotation");

		drag->setMimeData(mimeData);
		drag->setPixmap(*point);

		Qt::DropAction dropAction = drag->exec(Qt::MoveAction);
	}
}

void DragAndDropGraphicsScene::dropEvent(QGraphicsSceneDragDropEvent* event) {
	if (this->isPoint) {
		this->annotatedImage = this->annotateTab->getImage()->copy();

		float x = event->scenePos().x(), y = event->scenePos().y();
		this->annotateTab->getAnnotations()[this->pointIndex].setX(x);
		this->annotateTab->getAnnotations()[this->pointIndex].setY(y);

		QPainter painter(&annotatedImage);
		painter.setPen(QPen(Qt::red, 5, Qt::SolidLine, Qt::RoundCap));
		for (int i = 0; i < NUM_ANNOTATIONS; ++i) {
			painter.drawPoint(this->annotateTab->getAnnotations()[i].x(), this->annotateTab->getAnnotations()[i].y());
		}

		std::vector<QPointF> convexHullPoints = this->annotateTab->getConvexHull();
		painter.setPen(QPen(Qt::white, 1, Qt::DashDotLine, Qt::RoundCap));

		int j;
		for (int i = 0; i < convexHullPoints.size() - 1; ++i) {
			j = i + 1;
			painter.drawLine(convexHullPoints[i].x(), convexHullPoints[i].y(), convexHullPoints[j].x(), convexHullPoints[j].y());
		}

		painter.end();

		QPixmap item = QPixmap::fromImage(this->annotatedImage);
		this->addPixmap(item);
		this->annotateTab->setAnnotationsText();
	}
	
	event->acceptProposedAction();
}

void DragAndDropGraphicsScene::dragEnterEvent(QGraphicsSceneDragDropEvent* event) {
	event->acceptProposedAction();
}

void DragAndDropGraphicsScene::dragMoveEvent(QGraphicsSceneDragDropEvent* event) {
	event->acceptProposedAction();
}
