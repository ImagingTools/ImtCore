#include <imtrest/CHttpResponder.h>


// ImtCore includes
#include <imtrest/IResponse.h>
#include <imtrest/IProtocolEngine.h>


namespace imtrest
{


// public methods

// reimplemented (IResponder)

bool CHttpResponder::SendResponse(const IResponse& response, QAbstractSocket& socket) const
{
	int protocolStatusCode = -1;
	QByteArray statusLiteral;

	bool retVal = response.GetProtocolEngine().GetProtocolStatusCode(response.GetStatusCode(), protocolStatusCode, statusLiteral);
	if (!retVal){
		return false;
	}

	if (!socket.isOpen()){
		return false;
	}

	retVal = retVal && WriteStatus(protocolStatusCode, statusLiteral, socket);

	IResponse::Headers headers = response.GetHeaders();

	for (IResponse::Headers::ConstIterator headerIter = headers.constBegin(); headerIter != headers.constEnd(); ++headerIter){
		retVal = retVal && WriteHeader(headerIter.key(), headerIter.value(), socket);
	}

	const QByteArray& contentData = response.GetData();
	quint64 contentLength = contentData.size();
	retVal = retVal && WriteHeader(QByteArrayLiteral("Content-Length"), QByteArray::number(contentLength), socket);

	retVal = retVal && WriteBody(contentData, socket);

	return retVal;
}


// protected methods

bool CHttpResponder::WriteStatus(int statusCode, const QByteArray& statusCodeLiteral, QAbstractSocket& socket) const
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


bool CHttpResponder::WriteHeader(const QByteArray & headerKey, const QByteArray & value, QAbstractSocket& socket) const
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


bool CHttpResponder::WriteBody(const QByteArray& data, QAbstractSocket& socket) const
{
	return false;
}


} // namespace imtrest


