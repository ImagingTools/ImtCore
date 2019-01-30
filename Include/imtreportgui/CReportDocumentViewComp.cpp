#include <imtreportgui/CReportDocumentViewComp.h>


// Qt includes
#include <QtPrintSupport/QPrinter>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGraphicsProxyWidget>
#include <QtWidgets/QTableWidget>
#include <QtGui/QScreen>

// ImtCore includes
#include <imtreport/IReportPage.h>
#include <imtreportgui/CGraphicsElementShapeFactory.h>


namespace imtreportgui
{


// static members
const qreal CReportDocumentViewComp::s_A4WidthMm = 210.0;
const qreal CReportDocumentViewComp::s_A4HeightMm = 297.0;
const qreal CReportDocumentViewComp::s_MmPerInch = 25.4;


// public methods

CReportDocumentViewComp::CReportDocumentViewComp()
	:m_fileCommands("&File", 100),
	m_viewCommands("&View", 100),
	m_exportToPdfCommand("Export to PDF...", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, 1984),
	m_showGridCommand("Show grid", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR | ibase::ICommand::CF_ONOFF, 1985)
{
	m_showGridCommand.setChecked(true);

	connect(&m_showGridCommand, SIGNAL(triggered()), this, SLOT(OnShowGrid()));
	connect(&m_exportToPdfCommand, SIGNAL(triggered()), this, SLOT(OnExportToPdf()));

	m_fileCommands.InsertChild(&m_exportToPdfCommand);
	m_viewCommands.InsertChild(&m_showGridCommand);

	m_rootCommands.InsertChild(&m_fileCommands);
	m_rootCommands.InsertChild(&m_viewCommands);
}


// reimplemented (ibase::ICommandsProvider)

const ibase::IHierarchicalCommand* CReportDocumentViewComp::GetCommands() const
{
	return &m_rootCommands;
}


// protected methods

// reimplemented (iqtgui::TGuiObserverWrap)

void CReportDocumentViewComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	m_sceneDecorations.clear();

	imtreport::IReportDocument* documentPtr = GetObjectPtr();
	Q_ASSERT(documentPtr != NULL);

	for (int i = 0; i < documentPtr->GetPagesCount(); i++){
		const imtreport::IReportPage* pagePtr = documentPtr->GetReportPage(i);
		Q_ASSERT(pagePtr);

		QGraphicsScene* scenePtr = new QGraphicsScene();
		UpdateSceneDecoration(*scenePtr);
		UpdateSceneShapes(*pagePtr, *scenePtr);

		QGraphicsView* viewPtr = new QGraphicsView(scenePtr, PagesTabs);
		PagesTabs->addTab(viewPtr, "Page " + QString::number(i + 1));
	}
}


// reimplemented (iqtgui::CGuiComponentBase)

void CReportDocumentViewComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();
}


void CReportDocumentViewComp::OnGuiDestroyed()
{
	BaseClass::OnGuiDestroyed();
}


void CReportDocumentViewComp::OnGuiRetranslate()
{
	BaseClass::OnGuiRetranslate();

	m_exportToPdfCommand.SetVisuals(tr("Export to PDF..."), tr("PDF..."), tr("Export current report to PDF file"), QIcon(":/Icons/Pdf"));
	m_showGridCommand.SetVisuals(tr("Show/Hide Grid"), tr("Grid"), tr("Set grid visible/unvisible"), QIcon(":/Icons/Grid2"));
}


void CReportDocumentViewComp::OnShowGrid()
{
	ShowSceneDecoration(true, !m_isGridShown);
}


void CReportDocumentViewComp::OnExportToPdf()
{
	if (PagesTabs->count() <= 0){
		QMessageBox::warning(GetWidget(), tr("Export to PDF"), tr("Report is empty"));
		return;
	}

	QString fileName = QFileDialog::getSaveFileName(GetWidget(), tr("Export to PDF..."), "", tr("Report Files (*.pdf)"));
	if (fileName.isEmpty()){
		return;
	}

	bool ok = true;
	bool wasGridShown = m_isGridShown;

	ShowSceneDecoration(false, false);

	QPrinter printer(QPrinter::HighResolution);
	printer.setPageSize(QPrinter::A4);
	printer.setOrientation(QPrinter::Portrait);
	printer.setOutputFormat(QPrinter::PdfFormat);
	printer.setOutputFileName(fileName);
	printer.setPageMargins({ 0.0, 0.0, 0.0, 0.0 });

	QPainter painter(&printer);

	for (int i = 0; i < PagesTabs->count(); i++){
		QGraphicsView* viewPtr = dynamic_cast<QGraphicsView*>(PagesTabs->widget(i));
		Q_ASSERT(viewPtr != nullptr);

		viewPtr->scene()->render(&painter);

		if (i < PagesTabs->count() - 1){ // initially printer already has one default page so don't add a new one on the last iteration
			if (!printer.newPage()){
				QMessageBox::critical(GetWidget(), tr("Export to PDF"), tr("Failed to export report"));
				ok = false;
				break;
			}
		}
	}

	ShowSceneDecoration(true, wasGridShown);

	if (ok){
		QMessageBox::information(GetWidget(), tr("Export to PDF"), tr("Report has been exported successfully"));
	}
}


