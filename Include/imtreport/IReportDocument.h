// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <idoc/IMultiPageDocument.h>


namespace imtreport
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


} // namespace imtreport


