#include <imtreportgui/CReportDocumentViewComp.h>


// Qt includes
#include <QtPrintSupport/QPrinter>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QGraphicsView>
#include <QtGui/QScreen>

// ImtCore includes
#include <imtreportgui/CReportSceneBuilder.h>


namespace imtreportgui
{


// static members
const qreal CReportDocumentViewComp::s_A4WidthMm = 210.0;
const qreal CReportDocumentViewComp::s_A4HeightMm = 297.0;


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

	CReportSceneBuilder::ReportScenes scenes = CReportSceneBuilder::Build(*documentPtr);

	for (int i = 0; i < scenes.size(); ++i){
		QGraphicsScene* scenePtr = scenes[i];
		Q_ASSERT(scenePtr);

		CreateSceneDecoration(*scenePtr);

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

void CReportDocumentViewComp::CreateSceneDecoration(QGraphicsScene& scene)
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

	// send scene decoration on background layer of the scene
	sceneBorderPtr->setZValue(-1);
	sceneGridPtr->setZValue(-1);

	m_sceneDecorations.push_back(SceneDecoration(sceneBorderPtr, sceneGridPtr));
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


QPointF CReportDocumentViewComp::MapPointToScene(const QPointF& point)
{
	// map shape coordinates given in mm to scene's coordinates
	QScreen* screenPtr = QGuiApplication::primaryScreen();
	Q_ASSERT(screenPtr);

	qreal x = point.x() * screenPtr->physicalDotsPerInchX() / 25.4;
	qreal y = point.y() * screenPtr->physicalDotsPerInchY() / 25.4;

	return QPointF(x, y);
}


} // namespace imtreportgui


