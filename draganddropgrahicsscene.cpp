#include "draganddropgraphicsscene.h";
#include "annotatetab.h";

DragAndDropGraphicsScene::DragAndDropGraphicsScene( AnnotateTab* annotateTab, ImageType imageType) {
	this->annotateTab = annotateTab;
	this->imageType = imageType;

	// Draw annotations if any
	QPainter painter(this->imageType == Color ? this->annotateTab->getAnnotatedColorImage() : this->annotateTab->getAnnotatedDepthToColorImage());

	painter.setPen(QPen(Qt::red, 8, Qt::SolidLine, Qt::RoundCap));

	for (int i = 0; i < NUM_ANNOTATIONS; ++i) {
		if (!this->annotateTab->getAnnotations()[i].isNull()) {
			painter.drawPoint(this->annotateTab->getAnnotations()[i].x(), this->annotateTab->getAnnotations()[i].y());
		}
	}

	painter.end();

	this->addPixmap(QPixmap::fromImage(this->imageType == Color ? *this->annotateTab->getAnnotatedColorImage() : *this->annotateTab->getAnnotatedDepthToColorImage()));
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
		this->annotateTab->recopyAnnotatedImage();

		float x = event->scenePos().x(), y = event->scenePos().y();
		this->annotateTab->getAnnotations()[this->pointIndex].setX(x);
		this->annotateTab->getAnnotations()[this->pointIndex].setY(y);

		QPainter painter(this->annotateTab->getAnnotatedColorImage());
		painter.setPen(QPen(Qt::red, 8, Qt::SolidLine, Qt::RoundCap));
		for (int i = 0; i < NUM_ANNOTATIONS; ++i) {
			painter.drawPoint(this->annotateTab->getAnnotations()[i].x(), this->annotateTab->getAnnotations()[i].y());
		}

		painter.end();

		QPainter painter2(this->annotateTab->getAnnotatedDepthToColorImage());
		painter2.setPen(QPen(Qt::red, 8, Qt::SolidLine, Qt::RoundCap));
		for (int i = 0; i < NUM_ANNOTATIONS; ++i) {
			painter2.drawPoint(this->annotateTab->getAnnotations()[i].x(), this->annotateTab->getAnnotations()[i].y());
		}

		painter2.end();

		// Create a copy of rescaled annotated image to be displayed
		int width = this->annotateTab->getParent()->ui.graphicsViewAnnotation->width(), height = this->annotateTab->getParent()->ui.graphicsViewAnnotation->height();
		QImage annotatedColorImageRescaled = this->annotateTab->getAnnotatedColorImage()->scaled(width, height, Qt::KeepAspectRatio);

		width = this->annotateTab->getParent()->ui.graphicsViewAnnotation2->width();  height = this->annotateTab->getParent()->ui.graphicsViewAnnotation2->height();
		QImage annotatedDepthToColorImageRescaled = this->annotateTab->getAnnotatedDepthToColorImage()->scaled(width, height, Qt::KeepAspectRatio);
		
		this->annotateTab->getColorScene()->addPixmap(QPixmap::fromImage(*this->annotateTab->getAnnotatedColorImage()));
		this->annotateTab->getDepthToColorScene()->addPixmap(QPixmap::fromImage(*this->annotateTab->getAnnotatedDepthToColorImage()));
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
