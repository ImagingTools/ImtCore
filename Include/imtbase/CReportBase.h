#pragma once


// ACF includes
#include <imod/TModelWrap.h>
#include <idoc/TMultiPageDocumentWrap.h>
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imtbase/IReportDocument.h>


namespace imtbase
{


/**
	Base implementation of a report document.
*/
class CReportBase: public idoc::TMultiPageDocumentWrap<IReportDocument>
{
public:
	typedef idoc::TMultiPageDocumentWrap<IReportDocument> BaseClass;

	// reimplemented (IReportDocument)
	virtual const IReportPage* GetReportPage(int pageIndex) const;

	// reimplemented (idoc::IMultiPageDocument)
	virtual istd::IChangeable* InsertPage(
				const idoc::IDocumentMetaInfo* pageMetaInfoPtr = NULL,
				const iprm::IParamsSet* pageParameterPtr = NULL,
				int position = -1);

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const istd::IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS);
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS);

protected:
	// abstract methods

	/**
		Create the title (overview) page of the report.
	*/
	virtual istd::IChangeable* CreateOverviewPage(
				const iprm::IParamsSet* paramsPtr = nullptr,
				const idoc::IDocumentMetaInfo* pageMetaInfoPtr = nullptr) const = 0;

	/**
		Create the detail page of the report.
	*/
	virtual istd::IChangeable* CreateDetailPage(
				const iprm::IParamsSet* paramsPtr = nullptr,
				const idoc::IDocumentMetaInfo* pageMetaInfoPtr = nullptr) const = 0;
};


} // namespace imtbase


