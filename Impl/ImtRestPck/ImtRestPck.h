#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtrest/CWebSocketServerComp.h>
#include <imtrest/CWebSocketProtocolEngineComp.h>
#include <imtrest/CTcpServerComp.h>
#include <imtrest/CHttpProtocolEngineComp.h>
#include <imtrest/CHttpRootServletComp.h>
#include <imtrest/CHttpFileBasedServletComp.h>
#include <imtrest/CHttpHtmlFolderBasedServletComp.h>
#include <imtrest/CHttpRedirectComp.h>
#include <imtrest/CHttpFileProviderBasedServletComp.h>
#include <imtrest/CHttpFileControllerServletComp.h>
#include <imtrest/CWorkerManagerComp.h>
#include <imtrest/CServerLogDataProviderComp.h>
#include <imtrest/CTcpProtocolEngineComp.h>
#include <imtrest/CDelegatedServletComp.h>


/**
	ImtRestPck package
*/
namespace ImtRestPck
{


typedef imtrest::CTcpServerComp TcpServer;
typedef icomp::TModelCompWrap<imtrest::CWebSocketServerComp> WebSocketServer;
typedef imtrest::CWebSocketProtocolEngineComp WebSocketProtocolEngine;
typedef imtrest::CHttpProtocolEngineComp HttpProtocolEngine;
typedef imtrest::CHttpRootServletComp HttpRootServlet;
typedef imtrest::CHttpFileBasedServletComp HttpFileBasedServlet;
typedef imtrest::CHttpHtmlFolderBasedServletComp HttpHtmlFolderBasedServlet;
typedef imtrest::CHttpRedirectComp HttpRedirectServlet;
typedef imtrest::CHttpFileProviderBasedServletComp HttpFileProviderBasedServlet;
typedef imtrest::CHttpFileControllerServletComp HttpFileControllerServlet;
typedef imtrest::CWorkerManagerComp WorkerManaqer;
typedef imtrest::CServerLogDataProviderComp ServerLogDataProvider;
typedef imtrest::CTcpProtocolEngineComp TcpProtocolEngine;
typedef imtrest::CDelegatedServletComp DelegatedServlet;


} // namespace ImtRestPck



