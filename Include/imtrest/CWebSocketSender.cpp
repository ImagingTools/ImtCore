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

		m_webSocketPtr->sendTextMessage(contentData);

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

		m_webSocketPtr->sendTextMessage(contentData);

		return true;
	}

	return false;

}


} // namespace imtrest


