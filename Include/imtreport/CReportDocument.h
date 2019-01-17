#pragma once


// ACF includes
#include <imod/TModelWrap.h>
#include <idoc/TMultiPageDocumentWrap.h>
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imtreport/IReportDocument.h>


namespace imtreport
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
};


} // namespace imtreport


