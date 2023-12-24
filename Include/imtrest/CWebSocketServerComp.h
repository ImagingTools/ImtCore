#pragma once


// Qt includes
#include <QtWebSockets/QWebSocketServer>
#include <QtWebSockets/QWebSocket>
#include <QtCore/QSharedPointer>

// ACF includes
#include <istd/TPointerVector.h>
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/IUrlParam.h>
#include <imtrest/IRequest.h>
#include <imtrest/IRequestServlet.h>
#include <imtrest/IProtocolEngine.h>
#include <imtrest/ISuscriberEngine.h>
#include <imtrest/IRequestManager.h>
#include <imtrest/CWebSocketSender.h>
#include <imtauth/ILoginStatusProvider.h>


namespace imtrest
{


/**
	TCP-based communication server.
	The server uses the underlaying protocol engine for creation of requests and responses.
*/
class CWebSocketServerComp:
			public QObject,
			public ilog::CLoggerComponentBase,
			virtual public IRequestManager,
			virtual public imtauth::ILoginStatusProvider,
			virtual public istd::IChangeable
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CWebSocketServerComp);
		I_REGISTER_INTERFACE(IRequestManager)
		I_REGISTER_INTERFACE(imtauth::ILoginStatusProvider)
		I_ASSIGN(m_requestServerHandlerCompPtr, "RequestServerHandler", "Request handler registered for the server", true, "RequestServerHandler");
		I_ASSIGN(m_requestClientHandlerCompPtr, "RequestClientHandler", "Request handler registered for the client", true, "RequestClientHandler");
		I_ASSIGN(m_protocolEngineCompPtr, "ProtocolEngine", "Protocol engine used in the server", true, "ProtocolEngine");
		I_ASSIGN(m_subscriberEngineCompPtr, "SubscriberEngine", "Subscriber engine used in the server", false, "SubscriberEngine");
		I_ASSIGN(m_startServerOnCreateAttrPtr, "StartServerOnCreate", "If enabled, the server will be started on after component creation", true, true);
		I_ASSIGN(m_webSocketServerPortCompPtr, "WebSocketServerPort", "Parameter providing the WebSocket-server port to be listened", false, "WebSocketServerPort");
	I_END_COMPONENT

	// reimplemented (icomp::IRequestManager)
	virtual const IRequest* GetRequest(const QByteArray& requestId) const override;
	virtual const ISender* GetSender(const QByteArray& requestId) const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

	// reimplemented (imtauth::ILoginStatusProvider)
	virtual int GetLoginStatus(const QByteArray& clientId = QByteArray()) const override;

private:
	bool StartListening(const QHostAddress& address = QHostAddress::Any, quint16 port = 0);

private Q_SLOTS:
	void HandleNewConnections();
	void OnSocketDisconnected();
	void OnWebSocketTextMessage(const QString& textMessage);
	void OnWebSocketBinaryMessage(const QByteArray& dataMessage);
	void OnError(QAbstractSocket::SocketError error);
	void OnTimeout();

protected:
	QTimer m_timer;
	typedef QVector<QWebSocketServer*> Servers;
	Servers m_servers;
	QMap <QByteArray, QSharedPointer<CWebSocketSender>> m_senders;
	QMap <QByteArray, imtauth::ILoginStatusProvider::LoginStatusFlags> m_senderLoginStatusMap;

private:
	I_REF(imtrest::IRequestServlet, m_requestServerHandlerCompPtr);
	I_REF(imtrest::IRequestServlet, m_requestClientHandlerCompPtr);
	I_REF(IProtocolEngine, m_protocolEngineCompPtr);
	I_REF(ISubscriberEngine, m_subscriberEngineCompPtr);
	I_ATTR(bool, m_startServerOnCreateAttrPtr);
	I_REF(imtbase::IUrlParam, m_webSocketServerPortCompPtr);
};


} // namespace imtrest


