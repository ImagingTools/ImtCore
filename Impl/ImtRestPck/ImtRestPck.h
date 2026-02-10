// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtbase/TComponentFactoryComp.h>
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
#include <imtrest/CTcpProtocolEngineComp.h>
#include <imtrest/CDelegatedServletComp.h>
#include <imtrest/CDelegatedBinaryDataProviderComp.h>
#include <imtrest/CBinaryDataProviderComp.h>
#include <imtrest/CIconDataProviderComp.h>
#include <imtrest/CHttpFileUploadServletComp.h>
#include <imtrest/CUdpServerComp.h>


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
typedef imtrest::CTcpProtocolEngineComp TcpProtocolEngine;
typedef imtrest::CDelegatedServletComp DelegatedServlet;
typedef imtrest::CDelegatedBinaryDataProviderComp DelegatedBinaryDataProviders;
typedef imtrest::CBinaryDataProviderComp BinaryDataProvider;
typedef imtrest::CIconDataProviderComp IconDataProvider;
typedef imtrest::CHttpFileUploadServletComp HttpFileUploadServlet;
typedef imtrest::CUdpServerComp UdpServer;

} // namespace ImtRestPck


