#pragma once


// Qt includes
#include <QtWebSockets/QWebSocket>
#include <QtCore/QTimer>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <istd/TPointerVector.h>
#include <iprm/ITextParam.h>
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtbase/IUrlParam.h>
#include <imtcom/IConnectionController.h>
#include <imtcom/IConnectionStatusProvider.h>
#include <imtrest/IRequestServlet.h>
#include <imtrest/IProtocolEngine.h>
#include <imtrest/ISender.h>
#include <imtrest/IRequestManager.h>
#include <imtclientgql/IGqlClient.h>


namespace imtclientgql
{


class CWebSocketClientComp:
			public QObject,
			public ilog::CLoggerComponentBase,
			virtual public imtrest::ISender,
			virtual public imtcom::IConnectionController,
			virtual public imtrest::IRequestManager,
			virtual public imtclientgql::IGqlClient
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;

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
		I_ASSIGN(m_webSocketServerAddressCompPtr, "WebSocketServerAddress", "Parameter providing the websocket-server address to be connected", true, "WebSocketServerAddress");
		I_ASSIGN(m_serverLoginAttrPtr, "ServerLoginParam", "Parameter providing the server login to be connected", false, "");
		I_ASSIGN(m_serverPasswordAttrPtr, "ServerPasswordParam", "Parameter providing the server password to be connected", false, "");
		I_ASSIGN(m_clientIdAttrPtr, "ClientId", "ID of the client that needs to be identified on the server", false, "");
		I_ASSIGN(m_clientIdCompPtr, "ClientIdParam", "Parameter providing the client-ID that needs to be identified on the server", false, "ClientIdParam");
		I_ASSIGN(m_productId, "ProductId", "Product-ID used with corresponded grapgQl requests", false, "");
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

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

Q_SIGNALS:
	void EmitQueryDataReceived(int resultCode = 1);
	void EmitWebSocketClose(QWebSocketProtocol::CloseCode closeCode = QWebSocketProtocol::CloseCodeNormal,
							const QString &reason = QString());
	void EmitStopTimer();
	void EmitStartTimer();
	void EmitSendTextMessage(const QByteArray message) const;

private Q_SLOTS:
	void OnTimeout();
	void OnWebSocketConnected();
	void OnWebSocketDisconnected();
	void OnWebSocketError(QAbstractSocket::SocketError error);
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
	I_REF(imtbase::IUrlParam, m_webSocketServerAddressCompPtr);
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

	imod::TModelWrap<ConnectionStatusProvider> m_connectionStatusProvider;
};


} // namespace imtclientgql