// private methods

void CReportDocumentViewComp::UpdateSceneDecoration(QGraphicsScene& scene)
{
	// set scene rect
	QRectF sceneRect(QPointF(0.0, 0.0), MapPointToScene(QPointF(s_A4WidthMm, s_A4HeightMm)));
	scene.setSceneRect(sceneRect);

	QGraphicsRectItem* sceneBorderPtr = new QGraphicsRectItem(sceneRect);
	scene.addItem(sceneBorderPtr);

	// set scene grid
	QGraphicsItemGroup* sceneGridPtr = new QGraphicsItemGroup();
	m_isGridShown ? sceneGridPtr->show() : sceneGridPtr->hide();
	scene.addItem(sceneGridPtr);

	for (qreal i = 0.0; i < s_A4WidthMm; i += 5.0)
	{
		QPointF p1 = MapPointToScene(QPointF(i, 0.0));
		QPointF p2 = MapPointToScene(QPointF(i, s_A4HeightMm));

		QGraphicsLineItem* linePtr = new QGraphicsLineItem(QLineF(p1, p2));
		linePtr->setPen(QPen(Qt::lightGray));
		sceneGridPtr->addToGroup(linePtr);
	}

	for (qreal i = 0.0; i < s_A4HeightMm; i += 5.0)
	{
		QPointF p1 = MapPointToScene(QPointF(0.0, i));
		QPointF p2 = MapPointToScene(QPointF(s_A4WidthMm, i));

		QGraphicsLineItem* linePtr = new QGraphicsLineItem(QLineF(p1, p2));
		linePtr->setPen(QPen(Qt::lightGray));
		sceneGridPtr->addToGroup(linePtr);
	}

	m_sceneDecorations.push_back(SceneDecoration(sceneBorderPtr, sceneGridPtr));
}


void CReportDocumentViewComp::UpdateSceneShapes(const imtreport::IReportPage& page, QGraphicsScene& scene)
{
	imtreportgui::CGraphicsElementShapeFactory shapeFactory;

	imtreport::IReportPage::ElementIds ids = page.GetPageElements();
	for (const QByteArray& elementId : ids){
		const i2d::IObject2d* elementPtr = page.GetPageElement(elementId);
		Q_ASSERT(elementPtr != nullptr);

		QGraphicsItem* itemPtr = shapeFactory.CreateShape(*elementPtr);

		ConvertShapeCoodinates(elementPtr, itemPtr);

		scene.addItem(itemPtr);
	}
}


void CReportDocumentViewComp::ShowSceneDecoration(const bool showBorder, const bool showGrid)
{
	for (SceneDecoration& sceneDecoration : m_sceneDecorations){
		Q_ASSERT(sceneDecoration.first);
		Q_ASSERT(sceneDecoration.second);
		showBorder ? sceneDecoration.first->show()  : sceneDecoration.first->hide();
		showGrid   ? sceneDecoration.second->show() : sceneDecoration.second->hide();
	}

	m_isGridShown = showGrid;
}


QPointF CReportDocumentViewComp::MapPointToScene(const QPointF& point) const
{
	// map shape coordinates given in mm to scene's coordinates
	QScreen* screenPtr = QGuiApplication::primaryScreen();
	Q_ASSERT(screenPtr);

	qreal x = point.x() * screenPtr->physicalDotsPerInchX() / s_MmPerInch;
	qreal y = point.y() * screenPtr->physicalDotsPerInchY() / s_MmPerInch;

	return QPointF(x, y);
}


QRectF CReportDocumentViewComp::MapRectToScene(const QRectF& rect) const
{
	return QRectF(MapPointToScene(rect.topLeft()), MapPointToScene(rect.bottomRight()));
}


