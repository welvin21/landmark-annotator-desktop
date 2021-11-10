#include "annotatetab.h"
#include "draganddropgraphicsscene.h"

AnnotateTab::AnnotateTab(DesktopApp* parent) {
	this->parent = parent;

	this->colorImage = this->parent->captureTab->getQCapturedColorImage().copy();
	this->depthToColorImage = this->parent->captureTab->getQCapturedDepthToColorImage().copy();

	int width = this->parent->ui.graphicsViewAnnotation->width(), height = this->parent->ui.graphicsViewAnnotation->height();
	this->annotatedColorImage = this->colorImage.copy().scaled(width, height, Qt::KeepAspectRatio);

	width = this->parent->ui.graphicsViewAnnotation2->width();  height = this->parent->ui.graphicsViewAnnotation2->height();
	this->annotatedDepthToColorImage = this->depthToColorImage.copy().scaled(width, height, Qt::KeepAspectRatio);

	this->colorScene = new DragAndDropGraphicsScene(this, Color);
	this->depthToColorScene = new DragAndDropGraphicsScene(this, DepthToColor);

	this->parent->ui.graphicsViewAnnotation->setScene(this->colorScene);
	this->parent->ui.graphicsViewAnnotation->show();

	this->parent->ui.graphicsViewAnnotation2->setScene(this->depthToColorScene);
	this->parent->ui.graphicsViewAnnotation2->show();

	QObject::connect(this->parent->ui.annotateButtonAnnotateTab, &QPushButton::clicked, [this]() {
		int width = this->annotatedColorImage.width(), height = this->annotatedColorImage.height();

		for (int i = 0; i < NUM_ANNOTATIONS; ++i) {
			int randX = rand() % (width + 1);
			int randY = rand() % (height + 1);

			this->annotations[i] = QPointF((float) randX, (float) randY);
		}

		this->drawAnnotations();
		this->setAnnotationsText();
	});

	QObject::connect(this->parent->ui.saveButtonAnnotateTab, &QPushButton::clicked, [this]() {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image File"), QString(), tr("Images (*.png)"));
        int width = this->parent->ui.graphicsViewAnnotation->width(), height = this->parent->ui.graphicsViewAnnotation->height();

        if (!fileName.isEmpty()) {
            this->annotatedColorImage.save(fileName);
			
			QString jsonFileName = QFileDialog::getSaveFileName(this, tr("Save coordinates json file"), QString(), tr("JSON (*.json)"));
			if (!jsonFileName.isEmpty()) {
				QFile jsonFile(jsonFileName);
				jsonFile.open(QFile::WriteOnly);

				QJsonDocument document = this->getAnnotationsJson();

				jsonFile.write(document.toJson());
			}
        }
	});
}

void AnnotateTab::reloadCurrentImage() {
	// Remove existing annotations in annotations member variable
	for (int i = 0; i < NUM_ANNOTATIONS; ++i) this->annotations[i] = QPoint();

	this->colorImage = this->parent->captureTab->getQCapturedColorImage().copy();
	this->depthToColorImage = this->parent->captureTab->getQCapturedDepthToColorImage().copy();

	int width = this->parent->ui.graphicsViewAnnotation->width(), height = this->parent->ui.graphicsViewAnnotation->height();
	this->annotatedColorImage = this->colorImage.copy().scaled(width, height, Qt::KeepAspectRatio);

	width = this->parent->ui.graphicsViewAnnotation2->width(); height = this->parent->ui.graphicsViewAnnotation2->height();
	this->annotatedDepthToColorImage = this->depthToColorImage.copy().scaled(width, height, Qt::KeepAspectRatio);

	// Deallocate heap memory used by previous GGraphicsScene object
    if (this->colorScene) delete this->colorScene;
	if (this->depthToColorScene) delete this->depthToColorScene;

	this->colorScene = new DragAndDropGraphicsScene(this, Color);
	this->depthToColorScene = new DragAndDropGraphicsScene(this, DepthToColor);

	this->parent->ui.graphicsViewAnnotation->setScene(this->colorScene);
	this->parent->ui.graphicsViewAnnotation->show();

	this->parent->ui.graphicsViewAnnotation2->setScene(this->depthToColorScene);
	this->parent->ui.graphicsViewAnnotation2->show();

    this->parent->ui.annotateButtonAnnotateTab->click();
}

