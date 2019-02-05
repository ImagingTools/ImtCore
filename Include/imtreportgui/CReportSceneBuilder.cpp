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

		scene.addItem(CreateSceneShape(*pageElementPtr));
	}
}


QGraphicsItem* CReportSceneBuilder::CreateSceneShape(const i2d::IObject2d& pageElement)
{
	const imtreport::CRectangleElement* rectPageElementPtr = dynamic_cast<const imtreport::CRectangleElement*>(&pageElement);
	if (rectPageElementPtr){
		return CreateRect(*rectPageElementPtr);
	}

	const imtreport::CCircleElement* circleSceneElementPtr = dynamic_cast<const imtreport::CCircleElement*>(&pageElement);
	if (circleSceneElementPtr){
		return CreateEllipse(*circleSceneElementPtr);
	}

	const imtreport::CTextLabelElement* labelSceneElementPtr = dynamic_cast<const imtreport::CTextLabelElement*>(&pageElement);
	if (labelSceneElementPtr){
		return CreateLabel(*labelSceneElementPtr);
	}

	const imtreport::CPolygonElement* polygonElementPtr = dynamic_cast<const imtreport::CPolygonElement*>(&pageElement);
	if (polygonElementPtr){
		return CreatePolygone(*polygonElementPtr);
	}

	const imtreport::CLineElement* lineElementPtr = dynamic_cast<const imtreport::CLineElement*>(&pageElement);
	if (lineElementPtr){
		return CreateLine(*lineElementPtr);
	}

	const imtreport::CImageRectangleElement* imageElementPtr = dynamic_cast<const imtreport::CImageRectangleElement*>(&pageElement);
	if (imageElementPtr){
		return CreateImage(*imageElementPtr);
	}

	const imtreport::CTextTable* tableElementPtr = dynamic_cast<const imtreport::CTextTable*>(&pageElement);
	if (tableElementPtr){
		return CreateTextTable(*tableElementPtr);
	}

	return NULL;
}


QGraphicsItem* CReportSceneBuilder::CreateRect(const imtreport::CRectangleElement& pageElement)
{
	QGraphicsRectItem* sceneElementPtr = new QGraphicsRectItem(MapRectToScene(pageElement));
	SetShapePenAndBrush(pageElement, *sceneElementPtr);
	return sceneElementPtr;
}


QGraphicsItem* CReportSceneBuilder::CreateEllipse(const imtreport::CCircleElement& pageElement)
{
	QGraphicsEllipseItem* sceneElementPtr = new QGraphicsEllipseItem(MapRectToScene(pageElement.GetBoundingBox()));
	SetShapePenAndBrush(pageElement, *sceneElementPtr);
	return sceneElementPtr;
}


QGraphicsItem* CReportSceneBuilder::CreateLabel(const imtreport::CTextLabelElement& pageElement)
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

	return sceneElementPtr;
}


QGraphicsItem* CReportSceneBuilder::CreateLine(const imtreport::CLineElement& pageElement)
{
	QGraphicsLineItem* sceneElementPtr = new QGraphicsLineItem();

	QPointF point1 = MapPointToScene(pageElement.GetPoint1());
	QPointF point2 = MapPointToScene(pageElement.GetPoint2());
	sceneElementPtr->setLine(QLineF(point1, point2));

	return sceneElementPtr;
}

QGraphicsItem* CReportSceneBuilder::CreatePolygone(const imtreport::CPolygonElement& pageElement)
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

	return sceneElementPtr;
}


QGraphicsItem* CReportSceneBuilder::CreateImage(const imtreport::CImageRectangleElement& pageElement)
{
	QString imgPath = pageElement.GetImagePath();

	if (!QFileInfo(imgPath).exists()){
		qDebug(QString("CGraphicsElementShapeFactory: Image %1 can not be found!").arg(imgPath).toLocal8Bit().constData());
	}

	QGraphicsPixmapItem* sceneElementPtr = new QGraphicsPixmapItem(QPixmap(imgPath));

	QRectF rect = MapRectToScene(pageElement);
	sceneElementPtr->setPos(rect.topLeft());

	QSize pixmapSize = sceneElementPtr->pixmap().size();

	if (pixmapSize.width() > 0){
		qreal scale = rect.width() / pixmapSize.width();
		sceneElementPtr->setScale(scale);
	}
	else{
		sceneElementPtr->setScale(1.0);
	}

	return sceneElementPtr;
}


QGraphicsItem* CReportSceneBuilder::CreateTextTable(const imtreport::CTextTable& pageElement)
{
	int rowCount = pageElement.GetRowCount();
	if (pageElement.IsHorizontalHeaderVisible())
		rowCount++;

	int columnCount = pageElement.GetColumnCount();
	if (pageElement.IsVerticalHeaderVisible())
		columnCount++;

	QRectF rect = MapRectToScene(pageElement);

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
	tableWidget->setGeometry(rect.toAlignedRect());
	tableWidget->setFixedSize(rect.size().toSize());

	// set horizontal header items
	if (pageElement.IsHorizontalHeaderVisible()){
		for (int col = 0; col < pageElement.GetColumnCount(); col++){
			const imtreport::CTextTableItem& tableItem = pageElement.GetHorizontalHeaderItem(col);
			tableWidget->setItem(0, col, CreateTextTableItem(tableItem));
		}
	}

	// set vertical header items
	if (pageElement.IsVerticalHeaderVisible()){
		for (int row = 0; row < pageElement.GetRowCount(); row++){
			const imtreport::CTextTableItem& tableItem = pageElement.GetVerticalHeaderItem(row);
			tableWidget->setItem(row, 0, CreateTextTableItem(tableItem));
		}
	}

	// set items
	for (int col = 0; col < pageElement.GetColumnCount(); col++){
		for (int row = 0; row < pageElement.GetRowCount(); row++){
			const imtreport::CTextTableItem& tableItem = pageElement.GetItem(row, col);

			int widgetRow = pageElement.IsHorizontalHeaderVisible() ? row + 1 : row;
			int widgetCol = pageElement.IsVerticalHeaderVisible() ? col + 1 : col;

			tableWidget->setItem(widgetRow, widgetCol, CreateTextTableItem(tableItem));
		}
	}

	QGraphicsProxyWidget* sceneElementPtr = new QGraphicsProxyWidget();
	sceneElementPtr->setWidget(tableWidget);
	sceneElementPtr->setPreferredSize(rect.size());

	return sceneElementPtr;
}


QTableWidgetItem* CReportSceneBuilder::CreateTextTableItem(const imtreport::CTextTableItem& tableItem)
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