QFont CReportDocumentViewComp::MapFontToScene(const QString& fontName, double fontSize) const
{
	double sceneFontSize = MapPointToScene(QPointF(fontSize, 0.0)).x();
	return QFont(fontName, sceneFontSize);
}


void CReportDocumentViewComp::ConvertShapeCoodinates(const i2d::IObject2d* pageElementPtr, QGraphicsItem* sceneElementPtr) const
{
	if ((pageElementPtr == nullptr) || (sceneElementPtr == nullptr)){
		return;
	}

	const imtreport::CRectangleElement* rectPageElement = dynamic_cast<const imtreport::CRectangleElement*>(pageElementPtr);
	if (rectPageElement){
		return ConvertRectCoodinates(rectPageElement, sceneElementPtr);
	}

	const imtreport::CCircleElement* circleSceneElement = dynamic_cast<const imtreport::CCircleElement*>(pageElementPtr);
	if (circleSceneElement){
		return ConvertEllipseCoodinates(circleSceneElement, sceneElementPtr);
	}

	const imtreport::CTextLabelElement* labelSceneElement = dynamic_cast<const imtreport::CTextLabelElement*>(pageElementPtr);
	if (labelSceneElement){
		return ConvertLabelCoodinates(labelSceneElement, sceneElementPtr);
	}

	const imtreport::CPolygonElement* polygonElement = dynamic_cast<const imtreport::CPolygonElement*>(pageElementPtr);
	if (polygonElement){
		return ConvertPolygoneCoodinates(polygonElement, sceneElementPtr);
	}

	const imtreport::CLineElement* lineElement = dynamic_cast<const imtreport::CLineElement*>(pageElementPtr);
	if (lineElement){
		return ConvertLineCoodinates(lineElement, sceneElementPtr);
	}

	const imtreport::CImageRectangleElement* imageElement = dynamic_cast<const imtreport::CImageRectangleElement*>(pageElementPtr);
	if (imageElement){
		return ConvertImageCoodinates(imageElement, sceneElementPtr);
	}

	const imtreport::CTextTable* tableElementPtr = dynamic_cast<const imtreport::CTextTable*>(pageElementPtr);
	if (tableElementPtr){
		return ConvertTableCoodinates(tableElementPtr, sceneElementPtr);
	}
}


void CReportDocumentViewComp::ConvertRectCoodinates(const imtreport::CRectangleElement* pageElement, QGraphicsItem* sceneElement) const
{
	QGraphicsRectItem* rectSceneElement = dynamic_cast<QGraphicsRectItem*>(sceneElement);
	Q_ASSERT(rectSceneElement);
	Q_ASSERT(pageElement);

	QRectF rect = MapRectToScene(iqt::GetQRectF(*pageElement));
	rectSceneElement->setRect(rect);
}


void CReportDocumentViewComp::ConvertEllipseCoodinates(const imtreport::CCircleElement* pageElement, QGraphicsItem* sceneElement) const
{
	QGraphicsEllipseItem* ellipseSceneElement = dynamic_cast<QGraphicsEllipseItem*>(sceneElement);
	Q_ASSERT(ellipseSceneElement);
	Q_ASSERT(pageElement);

	QRectF rect = MapRectToScene(iqt::GetQRectF(pageElement->GetBoundingBox()));
	ellipseSceneElement->setRect(MapRectToScene(rect));
}


void CReportDocumentViewComp::ConvertLabelCoodinates(const imtreport::CTextLabelElement* pageElement, QGraphicsItem* sceneElement) const
{
	QGraphicsTextItem* labelSceneElement = dynamic_cast<QGraphicsTextItem*>(sceneElement);
	Q_ASSERT(labelSceneElement);
	Q_ASSERT(pageElement);

	QRectF rect = MapRectToScene(pageElement->GetRectangle());
	labelSceneElement->setPos(rect.topLeft());

	if (rect.width() > 0.0)
		labelSceneElement->setTextWidth(rect.width());

	labelSceneElement->setFont(MapFontToScene(pageElement->GetFontName(), pageElement->GetFontSize()));
}


void CReportDocumentViewComp::ConvertLineCoodinates(const imtreport::CLineElement* pageElement, QGraphicsItem* sceneElement) const
{
	QGraphicsLineItem* lineSceneElement = dynamic_cast<QGraphicsLineItem*>(sceneElement);
	Q_ASSERT(lineSceneElement);
	Q_ASSERT(pageElement);

	QPointF point1 = MapPointToScene(pageElement->GetPoint1());
	QPointF point2 = MapPointToScene(pageElement->GetPoint2());
	lineSceneElement->setLine(QLineF(point1, point2));
}

