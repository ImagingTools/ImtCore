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

	// Do NOT access m_webSocketPtr here — this method is called from publisher
	// worker threads, and m_webSocketPtr tracks a QWebSocket on the main thread.
	// Just emit the queued signal unconditionally; the main-thread
	// OnSendTextMessage slot validates the QPointer safely.
	const QByteArray& contentData = response->GetData();
	emit SendTextMessage(contentData);
	return true;
}


bool CWebSocketSender::SendRequest(ConstRequestPtr& request) const
{
	// Same reasoning as SendResponse — do not access m_webSocketPtr from a
	// worker thread.
	const QByteArray& contentData = request->GetBody();
	emit SendTextMessage(contentData);
	return true;
}


void CWebSocketSender::OnSendTextMessage(const QByteArray& data) const
{
	// Re-check the guard here: this slot is invoked via QueuedConnection on the
	// main thread.  The QWebSocket may have been destroyed between the emit and
	// the dispatch of this slot.
	if (!m_webSocketPtr.isNull()){
		if (!m_webSocketPtr->isValid()){
			return;
		}

		m_webSocketPtr->sendTextMessage(data);
	}
}


} // namespace imtrest


