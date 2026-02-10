// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CQmlWebSocket.h>


namespace imtqml
{


CQmlWebSocket::CQmlWebSocket(QObject* parent)
	: QObject(parent)
{
	connect(&m_socket,& QWebSocket::connected, this,& CQmlWebSocket::onConnected);
	connect(&m_socket,& QWebSocket::disconnected, this,& CQmlWebSocket::onDisconnected);
	connect(&m_socket,& QWebSocket::textMessageReceived, this,& CQmlWebSocket::textMessageReceived);
	connect(&m_socket,& QWebSocket::binaryMessageReceived, this,& CQmlWebSocket::binaryMessageReceived);

#if (QT_VERSION >= QT_VERSION_CHECK(6, 5, 0))
	connect(&m_socket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::errorOccurred),
			this,& CQmlWebSocket::onError);
#else
	connect(&m_socket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), this, &CQmlWebSocket::onError);
#endif
	connect(&m_socket, &QWebSocket::sslErrors, this, &CQmlWebSocket::onSslErrors);
}


QUrl CQmlWebSocket::url() const
{
	return m_url;
}


void CQmlWebSocket::setUrl(const QUrl& url)
{
	if (m_url == url){
		return;
	}

	m_url = url;
	emit urlChanged();

	m_socket.open(m_url);
	updateStatus();

	emit activeChanged();
}


bool CQmlWebSocket::isActive() const
{
	return m_socket.state() == QAbstractSocket::ConnectedState;
}


void CQmlWebSocket::setActive(bool isActive)
{
	if (!isActive){
		if (m_socket.state() != QAbstractSocket::UnconnectedState) {
			m_socket.abort();
		}
	}

	updateStatus();
	
	if (isActive){
		m_socket.open(m_url);
	}
}


QString CQmlWebSocket::errorString() const
{
	return m_socket.errorString();
}


QString CQmlWebSocket::negotiatedSubprotocol() const
{
	return QString();
}


QStringList CQmlWebSocket::requestedSubprotocols() const
{
	return m_requestedSubprotocols;
}


void CQmlWebSocket::setRequestedSubprotocols(const QStringList& /*protocols*/)
{
}


CQmlWebSocket::Status CQmlWebSocket::status() const
{
	return m_status;
}


void CQmlWebSocket::sendTextMessage(const QString& message)
{
	m_socket.sendTextMessage(message);
}


void CQmlWebSocket::sendBinaryMessage(const QByteArray& message)
{
	m_socket.sendBinaryMessage(message);
}


void CQmlWebSocket::onConnected()
{
	m_status = Open;
	emit statusChanged(m_status);
	emit activeChanged();
	emit negotiatedSubprotocolChanged();
}


void CQmlWebSocket::onDisconnected()
{
	m_status = Closed;
	emit statusChanged(m_status);
	emit activeChanged();
}


void CQmlWebSocket::onError(QAbstractSocket::SocketError)
{
	m_status = Error;
	emit statusChanged(m_status);
	emit errorStringChanged();
}


void CQmlWebSocket::onSslErrors(const QList<QSslError>& errors)
{
	m_socket.ignoreSslErrors();
	emit sslErrors(errors);
}


void CQmlWebSocket::updateStatus()
{
	switch (m_socket.state()){
	case QAbstractSocket::ConnectingState:
		m_status = Connecting;
		break;
	case QAbstractSocket::ConnectedState:
		m_status = Open;
		break;
	case QAbstractSocket::ClosingState:
		m_status = Closing;
		break;
	case QAbstractSocket::UnconnectedState:
		m_status = Closed;
		break;
	default:
		m_status = Error;
		break;
	}
	emit statusChanged(m_status);
}


} // namespace imtqml


