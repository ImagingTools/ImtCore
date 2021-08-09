#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtrest/CWebSocketServerComp.h>
#include <imtrest/CWebSocketProtocolEngineComp.h>
#include <imtrest/CWebSocketSuscriberEngineComp.h>
#include <imtrest/CTcpServerComp.h>
#include <imtrest/CHttpProtocolEngineComp.h>
#include <imtrest/CHttpRootServletComp.h>
#include <imtrest/CHttpFileBasedServletComp.h>
#include <imtrest/CHttpHtmlFolderBasedServletComp.h>
#include <imtrest/CJsonModelBasedServletComp.h>
#include <imtrest/CHttpRedirectComp.h>
#include <imtrest/CHttpFileProviderBasedServletComp.h>
#include <imtrest/CHttpClientRequest.h>
#include <imtrest/CHttpClientComp.h>


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
typedef imtrest::CHttpRootServletComp HttpRootServlet;
typedef imtrest::CHttpFileBasedServletComp HttpFileBasedServlet;
typedef imtrest::CHttpHtmlFolderBasedServletComp HttpHtmlFolderBasedServlet;
typedef imtrest::CJsonModelBasedServletComp JsonModelBasedServlet;
typedef imtrest::CHttpRedirectComp HttpRedirectServlet;
typedef imtrest::CHttpFileProviderBasedServletComp HttpFileProviderBasedServlet;
typedef imtrest::CHttpClientComp HttpClientRequestManager;
typedef icomp::TModelCompWrap<icomp::TMakeComponentWrap<imtrest::CHttpClientRequest>> HttpClientRequestFactory;


} // namespace ImtRestPck



