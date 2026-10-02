// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CWebSocketSender.h>


// Qt includes
#include <QtWebSockets/QWebSocket>


namespace imtrest
{


// public methods

CWebSocketSender::CWebSocketSender(QWebSocket* webSocketPtr): m_webSocketPtr(webSocketPtr)
{
	QObject::connect(this, &CWebSocketSender::SendTextMessage, this, &CWebSocketSender::OnSendTextMessage, Qt::ConnectionType::QueuedConnection);
}


QPointer<QWebSocket> CWebSocketSender::GetSocket() const
{
	return m_webSocketPtr;
}


bool CWebSocketSender::IsSocketValid() const
{
	return !m_webSocketPtr.isNull() && m_webSocketPtr->isValid();
}

// reimplemented (ITransport)

bool CWebSocketSender::SendData(QByteArray& data) const
{
	if (!IsSocketValid()){
		return false;
	}

	emit SendTextMessage(data);

	return true;
}


void CWebSocketSender::OnSendTextMessage(const QByteArray& data) const
{
	if (IsSocketValid()){
		m_webSocketPtr->sendTextMessage(data);
	}
}


} // namespace imtrest
