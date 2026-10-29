// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

// ACF includes
#include <istd/TPointerVector.h>
#include <ibase/TRuntimeStatusHanderCompWrap.h>
#include <ilog/TLoggerCompWrap.h>
#include <istd/TDelPtr.h>
#include <iprm/IParamsSet.h>
#include <ifile/IFileNameParam.h>
#include <imod/CMultiModelDispatcherBase.h>

// ImtCore includes
#include <imtcom/IServerConnectionInterface.h>
#include <imtrest/IRequest.h>
#include <imtrest/IRequestServlet.h>
#include <imtrest/IProtocolEngine.h>
#include <imtrest/IResponseDispatcher.h>
#include <imtrest/IServer.h>
#include <imtrest/CConnectionTimeoutTracker.h>
#include <imtcom/ISslConfigurationManager.h>


namespace imtrest
{


class CMultiThreadServer;


/**
	TCP-based communication server.
	The server uses the underlaying protocol engine for creation of requests and responses.
*/
class CTcpServerComp:
			public QObject,
			public ibase::TRuntimeStatusHanderCompWrap<ilog::CLoggerComponentBase>,
			private imod::CMultiModelDispatcherBase,
			virtual public IResponseDispatcher,
			virtual public IServer
{
	Q_OBJECT
public:
	typedef ibase::TRuntimeStatusHanderCompWrap<ilog::CLoggerComponentBase> BaseClass;
	typedef imod::CMultiModelDispatcherBase BaseClass2;

	I_BEGIN_COMPONENT(CTcpServerComp);
		I_REGISTER_INTERFACE(IResponseDispatcher)
		I_REGISTER_INTERFACE(IServer)
		I_ASSIGN(m_requestHandlerCompPtr, "RequestHandler", "Request handler registered for the server", true, "RequestHandler");
		I_ASSIGN(m_protocolEngineCompPtr, "ProtocolEngine", "Protocol engine used in the server", true, "ProtocolEngine");
		I_ASSIGN(m_threadsLimitAttrPtr, "ThreadsLimit", "Limit of threads", true, 5);
		I_ASSIGN(m_requestInactivityTimeoutAttrPtr, "RequestInactivityTimeoutMs", "Time in milliseconds without any data while a request is received or a response is sent (the incomplete HTTP request is answered with 408). 0 - no limit", true, 30000);
		I_ASSIGN(m_keepAliveTimeoutAttrPtr, "KeepAliveTimeoutMs", "Time in milliseconds without any activity of an idle connection, before the first request and between the requests. 0 - no limit", true, 15000);
		I_ASSIGN(m_requestProcessingTimeoutAttrPtr, "RequestProcessingTimeoutMs", "Time in milliseconds that a handler can work on a request. The request that is not answered in time gets 503 and the connection is closed, the handler is not stopped. 0 - no limit (a handler can work long, for example a calculation)", true, 0);
		I_ASSIGN(m_writeHighWatermarkAttrPtr, "WriteHighWatermark", "Limit of the unsent data of a connection in bytes. A response with a body that is read by parts is not read while the socket has so much unsent data, so a slow client slows the reading down and the memory does not depend on the size of the response", true, 1048576);
		I_ASSIGN(m_startServerOnCreateAttrPtr, "StartServerOnCreate", "If enabled, the server will be started on after component creation", true, true);
		I_ASSIGN(m_serverConnnectionInterfaceCompPtr, "ServerInterface", "Parameter providing the server connection interface to be listened", true, "ServerInterface");
		I_ASSIGN(m_sslConfigurationCompPtr, "SslConfiguration", "SSL Configuration is used by networking classes to relay information about an open SSL connection and to allow the server to control certain features of that connection.", false, "SslConfiguration")
		I_ASSIGN_TO(m_sslConfigurationModelCompPtr, m_sslConfigurationCompPtr, false)
		I_ASSIGN(m_sslConfigurationManagerCompPtr, "SslConfigurationManager", "SSL configuration manager, used to create an SSL configuration for server", false, "SslConfigurationManager")
	I_END_COMPONENT

	CTcpServerComp();
	~CTcpServerComp();

	imtrest::IRequestServlet* GetRequestServlet();
	imtrest::IProtocolEngine* GetProtocolEngine();
	int GetThreadsLimit();

	/**
		Get the time limits of the connections of the server.
	*/
	CConnectionTimeoutConfig GetConnectionTimeoutConfig() const;

	/**
		Get the limit of the unsent data of a connection in bytes. A response with a body that is read by parts (see
		IStreamingResponse) is read only while the unsent data of the socket are less than the limit.
	*/
	qint64 GetWriteHighWatermark() const;

protected:
	// reimplemented (imod::CMultiModelDispatcherBase)
	virtual void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (ibase::TRuntimeStatusHanderCompWrap)
	virtual void OnSystemShutdown() override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

	// reimplemented (imtrest::IResponseDispatcher)
	virtual bool SendResponse(const QByteArray& requestId, ConstResponsePtr& response) const override;
	virtual bool SendRequest(const QByteArray& requestId, ConstRequestPtr& request) const override;

	// reimplemented (imtrest::IServer)
	virtual bool StartServer() override;
	virtual bool StopServer() override;
	virtual ServerStatus GetServerStatus() const override;

private:
	bool StartListening(const QHostAddress& address = QHostAddress::Any, quint16 port = 0);
	bool EnsureServerStarted();

private Q_SLOTS:
	void OnNewThreadConnection(const IRequest* request, const QByteArray& subCommandId);

private:
	I_REF(imtrest::IRequestServlet, m_requestHandlerCompPtr);
	I_REF(IProtocolEngine, m_protocolEngineCompPtr);
	I_REF(imtcom::IServerConnectionInterface, m_serverConnnectionInterfaceCompPtr);
	I_REF(iprm::IParamsSet, m_sslConfigurationCompPtr);
	I_REF(imod::IModel, m_sslConfigurationModelCompPtr);
	I_REF(imtcom::ISslConfigurationManager, m_sslConfigurationManagerCompPtr);

	I_ATTR(int, m_threadsLimitAttrPtr);
	I_ATTR(int, m_requestInactivityTimeoutAttrPtr);
	I_ATTR(int, m_keepAliveTimeoutAttrPtr);
	I_ATTR(int, m_requestProcessingTimeoutAttrPtr);
	I_ATTR(int, m_writeHighWatermarkAttrPtr);
	I_ATTR(bool, m_startServerOnCreateAttrPtr);
	I_ATTR(bool, m_isMultiThreadingAttrPtr);


	istd::TDelPtr<CMultiThreadServer> m_serverPtr;

	typedef istd::TPointerVector<IRequest> Requests;
	Requests m_requests;

	bool m_isInitialized = false;
};


} // namespace imtrest


