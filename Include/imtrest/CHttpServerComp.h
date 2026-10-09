// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtrest/CHttpTransport.h>

#ifdef IMTREST_HTTP_SERVER_AVAILABLE


// STL includes
#include <memory>

// Qt includes
#include <QtCore/QObject>

// ACF includes
#include <ibase/TRuntimeStatusHanderCompWrap.h>
#include <ilog/TLoggerCompWrap.h>
#include <iprm/IParamsSet.h>
#include <imod/CMultiModelDispatcherBase.h>

// ImtCore includes
#include <imtcom/IServerConnectionInterface.h>
#include <imtcom/ISslConfigurationManager.h>
#include <imtrest/CHttpRequest.h>
#include <imtrest/IHttpTransportHandler.h>
#include <imtrest/IProtocolEngine.h>
#include <imtrest/IRequestServlet.h>
#include <imtrest/IResponseDispatcher.h>
#include <imtrest/IServer.h>
#include <imtrest/IWebSocketUpgradeHandler.h>


namespace imtrest
{


/**
	HTTP(S) server based on QtHttpServer.
	The component connects the network transport (CHttpTransport) with the domain model of imtrest:
	the transport requests are converted into IRequest objects (using the protocol engine) and passed to the request servlet
	in the thread of the component. Responses are accepted over IResponseDispatcher from any thread.
	WebSocket upgrade requests on the same port are passed to the optional IWebSocketUpgradeHandler.

	Threading:
	- Network I/O (TLS, HTTP parsing, keep-alive, writing of responses) runs in a fixed pool of I/O threads.
	- The request servlet is called in the thread of the component (main thread), as with the previous CTcpServerComp.
	  The servlet must not block (e.g. CWorkerManagerComp dispatches the request to its worker threads and returns immediately).
	- SendResponse is thread-safe and does not block.
*/
class CHttpServerComp:
			public QObject,
			public ibase::TRuntimeStatusHanderCompWrap<ilog::CLoggerComponentBase>,
			private imod::CMultiModelDispatcherBase,
			virtual public IResponseDispatcher,
			virtual public IServer,
			virtual protected IHttpTransportHandler
{
public:
	typedef ibase::TRuntimeStatusHanderCompWrap<ilog::CLoggerComponentBase> BaseClass;
	typedef imod::CMultiModelDispatcherBase BaseClass2;

	I_BEGIN_COMPONENT(CHttpServerComp);
		I_REGISTER_INTERFACE(IResponseDispatcher)
		I_REGISTER_INTERFACE(IServer)
		I_ASSIGN(m_requestHandlerCompPtr, "RequestHandler", "Request handler registered for the server", true, "RequestHandler");
		I_ASSIGN(m_protocolEngineCompPtr, "ProtocolEngine", "Protocol engine used in the server", true, "ProtocolEngine");
		I_ASSIGN(m_serverConnnectionInterfaceCompPtr, "ServerInterface", "Parameter providing the server connection interface to be listened", true, "ServerInterface");
		I_ASSIGN(m_sslConfigurationCompPtr, "SslConfiguration", "SSL Configuration is used by networking classes to relay information about an open SSL connection and to allow the server to control certain features of that connection.", false, "SslConfiguration")
		I_ASSIGN_TO(m_sslConfigurationModelCompPtr, m_sslConfigurationCompPtr, false)
		I_ASSIGN(m_sslConfigurationManagerCompPtr, "SslConfigurationManager", "SSL configuration manager, used to create an SSL configuration for server", false, "SslConfigurationManager")
		I_ASSIGN(m_webSocketUpgradeHandlerCompPtr, "WebSocketUpgradeHandler", "Handler of WebSocket upgrade requests received on the HTTP port. If not set, the upgrade requests are rejected", false, "WebSocketUpgradeHandler");
		I_ASSIGN(m_startServerOnCreateAttrPtr, "StartServerOnCreate", "If enabled, the server will be started on after component creation", true, true);
		I_ASSIGN(m_ioThreadCountAttrPtr, "IoThreadCount", "Number of network I/O threads. 0 means the number of CPU cores", true, 0);
		I_ASSIGN(m_requestHeaderTimeoutAttrPtr, "RequestHeaderTimeout", "Timeout (in ms) for the TLS handshake and the headers of the first request on a new connection. 0 disables the timeout", true, 10000);
		I_ASSIGN(m_keepAliveTimeoutAttrPtr, "KeepAliveTimeout", "Timeout (in ms) for idle keep-alive connections. 0 disables the timeout", true, 60000);
	I_END_COMPONENT

	CHttpServerComp();
	~CHttpServerComp() override;

	// reimplemented (imtrest::IResponseDispatcher)
	virtual bool SendResponse(const QByteArray& requestId, ConstResponsePtr& response) const override;
	virtual bool SendRequest(const QByteArray& requestId, ConstRequestPtr& request) const override;

	// reimplemented (imtrest::IServer)
	virtual bool StartServer() override;
	virtual bool StopServer() override;
	virtual ServerStatus GetServerStatus() const override;

protected:
	// reimplemented (imtrest::IHttpTransportHandler)
	virtual bool OnHttpRequest(const RequestData& request) override;
	virtual void OnHttpRequestAborted(const QByteArray& requestId) override;
	virtual bool OnWebSocketHandshake(QTcpSocket* socketPtr) override;

	// reimplemented (imod::CMultiModelDispatcherBase)
	virtual void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (ibase::TRuntimeStatusHanderCompWrap)
	virtual void OnSystemShutdown() override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	bool EnsureServerStarted();
	void ProcessRequest(const RequestData& requestData);
	static CHttpRequest::MethodType GetMethodType(const QByteArray& method);

private:
	I_REF(imtrest::IRequestServlet, m_requestHandlerCompPtr);
	I_REF(IProtocolEngine, m_protocolEngineCompPtr);
	I_REF(imtcom::IServerConnectionInterface, m_serverConnnectionInterfaceCompPtr);
	I_REF(iprm::IParamsSet, m_sslConfigurationCompPtr);
	I_REF(imod::IModel, m_sslConfigurationModelCompPtr);
	I_REF(imtcom::ISslConfigurationManager, m_sslConfigurationManagerCompPtr);
	I_REF(IWebSocketUpgradeHandler, m_webSocketUpgradeHandlerCompPtr);
	I_ATTR(bool, m_startServerOnCreateAttrPtr);
	I_ATTR(int, m_ioThreadCountAttrPtr);
	I_ATTR(int, m_requestHeaderTimeoutAttrPtr);
	I_ATTR(int, m_keepAliveTimeoutAttrPtr);

	std::unique_ptr<CHttpTransport> m_transportPtr;

	bool m_isInitialized = false;
};


} // namespace imtrest


#endif // IMTREST_HTTP_SERVER_AVAILABLE


