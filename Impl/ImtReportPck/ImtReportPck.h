// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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



