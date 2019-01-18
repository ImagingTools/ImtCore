#include <imtreportgui/CGraphicsElementShapeFactory.h>


// ACF includes
#include <iqt/iqt.h>

// Qt includes
#include <QtGui/QPen>
#include <QtGui/QFont>
#include <QtGui/QFontMetrics>
#include <QtGui/QScreen>
#include <QtGui/QGuiApplication>
#if QT_VERSION >= 0x050000
#include <QtWidgets/QGraphicsRectItem>
#else
#include <QtGui/QGraphicsRectItem>
#endif

// GraphicsDocument includes
#include <imtreport/TGraphicsElement.h>


namespace imtreportgui
{


// public methods

QGraphicsItem* CGraphicsElementShapeFactory::CreateShape(const imtreport::IGraphicsElement& graphicsElement) const
{
	double strokeWidth = -1;
	QGraphicsItem* itemPtr = NULL;
	const imtreport::CRectangleElement* rectangleElementPtr = dynamic_cast<const imtreport::CRectangleElement*> (&graphicsElement);
	if (rectangleElementPtr != NULL){
		QRectF rect = MapRectToScene(iqt::GetQRectF(*rectangleElementPtr));
		itemPtr = new QGraphicsRectItem(rect);
		strokeWidth = rectangleElementPtr->GetStrokeWidth();
	}

	const imtreport::CCircleElement* circleElementPtr = dynamic_cast<const imtreport::CCircleElement*>(&graphicsElement);
	if (circleElementPtr != NULL){
		QRectF rect = MapRectToScene(iqt::GetQRectF(circleElementPtr->GetBoundingBox()));
		itemPtr = new QGraphicsEllipseItem(rect);
	}

	const imtreport::CTextLabelElement* labelElementPtr = dynamic_cast<const imtreport::CTextLabelElement*> (&graphicsElement);
	if (labelElementPtr != NULL){
		QGraphicsSimpleTextItem* textItemPtr = new QGraphicsSimpleTextItem(labelElementPtr->GetText());

		double fontSize = labelElementPtr->GetFontSize();
		fontSize *= 25.4 / 72.0;	// convert the font size in mm (1 pt = 25.4/72 mm = 0.3527 mm)

		// still unclear why the screen transformation of the scene displays the text to big - so scale it down hardcoded:
		fontSize *= 0.9;

		QFont labelFont(labelElementPtr->GetFontName(), fontSize);
		textItemPtr->setFont(labelFont);

		textItemPtr->setPos(MapPointToScene(labelElementPtr->GetPosition()));

		itemPtr = textItemPtr;
	}

	const imtreport::CPolygonElement* polygonElementPtr = dynamic_cast<const imtreport::CPolygonElement*> (&graphicsElement);
	if (polygonElementPtr != NULL){
		QPolygonF polygon;
		for (int i = 0; i < polygonElementPtr->GetNodesCount(); ++i){
			const i2d::CVector2d& node = polygonElementPtr->GetNodePos(i);
			polygon.append(MapPointToScene(node));
		}

		itemPtr = new QGraphicsPolygonItem(polygon);
	}

	const imtreport::CImageRectangleElement* imageRectangleElementPtr = dynamic_cast<const imtreport::CImageRectangleElement*> (&graphicsElement);
	if (imageRectangleElementPtr != NULL){
		QString imgPath = imageRectangleElementPtr->GetImagePath();

		if (!QFileInfo(imgPath).exists()){
			qDebug(QString("CGraphicsElementShapeFactory: Image %1 can not be found!").arg(imgPath).toLocal8Bit().constData());
		}

		//pixmap = pixmap.scaled(rect.width(), rect.height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

		QPixmap pixmap(imgPath);
		itemPtr = new QGraphicsPixmapItem(pixmap);

		QRectF rect = MapRectToScene(iqt::GetQRectF(*imageRectangleElementPtr));
		itemPtr->setPos(rect.topLeft());

		QSize s = pixmap.size();

		if (s.width() > 0){
			double scale = rect.width() / s.width();
			itemPtr->setScale(scale);
		}
		else{
			itemPtr->setScale(1);
		}
	}
	
	QAbstractGraphicsShapeItem* abstractGraphicsShapeItemPtr = dynamic_cast<QAbstractGraphicsShapeItem*> (itemPtr);
	if (abstractGraphicsShapeItemPtr != NULL){
		QPen pen(graphicsElement.GetStrokeColor());

		if (strokeWidth > 0)
			pen.setWidthF(strokeWidth);

		abstractGraphicsShapeItemPtr->setPen(pen);
		abstractGraphicsShapeItemPtr->setBrush(graphicsElement.GetFillColor());
	}

	return itemPtr;
}


// private methods

QPointF CGraphicsElementShapeFactory::MapPointToScene(const QPointF& point)
{
	// map shape coordinates given in mm to scene's coordinates
	QScreen* screen = QGuiApplication::primaryScreen();
	Q_ASSERT(screen);

	return QPointF(point.x() * screen->physicalDotsPerInchX() / 25.4,
                   point.y() * screen->physicalDotsPerInchY() / 25.4);
}


QRectF CGraphicsElementShapeFactory::MapRectToScene(const QRectF& rect)
{
	return QRectF(MapPointToScene(rect.topLeft()),
		MapPointToScene(rect.bottomRight()));
}


} // namespace imtreportgui


