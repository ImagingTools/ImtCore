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
		CreateSceneShapes(*pagePtr, *scenePtr);

		retVal[i] = scenePtr;
	}

	return retVal;
}


// private methods

void CReportSceneBuilder::SetupScene(QGraphicsScene& scene)
{
	scene.setSceneRect(MapRectToScene(QRectF(0.0, 0.0, s_A4WidthMm, s_A4HeightMm)));
}


void CReportSceneBuilder::CreateSceneShapes(const imtreport::IReportPage& page, QGraphicsScene& scene)
{
	imtreport::IReportPage::ElementIds ids = page.GetPageElements();
	for (const QByteArray& elementId : ids){
		const i2d::IObject2d* pageElementPtr = page.GetPageElement(elementId);
		Q_ASSERT(pageElementPtr != nullptr);

		CreateSceneShape(*pageElementPtr, scene);
	}
}


void CReportSceneBuilder::CreateSceneShape(const i2d::IObject2d& pageElement, QGraphicsScene& scene)
{
	const imtreport::CRectangleElement* rectPageElementPtr = dynamic_cast<const imtreport::CRectangleElement*>(&pageElement);
	if (rectPageElementPtr){
		return CreateRect(*rectPageElementPtr, scene);
	}

	const imtreport::CCircleElement* circleSceneElementPtr = dynamic_cast<const imtreport::CCircleElement*>(&pageElement);
	if (circleSceneElementPtr){
		return CreateEllipse(*circleSceneElementPtr, scene);
	}

	const imtreport::CTextLabelElement* labelSceneElementPtr = dynamic_cast<const imtreport::CTextLabelElement*>(&pageElement);
	if (labelSceneElementPtr){
		return CreateLabel(*labelSceneElementPtr, scene);
	}

	const imtreport::CPolygonElement* polygonElementPtr = dynamic_cast<const imtreport::CPolygonElement*>(&pageElement);
	if (polygonElementPtr){
		return CreatePolygone(*polygonElementPtr, scene);
	}

	const imtreport::CLineElement* lineElementPtr = dynamic_cast<const imtreport::CLineElement*>(&pageElement);
	if (lineElementPtr){
		return CreateLine(*lineElementPtr, scene);
	}

	const imtreport::CImageRectangleElement* imageElementPtr = dynamic_cast<const imtreport::CImageRectangleElement*>(&pageElement);
	if (imageElementPtr){
		return CreateImage(*imageElementPtr, scene);
	}

	const imtreport::CTextTable* tableElementPtr = dynamic_cast<const imtreport::CTextTable*>(&pageElement);
	if (tableElementPtr){
		return CreateTextTable(*tableElementPtr, scene);
	}
}


void CReportSceneBuilder::CreateRect(const imtreport::CRectangleElement& pageElement, QGraphicsScene& scene)
{
	QGraphicsRectItem* sceneElementPtr = new QGraphicsRectItem(MapRectToScene(pageElement));
	SetShapePenAndBrush(pageElement, *sceneElementPtr);
	scene.addItem(sceneElementPtr);
}


void CReportSceneBuilder::CreateEllipse(const imtreport::CCircleElement& pageElement, QGraphicsScene& scene)
{
	QGraphicsEllipseItem* sceneElementPtr = new QGraphicsEllipseItem(MapRectToScene(pageElement.GetBoundingBox()));
	SetShapePenAndBrush(pageElement, *sceneElementPtr);
	scene.addItem(sceneElementPtr);
}


void CReportSceneBuilder::CreateLabel(const imtreport::CTextLabelElement& pageElement, QGraphicsScene& scene)
{
	QGraphicsTextItem* sceneElementPtr = new QGraphicsTextItem();

	QRectF rect = MapRectToScene(pageElement.GetRectangle());
	sceneElementPtr->setPos(rect.topLeft());

	if (rect.width() > 0.0)
		sceneElementPtr->setTextWidth(rect.width());

	sceneElementPtr->setFont(MapFontToScene(pageElement.GetFont()));
	sceneElementPtr->setDefaultTextColor(pageElement.GetFillColor());

	QTextOption option = sceneElementPtr->document()->defaultTextOption();
	option.setAlignment(pageElement.GetAlignment());
	sceneElementPtr->document()->setDefaultTextOption(option);

	scene.addItem(sceneElementPtr);
}


void CReportSceneBuilder::CreateLine(const imtreport::CLineElement& pageElement, QGraphicsScene& scene)
{
	QGraphicsLineItem* sceneElementPtr = new QGraphicsLineItem();

	QPointF point1 = MapPointToScene(pageElement.GetPoint1());
	QPointF point2 = MapPointToScene(pageElement.GetPoint2());
	sceneElementPtr->setLine(QLineF(point1, point2));

	scene.addItem(sceneElementPtr);
}

void CReportSceneBuilder::CreatePolygone(const imtreport::CPolygonElement& pageElement, QGraphicsScene& scene)
{
	QGraphicsPolygonItem* sceneElementPtr = new QGraphicsPolygonItem();

	QPolygonF polygon;
	polygon.reserve(pageElement.GetNodesCount());

	for (int i = 0; i < pageElement.GetNodesCount(); ++i){
		const i2d::CVector2d& node = pageElement.GetNodePos(i);
		polygon.append(MapPointToScene(node));
	}

	sceneElementPtr->setPolygon(polygon);

	SetShapePenAndBrush(pageElement, *sceneElementPtr);

	scene.addItem(sceneElementPtr);
}


