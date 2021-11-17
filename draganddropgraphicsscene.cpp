#include "draganddropgraphicsscene.h";
#include "annotatetab.h";

DragAndDropGraphicsScene::DragAndDropGraphicsScene( AnnotateTab* annotateTab, ImageType imageType) {
	this->annotateTab = annotateTab;
	this->imageType = imageType;

	// Draw annotations if any
	QPainter painter(this->imageType == Color ? this->annotateTab->getAnnotatedColorImage() : this->annotateTab->getAnnotatedDepthToColorImage());

	painter.setPen(QPen(Qt::red, 8, Qt::SolidLine, Qt::RoundCap));
	for(auto it: *this->annotateTab->getAnnotations()) {
		if(!it.second.isNull()) painter.drawPoint(it.second.x(), it.second.y());
	}

	painter.setPen(QPen(Qt::white , 2, Qt::SolidLine, Qt::RoundCap));
	for(auto it: *this->annotateTab->getAnnotations()) {
		if(!it.second.isNull()) painter.drawText(it.second.x(), it.second.y(), QString::fromStdString(it.first));
	}

	painter.end();

	this->addPixmap(QPixmap::fromImage(this->imageType == Color ? *this->annotateTab->getAnnotatedColorImage() : *this->annotateTab->getAnnotatedDepthToColorImage()));
	this->annotateTab->setAnnotationsText();
}

void DragAndDropGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
	float x = event->scenePos().x(), y = event->scenePos().y();
	this->isPoint = false;
	this->pointKey = "";

	for(auto it: *this->annotateTab->getAnnotations()) {
		if (abs(it.second.x() - x) <= 5 && abs(it.second.y() - y) <= 5) {
			this->isPoint = true;
			this->pointKey = it.first;
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
		(*this->annotateTab->getAnnotations())[this->pointKey].setX(x);
		(*this->annotateTab->getAnnotations())[this->pointKey].setY(y);

		QPainter painter(this->annotateTab->getAnnotatedColorImage());
		QPainter painter2(this->annotateTab->getAnnotatedDepthToColorImage());

		painter.setPen(QPen(Qt::red, 8, Qt::SolidLine, Qt::RoundCap));
		painter2.setPen(QPen(Qt::red, 8, Qt::SolidLine, Qt::RoundCap));
		for(auto it: *this->annotateTab->getAnnotations()) {
			painter.drawPoint(it.second.x(), it.second.y());
			painter2.drawPoint(it.second.x(), it.second.y());
		}

		painter.setPen(QPen(Qt::white , 2, Qt::SolidLine, Qt::RoundCap));
		painter2.setPen(QPen(Qt::white , 2, Qt::SolidLine, Qt::RoundCap));
		for(auto it: *this->annotateTab->getAnnotations()) {
			painter.drawText(it.second.x(), it.second.y(), QString::fromStdString(it.first));
			painter2.drawText(it.second.x(), it.second.y(), QString::fromStdString(it.first));
		}

		painter.end();
		painter2.end();
		
		if (this->imageType == Color) {
			this->addPixmap(QPixmap::fromImage(*this->annotateTab->getAnnotatedColorImage()));
			this->annotateTab->getDepthToColorScene()->addPixmap(QPixmap::fromImage(*this->annotateTab->getAnnotatedDepthToColorImage()));
		}

		if (this->imageType == DepthToColor) {
			this->annotateTab->getColorScene()->addPixmap(QPixmap::fromImage(*this->annotateTab->getAnnotatedColorImage()));
			this->addPixmap(QPixmap::fromImage(*this->annotateTab->getAnnotatedDepthToColorImage()));
		}
			
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
