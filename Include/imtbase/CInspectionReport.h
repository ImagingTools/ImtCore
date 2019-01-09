#pragma once


// ImtCore includes
#include <imtbase/CReportBase.h>


namespace imtbase
{


/**
	Base implementation of a report document.
*/
class CInspectionReport: public CReportBase
{
public:
	typedef CReportBase BaseClass;

protected:
	// reimplemented (CReportBase)
	virtual istd::IChangeable* CreateOverviewPage(
				const iprm::IParamsSet* paramsPtr = nullptr,
				const idoc::IDocumentMetaInfo* pageMetaInfoPtr = nullptr) const;
	virtual istd::IChangeable* CreateDetailPage(
				const iprm::IParamsSet* paramsPtr = nullptr,
				const idoc::IDocumentMetaInfo* pageMetaInfoPtr = nullptr) const;
};


} // namespace imtbase


