#include <imtrest/CWebSocketSender.h>


// Qt includes
#include <QtCore/QDataStream>
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


const QWebSocket* CWebSocketSender::GetSocket() const
{
	return m_webSocketPtr;
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

	if (m_webSocketPtr != nullptr){
		if (!m_webSocketPtr->isValid()){
			return false;
		}

		const QByteArray& contentData = response->GetData();

		emit SendTextMessage(contentData);

		return true;
	}

	return false;
}


bool CWebSocketSender::SendRequest(ConstRequestPtr& reguest) const
{
	if (m_webSocketPtr != nullptr){
		if (!m_webSocketPtr->isValid()){
			return false;
		}

		const QByteArray& contentData = reguest->GetBody();

		emit SendTextMessage(contentData);

		return true;
	}

	return false;

}


void CWebSocketSender::OnSendTextMessage(const QByteArray& data) const
{
	if (m_webSocketPtr != nullptr){
		if (!m_webSocketPtr->isValid()){
			return;
		}

		m_webSocketPtr->sendTextMessage(data);
	}
}


} // namespace imtrest


