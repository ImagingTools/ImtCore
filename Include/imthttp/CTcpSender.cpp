#include <imthttp/CTcpSender.h>


// Qt includes
#include <QtCore/QDataStream>

// ImtCore includes
#include <imthttp/IResponse.h>
#include <imthttp/IProtocolEngine.h>


namespace imthttp
{


// public methods

CTcpSender::CTcpSender(QAbstractSocket* tcpSocketPtr)
{
	m_tcpSocketPtr = tcpSocketPtr;
}


// reimplemented (IRequest)

bool CTcpSender::SendResponse(ConstResponsePtr& response) const
{
	if (!response.IsValid()){
		return false;
	}

	int protocolStatusCode = -1;
	QByteArray statusLiteral;

	bool retVal = response->GetProtocolEngine().GetProtocolStatusCode(response->GetStatusCode(), protocolStatusCode, statusLiteral);
	if (!retVal){
		return false;
	}

	if (m_tcpSocketPtr != nullptr){
		if (!m_tcpSocketPtr->isOpen()){
			return false;
		}

		const QByteArray& contentData = response->GetData();

		retVal = retVal && WriteBody(contentData, *m_tcpSocketPtr);

		return retVal;
	}

	return false;
}


bool imthttp::CTcpSender::SendRequest(ConstRequestPtr& /*reguest*/) const
{
	return false;
}


// protected methods

bool CTcpSender::WriteBody(const QByteArray& data, QAbstractSocket& socket) const
{
	socket.write(data);

	return true;
}


} // namespace imthttp


