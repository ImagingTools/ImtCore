// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CWebSocketSender.h>


// Qt includes
#include <QtWebSockets/QWebSocket>

// ImtCore includes
#include <imtrest/IResponse.h>
#include <imtrest/IProtocolEngine.h>


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

// reimplemented (IRequest)

bool CWebSocketSender::SendResponse(ConstResponsePtr& response) const
{
	int protocolStatusCode = -1;
	QByteArray statusLiteral;

	bool retVal = response->GetProtocolEngine().GetProtocolStatusCode(response->GetStatusCode(), protocolStatusCode, statusLiteral);
	if (!retVal){
		return false;
	}

	if (IsSocketValid()){
		const QByteArray& contentData = response->GetData();

		emit SendTextMessage(contentData);

		return true;
	}

	return false;
}


bool CWebSocketSender::SendRequest(ConstRequestPtr& request) const
{
	if (IsSocketValid()){
		const QByteArray& contentData = request->GetBody();

		emit SendTextMessage(contentData);

		return true;
	}

	return false;

}


void CWebSocketSender::OnSendTextMessage(const QByteArray& data) const
{
	if (IsSocketValid()){
		m_webSocketPtr->sendTextMessage(data);
	}
}


} // namespace imtrest
