#include "ImtHttpPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtHttpPck
{


I_EXPORT_PACKAGE(
			"ImtHttpPck",
			"Http component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			HttpFileTransfer,
			"Component for transfering files over http",
			"File Http Transfer");

I_EXPORT_COMPONENT(
			TcpServer,
			"General TCP-server",
			"TCP Connection Server");

I_EXPORT_COMPONENT(
			WebSocketServer,
			"General WebSocket-server",
			"WebSocket Connection Server");

I_EXPORT_COMPONENT(
			WebSocketProtocolEngine,
			"WebSocket protocol engine",
			"WebSocket Connection Server HTTP");

I_EXPORT_COMPONENT(
			HttpProtocolEngine,
			"HTTP protocol engine",
			"TCP Connection Server HTTP");

I_EXPORT_COMPONENT(
			TcpProtocolEngine,
			"General TCP protocol engine",
			"Tcp Connection Server");

I_EXPORT_COMPONENT(
			WorkerManager,
			"The manager of request handlers factory for multi-thread request server",
			"HTTP Handler Request Factory");

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
	UdpServer,
	"UDP-server",
	"UDP Connection Server");


} // namespace ImtHttpPck


