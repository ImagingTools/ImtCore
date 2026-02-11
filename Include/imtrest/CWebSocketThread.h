// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QThread>
#include <QtCore/QMutex>

// ImtCore includes
#include <imtrest/IRequestServlet.h>
#include <imtrest/ISender.h>

// Qt includes
#include <QtWebSockets/QWebSocket>
#include <QtCore/QPointer>


namespace imtrest
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

	explicit CWebSocketThread(imtrest::CWebSocketServerComp *parent);
	void SetWebSocket(QWebSocket* webSocketPtr);
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
	imtrest::IProtocolEngine* m_enginePtr;
	mutable QMutex m_socketDescriptorMutex;
	mutable QMutex m_statusMutex;
	Status m_status;
	QPointer<QWebSocket> m_socket;
	bool m_isSecureConnection;

	imtrest::IProtocolEngine* m_httpEnginePtr;
	imtrest::IRequestServlet* m_requestServerHandlerPtr;
	imtrest::IRequestServlet* m_requestClientHandlerPtr;
	QByteArray m_productId;

	QByteArray m_requestId;
};


} // namespace imtrest


