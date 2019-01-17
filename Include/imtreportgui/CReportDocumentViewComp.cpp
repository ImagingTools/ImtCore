#include <imtreportgui/CReportDocumentViewComp.h>


// Qt includes
#include <QtPrintSupport/QPrinter>
#include <QtWidgets/qmessagebox.h>

// ImtCore includes
#include <imtreport/IReportPage.h>
#include <imtreportgui/CGraphicsElementShapeFactory.h>


namespace imtreportgui
{


// public methods

CReportDocumentViewComp::CReportDocumentViewComp()
	:m_commands("&View", 100)
{
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
	m_scene.setSceneRect(0, 0, s_A4Width, s_A4Height);

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

	PageView->setFixedWidth(PageView->width() / PageView->widthMM() * s_A4Width);
	PageView->setFixedHeight(PageView->height() / PageView->heightMM() * s_A4Height);
	PageView->setScene(&m_scene);
}


void CReportDocumentViewComp::OnGuiDestroyed()
{
	SaveToPDF(); // to be removed, just for test

	BaseClass::OnGuiDestroyed();
}


void CReportDocumentViewComp::OnGuiRetranslate()
{
	BaseClass::OnGuiRetranslate();
}


// private methods
void CReportDocumentViewComp::SaveToPDF()
{
	QPrinter printer(QPrinter::HighResolution);
	printer.setPageSize(QPrinter::A4);
	printer.setOrientation(QPrinter::Portrait);
	printer.setOutputFormat(QPrinter::PdfFormat);
	printer.setOutputFileName("c:\\work\\test.pdf");

	QPainter painter(&printer);
	m_scene.render(&painter, QRectF(), QRectF());

	//printer.newPage(); // perhaps it might help for paging in PDF
}

} // namespace imtreportgui


