#include <imtreportgui/CReportSceneBuilder.h>


// Qt includes
#include <QtGui/QScreen>
#include <QtGui/QGuiApplication>
#include <QtGui/QTextDocument>
#include <QtGui/QTextOption>
#include <QtWidgets/QGraphicsProxyWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QGraphicsProxyWidget>

// ImtCore includes
#include <imtreport/IReportPage.h>


namespace imtreportgui
{


// static members
const qreal CReportSceneBuilder::s_A4WidthMm = 210.0;
const qreal CReportSceneBuilder::s_A4HeightMm = 297.0;
const qreal CReportSceneBuilder::s_margin = 5.0;


// public methods

CReportSceneBuilder::CReportSceneBuilder()
{
}


CReportSceneBuilder::ReportScenes CReportSceneBuilder::Build(const imtreport::IReportDocument& reportDocument, int resolutionDpi)
{
	ReportScenes retVal;
	retVal.resize(reportDocument.GetPagesCount());

	for (int i = 0; i < reportDocument.GetPagesCount(); i++){
		const imtreport::IReportPage* pagePtr = reportDocument.GetReportPage(i);
		Q_ASSERT(pagePtr);

		QGraphicsScene* scenePtr = new QGraphicsScene();

		SetupScene(*scenePtr, resolutionDpi);
		CreateSceneShapes(*pagePtr, *scenePtr, resolutionDpi);

		retVal[i] = scenePtr;
	}

	return retVal;
}


// private methods

void CReportSceneBuilder::SetupScene(QGraphicsScene& scene, int resolutionDpi)
{
	scene.setSceneRect(MapRectToScene(QRectF(0.0, 0.0, s_A4WidthMm, s_A4HeightMm), resolutionDpi));
}


void CReportSceneBuilder::CreateSceneShapes(const imtreport::IReportPage& page, QGraphicsScene& scene, int resolutionDpi)
{
	imtreport::IReportPage::ElementIds ids = page.GetPageElements();
	for (const QByteArray& elementId : ids){
		const i2d::IObject2d* pageElementPtr = page.GetPageElement(elementId);
		Q_ASSERT(pageElementPtr != nullptr);

		CreateSceneShape(*pageElementPtr, scene, resolutionDpi);
	}
}


void CReportSceneBuilder::CreateSceneShape(const i2d::IObject2d& pageElement, QGraphicsScene& scene, int resolutionDpi)
{
	const imtreport::CRectangleElement* rectPageElementPtr = dynamic_cast<const imtreport::CRectangleElement*>(&pageElement);
	if (rectPageElementPtr){
		return CreateRect(*rectPageElementPtr, scene, resolutionDpi);
	}

	const imtreport::CCircleElement* circleSceneElementPtr = dynamic_cast<const imtreport::CCircleElement*>(&pageElement);
	if (circleSceneElementPtr){
		return CreateEllipse(*circleSceneElementPtr, scene, resolutionDpi);
	}

	const imtreport::CTextLabelElement* labelSceneElementPtr = dynamic_cast<const imtreport::CTextLabelElement*>(&pageElement);
	if (labelSceneElementPtr){
		return CreateLabel(*labelSceneElementPtr, scene, resolutionDpi);
	}

	const imtreport::CPolygonElement* polygonElementPtr = dynamic_cast<const imtreport::CPolygonElement*>(&pageElement);
	if (polygonElementPtr){
		return CreatePolygone(*polygonElementPtr, scene, resolutionDpi);
	}

	const imtreport::CLineElement* lineElementPtr = dynamic_cast<const imtreport::CLineElement*>(&pageElement);
	if (lineElementPtr){
		return CreateLine(*lineElementPtr, scene, resolutionDpi);
	}

	const imtreport::CImageRectangleElement* imageElementPtr = dynamic_cast<const imtreport::CImageRectangleElement*>(&pageElement);
	if (imageElementPtr){
		return CreateImage(*imageElementPtr, scene, resolutionDpi);
	}

	const imtreport::CTextTable* tableElementPtr = dynamic_cast<const imtreport::CTextTable*>(&pageElement);
	if (tableElementPtr){
		return CreateTextTable(*tableElementPtr, scene, resolutionDpi);
	}
}


void CReportSceneBuilder::CreateRect(const imtreport::CRectangleElement& pageElement, QGraphicsScene& scene, int resolutionDpi)
{
	QGraphicsRectItem* sceneElementPtr = new QGraphicsRectItem(MapRectToScene(pageElement, resolutionDpi));
	SetShapePenAndBrush(pageElement, *sceneElementPtr);
	scene.addItem(sceneElementPtr);
}


void CReportSceneBuilder::CreateEllipse(const imtreport::CCircleElement& pageElement, QGraphicsScene& scene, int resolutionDpi)
{
	QGraphicsEllipseItem* sceneElementPtr = new QGraphicsEllipseItem(MapRectToScene(pageElement.GetBoundingBox(), resolutionDpi));
	SetShapePenAndBrush(pageElement, *sceneElementPtr);
	scene.addItem(sceneElementPtr);
}


void CReportSceneBuilder::CreateLabel(const imtreport::CTextLabelElement& pageElement, QGraphicsScene& scene, int resolutionDpi)
{
	QGraphicsTextItem* sceneElementPtr = new QGraphicsTextItem();

	QRectF rect = MapRectToScene(pageElement.GetRectangle(), resolutionDpi);
	sceneElementPtr->setPos(rect.topLeft());

	if (rect.width() > 0.0)
		sceneElementPtr->setTextWidth(rect.width());

	sceneElementPtr->setFont(ConvertToQFont(pageElement.GetFont()));
	sceneElementPtr->setDefaultTextColor(pageElement.GetGraphicsAttributes().fillColor);

	QTextOption option = sceneElementPtr->document()->defaultTextOption();
	option.setAlignment(pageElement.GetAlignment());
	sceneElementPtr->document()->setDefaultTextOption(option);

	scene.addItem(sceneElementPtr);
}


void CReportSceneBuilder::CreateLine(const imtreport::CLineElement& pageElement, QGraphicsScene& scene, int resolutionDpi)
{
	QGraphicsLineItem* sceneElementPtr = new QGraphicsLineItem();

	QPointF point1 = MapPointToScene(pageElement.GetPoint1(), resolutionDpi);
	QPointF point2 = MapPointToScene(pageElement.GetPoint2(), resolutionDpi);
	sceneElementPtr->setLine(QLineF(point1, point2));

	scene.addItem(sceneElementPtr);
}

void CReportSceneBuilder::CreatePolygone(const imtreport::CPolygonElement& pageElement, QGraphicsScene& scene, int resolutionDpi)
{
	QGraphicsPolygonItem* sceneElementPtr = new QGraphicsPolygonItem();

	QPolygonF polygon;
	polygon.reserve(pageElement.GetNodesCount());

	for (int i = 0; i < pageElement.GetNodesCount(); ++i){
		const i2d::CVector2d& node = pageElement.GetNodePos(i);
		polygon.append(MapPointToScene(node, resolutionDpi));
	}

	sceneElementPtr->setPolygon(polygon);

	SetShapePenAndBrush(pageElement, *sceneElementPtr);

	scene.addItem(sceneElementPtr);
}


void CReportSceneBuilder::CreateImage(const imtreport::CImageRectangleElement& pageElement, QGraphicsScene& scene, int resolutionDpi)
{
	QString imgPath = pageElement.GetImagePath();

	if (!QFileInfo(imgPath).exists()){
		qDebug(QString("CGraphicsElementShapeFactory: Image %1 can not be found!").arg(imgPath).toLocal8Bit().constData());
	}

	QGraphicsPixmapItem* sceneElementPtr = new QGraphicsPixmapItem(QPixmap(imgPath));

	QRectF rect = MapRectToScene(pageElement, resolutionDpi);

	// set scale
	qreal scale = 1.0;

	QSize pixmapSize = sceneElementPtr->pixmap().size();
	if (pixmapSize.height() > 0){
		scale = rect.height() / pixmapSize.height();
	}
	sceneElementPtr->setScale(scale);

	// set position
	QPointF pos;
	pos.setX(scene.sceneRect().center().x() - sceneElementPtr->pixmap().width() * scale / 2);
	pos.setY(rect.top());
	sceneElementPtr->setPos(pos);

	scene.addItem(sceneElementPtr);
}


void CReportSceneBuilder::CreateTextTable(const imtreport::CTextTable& pageElement, QGraphicsScene& scene, int resolutionDpi)
{
	CTextTableProxy tableProxy(pageElement);

	QRectF cellRect;
	cellRect.setTop(MapCoordinateToScene(pageElement.GetTop(), false, resolutionDpi));

	for (int row = 0; row < tableProxy.GetRowCount(); ++row){
		cellRect.setLeft(MapCoordinateToScene(pageElement.GetLeft(), true, resolutionDpi));
		cellRect.setHeight(GetTextTableRowHeight(pageElement, tableProxy, row, resolutionDpi));

		for (int col = 0; col < tableProxy.GetColumnCount(); ++col){
			const imtreport::CTextTableItem* tableItem = tableProxy.GetItem(row, col);
			Q_ASSERT(tableItem);

			cellRect.setWidth(MapCoordinateToScene(pageElement.GetColumnWidth(col), true, resolutionDpi));

			CreateTextTableItem(*tableItem, cellRect, scene);

			cellRect.setLeft(cellRect.left() + cellRect.width());
		}

		cellRect.setTop(cellRect.top() + cellRect.height());
	}
}


void CReportSceneBuilder::CreateTextTableItem(const imtreport::CTextTableItem& tableItem, const QRectF& cellRect, QGraphicsScene& scene)
{
	// add cell border
	QGraphicsRectItem* rectItemPtr = new QGraphicsRectItem(cellRect);
	rectItemPtr->setBrush(ConvertToQColor(tableItem.GetBackgroundColor()));

	scene.addItem(rectItemPtr);

	// add cell icon (if any)
	QPointF pos(cellRect.left() + s_margin, cellRect.top() + s_margin);

	if (!tableItem.GetImage().IsEmpty()){
		QPixmap pixmap = QPixmap::fromImage(tableItem.GetImage().GetQImage());

		QGraphicsPixmapItem* pixmapItemPtr = new QGraphicsPixmapItem(pixmap);
		pixmapItemPtr->setPos(pos);

		scene.addItem(pixmapItemPtr);

		pos.setX(pos.x() + pixmap.width() + s_margin * 2);
	}

	// add cell text
	QGraphicsTextItem* textItemPtr = new QGraphicsTextItem();
	textItemPtr->setPos(pos);
	textItemPtr->setPlainText(tableItem.GetText());
	textItemPtr->setFont(ConvertToQFont(tableItem.GetFont()));
	textItemPtr->setDefaultTextColor(ConvertToQColor(tableItem.GetForegroundColor()));
	textItemPtr->setTextWidth(cellRect.width() - s_margin * 2);

	QTextOption option = textItemPtr->document()->defaultTextOption();
	option.setAlignment(tableItem.GetAlignment());
	textItemPtr->document()->setDefaultTextOption(option);
	textItemPtr->document()->setDocumentMargin(0.0);

	scene.addItem(textItemPtr);
}


qreal CReportSceneBuilder::GetTextTableRowHeight(const imtreport::CTextTable& table, const CTextTableProxy& tableProxy, int row, int resolutionDpi)
{
	qreal rowHeight = 0.0;
	const qreal maxRowHeight = 100.0;

	for (int col = 0; col < tableProxy.GetColumnCount(); ++col){
		const imtreport::CTextTableItem* tableItem = tableProxy.GetItem(row, col);
		Q_ASSERT(tableItem);

		QFontMetrics fontMetrics(ConvertToQFont(tableItem->GetFont()));

		qreal colWidth = MapCoordinateToScene(table.GetColumnWidth(col), true, resolutionDpi);
		QRectF constrainingRect(0, 0, colWidth - s_margin * 2, maxRowHeight);

		QRect textRect = fontMetrics.boundingRect(constrainingRect.toRect(),
			tableItem->GetAlignment() | Qt::TextWordWrap,
			tableItem->GetText());

		double rowTextHeight = textRect.height();
		double rowIconHeight = tableItem->GetImage().GetQImage().height();

		rowHeight = qMax(rowHeight, qMax(rowTextHeight, rowIconHeight));
	}

	return rowHeight + s_margin * 2;
}


void CReportSceneBuilder::SetShapePenAndBrush(const imtreport::IDrawElement& pageElement, QAbstractGraphicsShapeItem& sceneElement)
{
	QPen pen(pageElement.GetGraphicsAttributes().strokeColor);

	double strokeWidth = pageElement.GetGraphicsAttributes().strokeWidth;
	if (strokeWidth >= 0){
		pen.setWidthF(strokeWidth);
	}
	else{
		pen = Qt::NoPen;
	}

	sceneElement.setPen(pen);
	sceneElement.setBrush(pageElement.GetGraphicsAttributes().fillColor);
}


qreal CReportSceneBuilder::MapCoordinateToScene(qreal value, bool isX, int resolutionDpi)
{
	// map shape coordinates given in mm to scene's coordinates
	if (resolutionDpi <= 0){
		QScreen* screenPtr = QGuiApplication::primaryScreen();
		Q_ASSERT(screenPtr);
		resolutionDpi = isX ? screenPtr->logicalDotsPerInchX() : screenPtr->logicalDotsPerInchY();
	}

	return value * resolutionDpi / 25.4;
}


QPointF CReportSceneBuilder::MapPointToScene(const QPointF& point, int resolutionDpi)
{
	QPointF retVal;
	retVal.setX(MapCoordinateToScene(point.x(), true, resolutionDpi));
	retVal.setY(MapCoordinateToScene(point.y(), false, resolutionDpi));

	return retVal;
}


QRectF CReportSceneBuilder::MapRectToScene(const QRectF& rect, int resolutionDpi)
{
	return QRectF(MapPointToScene(rect.topLeft(), resolutionDpi), MapPointToScene(rect.bottomRight(), resolutionDpi));
}


QFont CReportSceneBuilder::ConvertToQFont(const imtreport::CFont& font)
{
	QFont sceneFont(font.GetName(), font.GetSize());

	if (font.GetFontFlags() & imtreport::CFont::FF_BOLD){
		sceneFont.setBold(true);
	}

	if (font.GetFontFlags() & imtreport::CFont::FF_ITALIC){
		sceneFont.setItalic(true);
	}

	if (font.GetFontFlags() & imtreport::CFont::FF_UNDERLINE){
		sceneFont.setUnderline(true);
	}

	return sceneFont;
}


QColor CReportSceneBuilder::ConvertToQColor(const icmm::CRgb& color)
{
	return qRgb(color[0] * 255, color[1] * 255, color[2] * 255);
}


} // namespace imtreportgui


