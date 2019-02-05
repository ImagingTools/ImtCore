#include <imtreportgui/CGraphicsElementShapeFactory.h>


// Qt includes
#include <QtGui/QPen>
#include <QtGui/QTextDocument>
#include <QtGui/QTextOption>
#if QT_VERSION >= 0x050000
#include <QtWidgets/QGraphicsRectItem>
#else
#include <QtGui/QGraphicsRectItem>
#endif
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QGraphicsProxyWidget>

// ImtCore includes
#include <imtreport/TGraphicsElement.h>


namespace imtreportgui
{


// public methods

QGraphicsItem* CGraphicsElementShapeFactory::CreateShape(const i2d::IObject2d& object2d) const
{
	QGraphicsItem* itemPtr = NULL;

	const imtreport::CRectangleElement* rectangleElementPtr = dynamic_cast<const imtreport::CRectangleElement*>(&object2d);
	if (rectangleElementPtr != NULL){
		itemPtr = new QGraphicsRectItem();
	}

	const imtreport::CLineElement* lineElementPtr = dynamic_cast<const imtreport::CLineElement*>(&object2d);
	if (lineElementPtr != NULL) {
		itemPtr = new QGraphicsLineItem();
	}

	const imtreport::CCircleElement* circleElementPtr = dynamic_cast<const imtreport::CCircleElement*>(&object2d);
	if (circleElementPtr != NULL){
		itemPtr = new QGraphicsEllipseItem();
	}

	const imtreport::CTextLabelElement* labelElementPtr = dynamic_cast<const imtreport::CTextLabelElement*>(&object2d);
	if (labelElementPtr != NULL){
		QGraphicsTextItem* textItemPtr = new QGraphicsTextItem(labelElementPtr->GetText());
		textItemPtr->setDefaultTextColor(labelElementPtr->GetFillColor());

		QTextOption option = textItemPtr->document()->defaultTextOption();
		option.setAlignment(labelElementPtr->GetAlignment());
		textItemPtr->document()->setDefaultTextOption(option);

		itemPtr = textItemPtr;
	}

	const imtreport::CPolygonElement* polygonElementPtr = dynamic_cast<const imtreport::CPolygonElement*>(&object2d);
	if (polygonElementPtr != NULL){
		itemPtr = new QGraphicsPolygonItem();
	}

	const imtreport::CImageRectangleElement* imageRectangleElementPtr = dynamic_cast<const imtreport::CImageRectangleElement*>(&object2d);
	if (imageRectangleElementPtr != NULL){
		QString imgPath = imageRectangleElementPtr->GetImagePath();

		if (!QFileInfo(imgPath).exists()){
			qDebug(QString("CGraphicsElementShapeFactory: Image %1 can not be found!").arg(imgPath).toLocal8Bit().constData());
		}

		itemPtr = new QGraphicsPixmapItem(QPixmap(imgPath));
	}
	
	const imtreport::CTextTable* tablePtr = dynamic_cast<const imtreport::CTextTable*>(&object2d);
	if (tablePtr != NULL){
		itemPtr = CreateTextTable(*tablePtr);
	}

	QAbstractGraphicsShapeItem* abstractGraphicsShapeItemPtr = dynamic_cast<QAbstractGraphicsShapeItem*>(itemPtr);
	const imtreport::IGraphicsElement* graphicsElementPtr = dynamic_cast<const imtreport::IGraphicsElement*>(&object2d);
	if (abstractGraphicsShapeItemPtr != NULL && graphicsElementPtr != NULL){
		QPen pen(graphicsElementPtr->GetStrokeColor());

		double strokeWidth = graphicsElementPtr->GetStrokeWidth();
		if (strokeWidth >= 0){
			pen.setWidthF(strokeWidth);
		}
		else{
			pen = Qt::NoPen;
		}

		abstractGraphicsShapeItemPtr->setPen(pen);
		abstractGraphicsShapeItemPtr->setBrush(graphicsElementPtr->GetFillColor());
	}

	return itemPtr;
}


QGraphicsItem* CGraphicsElementShapeFactory::CreateTextTable(const imtreport::CTextTable& table) const
{
	int rowCount = table.GetRowCount();
	if (table.IsHorizontalHeaderVisible())
		rowCount++;

	int columnCount = table.GetColumnCount();
	if (table.IsVerticalHeaderVisible())
		columnCount++;

	QTableWidget* tableWidget = new QTableWidget(rowCount, columnCount);
	tableWidget->setFrameShape(QFrame::NoFrame);
	tableWidget->setFocusPolicy(Qt::NoFocus);
	tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	tableWidget->setSelectionMode(QAbstractItemView::NoSelection);

	tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	tableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	tableWidget->horizontalHeader()->hide();

	tableWidget->verticalHeader()->hide();

	QGraphicsProxyWidget* sceneTableWidget = new QGraphicsProxyWidget();
	sceneTableWidget->setWidget(tableWidget);

	return sceneTableWidget;
}


} // namespace imtreportgui


