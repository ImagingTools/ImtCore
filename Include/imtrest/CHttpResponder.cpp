#include <imtrest/CHttpResponder.h>


// ImtCore includes
#include <imtrest/IResponse.h>
#include <imtrest/IProtocolEngine.h>


namespace imtrest
{


// public methods

// reimplemented (IResponder)

bool CHttpResponder::SendResponse(const IResponse& response, QIODevice& device) const
{
	int protocolStatusCode = -1;
	QByteArray statusLiteral;

	bool retVal = response.GetProtocolEngine().GetProtocolStatusCode(response.GetStatusCode(), protocolStatusCode, statusLiteral);
	if (!retVal){
		return false;
	}

	if (!device.isOpen()){
		return false;
	}

	retVal = retVal && WriteStatus(protocolStatusCode, statusLiteral, device);

	IResponse::Headers headers = response.GetHeaders();

	for (IResponse::Headers::ConstIterator headerIter = headers.constBegin(); headerIter != headers.constEnd(); ++headerIter){
		retVal = retVal && WriteHeader(headerIter.key(), headerIter.value(), device);
	}

	const QByteArray& contentData = response.GetData();
	quint64 contentLength = contentData.size();
	retVal = retVal && WriteHeader(QByteArrayLiteral("Content-Length"), QByteArray::number(contentLength), device);

	retVal = retVal && WriteBody(contentData, device);

	return retVal;
}


// protected methods

bool CHttpResponder::WriteStatus(int statusCode, const QByteArray& statusCodeLiteral, QIODevice & outputDevice) const
{
	Q_ASSERT(outputDevice.isOpen());
	if (!outputDevice.isOpen()){
		return false;
	}

	outputDevice.write("HTTP/");
	outputDevice.write(QByteArray::number(1));
	outputDevice.write(".");
	outputDevice.write(QByteArray::number(0));
	outputDevice.write(" ");
	outputDevice.write(QByteArray::number(statusCode));
	outputDevice.write(" ");
	outputDevice.write(statusCodeLiteral);
	outputDevice.write("\r\n");

	return true;
}


bool CHttpResponder::WriteHeader(const QByteArray & headerKey, const QByteArray & value, QIODevice & outputDevice) const
{
	Q_ASSERT(outputDevice.isOpen());
	if (!outputDevice.isOpen()){
		return false;
	}

	outputDevice.write(headerKey);
	outputDevice.write(": ");
	outputDevice.write(value);
	outputDevice.write("\r\n");

	return true;
}


bool CHttpResponder::WriteBody(const QByteArray & data, QIODevice & outputDevice) const
{
	return false;
}

} // namespace imtrest


