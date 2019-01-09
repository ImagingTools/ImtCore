#include <imtbase/CInspectionReport.h>


// ACF includes
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtbase/CSingleInspectionReportPage.h>
#include <imtbase/CInspectionReportTitlePage.h>


namespace imtbase
{


// protected methods

// reimplemented (CReportBase)

istd::IChangeable * CInspectionReport::CreateOverviewPage(const iprm::IParamsSet * paramsPtr, const idoc::IDocumentMetaInfo * pageMetaInfoPtr) const
{
	return new imod::TModelWrap<CInspectionReportTitlePage>;
}


istd::IChangeable * CInspectionReport::CreateDetailPage(const iprm::IParamsSet * paramsPtr, const idoc::IDocumentMetaInfo * pageMetaInfoPtr) const
{
	return new imod::TModelWrap<CSingleInspectionReportPage>;
}

} // namespace imtbase


