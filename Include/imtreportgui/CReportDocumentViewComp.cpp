#include <imtreportgui/CReportDocumentViewComp.h>


// ImtCore includes
#include <imtreport/IReportPage.h>
#include <imtreport/CGraphicsElementShapeFactory.h>


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

	imtreport::CGraphicsElementShapeFactory shapeFactory;

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


} // namespace imtreportgui


