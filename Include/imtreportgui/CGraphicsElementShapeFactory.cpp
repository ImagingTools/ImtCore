#include <imtreportgui/CGraphicsElementShapeFactory.h>


// ACF includes
#include <iqt/iqt.h>

// Qt includes
#include <QtGui/QPen>
#include <QtGui/QFont>
#include <QtGui/QFontMetrics>
#include <QtGui/QTextDocument>
#include <QtGui/QTextOption>
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
	double strokeWidth = graphicsElement.GetStrokeWidth();
	QGraphicsItem* itemPtr = NULL;

	const imtreport::CRectangleElement* rectangleElementPtr = dynamic_cast<const imtreport::CRectangleElement*> (&graphicsElement);
	if (rectangleElementPtr != NULL){
		itemPtr = new QGraphicsRectItem();
	}

	const imtreport::CLineElement* lineElementPtr = dynamic_cast<const imtreport::CLineElement*>(&graphicsElement);
	if (lineElementPtr != NULL) {
		itemPtr = new QGraphicsLineItem();
	}

	const imtreport::CCircleElement* circleElementPtr = dynamic_cast<const imtreport::CCircleElement*>(&graphicsElement);
	if (circleElementPtr != NULL){
		itemPtr = new QGraphicsEllipseItem();
	}

	const imtreport::CTextLabelElement* labelElementPtr = dynamic_cast<const imtreport::CTextLabelElement*> (&graphicsElement);
	if (labelElementPtr != NULL){
		QGraphicsSimpleTextItem* textItemPtr = new QGraphicsSimpleTextItem(labelElementPtr->GetText());

		//QTextOption option = textItemPtr->document()->defaultTextOption();
		//option.setAlignment(labelElementPtr->GetAlignment());
		//textItemPtr->document()->setDefaultTextOption(option);

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

		if (strokeWidth >= 0) {
			pen.setWidthF(strokeWidth);
		}
		else{
			pen = Qt::NoPen;
		}

		abstractGraphicsShapeItemPtr->setPen(pen);
		abstractGraphicsShapeItemPtr->setBrush(graphicsElement.GetFillColor());
	}

	return itemPtr;
}


} // namespace imtreportgui


