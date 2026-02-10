// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtWebSockets/QWebSocket>
#include <QSslError>
#include <QtCore/QUrl>


namespace imtqml
{


class CQmlWebSocket : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)
	Q_PROPERTY(bool active READ isActive WRITE setActive NOTIFY activeChanged)
	Q_PROPERTY(QString errorString READ errorString NOTIFY errorStringChanged)
	Q_PROPERTY(QString negotiatedSubprotocol READ negotiatedSubprotocol NOTIFY negotiatedSubprotocolChanged)
	Q_PROPERTY(QStringList requestedSubprotocols READ requestedSubprotocols WRITE setRequestedSubprotocols NOTIFY requestedSubprotocolsChanged)
	Q_PROPERTY(Status status READ status NOTIFY statusChanged)

public:
	enum Status 
	{
		Connecting,
		Open,
		Closing,
		Closed,
		Error
	};
	Q_ENUM(Status)
	
	explicit CQmlWebSocket(QObject* parent = nullptr);

	QUrl url() const;
	void setUrl(const QUrl& url);

	bool isActive() const;
	void setActive(bool isActive);

	QString errorString() const;
	QString negotiatedSubprotocol() const;
	QStringList requestedSubprotocols() const;
	void setRequestedSubprotocols(const QStringList& protocols);

	Status status() const;

	Q_INVOKABLE void sendTextMessage(const QString& message);
	Q_INVOKABLE void sendBinaryMessage(const QByteArray& message);

signals:
	void binaryMessageReceived(const QString& message);
	void textMessageReceived(const QString& message);
	void statusChanged(Status status);
	void activeChanged();
	void errorStringChanged();
	void negotiatedSubprotocolChanged();
	void requestedSubprotocolsChanged();
	void urlChanged();
	void sslErrors(const QList<QSslError>& errors);

private slots:
	void onConnected();
	void onDisconnected();
	void onError(QAbstractSocket::SocketError error);
	void onSslErrors(const QList<QSslError>& errors);
	
private:
	void updateStatus();

	QWebSocket m_socket;
	QUrl m_url;
	QStringList m_requestedSubprotocols;
	Status m_status = Closed;
};


} // namespace imtqml


