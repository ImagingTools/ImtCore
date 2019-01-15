#pragma once


// ACF includes
#include <idoc/IMultiPageDocument.h>


namespace imtreports
{


class IReportPage;


/**
	Interface for defining a general report document.
*/
class IReportDocument: virtual public idoc::IMultiPageDocument
{
public:
	/**
		Get the report page at the given index.
	*/
	virtual const IReportPage* GetReportPage(int pageIndex) const = 0;
};


} // namespace imtreports


