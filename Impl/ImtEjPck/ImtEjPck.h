// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
//#include <imtej/IReportProvider.h>
//#include <imtej/IEjDocumentProvider.h>
#include <imtej/CReportExport.h>

/**
	ImtEjPck package
*/
namespace ImtEjPck
{

//typedef icomp::TMakeComponentWrap<imtej::CReportProvider,imtej::IReportProvider> ReportProvider;
//typedef icomp::TMakeComponentWrap<imtej::CReportProvider,imtej::IEjDocumentProvider> ReportProvider;
typedef icomp::TMakeComponentWrap<imtej::CReportExport,imtej::IReportExport> EjExport;

} // namespace ImtEjPck



