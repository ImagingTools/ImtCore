#include "ImtReportGuiPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtReportGuiPck
{


I_EXPORT_PACKAGE(
			"ImtReportGuiPck",
			"ImagingTools report UI-component package",
			IM_PROJECT("\"ImagingTools Core\"") IM_COMPANY("ImagingTools") "Report GUI");

I_EXPORT_COMPONENT(
			ReportDocumentView,
			"Report view",
			"Report Document View");

I_EXPORT_COMPONENT(
			ReportPdfPersistence,
			"PDF-format report document persistence",
			"PDF Report Persistence");

I_EXPORT_COMPONENT(
			ReportTiffPersistence,
			"TIFF-format report document persistence",
			"TIFF Report Persistence");


} // namespace ImtReportGuiPck


