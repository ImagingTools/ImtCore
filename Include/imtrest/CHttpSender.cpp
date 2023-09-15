#include <imtrest/CHttpSender.h>


// Qt includes
#include <QtCore/QDataStream>

// ImtCore includes
#include <imtrest/IResponse.h>
#include <imtrest/IProtocolEngine.h>


namespace imtrest
{


// public methods

CHttpSender::CHttpSender(QAbstractSocket* tcpSocketPtr)
{
	m_tcpSocketPtr = tcpSocketPtr;
}


// reimplemented (IRequest)

bool CHttpSender::SendResponse(ConstResponsePtr& response) const
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

		retVal = retVal && WriteStatus(protocolStatusCode, statusLiteral, *m_tcpSocketPtr);

		IResponse::Headers headers = response->GetHeaders();

		for (IResponse::Headers::ConstIterator headerIter = headers.constBegin(); headerIter != headers.constEnd(); ++headerIter){
			retVal = retVal && WriteHeader(headerIter.key(), headerIter.value(), *m_tcpSocketPtr);
		}

		const QByteArray& contentData = response->GetData();
		quint64 contentLength = contentData.size();

		retVal = retVal && WriteHeader(QByteArray("Content-Length"), QByteArray::number(contentLength), *m_tcpSocketPtr);
		retVal = retVal && WriteHeader(QByteArray("Content-Type"), response->GetDataTypeId(), *m_tcpSocketPtr);

		retVal = retVal && WriteBody(contentData, *m_tcpSocketPtr);

		return retVal;
	}

	// \todo Implement Websocket implementation at this point!

	return false;
}


bool imtrest::CHttpSender::SendRequest(ConstRequestPtr& reguest) const
{
	return false;
}

// protected methods

bool CHttpSender::WriteStatus(int statusCode, const QByteArray& statusCodeLiteral, QAbstractSocket& socket) const
{
	Q_ASSERT(socket.isOpen());
	if (!socket.isOpen()){
		return false;
	}

	socket.write("HTTP/");
	socket.write(QByteArray::number(1));
	socket.write(".");
	socket.write(QByteArray::number(0));
	socket.write(" ");
	socket.write(QByteArray::number(statusCode));
	socket.write(" ");
	socket.write(statusCodeLiteral);
	socket.write("\r\n");

	return true;
}


bool CHttpSender::WriteHeader(const QByteArray& headerKey, const QByteArray& value, QAbstractSocket& socket) const
{
	Q_ASSERT(socket.isOpen());
	if (!socket.isOpen()){
		return false;
	}

	socket.write(headerKey);
	socket.write(": ");
	socket.write(value);
	socket.write("\r\n");

	return true;
}


bool CHttpSender::WriteBody(const QByteArray& data, QAbstractSocket& socket) const
{
	socket.write("\r\n");
	socket.write(data);

	return true;
}


} // namespace imtrest


