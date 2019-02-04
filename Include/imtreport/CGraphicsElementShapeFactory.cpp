#include <imtreport/CGraphicsElementShapeFactory.h>


// ACF includes
#include <iqt/iqt.h>

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


namespace imtreport
{


// public methods

QGraphicsItem* CGraphicsElementShapeFactory::CreateShape(const i2d::IObject2d& object2d) const
{
	QGraphicsItem* itemPtr = NULL;

	const CRectangleElement* rectangleElementPtr = dynamic_cast<const CRectangleElement*>(&object2d);
	if (rectangleElementPtr != NULL){
		itemPtr = new QGraphicsRectItem();
	}

	const CLineElement* lineElementPtr = dynamic_cast<const CLineElement*>(&object2d);
	if (lineElementPtr != NULL) {
		itemPtr = new QGraphicsLineItem();
	}

	const CCircleElement* circleElementPtr = dynamic_cast<const CCircleElement*>(&object2d);
	if (circleElementPtr != NULL){
		itemPtr = new QGraphicsEllipseItem();
	}

	const CTextLabelElement* labelElementPtr = dynamic_cast<const CTextLabelElement*>(&object2d);
	if (labelElementPtr != NULL){
		QGraphicsTextItem* textItemPtr = new QGraphicsTextItem(labelElementPtr->GetText());
		textItemPtr->setDefaultTextColor(labelElementPtr->GetFillColor());

		QTextOption option = textItemPtr->document()->defaultTextOption();
		option.setAlignment(labelElementPtr->GetAlignment());
		textItemPtr->document()->setDefaultTextOption(option);

		itemPtr = textItemPtr;
	}

	const CPolygonElement* polygonElementPtr = dynamic_cast<const CPolygonElement*>(&object2d);
	if (polygonElementPtr != NULL){
		itemPtr = new QGraphicsPolygonItem();
	}

	const CImageRectangleElement* imageRectangleElementPtr = dynamic_cast<const CImageRectangleElement*>(&object2d);
	if (imageRectangleElementPtr != NULL){
		QString imgPath = imageRectangleElementPtr->GetImagePath();

		if (!QFileInfo(imgPath).exists()){
			qDebug(QString("CGraphicsElementShapeFactory: Image %1 can not be found!").arg(imgPath).toLocal8Bit().constData());
		}

		itemPtr = new QGraphicsPixmapItem(QPixmap(imgPath));
	}
	
	const CTextTable* tablePtr = dynamic_cast<const CTextTable*>(&object2d);
	if (tablePtr != NULL){
		itemPtr = CreateTextTable(*tablePtr);
	}

	QAbstractGraphicsShapeItem* abstractGraphicsShapeItemPtr = dynamic_cast<QAbstractGraphicsShapeItem*>(itemPtr);
	const IGraphicsElement* graphicsElementPtr = dynamic_cast<const IGraphicsElement*>(&object2d);
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


QGraphicsItem* CGraphicsElementShapeFactory::CreateTextTable(const CTextTable& table) const
{
	QTableWidget* tableWidget = new QTableWidget(table.GetRowCount(), table.GetColumnCount());
	tableWidget->setFrameShape(QFrame::Box);
	tableWidget->setFocusPolicy(Qt::NoFocus);
	tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	tableWidget->setSelectionMode(QAbstractItemView::NoSelection);

	tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	tableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	if (table.IsHorizontalHeaderVisible()){
		tableWidget->horizontalHeader()->show();
		tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
		tableWidget->horizontalHeader()->setStretchLastSection(true);
	}
	else{
		tableWidget->horizontalHeader()->hide();
	}

	if (table.IsVerticalHeaderVisible()){
		tableWidget->verticalHeader()->show();
	}
	else{
		tableWidget->verticalHeader()->hide();
	}

	QGraphicsProxyWidget* sceneTableWidget = new QGraphicsProxyWidget();
	sceneTableWidget->setWidget(tableWidget);

	return sceneTableWidget;
}


} // namespace imtreportgui


