#include "ImtRestPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtRestPck
{


I_EXPORT_PACKAGE(
			"ImtRestPck",
			"REST-based communication component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

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

I_EXPORT_COMPONENT(
            HttpFileBasedHandler,
            "HTTP file handler",
            "HTTP File Template");

I_EXPORT_COMPONENT(
		HttpHtmlFolderBasedHandler,
		"Generates HTML-document with list of objects for all the files and directories in the directory",
		"HTTP File Dir Directory HTML");

I_EXPORT_COMPONENT(
		HttpRedirectHandler,
		"Redirects to other command",
		"HTTP Redirect");

I_EXPORT_COMPONENT(
		JsonModelBasedHandler,
		"Generates Json model for QML??",
		"JSON Model");


} // namespace ImtRestPck


