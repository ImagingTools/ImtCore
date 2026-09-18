// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QStringList>

// ImtCore includes
#include <imtrest/IRequestServlet.h>
#include <imtrest/ITransport.h>
#include <imtrest/CWebSocketRequest.h>

// Qt includes
#include <QtWebSockets/QWebSocket>
#include <QtCore/QPointer>


namespace imtrest
{


class CWebSocketServerComp;


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

	explicit CWebSocketThread(imtrest::CWebSocketServerComp *parent);
	void SetWebSocket(QWebSocket* webSocketPtr);
	const QWebSocket* GetWebSocket() const;
	void SetSocketStatus(Status socketStatus);
	Status GetSocketStatus();
	QByteArray GetRequestId();
	// imtrest::IRequestServlet* GetRequestServlet();

	[[nodiscard]] bool IsSecureConnection() const;
	void EnableSecureConnection(bool isSecureConnection = true);

	// reimplemented (QThread)
	virtual void run() override;

public Q_SLOTS:
	void OnWebSocketTextMessage(const QString& textMessage);
	void OnSocketDisconnected();

private Q_SLOTS:
	void OnWebSocketBinaryMessage(const QByteArray& dataMessage);
	void OnError(QAbstractSocket::SocketError error);
	void OnTimeout();
	void OnAcceptError(QAbstractSocket::SocketError socketError);
	void OnSslErrors(const QList<QSslError> &errors);
	void OnSendTextMessage(const QByteArray& data) const;
Q_SIGNALS:
	void SendTextMessage(const QByteArray& data) const;
	void TextMessageReceived(const QString& msg);
	void SocketDisconnected();
	void SocketError(QAbstractSocket::SocketError error);

private:
	QPointer<QWebSocket> GetValidWebSocket() const;

	// Processes a single received text message. Must only be called from the
	// drain loop in OnWebSocketTextMessage() so that nested event loops
	// (e.g. synchronous auth I/O) cannot recurse back into message processing.
	void ProcessTextMessage(const QString& textMessage);

	CWebSocketServerComp* m_server;
	imtrest::IProtocolEngine* m_enginePtr;
	mutable QMutex m_socketDescriptorMutex;
	mutable QMutex m_statusMutex;
	Status m_status;
	QPointer<QWebSocket> m_socket;
	bool m_isSecureConnection;
	QList<CWebSocketRequest*> m_requestList;

	imtrest::IProtocolEngine* m_httpEnginePtr;
	imtrest::IRequestServlet* m_requestServerHandlerPtr;
	imtrest::IRequestServlet* m_requestClientHandlerPtr;
	QByteArray m_productId;

	QByteArray m_requestId;
	QObject m_receiver;

	// Re-entrancy guard for OnWebSocketTextMessage(). A message handler may
	// block in a nested QEventLoop (synchronous JWT validation via DoSyncPost),
	// during which further queued TextMessageReceived events would otherwise
	// recurse back into OnWebSocketTextMessage and eventually overflow the
	// stack. While a message is being processed, incoming messages are queued
	// here and drained sequentially by the active handler instead.
	bool m_isProcessingMessage = false;
	QStringList m_pendingMessages;
};


} // namespace imtrest

