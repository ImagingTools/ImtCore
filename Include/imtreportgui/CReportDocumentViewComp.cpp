#include <imtreportgui/CReportDocumentViewComp.h>


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

	int pageCount = documentPtr->GetPagesCount();
	if (pageCount > 0){
		const imtreport::IReportPage* pagePtr = documentPtr->GetReportPage(0);
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


} // namespace imtreportgui


