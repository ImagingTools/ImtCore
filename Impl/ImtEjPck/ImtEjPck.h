#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtej/CReportProvider.h>
#include <imtej/CReportExport.h>

/**
	ImtEjPck package
*/
namespace ImtEjPck
{

typedef icomp::TMakeComponentWrap<imtej::CReportProvider,imtej::IReportProvider> EjProvider;
typedef icomp::TMakeComponentWrap<imtej::CReportExport,imtej::IReportExport> EjExport;

} // namespace ImtEjPck