void CReportSceneBuilder::CreateImage(const imtreport::CImageRectangleElement& pageElement, QGraphicsScene& scene)
{
	QString imgPath = pageElement.GetImagePath();

	if (!QFileInfo(imgPath).exists()){
		qDebug(QString("CGraphicsElementShapeFactory: Image %1 can not be found!").arg(imgPath).toLocal8Bit().constData());
	}

	QGraphicsPixmapItem* sceneElementPtr = new QGraphicsPixmapItem(QPixmap(imgPath));

	QRectF rect = MapRectToScene(pageElement);
	sceneElementPtr->setPos(rect.topLeft());

	QSize pixmapSize = sceneElementPtr->pixmap().size();

	if (pixmapSize.height() > 0){
		qreal scale = rect.height() / pixmapSize.height();

		sceneElementPtr->setScale(scale);
	}
	else{
		sceneElementPtr->setScale(1.0);
	}

	scene.addItem(sceneElementPtr);
}


void CReportSceneBuilder::CreateTextTable(const imtreport::CTextTable& pageElement, QGraphicsScene& scene)
{
	CTextTableProxy tableProxy(pageElement);

	QRectF cellRect;
	cellRect.setTop(pageElement.GetTop());

	for (int row = 0; row < tableProxy.GetRowCount(); ++row){
		cellRect.setLeft(pageElement.GetLeft());

		for (int col = 0; col < tableProxy.GetColumnCount(); ++col){
			const imtreport::CTextTableItem* tableItem = tableProxy.GetItem(row, col);
			Q_ASSERT(tableItem);

			cellRect.setWidth(pageElement.GetColumnWidth(col));
			cellRect.setHeight(GetTextTableRowHeight(tableProxy, row));

			CreateTextTableItem(*tableItem, cellRect, scene);

			cellRect.setLeft(cellRect.left() + cellRect.width());
		}

		cellRect.setTop(cellRect.top() + cellRect.height());
	}
}


void CReportSceneBuilder::CreateTextTableItem(const imtreport::CTextTableItem& tableItem, const QRectF& cellRect, QGraphicsScene& scene)
{
	// add cell border
	QGraphicsRectItem* rectItemPtr = new QGraphicsRectItem(MapRectToScene(cellRect));
	rectItemPtr->setBrush(ConvertToQColor(tableItem.GetBackgroundColor()));

	scene.addItem(rectItemPtr);

	// add cell icon (if any)
	QPointF pos(cellRect.left() + 1.0, cellRect.top() + 1.0);

	if (!tableItem.GetImage().IsEmpty()){
		QPixmap pixmap = QPixmap::fromImage(tableItem.GetImage().GetQImage());

		QGraphicsPixmapItem* pixmapItemPtr = new QGraphicsPixmapItem(pixmap);
		pixmapItemPtr->setPos(MapPointToScene(pos));

		scene.addItem(pixmapItemPtr);

		pos.setX(pos.x() + pixmap.widthMM() + 2.0);
	}

	// add cell text
	QGraphicsTextItem* textItemPtr = new QGraphicsTextItem();
	textItemPtr->setPos(MapPointToScene(pos));
	textItemPtr->setPlainText(tableItem.GetText());
	textItemPtr->setFont(MapFontToScene(tableItem.GetFont()));
	textItemPtr->setDefaultTextColor(ConvertToQColor(tableItem.GetForegroundColor()));
	textItemPtr->setTextWidth(MapPointToScene(QPointF(cellRect.width(), cellRect.height())).x());

	QTextOption option = textItemPtr->document()->defaultTextOption();
	option.setAlignment(tableItem.GetAlignment());
	textItemPtr->document()->setDefaultTextOption(option);

	scene.addItem(textItemPtr);
}


double CReportSceneBuilder::GetTextTableRowHeight(const CTextTableProxy& table, int row)
{
	double maxCellHeight = 0.0;
	const double cellMargin = 4.0;

	for (int col = 0; col < table.GetColumnCount(); ++col){
		const imtreport::CTextTableItem* tableItem = table.GetItem(row, col);
		Q_ASSERT(tableItem);

		QFont font(tableItem->GetFont().GetName(), tableItem->GetFont().GetSize());
		double cellFontHeight = QFontMetrics(font).height() + cellMargin;
		double cellIconHeight = tableItem->GetImage().GetQImage().heightMM();

		maxCellHeight = qMax(maxCellHeight, qMax(cellFontHeight, cellIconHeight));
	}

	return maxCellHeight;
}


void CReportSceneBuilder::SetShapePenAndBrush(const imtreport::IGraphicsElement& pageElement, QAbstractGraphicsShapeItem& sceneElement)
{
	QPen pen(pageElement.GetStrokeColor());

	double strokeWidth = pageElement.GetStrokeWidth();
	if (strokeWidth >= 0){
		pen.setWidthF(strokeWidth);
	}
	else{
		pen = Qt::NoPen;
	}

	sceneElement.setPen(pen);
	sceneElement.setBrush(pageElement.GetFillColor());
}


QPointF CReportSceneBuilder::MapPointToScene(const QPointF& point)
{
	// map shape coordinates given in mm to scene's coordinates
	QScreen* screenPtr = QGuiApplication::primaryScreen();
	Q_ASSERT(screenPtr);

	double logicalX = screenPtr->logicalDotsPerInchX();
	double logicalY = screenPtr->logicalDotsPerInchY();

	qreal x = point.x() * logicalX / 25.4;
	qreal y = point.y() * logicalY / 25.4;

	return QPointF(x, y);
}


QRectF CReportSceneBuilder::MapRectToScene(const QRectF& rect)
{
	return QRectF(MapPointToScene(rect.topLeft()), MapPointToScene(rect.bottomRight()));
}


QFont CReportSceneBuilder::MapFontToScene(const imtreport::CFont& font)
{
	QFont sceneFont(font.GetName());
	sceneFont.setPointSize(MapPointToScene(QPointF(font.GetSize(), 0.0)).x());

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


