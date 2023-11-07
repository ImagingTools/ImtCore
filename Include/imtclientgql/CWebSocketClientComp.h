#pragma once


// Qt includes
#include <QWebSocket>
#include <QtCore/QTimer>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/ITextParam.h>

// ImtCore includes
#include <imtauth/ILoginStatusProvider.h>
#include <imtrest/IRequestServlet.h>
#include <imtrest/IProtocolEngine.h>
#include <imtrest/ISender.h>
#include <imtrest/IRequestManager.h>
#include <imtgql/IGqlRequest.h>
#include <imtgql/IGqlResponseHandler.h>
#include <imtclientgql/IGqlClient.h>



namespace imtclientgql
{


class CWebSocketClientComp:
			public QObject,
			public ilog::CLoggerComponentBase,
			virtual public imtrest::ISender,
			virtual public imtauth::ILoginStatusProvider,
			virtual public imtrest::IRequestManager,
			virtual public imtclientgql::IGqlClient
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CWebSocketClientComp);
		I_REGISTER_INTERFACE(ISender)
		I_REGISTER_INTERFACE(IRequestManager)
		I_REGISTER_INTERFACE(imtauth::ILoginStatusProvider)
		I_REGISTER_INTERFACE(imtclientgql::IGqlClient)
		I_ASSIGN(m_serverRequestHandlerCompPtr, "ServerRequestHandler", "Request handler registered for the server", true, "ServerRequestHandler");
		I_ASSIGN(m_clientRequestHandlerCompPtr, "ClientRequestHandler", "Request handler registered for the client", true, "ClientRequestHandler");
		I_ASSIGN(m_protocolEngineCompPtr, "ProtocolEngine", "Protocol engine used in the server", true, "ProtocolEngine");
		I_ASSIGN(m_serverAddressAttrPtr, "ServerAddress", "Server address to be listened", false, "ServerAddress");
		I_ASSIGN(m_serverPortAttrPtr, "ServerPort", "Server port to be listened", false, 0);
		I_ASSIGN(m_connectOnCreateAttrPtr, "ConnectOnCreate", "If enabled, the client will be connect to server on after component creation", true, true);
		I_ASSIGN(m_webSocketServerPortCompPtr, "ServerPortParam", "Parameter providing the server port to be connected", false, "ServerPortParam");
		I_ASSIGN(m_webSocketServerAddressCompPtr, "ServerAddressParam", "Parameter providing the server address to be connected", false, "ServerAddressParam");
		I_ASSIGN(m_serverLoginAttrPtr, "ServerLoginParam", "Parameter providing the server login to be connected", false, "ServerLoginParam");
		I_ASSIGN(m_serverPasswordAttrPtr, "ServerPasswordParam", "Parameter providing the server password to be connected", false, "ServerPasswordParam");
	I_END_COMPONENT;

	CWebSocketClientComp();

	// reimplemented (imtclientgql::IGqlClient)
	virtual bool SendRequest(const imtgql::IGqlRequest& request, imtgql::IGqlResponseHandler& responseHandler) const override;

	// reimplemented (imtrest::ISender)
	virtual bool SendResponse(imtrest::ConstResponsePtr& response) const override;
	virtual bool SendRequest(imtrest::ConstRequestPtr& request) const override;

	// reimplemented (imtauth::ILoginStatusProvider)
	virtual int GetLoginStatus() const override;

	// reimplemented (imtrest::IRequestManager)
	virtual const imtrest::IRequest* GetRequest(const QByteArray& requestId) const override;
	virtual const imtrest::ISender* GetSender(const QByteArray& requestId) const override;

protected:
	QByteArray Sign(const QByteArray& message, const QByteArray& key = "") const;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

Q_SIGNALS:
	void OnQueryDataReceived(int resultCode = 2);

private Q_SLOTS:
	void OnConnectedTimer();
	void OnWebSocketConnected();
	void OnWebSocketDisConnected();
	void OnWebSocketError(QAbstractSocket::SocketError error);
	void OnWebSocketTextMessageReceived(const QString& message);
	void OnWebSocketBinaryMessageReceived(const QByteArray& message);


private:
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

	virtual void Connect();

private:
	I_ATTR(bool, m_connectOnCreateAttrPtr);
	I_REF(imtrest::IRequestServlet, m_serverRequestHandlerCompPtr);
	I_REF(imtrest::IRequestServlet, m_clientRequestHandlerCompPtr);
	I_REF(imtrest::IProtocolEngine, m_protocolEngineCompPtr);
	I_ATTR(QByteArray, m_serverAddressAttrPtr);
	I_ATTR(int, m_serverPortAttrPtr);
	I_ATTR(QByteArray, m_serverLoginAttrPtr);
	I_ATTR(QByteArray, m_serverPasswordAttrPtr);
	I_REF(iprm::ITextParam, m_webSocketServerPortCompPtr);
	I_REF(iprm::ITextParam, m_webSocketServerAddressCompPtr);
	I_REF(iprm::ITextParam, m_webSocketServerLoginCompPtr);
	I_REF(iprm::ITextParam, m_webSocketServerPasswordCompPtr);

	mutable QWebSocket m_webSocket;
	imtauth::ILoginStatusProvider::LoginStatusFlags m_loginStatus;
	QTimer m_refreshTimer;
	mutable QMap<QString, QByteArray> m_queryDataMap;
};


} // namespace imtclientgql


