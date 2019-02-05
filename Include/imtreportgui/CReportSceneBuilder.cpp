#include <imtreportgui/CReportSceneBuilder.h>


// Qt includes
#include <QtGui/QScreen>
#include <QtGui/QGuiApplication>
#include <QtWidgets/QGraphicsProxyWidget>
#include <QtWidgets/QTableWidget>

// ImtCore includes
#include <imtreportgui/CGraphicsElementShapeFactory.h>
#include <imtreport/IReportPage.h>


namespace imtreportgui
{


// static members
const qreal CReportSceneBuilder::s_A4WidthMm = 210.0;
const qreal CReportSceneBuilder::s_A4HeightMm = 297.0;


// public methods

CReportSceneBuilder::CReportSceneBuilder()
{
}


CReportSceneBuilder::ReportScenes CReportSceneBuilder::Build(const imtreport::IReportDocument& reportDocument)
{
	ReportScenes retVal;
	retVal.resize(reportDocument.GetPagesCount());

	for (int i = 0; i < reportDocument.GetPagesCount(); i++){
		const imtreport::IReportPage* pagePtr = reportDocument.GetReportPage(i);
		Q_ASSERT(pagePtr);

		QGraphicsScene* scenePtr = new QGraphicsScene();

		SetupScene(*scenePtr);
		BuildSceneShapes(*pagePtr, *scenePtr);

		retVal[i] = scenePtr;
	}

	return retVal;
}


// private methods

void CReportSceneBuilder::SetupScene(QGraphicsScene& scene)
{
	scene.setSceneRect(MapRectToScene(QRectF(0.0, 0.0, s_A4WidthMm, s_A4HeightMm)));
}


void CReportSceneBuilder::BuildSceneShapes(const imtreport::IReportPage& page, QGraphicsScene& scene)
{
	CGraphicsElementShapeFactory shapeFactory;

	imtreport::IReportPage::ElementIds ids = page.GetPageElements();
	for (const QByteArray& elementId : ids){
		const i2d::IObject2d* elementPtr = page.GetPageElement(elementId);
		Q_ASSERT(elementPtr != nullptr);

		QGraphicsItem* itemPtr = shapeFactory.CreateShape(*elementPtr);
		Q_ASSERT(itemPtr);

		ConvertShapeCoodinates(*elementPtr, *itemPtr);

		scene.addItem(itemPtr);
	}
}


void CReportSceneBuilder::ConvertShapeCoodinates(const i2d::IObject2d& pageElement, QGraphicsItem& sceneElement)
{
	const imtreport::CRectangleElement* rectPageElementPtr = dynamic_cast<const imtreport::CRectangleElement*>(&pageElement);
	if (rectPageElementPtr){
		return ConvertRectCoodinates(*rectPageElementPtr, sceneElement);
	}

	const imtreport::CCircleElement* circleSceneElementPtr = dynamic_cast<const imtreport::CCircleElement*>(&pageElement);
	if (circleSceneElementPtr){
		return ConvertEllipseCoodinates(*circleSceneElementPtr, sceneElement);
	}

	const imtreport::CTextLabelElement* labelSceneElementPtr = dynamic_cast<const imtreport::CTextLabelElement*>(&pageElement);
	if (labelSceneElementPtr){
		return ConvertLabelCoodinates(*labelSceneElementPtr, sceneElement);
	}

	const imtreport::CPolygonElement* polygonElementPtr = dynamic_cast<const imtreport::CPolygonElement*>(&pageElement);
	if (polygonElementPtr){
		return ConvertPolygoneCoodinates(*polygonElementPtr, sceneElement);
	}

	const imtreport::CLineElement* lineElementPtr = dynamic_cast<const imtreport::CLineElement*>(&pageElement);
	if (lineElementPtr){
		return ConvertLineCoodinates(*lineElementPtr, sceneElement);
	}

	const imtreport::CImageRectangleElement* imageElementPtr = dynamic_cast<const imtreport::CImageRectangleElement*>(&pageElement);
	if (imageElementPtr){
		return ConvertImageCoodinates(*imageElementPtr, sceneElement);
	}

	const imtreport::CTextTable* tableElementPtr = dynamic_cast<const imtreport::CTextTable*>(&pageElement);
	if (tableElementPtr){
		return ConvertTableCoodinates(*tableElementPtr, sceneElement);
	}
}


void CReportSceneBuilder::ConvertRectCoodinates(const imtreport::CRectangleElement& pageElement, QGraphicsItem& sceneElement)
{
	QGraphicsRectItem* rectSceneElementPtr = dynamic_cast<QGraphicsRectItem*>(&sceneElement);
	Q_ASSERT(rectSceneElementPtr);

	QRectF rect = MapRectToScene(pageElement);
	rectSceneElementPtr->setRect(rect);
}


void CReportSceneBuilder::ConvertEllipseCoodinates(const imtreport::CCircleElement& pageElement, QGraphicsItem& sceneElement)
{
	QGraphicsEllipseItem* ellipseSceneElementPtr = dynamic_cast<QGraphicsEllipseItem*>(&sceneElement);
	Q_ASSERT(ellipseSceneElementPtr);

	QRectF rect = MapRectToScene(pageElement.GetBoundingBox());
	ellipseSceneElementPtr->setRect(MapRectToScene(rect));
}


void CReportSceneBuilder::ConvertLabelCoodinates(const imtreport::CTextLabelElement& pageElement, QGraphicsItem& sceneElement)
{
	QGraphicsTextItem* labelSceneElementPtr = dynamic_cast<QGraphicsTextItem*>(&sceneElement);
	Q_ASSERT(labelSceneElementPtr);

	QRectF rect = MapRectToScene(pageElement.GetRectangle());
	labelSceneElementPtr->setPos(rect.topLeft());

	if (rect.width() > 0.0)
		labelSceneElementPtr->setTextWidth(rect.width());

	labelSceneElementPtr->setFont(MapFontToScene(pageElement.GetFont()));
}


void CReportSceneBuilder::ConvertLineCoodinates(const imtreport::CLineElement& pageElement, QGraphicsItem& sceneElement)
{
	QGraphicsLineItem* lineSceneElementPtr = dynamic_cast<QGraphicsLineItem*>(&sceneElement);
	Q_ASSERT(lineSceneElementPtr);

	QPointF point1 = MapPointToScene(pageElement.GetPoint1());
	QPointF point2 = MapPointToScene(pageElement.GetPoint2());
	lineSceneElementPtr->setLine(QLineF(point1, point2));
}

void CReportSceneBuilder::ConvertPolygoneCoodinates(const imtreport::CPolygonElement& pageElement, QGraphicsItem& sceneElement)
{
	QGraphicsPolygonItem* polygoneSceneElementPtr = dynamic_cast<QGraphicsPolygonItem*>(&sceneElement);
	Q_ASSERT(polygoneSceneElementPtr);

	QPolygonF polygon;
	polygon.reserve(pageElement.GetNodesCount());

	for (int i = 0; i < pageElement.GetNodesCount(); ++i){
		const i2d::CVector2d& node = pageElement.GetNodePos(i);
		polygon.append(MapPointToScene(node));
	}

	polygoneSceneElementPtr->setPolygon(polygon);
}


void CReportSceneBuilder::ConvertImageCoodinates(const imtreport::CImageRectangleElement& pageElement, QGraphicsItem& sceneElement)
{
	QGraphicsPixmapItem* imageSceneElementPtr = dynamic_cast<QGraphicsPixmapItem*>(&sceneElement);
	Q_ASSERT(imageSceneElementPtr);

	QRectF rect = MapRectToScene(pageElement);
	imageSceneElementPtr->setPos(rect.topLeft());

	QSize pixmapSize = imageSceneElementPtr->pixmap().size();

	if (pixmapSize.width() > 0){
		qreal scale = rect.width() / pixmapSize.width();
		imageSceneElementPtr->setScale(scale);
	}
	else{
		imageSceneElementPtr->setScale(1.0);
	}
}


void CReportSceneBuilder::ConvertTableCoodinates(const imtreport::CTextTable& pageElement, QGraphicsItem& sceneElement)
{
	QGraphicsProxyWidget* tableSceneElementPtr = dynamic_cast<QGraphicsProxyWidget*>(&sceneElement);
	Q_ASSERT(tableSceneElementPtr);

	QTableWidget* tableWidget = dynamic_cast<QTableWidget*>(tableSceneElementPtr->widget());
	Q_ASSERT(tableWidget);

	QRectF rect = MapRectToScene(pageElement);

	tableSceneElementPtr->setPreferredSize(rect.size());

	tableWidget->setGeometry(rect.toAlignedRect());
	tableWidget->setFixedSize(rect.size().toSize());

	// set horizontal header items
	if (pageElement.IsHorizontalHeaderVisible()){
		for (int col = 0; col < pageElement.GetColumnCount(); col++){
			const imtreport::CTextTableItem& tableItem = pageElement.GetHorizontalHeaderItem(col);
			tableWidget->setItem(0, col, ConvertTableItem(tableItem));
		}
	}

	// set vertical header items
	if (pageElement.IsVerticalHeaderVisible()){
		for (int row = 0; row < pageElement.GetRowCount(); row++){
			const imtreport::CTextTableItem& tableItem = pageElement.GetVerticalHeaderItem(row);
			tableWidget->setItem(row, 0, ConvertTableItem(tableItem));
		}
	}

	// set items
	for (int col = 0; col < pageElement.GetColumnCount(); col++){
		for (int row = 0; row < pageElement.GetRowCount(); row++){
			const imtreport::CTextTableItem& tableItem = pageElement.GetItem(row, col);

			int widgetRow = pageElement.IsHorizontalHeaderVisible() ? row + 1 : row;
			int widgetCol = pageElement.IsVerticalHeaderVisible() ? col + 1 : col;

			tableWidget->setItem(widgetRow, widgetCol, ConvertTableItem(tableItem));
		}
	}
}


QTableWidgetItem* CReportSceneBuilder::ConvertTableItem(const imtreport::CTextTableItem& tableItem)
{
	QTableWidgetItem* tableWidgetItem = new QTableWidgetItem();

	tableWidgetItem->setText(tableItem.GetText());
	tableWidgetItem->setTextAlignment(tableItem.GetAlignment());
	tableWidgetItem->setFont(MapFontToScene(tableItem.GetFont()));
	tableWidgetItem->setForeground(QBrush(ConvertToQColor(tableItem.GetForegroundColor())));
	tableWidgetItem->setBackground(QBrush(ConvertToQColor(tableItem.GetBackgroundColor())));
	tableWidgetItem->setIcon(QPixmap::fromImage(tableItem.GetImage().GetQImage()));

	return tableWidgetItem;
}


QPointF CReportSceneBuilder::MapPointToScene(const QPointF& point)
{
	// map shape coordinates given in mm to scene's coordinates
	QScreen* screenPtr = QGuiApplication::primaryScreen();
	Q_ASSERT(screenPtr);

	qreal x = point.x() * screenPtr->physicalDotsPerInchX() / 25.4;
	qreal y = point.y() * screenPtr->physicalDotsPerInchY() / 25.4;

	return QPointF(x, y);
}


QRectF CReportSceneBuilder::MapRectToScene(const QRectF& rect)
{
	return QRectF(MapPointToScene(rect.topLeft()), MapPointToScene(rect.bottomRight()));
}


QFont CReportSceneBuilder::MapFontToScene(const imtreport::CFont& font)
{
	double sceneFontSize = MapPointToScene(QPointF(font.GetSize(), 0.0)).x();
	return QFont(font.GetName(), sceneFontSize);
}


QColor CReportSceneBuilder::ConvertToQColor(const icmm::CRgb& color)
{
	return qRgb(color[0] * 255, color[1] * 255, color[2] * 255);
}


} // namespace imtreportgui


