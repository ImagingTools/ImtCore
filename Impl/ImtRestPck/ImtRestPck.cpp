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
			WebSocketServer,
			"General TCP-server",
			"WebSocket Connection Server");

I_EXPORT_COMPONENT(
			WebSocketSubscriberEngine,
			"WebSocket subscriber engine",
			"WebSocket subscriber engine");

I_EXPORT_COMPONENT(
			WebSocketProtocolEngine,
			"WebSocket protocol engine",
			"WebSocket Connection Server HTTP REST");

I_EXPORT_COMPONENT(
			HttpProtocolEngine,
			"HTTP protocol engine",
			"TCP Connection Server HTTP REST");

I_EXPORT_COMPONENT(
			HttpRootServlet,
			"HTTP root servlet",
			"HTTP REST Root Protocol");

I_EXPORT_COMPONENT(
            HttpFileBasedServlet,
            "HTTP file servlet",
            "HTTP File Template");

I_EXPORT_COMPONENT(
		HttpHtmlFolderBasedServlet,
		"Generates HTML-document with list of objects for all the files and directories in the directory",
		"HTTP File Dir Directory HTML");

I_EXPORT_COMPONENT(
		HttpRedirectServlet,
		"Redirects to other command",
		"HTTP Redirect");

I_EXPORT_COMPONENT(
		HttpFileProviderBasedServlet,
		"Returns file by name \n Note: name - is a part of commandID",
		"FileProvider - based HTTP servlet");

I_EXPORT_COMPONENT(
		JsonModelBasedServlet,
		"Generates Json model for QML??",
		"JSON Model");


} // namespace ImtRestPck


