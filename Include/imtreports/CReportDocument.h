#pragma once


// ACF includes
#include <imod/TModelWrap.h>
#include <idoc/TMultiPageDocumentWrap.h>
#include <idoc/CStandardDocumentMetaInfo.h>

// imtreports includes
#include <imtreports/IReportDocument.h>


namespace imtreports
{


/**
	Base implementation of a report document.
*/
class CReportDocument: public idoc::TMultiPageDocumentWrap<IReportDocument>
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
};


} // namespace imtreports


