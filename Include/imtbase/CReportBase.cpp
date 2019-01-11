#include <imtbase/CReportBase.h>


// ImtCore includes
#include <imtbase/IReportPage.h>


namespace imtbase
{


// public methods

// reimplemented (IReportDocument)

const IReportPage* CReportBase::GetReportPage(int pageIndex) const
{
	Q_ASSERT(pageIndex >= 0);
	Q_ASSERT(pageIndex < m_documentPages.count());

	return dynamic_cast<const IReportPage*>(m_documentPages[pageIndex]);
}


// reimplemented (idoc::IMultiPageDocument)

istd::IChangeable* CReportBase::InsertPage(
			const idoc::IDocumentMetaInfo* pageMetaInfoPtr,
			const iprm::IParamsSet* pageParameterPtr,
			int position)
{
	Page newPage;

	if (pageMetaInfoPtr != NULL){
		if (!newPage.pageMetaInfo.CopyFrom(*pageMetaInfoPtr)){
			return NULL;
		}
	}

	istd::IChangeable* pageContentPtr = nullptr;

	if (position == 0){
		pageContentPtr = CreateOverviewPage(pageParameterPtr, pageMetaInfoPtr);
	}
	else{
		pageContentPtr = CreateDetailPage(pageParameterPtr, pageMetaInfoPtr);
	}

	if (pageContentPtr == nullptr){
		return nullptr;
	}

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


// reimplemented (istd::IChangeable)

bool CReportBase::CopyFrom(const istd::IChangeable& object, CompatibilityMode mode)
{
	if (BaseClass::CopyFrom(object, mode)){
		return true;
	}

	return false;
}


bool CReportBase::ResetData(CompatibilityMode /*mode*/)
{
	ResetPages();

	return true;
}


} // namespace imtbase


