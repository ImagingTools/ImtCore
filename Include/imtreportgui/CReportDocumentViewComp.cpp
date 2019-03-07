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


// public methods

CReportDocumentViewComp::CReportDocumentViewComp()
	:m_fileCommands("&File", 100),
	m_viewCommands("&View", 100),
	m_exportToPdfCommand("Export to PDF...", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, 1984),
	m_exportToTiffCommand("Export to TIFF...", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, 1985),
	m_showGridCommand("Show grid", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR | ibase::ICommand::CF_ONOFF, 1986),
	m_isGridShown(true)
{
	m_showGridCommand.setChecked(true);

	connect(&m_showGridCommand, SIGNAL(triggered()), this, SLOT(OnShowGrid()));
	connect(&m_exportToPdfCommand, SIGNAL(triggered()), this, SLOT(OnExportToPdf()));
	connect(&m_exportToTiffCommand, SIGNAL(triggered()), this, SLOT(OnExportToTiff()));

	m_fileCommands.InsertChild(&m_exportToPdfCommand);
	m_fileCommands.InsertChild(&m_exportToTiffCommand);
	m_viewCommands.InsertChild(&m_showGridCommand);

	m_rootCommands.InsertChild(&m_fileCommands);
	m_rootCommands.InsertChild(&m_viewCommands);
}


// reimplemented (ibase::ICommandsProvider)

const ibase::IHierarchicalCommand* CReportDocumentViewComp::GetCommands() const
{
	return &m_rootCommands;
}


// reimplemented (ilog::IMessageConsumer)

bool CReportDocumentViewComp::IsMessageSupported(
			int /*messageCategory*/,
			int /*messageId*/,
			const istd::IInformationProvider* /*messagePtr*/) const
{
	return true;
}


void CReportDocumentViewComp::AddMessage(const MessagePtr& messagePtr)
{
	if (!messagePtr.IsValid()){
		return;
	}

	QString messageText = messagePtr->GetInformationDescription();

	switch (messagePtr->GetInformationCategory()){
		case istd::IInformationProvider::IC_WARNING:
			QMessageBox::warning(GetWidget(), tr("Report document"), messageText);
			break;

		case istd::IInformationProvider::IC_ERROR:
		case istd::IInformationProvider::IC_CRITICAL:
			QMessageBox::critical(GetWidget(), tr("Report document"), messageText);
			break;

		default:
			QMessageBox::information(GetWidget(), tr("Report document"), messageText);
			break;
	}
}


// protected methods

// reimplemented (iqtgui::TGuiObserverWrap)

void CReportDocumentViewComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	m_sceneDecorations.clear();

	imtreport::IReportDocument* documentPtr = GetObjectPtr();
	Q_ASSERT(documentPtr != NULL);

	CReportSceneBuilder::ReportScenes scenes = CReportSceneBuilder::Build(*documentPtr, 0);

	for (int i = 0; i < scenes.size(); ++i){
		QGraphicsScene* scenePtr = scenes[i];
		Q_ASSERT(scenePtr != NULL);

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
	m_exportToTiffCommand.SetVisuals(tr("Export to TIFF..."), tr("TIFF..."), tr("Export current report to TIFF file"));
	m_showGridCommand.SetVisuals(tr("Show/Hide Grid"), tr("Grid"), tr("Set grid visible/unvisible"), QIcon(":/Icons/Grid2"));
}


void CReportDocumentViewComp::OnShowGrid()
{
	ShowSceneDecoration(true, !m_isGridShown);
}


void CReportDocumentViewComp::OnExportToPdf()
{
	if (m_pdfExportCompPtr.IsValid()){
		imtreport::IReportDocument* documentPtr = GetObjectPtr();
		Q_ASSERT(documentPtr != NULL);

		m_pdfExportCompPtr->SaveToFile(*documentPtr);
	}
}


void CReportDocumentViewComp::OnExportToTiff()
{
	if (m_tiffExportCompPtr.IsValid()){
		imtreport::IReportDocument* documentPtr = GetObjectPtr();
		Q_ASSERT(documentPtr != NULL);

		m_tiffExportCompPtr->SaveToFile(*documentPtr);
	}
}


// private methods

void CReportDocumentViewComp::CreateSceneDecoration(QGraphicsScene& scene)
{
	// add scene border
	QGraphicsRectItem* sceneBorderPtr = new QGraphicsRectItem(scene.sceneRect());
	scene.addItem(sceneBorderPtr);

	// add scene grid
	QGraphicsItemGroup* sceneGridPtr = new QGraphicsItemGroup();
	m_isGridShown ? sceneGridPtr->show() : sceneGridPtr->hide();
	scene.addItem(sceneGridPtr);

	// calculate grid cell size (5 mm in pixels)
	qreal gridCellWidth = QGuiApplication::primaryScreen()->physicalDotsPerInchX() * 5.0 / 25.4;
	qreal gridCellHeight = QGuiApplication::primaryScreen()->physicalDotsPerInchY() * 5.0 / 25.4;

	for (qreal i = 0.0; i < scene.sceneRect().width(); i += gridCellWidth)
	{
		QGraphicsLineItem* linePtr = new QGraphicsLineItem(i, 0.0, i, scene.sceneRect().height());
		linePtr->setPen(QPen(Qt::lightGray));
		sceneGridPtr->addToGroup(linePtr);
	}

	for (qreal i = 0.0; i < scene.sceneRect().height(); i += gridCellHeight)
	{
		QGraphicsLineItem* linePtr = new QGraphicsLineItem(0.0, i, scene.sceneRect().width(), i);
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


} // namespace imtreportgui


