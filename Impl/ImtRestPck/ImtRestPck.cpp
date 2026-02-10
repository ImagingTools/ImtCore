// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
		HttpFileControllerServlet,
		"Edits file by name \n Note: name - is a part of commandID",
		"FileProvider - based HTTP servlet");

I_EXPORT_COMPONENT(
		WorkerManaqer,
		"The manager of request handlers factory for multi-thread request server",
		"REST HTTP Handler Request Factory");

I_EXPORT_COMPONENT(
			TcpProtocolEngine,
			"General TCP protocol engine",
			"Tcp Connection Server");

I_EXPORT_COMPONENT(
			DelegatedServlet,
			"Delegator of the engine requests",
			"Delegator Request Communication Slave API");


I_EXPORT_COMPONENT(
			IconDataProvider,
			"Component for providing icon files (SVGs)",
			"Icon File Provider");



I_EXPORT_COMPONENT(
			BinaryDataProvider,
			"Binary data provider from folder",
			"Binary data provider");

I_EXPORT_COMPONENT(
			DelegatedBinaryDataProviders,
			"Delegated provider of a slave data provider",
			"Delegator Data Binary Provider");

I_EXPORT_COMPONENT(
			HttpFileUploadServlet,
			"Http temp file storage servlet",
			"Http Servlet File");

I_EXPORT_COMPONENT(
	UdpServer,
	"UDP-server",
	"UDP Connection Server");

} // namespace ImtRestPck


