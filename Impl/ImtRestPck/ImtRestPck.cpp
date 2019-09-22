#include "ImtRestPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtRestPck
{


I_EXPORT_PACKAGE(
			"ImtRestPck",
			"REST-based communication component package",
			IM_PROJECT("\"ImagingTools Core\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			TcpServer,
			"General TCP-server",
			"TCP Connection Server");

I_EXPORT_COMPONENT(
			HttpProtocolEngine,
			"HTTP protocol engine",
			"TCP Connection Server HTTP REST");

I_EXPORT_COMPONENT(
			HttpRootHandler,
			"HTTP root handler",
			"HTTP REST Root Protocol");


} // namespace ImtRestPck


