#pragma once

// ACF includes
#include <istd/IPolymorphic.h>

#include <ejdocument.h>
#include <../EJExport/docx/ejconvertordocx.h>
#include <../EJExport/pdf/ejconvertorpdf.h>
#include <../EJExport/xlsx/ejconvertorexcel.h>

namespace imtej
{


class IObject2d;


/**
	Interface for export EjDocument format to Office formats.
*/

class IReportExport: virtual public istd::IPolymorphic
{
public:
	virtual bool exportReport(EjDocument* ejDocument, QString const& file_name) = 0;
};


} // namespace imtej


