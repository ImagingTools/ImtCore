// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtWebSockets/QWebSocket>
#include <QtCore/QTimer>
#include <QtCore/QReadWriteLock>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <istd/TPointerVector.h>
#include <iprm/ITextParam.h>
#include <imod/TModelWrap.h>
#include <ibase/TRuntimeStatusHanderCompWrap.h>
#include <iprm/IParamsSet.h>
#include <iprm/IOptionsList.h>

// ImtCore includes
#include <imtcom/IServerConnectionInterface.h>
#include <imtcom/IConnectionController.h>
#include <imtcom/IConnectionStatusProvider.h>
#include <imtrest/IRequestServlet.h>
#include <imtrest/IProtocolEngine.h>
#include <imtrest/ISender.h>
#include <imtrest/IRequestManager.h>
#include <imtclientgql/IGqlClient.h>
#include <imtcom/ISslConfigurationManager.h>


namespace imtclientgql
{


class CWebSocketClientCompBase:public ibase::TRuntimeStatusHanderCompWrap <ilog::CLoggerComponentBase>
{
public:
	typedef ibase::TRuntimeStatusHanderCompWrap <ilog::CLoggerComponentBase> BaseClass;
	
	I_BEGIN_COMPONENT(CWebSocketClientCompBase);
		I_ASSIGN(m_sslConfigurationCompPtr, "SslConfiguration", "SSL Configuration is used by networking classes to relay information about an open SSL connection and to allow the server to control certain features of that connection.", false, "SslConfiguration")
		I_ASSIGN_TO(m_sslConfigurationModelCompPtr, m_sslConfigurationCompPtr, false)
		I_ASSIGN(m_sslConfigurationManagerCompPtr, "SslConfigurationManager", "SSL configuration manager, used to create an SSL configuration for server", false, "SslConfigurationManager")
		I_ASSIGN(m_subprotocolListCompPtr, "Subprotocols", "Web Socket subprotocols", false, "Subprotocols");
	I_END_COMPONENT;
		
protected:
	I_REF(iprm::IParamsSet, m_sslConfigurationCompPtr);
	I_REF(imod::IModel, m_sslConfigurationModelCompPtr);
	I_REF(imtcom::ISslConfigurationManager, m_sslConfigurationManagerCompPtr);
	I_REF(iprm::IOptionsList, m_subprotocolListCompPtr);
};

class CWebSocketClientComp:
			public QObject,
			public CWebSocketClientCompBase,
			virtual public imtrest::ISender,
			virtual public imtcom::IConnectionController,
			virtual public imtrest::IRequestManager,
			virtual public IGqlClient,
			private imod::CMultiModelDispatcherBase
{
	Q_OBJECT
public:
	typedef CWebSocketClientCompBase BaseClass;
	typedef imod::CMultiModelDispatcherBase BaseClass2;

	I_BEGIN_COMPONENT(CWebSocketClientComp);
		I_REGISTER_INTERFACE(ISender)
		I_REGISTER_INTERFACE(IRequestManager)
		I_REGISTER_INTERFACE(imtcom::IConnectionController)
		I_REGISTER_INTERFACE(imtclientgql::IGqlClient)
		I_REGISTER_SUBELEMENT(ConnectionStatusProvider);
		I_REGISTER_SUBELEMENT_INTERFACE_T(ConnectionStatusProvider, imtcom::IConnectionStatusProvider, ExtractConnectionStatusProvider);
		I_REGISTER_SUBELEMENT_INTERFACE_T(ConnectionStatusProvider, istd::IChangeable, ExtractConnectionStatusProvider);
		I_REGISTER_SUBELEMENT_INTERFACE_T(ConnectionStatusProvider, imod::IModel, ExtractConnectionStatusProvider);
		I_ASSIGN(m_serverRequestHandlerCompPtr, "ServerRequestHandler", "Request handler registered for the server", false, "ServerRequestHandler");
		I_ASSIGN(m_clientRequestHandlerCompPtr, "ClientRequestHandler", "Request handler registered for the client", false, "ClientRequestHandler");
		I_ASSIGN(m_httpProtocolEngineCompPtr, "HttpProtocolEngine", "Http Protocol engine used in the http client", false, "ProtocolEngine");
		I_ASSIGN(m_protocolEngineCompPtr, "ProtocolEngine", "Protocol engine used in the client", true, "ProtocolEngine");
		I_ASSIGN(m_connectOnCreateAttrPtr, "ConnectOnCreate", "If enabled, the client will be connect to server on after component creation", true, true);
		I_ASSIGN(m_serverConnectionCompPtr, "ServerConnectionInterface", "Parameter of the server communication interface", true, "ServerConnectionInterface");
		I_ASSIGN(m_serverLoginAttrPtr, "ServerLoginParam", "Parameter providing the server login to be connected", false, "");
		I_ASSIGN(m_serverPasswordAttrPtr, "ServerPasswordParam", "Parameter providing the server password to be connected", false, "");
		I_ASSIGN(m_clientIdAttrPtr, "ClientId", "ID of the client that needs to be identified on the server", false, "");
		I_ASSIGN(m_clientIdCompPtr, "ClientIdParam", "Parameter providing the client-ID that needs to be identified on the server", false, "ClientIdParam");
		I_ASSIGN(m_productId, "ProductId", "Product-ID used with corresponded GraphQl requests", false, "");
	I_END_COMPONENT;

	CWebSocketClientComp();

	// reimplemented (imtclientgql::IGqlClient)
	virtual GqlResponsePtr SendRequest(GqlRequestPtr requestPtr, imtbase::IUrlParam* urlParamPtr = nullptr) const override;

	// reimplemented (imtrest::ISender)
	virtual bool SendResponse(imtrest::ConstResponsePtr& response) const override;
	virtual bool SendRequest(imtrest::ConstRequestPtr& request) const override;

	// reimplemented (imtrest::IRequestManager)
	virtual const imtrest::ISender* GetSender(const QByteArray& requestId) const override;

	// reimplemented (imtcom::IConnectionController)
	virtual bool Connect() override;
	virtual bool Disconnect() override;

protected:
	QByteArray Sign(const QByteArray& message, const QByteArray& key = "") const;

	// reimplemented (imod::CMultiModelDispatcherBase)
	virtual void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (ibase::TRuntimeStatusHanderCompWrap)
	virtual void OnSystemShutdown() override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

Q_SIGNALS:
	void EmitQueryDataReceived(int resultCode = 1);
	void EmitWebSocketClose(
				QWebSocketProtocol::CloseCode closeCode = QWebSocketProtocol::CloseCodeNormal,
				const QString &reason = QString());
	void EmitStopTimer();
	void EmitStartTimer();
	void EmitSendTextMessage(const QByteArray message) const;

private Q_SLOTS:
	void OnTimeout();
	void OnWebSocketConnected();
	void OnWebSocketDisconnected();
	void OnWebSocketError(QAbstractSocket::SocketError error);
	void OnSslErrors(const QList<QSslError>& sslErrors);
	void OnWebSocketTextMessageReceived(const QString& message);
	void OnWebSocketBinaryMessageReceived(const QByteArray& message);
	void OnSendTextMessage(const QByteArray& message);

protected:
	virtual void EnsureWebSocketConnection();

	class NetworkOperation
	{
	public:
		NetworkOperation() = delete;
		NetworkOperation(int timeout, const CWebSocketClientComp* parent);
		~NetworkOperation();

		QEventLoop connectionLoop;
		bool timerFlag;
		QTimer timer;
	};

	class ConnectionStatusProvider:
				virtual public imtcom::IConnectionStatusProvider
	{
	public:
		ConnectionStatusProvider();
		virtual void SetConnectionStatus(ConnectionStatus status);

		// reimplemented (imtcom::IConnectionStatusProvider)
		virtual ConnectionStatus GetConnectionStatus() const override;

	private:
		ConnectionStatus m_connectionStatus;
	};

	template <typename InterfaceType>
	static InterfaceType* ExtractConnectionStatusProvider(CWebSocketClientComp& component)
	{
		return &component.m_connectionStatusProvider;
	}

private:
	I_REF(imtrest::IRequestServlet, m_serverRequestHandlerCompPtr);
	I_REF(imtrest::IRequestServlet, m_clientRequestHandlerCompPtr);
	I_REF(imtrest::IProtocolEngine, m_protocolEngineCompPtr);
	I_REF(imtrest::IProtocolEngine, m_httpProtocolEngineCompPtr);
	I_REF(imtcom::IServerConnectionInterface, m_serverConnectionCompPtr);
	I_REF(iprm::ITextParam, m_webSocketServerLoginCompPtr);
	I_REF(iprm::ITextParam, m_webSocketServerPasswordCompPtr);
	I_REF(iprm::ITextParam, m_clientIdCompPtr);
	I_ATTR(bool, m_connectOnCreateAttrPtr);
	I_ATTR(QByteArray, m_serverLoginAttrPtr);
	I_ATTR(QByteArray, m_serverPasswordAttrPtr);
	I_ATTR(QByteArray, m_clientIdAttrPtr);
	I_ATTR(QByteArray, m_productId);

	mutable QWebSocket m_webSocket;
	QTimer m_refreshTimer;
	mutable QMap<QString, QByteArray> m_queryDataMap;
	istd::TPointerVector<imtrest::IRequest> m_startQueries;
	istd::TPointerVector<imtrest::IRequest> m_cacheQueries;
	QAbstractSocket::SocketError m_lastSocketError;
	mutable QReadWriteLock m_queryDataMapLock;

	bool m_isInitialized = false;

	imod::TModelWrap<ConnectionStatusProvider> m_connectionStatusProvider;
};


} // namespace imtclientgql


