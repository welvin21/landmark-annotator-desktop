#include "annotatetab.h"
#include "draganddropgraphicsscene.h"

AnnotateTab::AnnotateTab(DesktopApp* parent) {
	this->parent = parent;

	this->image = this->parent->getQCurrentImage().copy();

	int width = this->parent->ui.graphicsViewAnnotation->width(), height = this->parent->ui.graphicsViewAnnotation->height();
	this->annotatedImage = this->image.copy().scaled(width, height, Qt::KeepAspectRatio);

	this->scene = new DragAndDropGraphicsScene(this);

	this->parent->ui.graphicsViewAnnotation->setScene(this->scene);
	this->parent->ui.graphicsViewAnnotation->show();

	QObject::connect(this->parent->ui.annotateButtonAnnotateTab, &QPushButton::clicked, [this]() {
		int width = this->annotatedImage.width(), height = this->annotatedImage.height();

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
            this->annotatedImage.save(fileName);
			
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

	this->image = this->parent->getQCurrentImage().copy();

	int width = this->parent->ui.graphicsViewAnnotation->width(), height = this->parent->ui.graphicsViewAnnotation->height();
	this->annotatedImage = this->image.copy().scaled(width, height, Qt::KeepAspectRatio);

	// Deallocate heap memory used by previous GGraphicsScene object
    if (this->scene) {
		delete this->scene;
    }

	this->scene = new DragAndDropGraphicsScene(this);

	this->parent->ui.graphicsViewAnnotation->setScene(this->scene);
	this->parent->ui.graphicsViewAnnotation->show();
}

void AnnotateTab::drawAnnotations() {
	// Deallocate heap memory used by previous GGraphicsScene object
    if (this->scene) {
		delete this->scene;
    }
	
	this->recopyAnnotatedImage();

	this->scene = new DragAndDropGraphicsScene(this);
	
	this->parent->ui.graphicsViewAnnotation->setScene(this->scene);
    this->parent->ui.graphicsViewAnnotation->show();
}


DesktopApp* AnnotateTab::getParent() {
	return this->parent;
}

QImage* AnnotateTab::getImage() {
	return &this->image;
}

QImage* AnnotateTab::getAnnotatedImage() {
	return &this->annotatedImage;
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

std::vector<QPointF> AnnotateTab::getConvexHull() {
	// Min number of points 3
	if (NUM_ANNOTATIONS < 3) return {};

	// Find leftmost point in annotations
	int l = 0;
	for (int i = 1; i < NUM_ANNOTATIONS; i++)
		if (this->annotations[i].x() < this->annotations[l].x())
			l = i;

	std::vector<QPointF> convexHullPoints;

	int p = l, q;
	do
	{
		convexHullPoints.push_back(QPointF(this->annotations[p].x(), this->annotations[p].y()));

		q = (p + 1) % NUM_ANNOTATIONS;
		for (int i = 0; i < NUM_ANNOTATIONS; i++)
		{
			if (orientation(this->annotations[p], this->annotations[i], this->annotations[q]) == 2)
				q = i;
		}

		p = q;

	} while (p != l);
	
	// Copy first point into last element
	if(convexHullPoints.size() > 0)
		convexHullPoints.push_back(convexHullPoints[0]);

	return convexHullPoints;
}

void AnnotateTab::setAnnotationsText() {
	QString text = "";
	if (this->annotations[0].isNull()) text.append("No annotations");
	else {
		for (int i = 0; i < NUM_ANNOTATIONS; ++i) {
			float x = this->annotations[i].x(), y = this->annotations[i].y();
			std::string plain_s = "Point " + std::to_string(i) + ": (" + std::to_string(x) + ", " + std::to_string(y) + ")\n";
			QString str = QString::fromUtf8(plain_s.c_str());
			text.append(str);
		}
	}

	this->parent->ui.annotationsText->setText(text);
}

void AnnotateTab::recopyAnnotatedImage() {
	int width = this->parent->ui.graphicsViewAnnotation->width(), height = this->parent->ui.graphicsViewAnnotation->height();
	this->annotatedImage = this->image.copy().scaled(width, height, Qt::KeepAspectRatio);
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
