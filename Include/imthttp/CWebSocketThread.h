#pragma once


// Qt includes
#include <QtCore/QThread>
#include <QtCore/QMutex>

// ImtCore includes
#include <imthttp/IRequestServlet.h>
#include <imthttp/ISender.h>

// Qt includes
#include <QtWebSockets/QWebSocket>
#include <QtCore/QPointer>


namespace imthttp
{


class CWebSocketServerComp;
class CWebSocketThread;


class CWebSocket: public QObject
{
	Q_OBJECT
public:
	CWebSocket(CWebSocketThread *parent);

public Q_SLOTS:
	void OnWebSocketTextMessage(const QString& textMessage);

private:
	CWebSocketThread* m_parent;
};


class CWebSocketThread:
			public QThread
{
	Q_OBJECT
public:
	enum Status
	{
		ST_START,
		ST_PROCESS,
		ST_CLOSE
	};

	explicit CWebSocketThread(imthttp::CWebSocketServerComp *parent);
	void SetWebSocket(QWebSocket* webSocketPtr);
	void SetSocketStatus(Status socketStatus);
	Status GetSocketStatus();
	QByteArray GetRequestId();
	// imthttp::IRequestServlet* GetRequestServlet();

	[[nodiscard]] bool IsSecureConnection() const;
	void EnableSecureConnection(bool isSecureConnection = true);

	// reimplemented (QThread)
	virtual void run() override;

public Q_SLOTS:
	void OnWebSocketTextMessage(const QString& textMessage);

private Q_SLOTS:
	void OnSocketDisconnected();
	void OnWebSocketBinaryMessage(const QByteArray& dataMessage);
	void OnError(QAbstractSocket::SocketError error);
	void OnTimeout();
	void OnAcceptError(QAbstractSocket::SocketError socketError);
	void OnSslErrors(const QList<QSslError> &errors);
	void OnSendTextMessage(const QByteArray& data) const;
Q_SIGNALS:
	void SendTextMessage(const QByteArray& data) const;

private:
	CWebSocketServerComp* m_server;
	imthttp::IProtocolEngine* m_enginePtr;
	mutable QMutex m_socketDescriptorMutex;
	mutable QMutex m_statusMutex;
	Status m_status;
	QPointer<QWebSocket> m_socket;
	bool m_isSecureConnection;

	imthttp::IProtocolEngine* m_httpEnginePtr;
	imthttp::IRequestServlet* m_requestServerHandlerPtr;
	imthttp::IRequestServlet* m_requestClientHandlerPtr;
	QByteArray m_productId;

	QByteArray m_requestId;
};


} // namespace imthttp


