#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtrest/TSocketServerComp.h>
#include <imtrest/CWebSocketServerComp.h>
#include <imtrest/CWebSocketProtocolEngineComp.h>
#include <imtrest/CWebSocketSuscriberEngineComp.h>
#include <imtrest/CTcpServerComp.h>
#include <imtrest/CHttpProtocolEngineComp.h>
#include <imtrest/CHttpRootHandlerComp.h>
#include <imtrest/CHttpFileBasedHandlerComp.h>
#include <imtrest/CHttpHtmlFolderBasedHandlerComp.h>
#include <imtrest/CJsonModelBasedHandlerComp.h>
#include <imtrest/CHttpRedirectComp.h>


/**
	ImtRestPck package
*/
namespace ImtRestPck
{


typedef imtrest::CTcpServerComp TcpServer;
typedef imtrest::CWebSocketServerComp WebSocketServer;
typedef imtrest::CWebSocketSubscriberEngineComp WebSocketSubscriberEngine;
typedef imtrest::CWebSocketProtocolEngineComp WebSocketProtocolEngine;
typedef imtrest::CHttpProtocolEngineComp HttpProtocolEngine;
typedef imtrest::CHttpRootHandlerComp HttpRootHandler;
typedef imtrest::CHttpFileBasedHandlerComp HttpFileBasedHandler;
typedef imtrest::CHttpHtmlFolderBasedHandlerComp HttpHtmlFolderBasedHandler;
typedef imtrest::CJsonModelBasedHandlerComp JsonModelBasedHandler;
typedef imtrest::CHttpRedirectComp HttpRedirectHandler;


} // namespace ImtRestPck