void AnnotateTab::drawAnnotations() {
	// Deallocate heap memory used by previous GGraphicsScene object
    if (this->colorScene) delete this->colorScene;
    if (this->depthToColorScene) delete this->depthToColorScene;
	
	this->recopyAnnotatedImage();

	this->colorScene = new DragAndDropGraphicsScene(this, Color);
	this->depthToColorScene = new DragAndDropGraphicsScene(this, DepthToColor);
	
	this->parent->ui.graphicsViewAnnotation->setScene(this->colorScene);
    this->parent->ui.graphicsViewAnnotation->show();

	this->parent->ui.graphicsViewAnnotation2->setScene(this->depthToColorScene);
    this->parent->ui.graphicsViewAnnotation2->show();
}


DesktopApp* AnnotateTab::getParent() {
	return this->parent;
}

QImage* AnnotateTab::getImage() {
	return &this->colorImage;
}

QImage* AnnotateTab::getAnnotatedColorImage() {
	return &this->annotatedColorImage;
}

QImage* AnnotateTab::getAnnotatedDepthToColorImage() {
	return &this->annotatedDepthToColorImage;
}

QPointF* AnnotateTab::getAnnotations() {
	return this->annotations;
}

int orientation(QPointF p, QPointF q, QPointF r)
{
	int val = (q.y() - p.y()) * (r.x() - q.x()) -
		(q.x() - p.x()) * (r.y() - q.y());

	if (val == 0) return 0;  // collinear
	return (val > 0) ? 1 : 2; // clock or counterclock wise
}

void AnnotateTab::setAnnotationsText() {
	QString text = "";
	if (this->annotations[0].isNull()) text.append("No annotations");
	else {
		for (int i = 0; i < NUM_ANNOTATIONS; ++i) {
			int x = this->annotations[i].x(), y = this->annotations[i].y();
			std::string plain_s = "Point " + std::to_string(i) + ": (" + std::to_string(x) + ", " + std::to_string(y) + ")\n";
			QString str = QString::fromUtf8(plain_s.c_str());
			text.append(str);
		}
	}

	this->parent->ui.annotationsText->setText(text);
}

void AnnotateTab::recopyAnnotatedImage() {
	int width = this->parent->ui.graphicsViewAnnotation->width(), height = this->parent->ui.graphicsViewAnnotation->height();
	this->annotatedColorImage = this->colorImage.copy().scaled(width, height, Qt::KeepAspectRatio);

	width = this->parent->ui.graphicsViewAnnotation2->width();  height = this->parent->ui.graphicsViewAnnotation2->height();
	this->annotatedDepthToColorImage = this->depthToColorImage.copy().scaled(width, height, Qt::KeepAspectRatio);
}

QJsonDocument AnnotateTab::getAnnotationsJson() {
	QJsonObject emptyJsonObject{};
	QJsonDocument document;

	if (!this->annotations[0].isNull()) {
		QJsonObject coordinates;

		for (int i = 0; i < NUM_ANNOTATIONS; ++i) {
			QJsonObject coordinate;
			coordinate.insert("x", this->annotations[i].x());
			coordinate.insert("y", this->annotations[i].y());
			coordinates.insert(QString::number(i), coordinate);
		}

		emptyJsonObject.insert("coordinates", coordinates);
	}

	document.setObject(emptyJsonObject);
	return document;
}

DragAndDropGraphicsScene* AnnotateTab::getColorScene() {
	return this->colorScene;
}

DragAndDropGraphicsScene* AnnotateTab::getDepthToColorScene() {
	return this->depthToColorScene;
}
