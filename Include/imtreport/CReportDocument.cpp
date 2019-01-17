#include <imtreport/CReportDocument.h>


// ImtCore includes
#include <imtreport/CReportPage.h>


namespace imtreport
{


// public methods

// reimplemented (IReportDocument)

const IReportPage* CReportDocument::GetReportPage(int pageIndex) const
{
	Q_ASSERT(pageIndex >= 0);
	Q_ASSERT(pageIndex < m_documentPages.count());

	return dynamic_cast<const IReportPage*>(m_documentPages[pageIndex].pagePtr.GetPtr());
}


// reimplemented (idoc::IMultiPageDocument)

istd::IChangeable* CReportDocument::InsertPage(
			const idoc::IDocumentMetaInfo* pageMetaInfoPtr,
			const iprm::IParamsSet* pageParameterPtr,
			int position)
{
	Q_UNUSED(pageParameterPtr);

	Page newPage;

	if (pageMetaInfoPtr != NULL){
		if (!newPage.pageMetaInfo.CopyFrom(*pageMetaInfoPtr)){
			return NULL;
		}
	}

	istd::IChangeable* pageContentPtr = new imod::TModelWrap<CReportPage>;

	newPage.pagePtr.SetPtr(pageContentPtr);

	istd::CChangeNotifier changePtr(this);

	if (position < 0){
		m_documentPages.push_back(newPage);
	}
	else{
		position = qMin(m_documentPages.count(), position);

		m_documentPages.insert(position, newPage);
	}

	return pageContentPtr;
}


} // namespace imtreport


