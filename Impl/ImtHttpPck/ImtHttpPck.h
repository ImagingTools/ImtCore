#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imthttp/CHttpFileTransfer.h>
#include <imthttp/CTcpServerComp.h>
#include <imthttp/CWebSocketServerComp.h>
#include <imthttp/CWebSocketProtocolEngineComp.h>
#include <imthttp/CHttpProtocolEngineComp.h>
#include <imthttp/CTcpProtocolEngineComp.h>
#include <imthttp/CWorkerManagerComp.h>
#include <imthttp/CBinaryDataProviderComp.h>
#include <imthttp/CDelegatedBinaryDataProviderComp.h>
#include <imthttp/CIconDataProviderComp.h>
#include <imthttp/CUdpServerComp.h>


namespace ImtHttpPck
{


typedef icomp::TMakeComponentWrap<imthttp::CHttpFileTransfer, imtcom::IFileTransfer> HttpFileTransfer;
typedef imthttp::CTcpServerComp TcpServer;
typedef icomp::TModelCompWrap<imthttp::CWebSocketServerComp> WebSocketServer;
typedef imthttp::CWebSocketProtocolEngineComp WebSocketProtocolEngine;
typedef imthttp::CHttpProtocolEngineComp HttpProtocolEngine;
typedef imthttp::CTcpProtocolEngineComp TcpProtocolEngine;
typedef imthttp::CWorkerManagerComp WorkerManager;
typedef imthttp::CBinaryDataProviderComp BinaryDataProvider;
typedef imthttp::CDelegatedBinaryDataProviderComp DelegatedBinaryDataProviders;
typedef imthttp::CIconDataProviderComp IconDataProvider;
typedef imthttp::CUdpServerComp UdpServer;


} // namespace ImtHttpPck


