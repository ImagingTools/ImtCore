#include "ImtEjPck.h"


// ACF includes
#include <icomp/export.h>



namespace ImtEjPck
{

I_EXPORT_PACKAGE(
			"ImtEjPck",
			"EJ document-related component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools") "Report");

//I_EXPORT_COMPONENT(
//			EjProvider,
//			"Document creator in EJ format",
//			"Create EJ Document");

I_EXPORT_COMPONENT(
			EjExport,
			"Export from EJ document to Office document",
			"Report EJ Document Office");

} // namespace ImtEjPck


