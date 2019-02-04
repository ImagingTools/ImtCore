#include "ImtReportGuiPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtReportGuiPck
{


I_EXPORT_PACKAGE("ImtReportGuiPck", "ImagingTools report UI-component package", "Report GUI UI UX View");

I_EXPORT_COMPONENT(
			ReportDocumentView,
			"Report view",
			"Report Document View");

I_EXPORT_COMPONENT(
			ReportDocumentPDFPersistence,
			"PDF-format report document persistence",
			"PDF Report Persistence");


} // namespace ImtReportGuiPck


