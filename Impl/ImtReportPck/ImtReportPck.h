#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtreport/CReportDocumentComp.h>
#include <imtreport/CInspectionReportBuilderComp.h>
#include <imtreport/CInspectionReportSimulatorComp.h>


/**
	ImtReportPck package
*/
namespace ImtReportPck
{


typedef icomp::TModelCompWrap<imtreport::CReportDocumentComp> ReportDocument;
typedef imtreport::CInspectionReportBuilderComp InspectionReportBuilder;
typedef imtreport::CInspectionReportSimulatorComp InspectionReportSimulator;


} // namespace ImtReportPck



