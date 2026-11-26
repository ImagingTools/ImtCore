// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtNetwork/QSslConfiguration>
#include <QtWebSockets/QWebSocket>
#include <QtCore/QSharedPointer>

// STL includes
#include <atomic>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <ibase/TRuntimeStatusHanderCompWrap.h>
#include <iprm/IParamsSet.h>
#include <iprm/IOptionsList.h>
#include <imod/CMultiModelDispatcherBase.h>

// ImtCore includes
#include <imtrest/IRequestServlet.h>
#include <imtrest/IProtocolEngine.h>
#include <imtrest/ISubscriberEngine.h>
#include <imtrest/IResponseDispatcher.h>
#include <imtrest/CWebSocketSender.h>
#include <imtrest/IServer.h>
#include <imtcom/IServerConnectionInterface.h>
#include <imtcom/IConnectionStatusProvider.h>
#include <imtcom/ISslConfigurationManager.h>


namespace imtrest
{


class CWebSocketThread;


/**
	WebSocket-based communication server.
	The server uses the underlaying protocol engine for creation of requests and responses.
*/
class CWebSocketServerComp:
			public QObject,
			public ibase::TRuntimeStatusHanderCompWrap<ilog::CLoggerComponentBase>,
			private imod::CMultiModelDispatcherBase,
			virtual public IResponseDispatcher,
			virtual public imtcom::IConnectionStatusProvider,
			virtual public IServer
{
	Q_OBJECT
public:
	typedef ibase::TRuntimeStatusHanderCompWrap<ilog::CLoggerComponentBase> BaseClass;
	typedef imod::CMultiModelDispatcherBase BaseClass2;

	I_BEGIN_COMPONENT(CWebSocketServerComp);
		I_REGISTER_INTERFACE(IResponseDispatcher)
		I_REGISTER_INTERFACE(imtcom::IConnectionStatusProvider)
		I_REGISTER_INTERFACE(IServer)
		I_ASSIGN(m_requestServerHandlerCompPtr, "RequestServerHandler", "Request handler registered for the server", false, "RequestServerHandler");
		I_ASSIGN(m_requestClientHandlerCompPtr, "RequestClientHandler", "Request handler registered for the client", false, "RequestClientHandler");
		I_ASSIGN(m_protocolEngineCompPtr, "ProtocolEngine", "Protocol engine used in the server", true, "ProtocolEngine");
		I_ASSIGN(m_httpProtocolEngineCompPtr, "HttpProtocolEngine", "Http Protocol engine used in the server", false, "HttpProtocolEngine");
		I_ASSIGN(m_subscriberEngineCompPtr, "SubscriberEngine", "Subscriber engine used in the server", false, "SubscriberEngine");
		I_ASSIGN(m_startServerOnCreateAttrPtr, "StartServerOnCreate", "If enabled, the server will be started on after component creation", true, true);
		I_ASSIGN(m_webServerInterfaceCompPtr, "WebServerConnectionInterface", "Parameter providing the WebSocket-server port to be listened. A change of its WebSocket port moves the server to the new port", true, "WebServerConnectionInterface");
		I_ASSIGN_TO(m_webServerInterfaceModelCompPtr, m_webServerInterfaceCompPtr, false);
		I_ASSIGN(m_sslConfigurationCompPtr, "SslConfiguration", "SSL Configuration is used by networking classes to relay information about an open SSL connection and to allow the server to control certain features of that connection.", false, "SslConfiguration")
		I_ASSIGN_TO(m_sslConfigurationModelCompPtr, m_sslConfigurationCompPtr, false)
		I_ASSIGN(m_sslConfigurationManagerCompPtr, "SslConfigurationManager", "SSL configuration manager, used to create an SSL configuration for server", false, "SslConfigurationManager")
		I_ASSIGN(m_productId, "ProductId", "Product-ID used with corresponded GraphQL-requests", false, "");
		I_ASSIGN(m_subprotocolListCompPtr, "SupportedSubprotocols", "Supported Web Socket subprotocols", false, "SupportedSubprotocols");
	I_END_COMPONENT

	IProtocolEngine* GetProtocolEngine();
	IProtocolEngine* GetHttpProtocolEngine();
	imtrest::IRequestServlet* GetRequestServerServlet();
	imtrest::IRequestServlet* GetRequestClientServlet();
	QByteArray GetProductId();
	void SetConnectionStatus(const QByteArray& clientId);

	// reimplemented (ilog::CLoggerComponentBase)
	bool SendInfoMessage(
				int id,
				const QString& message,
				const QString& messageSource = QString(),
				int flags = 0) const;

	bool SendErrorMessage(
				int id,
				const QString& message,
				const QString& messageSource = QString(),
				int flags = 0) const;

	void SendVerboseMessage(const QString& message, const QString& messageSource = QString()) const;
	void RegisterSender(const QByteArray& clientId, QWebSocket* webSocketPtr);


	// reimplemented (imtrest::IResponseDispatcher)
	virtual bool SendResponse(const QByteArray& requestId, ConstResponsePtr& response) const override;
	virtual bool SendRequest(const QByteArray& requestId, ConstRequestPtr& request) const override;

protected:
	// reimplemented (imod::CMultiModelDispatcherBase)
	virtual void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (ibase::TRuntimeStatusHanderCompWrap)
	virtual void OnSystemShutdown() override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	// reimplemented (imtcom::IConnectionStatusProvider)
	virtual ConnectionStatus GetConnectionStatus() const override;

	// reimplemented (imtrest::IServer)
	virtual bool StartServer() override;
	virtual bool StopServer() override;
	virtual ServerStatus GetServerStatus() const override;

private:
	enum ModelId
	{
		MI_SSL_CONFIGURATION,
		MI_SERVER_CONNECTION
	};

	bool EnsureServerStarted();

	/**
		Listen with a new server object, secure as the SSL configuration asks. If it asks for the encryption and it cannot be made, the
		server does not listen and has no server object: it is not started without the encryption.
	*/
	bool StartListening(const QHostAddress& address = QHostAddress::Any, quint16 port = 0);

	void StopListening();

	/**
		Get the encryption that the SSL configuration asks for.
		\return \c false if it is asked for, but cannot be made.
	*/
	bool GetSecureConnection(bool& isSecure, QSslConfiguration& sslConfiguration) const;

	/**
		Close the connections and listen again: after a change of the SSL configuration the clients connect again with it. The connections
		are closed as a client closes them: their workers finish through the usual disconnection and are used again.
	*/
	bool RestartServer();

	/**
		Start the server again with the changed SSL configuration, if the server is to work. The same configuration (the settings are
		loaded again) changes nothing, the connections stay.
	*/
	void RestartForSslConfiguration();

	/**
		Listen on the port of the server connection interface, if the port is changed and the server is to work (started on creation or
		by StartServer and not stopped). The established connections stay. When the new port cannot be listened on, the server stays
		on the previous port. A port of the system (0) or of no protocol (-1) is not followed.
	*/
	void MoveToConfiguredPort();

	/**
		Stop the threads of the connections and wait for them: they use the component.
	*/
	void StopConnectionThreads();

	/**
		Listen on another port with the same server object: the sockets of the established connections belong to it.
	*/
	bool ListenAgain(quint16 port);

private Q_SLOTS:
	void HandleNewConnections();
	void OnSocketDisconnected();
	void OnTimeout();
	void OnAcceptError(QAbstractSocket::SocketError socketError);
	void OnSslErrors(const QList<QSslError> &errors);

protected:
	QTimer m_timer;
	istd::TDelPtr<QWebSocketServer> m_webSocketServerPtr;
	QList<CWebSocketThread*> m_webSocketThreadList;
	QMap <QByteArray, QSharedPointer<CWebSocketSender>> m_senders;
	QMap <QByteArray, imtcom::IConnectionStatusProvider::ConnectionStatus> m_senderLoginStatusMap;
	mutable QReadWriteLock m_sendersLock;
private:
	I_REF(imtrest::IRequestServlet, m_requestServerHandlerCompPtr);
	I_REF(imtrest::IRequestServlet, m_requestClientHandlerCompPtr);
	I_REF(IProtocolEngine, m_protocolEngineCompPtr);
	I_REF(ISubscriberEngine, m_subscriberEngineCompPtr);
	I_ATTR(bool, m_startServerOnCreateAttrPtr);
	I_REF(imtcom::IServerConnectionInterface, m_webServerInterfaceCompPtr);
	I_REF(imod::IModel, m_webServerInterfaceModelCompPtr);
	I_REF(iprm::IParamsSet, m_sslConfigurationCompPtr);
	I_REF(imod::IModel, m_sslConfigurationModelCompPtr);
	I_REF(imtcom::ISslConfigurationManager, m_sslConfigurationManagerCompPtr);
	I_REF(IProtocolEngine, m_httpProtocolEngineCompPtr);
	I_ATTR(QByteArray, m_productId);
	I_REF(iprm::IOptionsList, m_subprotocolListCompPtr);

	bool m_isInitialized = false;

	/**
		The server is started (on creation or by StartServer) and not stopped by StopServer: a new port is taken by it.
	*/
	std::atomic<bool> m_isStartRequested = false;

	/**
		The port of the server connection interface that the server has tried last, -1 before. A port that could not be listened on is
		not tried again on each notification, only a changed one.
	*/
	int m_requestedPort = -1;
};


} // namespace imtrest