void CReportDocumentViewComp::ConvertPolygoneCoodinates(const imtreport::CPolygonElement* pageElement, QGraphicsItem* sceneElement) const
{
	QGraphicsPolygonItem* polygoneSceneElement = dynamic_cast<QGraphicsPolygonItem*>(sceneElement);
	Q_ASSERT(polygoneSceneElement);
	Q_ASSERT(pageElement);

	QPolygonF polygon;
	polygon.reserve(pageElement->GetNodesCount());

	for (int i = 0; i < pageElement->GetNodesCount(); ++i){
		const i2d::CVector2d& node = pageElement->GetNodePos(i);
		polygon.append(MapPointToScene(node));
	}

	polygoneSceneElement->setPolygon(polygon);
}


void CReportDocumentViewComp::ConvertImageCoodinates(const imtreport::CImageRectangleElement* pageElement, QGraphicsItem* sceneElement) const
{
	QGraphicsPixmapItem* imageSceneElement = dynamic_cast<QGraphicsPixmapItem*>(sceneElement);
	Q_ASSERT(imageSceneElement);
	Q_ASSERT(pageElement);

	QRectF rect = MapRectToScene(iqt::GetQRectF(*pageElement));
	imageSceneElement->setPos(rect.topLeft());

	QSize pixmapSize = imageSceneElement->pixmap().size();

	if (pixmapSize.width() > 0){
		qreal scale = rect.width() / pixmapSize.width();
		imageSceneElement->setScale(scale);
	}
	else{
		imageSceneElement->setScale(1.0);
	}
}


void CReportDocumentViewComp::ConvertTableCoodinates(const imtreport::CTextTable* pageElement, QGraphicsItem* sceneElement) const
{
	QGraphicsProxyWidget* tableSceneElement = dynamic_cast<QGraphicsProxyWidget*>(sceneElement);
	Q_ASSERT(tableSceneElement);
	Q_ASSERT(pageElement);

	QTableWidget* tableWidget = dynamic_cast<QTableWidget*>(tableSceneElement->widget());
	Q_ASSERT(tableWidget);

	QRectF rect = MapRectToScene(*pageElement);

	tableSceneElement->setPreferredSize(rect.size());

	tableWidget->setGeometry(rect.toAlignedRect());
	tableWidget->setFixedSize(rect.size().toSize());

	// set horizontal header items
	for (int col = 0; col < pageElement->GetColumnCount(); col++){
		const imtreport::CTextTableItem& tableItem = pageElement->GetHorizontalHeaderItem(col);
		tableWidget->setHorizontalHeaderItem(col, ConvertTableItem(tableItem));
		tableWidget->horizontalHeaderItem(col)->setBackground(tableItem.GetBackgroundColor());
	}

	// set vertical header items
	for (int row = 0; row < pageElement->GetRowCount(); row++){
		const imtreport::CTextTableItem& tableItem = pageElement->GetVerticalHeaderItem(row);
		tableWidget->setVerticalHeaderItem(row, ConvertTableItem(tableItem));
		tableWidget->verticalHeaderItem(row)->setBackground(tableItem.GetBackgroundColor());
	}

	// set items
	for (int col = 0; col < pageElement->GetColumnCount(); col++){
		for (int row = 0; row < pageElement->GetRowCount(); row++){
			const imtreport::CTextTableItem& tableItem = pageElement->GetItem(row, col);
			tableWidget->setItem(row, col, ConvertTableItem(tableItem));
			tableWidget->item(row, col)->setBackground(tableItem.GetBackgroundColor());
		}
	}
}


QTableWidgetItem* CReportDocumentViewComp::ConvertTableItem(const imtreport::CTextTableItem& tableItem) const
{
	QTableWidgetItem* tableWidgetItem = new QTableWidgetItem();

	tableWidgetItem->setText(tableItem.GetText());
	tableWidgetItem->setTextAlignment(tableItem.GetAlignment());
	tableWidgetItem->setFont(MapFontToScene(tableItem.GetFontName(), tableItem.GetFontSize()));
	tableWidgetItem->setForeground(QBrush(tableItem.GetForegroundColor()));
	tableWidgetItem->setIcon(tableItem.GetIcon());

	return tableWidgetItem;
}


} // namespace imtreportgui


