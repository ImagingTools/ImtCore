#include <imtreportgui/CGraphicsElementShapeFactory.h>


// ACF includes
#include <iqt/iqt.h>

// Qt includes
#include <QtGui/QPen>
#include <QtGui/QFont>
#include <QtGui/QFontMetrics>
#if QT_VERSION >= 0x050000
#include <QtWidgets/QGraphicsRectItem>
#else
#include <QtGui/QGraphicsRectItem>
#endif

// ImtCore includes
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
		itemPtr = new QGraphicsRectItem();
		strokeWidth = rectangleElementPtr->GetStrokeWidth();
	}

	const imtreport::CCircleElement* circleElementPtr = dynamic_cast<const imtreport::CCircleElement*>(&graphicsElement);
	if (circleElementPtr != NULL){
		itemPtr = new QGraphicsEllipseItem();
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

		itemPtr = textItemPtr;
	}

	const imtreport::CPolygonElement* polygonElementPtr = dynamic_cast<const imtreport::CPolygonElement*> (&graphicsElement);
	if (polygonElementPtr != NULL){
		itemPtr = new QGraphicsPolygonItem();
	}

	const imtreport::CImageRectangleElement* imageRectangleElementPtr = dynamic_cast<const imtreport::CImageRectangleElement*> (&graphicsElement);
	if (imageRectangleElementPtr != NULL){
		QString imgPath = imageRectangleElementPtr->GetImagePath();

		if (!QFileInfo(imgPath).exists()){
			qDebug(QString("CGraphicsElementShapeFactory: Image %1 can not be found!").arg(imgPath).toLocal8Bit().constData());
		}

		//pixmap = pixmap.scaled(rect.width(), rect.height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
		itemPtr = new QGraphicsPixmapItem(QPixmap(imgPath));
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


} // namespace imtreportgui


