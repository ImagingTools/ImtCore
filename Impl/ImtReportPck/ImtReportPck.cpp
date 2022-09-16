#include "ImtReportPck.h"


// ACF includes
#include <icomp/export.h>



namespace ImtReportPck
{


I_EXPORT_PACKAGE(
			"ImtReportGuiPck",
			"ImagingTools report component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools") "Report");

I_EXPORT_COMPONENT(
			ReportDocument,
			"Default report document",
			"Report Status Document");

I_EXPORT_COMPONENT(
			InspectionReportBuilder,
			"Inspection report builder",
			"Inspection Report Builder");

I_EXPORT_COMPONENT(
			InspectionReportSimulator,
			"Inspection report simulator",
			"Inspection Report Simulator");

} // namespace ImtReportPck


