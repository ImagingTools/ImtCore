#include <imtreportgui/CReportDocumentViewComp.h>


// Qt includes
#include <QtPrintSupport/QPrinter>
#include <QtWidgets/QFileDialog>

// ImtCore includes
#include <imtreport/IReportPage.h>
#include <imtreportgui/CGraphicsElementShapeFactory.h>


namespace imtreportgui
{


// public methods

CReportDocumentViewComp::CReportDocumentViewComp()
	:m_commands("&File", 100),
	m_exportToPdfCommand("Export to PDF...", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, 1984)
{
	connect(&m_exportToPdfCommand, SIGNAL(triggered()), this, SLOT(OnExportToPdf()));

	m_commands.InsertChild(&m_exportToPdfCommand);
	m_rootCommands.InsertChild(&m_commands);
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
	imtreport::IReportDocument* documentPtr = GetObjectPtr();
	Q_ASSERT(documentPtr != NULL);

	m_scene.clear();

	qreal pxMmRatio = static_cast<double>(PageView->width()) / static_cast<double>(PageView->widthMM());

	QRectF sceneRect(0.0, 0.0, s_A4Width * pxMmRatio, s_A4Height * pxMmRatio);
	m_scene.setSceneRect(sceneRect);
	m_scene.addRect(sceneRect, QPen(Qt::red));

	imtreportgui::CGraphicsElementShapeFactory shapeFactory;

	int pageCount = documentPtr->GetPagesCount();
	if (pageCount > 0){
		const imtreport::IReportPage* pagePtr = documentPtr->GetReportPage(0);
		Q_ASSERT(pagePtr != NULL);

		imtreport::IReportPage::ElementIds ids = pagePtr->GetPageElements();

		for (const QByteArray& elementId : ids){
			const imtreport::IGraphicsElement* elementPtr = pagePtr->GetPageElement(elementId);
			Q_ASSERT(elementPtr != nullptr);

			QGraphicsItem* itemPtr = shapeFactory.CreateShape(*elementPtr);

			m_scene.addItem(itemPtr);
		}
	}
}


// reimplemented (iqtgui::CGuiComponentBase)

void CReportDocumentViewComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	PageView->setScene(&m_scene);
}


void CReportDocumentViewComp::OnGuiDestroyed()
{
	BaseClass::OnGuiDestroyed();
}


void CReportDocumentViewComp::OnGuiRetranslate()
{
	BaseClass::OnGuiRetranslate();
}


void CReportDocumentViewComp::OnExportToPdf()
{
	QString fileName = QFileDialog::getSaveFileName(GetWidget(), tr("Export to PDF..."), "", "*.pdf");

	if (!fileName.isEmpty()) {
		QPrinter printer(QPrinter::HighResolution);
		printer.setPageSize(QPrinter::A4);
		printer.setOrientation(QPrinter::Portrait);
		printer.setOutputFormat(QPrinter::PdfFormat);
		printer.setOutputFileName(fileName);

		QPainter painter(&printer);
		m_scene.render(&painter);
	}
}


} // namespace imtreportgui


