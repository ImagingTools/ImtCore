#pragma once


// Qt includes
#include <QtWebSockets/QWebSocketServer>
#include <QtWebSockets/QWebSocket>
#include <QtCore/QSharedPointer>

// ACF includes
#include <istd/TPointerVector.h>
#include <ilog/TLoggerCompWrap.h>
#include <iprm/ITextParam.h>

// ImtCore includes
#include <imtrest/IRequest.h>
#include <imtrest/IRequestServlet.h>
#include <imtrest/IProtocolEngine.h>
#include <imtrest/ISuscriberEngine.h>
#include <imtrest/IRequestManager.h>
#include <imtrest/CWebSocketSender.h>


namespace imtrest
{


/**
	TCP-based communication server.
	The server uses the underlaying protocol engine for creation of requests and responses.
*/
class CWebSocketServerComp:
			public QObject,
			public ilog::CLoggerComponentBase,
			virtual public IRequestManager
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CWebSocketServerComp);
		I_REGISTER_INTERFACE(IRequestManager)
		I_ASSIGN(m_requestHandlerCompPtr, "RequestHandler", "Request handler registered for the server", true, "RequestHandler");
		I_ASSIGN(m_protocolEngineCompPtr, "ProtocolEngine", "Protocol engine used in the server", true, "ProtocolEngine");
		I_ASSIGN(m_subscriberEngineCompPtr, "SubscriberEngine", "Subscriber engine used in the server", false, "SubscriberEngine");
		I_ASSIGN(m_serverAddressAttrPtr, "ServerAddress", "Server address to be listened", false, "ServerAddress");
		I_ASSIGN(m_serverPortAttrPtr, "ServerPort", "Server port to be listened", false, 0);
		I_ASSIGN(m_startServerOnCreateAttrPtr, "StartServerOnCreate", "If enabled, the server will be started on after component creation", true, true);
		I_ASSIGN(m_webSocketServerPortCompPtr, "ServerPortParam", "Parameter providing the server port to be listened", false, "ServerPortParam");
	I_END_COMPONENT

	// reimplemented (icomp::IRequestManager)
	virtual const IRequest* GetRequest(const QByteArray& requestId) const override;
	virtual const ISender* GetSender(const QByteArray& requestId) const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	bool StartListening(const QHostAddress& address = QHostAddress::Any, quint16 port = 0);

private Q_SLOTS:
	void HandleNewConnections();
	void OnSocketDisconnected();
	void OnWebSocketTextMessage(const QString& textMessage);
	void OnWebSocketBinaryMessage(const QByteArray& dataMessage);

private:
	I_REF(imtrest::IRequestServlet, m_requestHandlerCompPtr);
	I_REF(IProtocolEngine, m_protocolEngineCompPtr);
	I_REF(ISubscriberEngine, m_subscriberEngineCompPtr);
	I_ATTR(QByteArray, m_serverAddressAttrPtr);
	I_ATTR(int, m_serverPortAttrPtr);
	I_ATTR(bool, m_startServerOnCreateAttrPtr);
	I_REF(iprm::ITextParam, m_webSocketServerPortCompPtr);

	typedef QVector<QWebSocketServer*> Servers;

	Servers m_servers;

	//	typedef istd::TPointerVector<IRequest> Requests;
	//	Requests m_requests;
		QMap <QByteArray, QSharedPointer<CWebSocketSender>> m_senders;
};


} // namespace imtrest


