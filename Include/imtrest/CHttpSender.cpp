// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CHttpSender.h>


// Qt includes
#include <QtCore/QDataStream>

// ImtCore includes
#include <imtrest/CHttpDateTools.h>
#include <imtrest/IResponse.h>
#include <imtrest/IProtocolEngine.h>


namespace
{


// Token (RFC 9110, section 5.6.2)
bool IsTokenChar(char c)
{
	if (((c >= '0') && (c <= '9')) || ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'))){
		return true;
	}

	static const QByteArray symbols("!#$%&'*+-.^_`|~");

	return symbols.contains(c);
}


// A name that is not a token or a value with CR, LF or NUL cannot be sent (RFC 9110, section 5.5)
bool IsValidHeader(const QByteArray& name, const QByteArray& value)
{
	if (name.isEmpty()){
		return false;
	}

	for (char c : name){
		if (!IsTokenChar(c)){
			return false;
		}
	}

	return !value.contains('\r') && !value.contains('\n') && !value.contains('\0');
}


bool IsLength(const QByteArray& value)
{
	if (value.isEmpty()){
		return false;
	}

	for (char c : value){
		if ((c < '0') || (c > '9')){
			return false;
		}
	}

	bool isOk = false;
	value.toLongLong(&isOk);

	return isOk;
}


void AppendConnectionTokens(const QByteArray& value, QByteArrayList& tokens)
{
	const QByteArrayList parts = value.split(',');
	for (const QByteArray& part : parts){
		const QByteArray token = part.trimmed();
		const QByteArray lowerToken = token.toLower();

		// The persistence of the connection is decided by the server
		if (token.isEmpty() || (lowerToken == "close") || (lowerToken == "keep-alive")){
			continue;
		}

		tokens.append(token);
	}
}


} // namespace


namespace imtrest
{


// public methods

CHttpSender::CHttpSender(QAbstractSocket* tcpSocketPtr)
{
	m_tcpSocketPtr = tcpSocketPtr;
}


// reimplemented (ITransport)

bool CHttpSender::SendData(const QByteArray& data) const
{
	if (m_tcpSocketPtr == nullptr || !m_tcpSocketPtr->isOpen()){
		return false;
	}

	m_tcpSocketPtr->write(data);

	return true;
}


// public methods

bool CHttpSender::BuildResponseData(const IResponse& response, QByteArray& data)
{
	return BuildResponseData(response, data, ResponseOptions());
}


bool CHttpSender::BuildResponseData(const IResponse& response, QByteArray& data, const ResponseOptions& options)
{
	int protocolStatusCode = -1;
	QByteArray statusLiteral;

	if (!response.GetProtocolEngine().GetProtocolStatusCode(response.GetStatusCode(), protocolStatusCode, statusLiteral)){
		return false;
	}

	// 1xx, 204 and 304 end with the headers, 205 has no content but needs the length 0 (RFC 9110, section 6.4.1 and 15.3.6)
	const bool isInformational = (protocolStatusCode < 200);
	const bool isEndedByHeaders = isInformational || (protocolStatusCode == 204) || (protocolStatusCode == 304);
	const bool isContentForbidden = isEndedByHeaders || (protocolStatusCode == 205);
	const bool isContentSent = !isContentForbidden && !options.isBodyOmitted;

	QByteArray headersData;
	QByteArray ownContentLength;
	QByteArrayList connectionTokens;
	bool hasDate = false;
	bool hasContentType = false;
	bool hasTransferEncoding = false;

	const IResponse::Headers headers = response.GetHeaders();
	for (IResponse::Headers::ConstIterator headerIter = headers.constBegin(); headerIter != headers.constEnd(); ++headerIter){
		const QByteArray& name = headerIter.key();
		const QByteArray& value = headerIter.value();
		if (!IsValidHeader(name, value)){
			continue;
		}

		const QByteArray lowerName = name.toLower();
		if (lowerName == "content-length"){
			if (ownContentLength.isEmpty() && IsLength(value.trimmed())){
				ownContentLength = value.trimmed();
			}

			continue;
		}

		if (lowerName == "connection"){
			AppendConnectionTokens(value, connectionTokens);

			continue;
		}

		hasDate = hasDate || (lowerName == "date");
		hasContentType = hasContentType || (lowerName == "content-type");
		hasTransferEncoding = hasTransferEncoding || ((lowerName == "transfer-encoding") && !value.trimmed().isEmpty());

		headersData += name + ": " + value + "\r\n";
	}

	const QByteArray& contentData = response.GetData();

	// The response is the only source of the length of the representation for 304 and HEAD (RFC 9110, section 8.6)
	QByteArray contentLength;
	if (hasTransferEncoding || isInformational || (protocolStatusCode == 204)){
		contentLength.clear();
	}
	else if (protocolStatusCode == 304){
		contentLength = ownContentLength;
	}
	else if (protocolStatusCode == 205){
		contentLength = QByteArrayLiteral("0");
	}
	else if (options.isBodyOmitted){
		contentLength = contentData.isEmpty() ? ownContentLength : QByteArray::number(contentData.size());
	}
	else{
		contentLength = QByteArray::number(contentData.size());
	}

	data.clear();
	data.append(QByteArrayLiteral("HTTP/1.1 "));
	data.append(QByteArray::number(protocolStatusCode));
	data.append(" ");
	data.append(statusLiteral);
	data.append("\r\n");
	data.append(headersData);

	// The server has the clock, so Date is required (RFC 9110, section 6.6.1)
	if (!hasDate && !isInformational){
		const QDateTime dateTime = options.dateTime.isValid() ? options.dateTime : QDateTime::currentDateTimeUtc();

		data.append("Date: " + CHttpDateTools::ToImfFixdate(dateTime) + "\r\n");
	}

	const QByteArray dataTypeId = response.GetDataTypeId();
	if (!hasContentType && !isContentForbidden && IsValidHeader(QByteArrayLiteral("Content-Type"), dataTypeId) && !dataTypeId.isEmpty()){
		data.append("Content-Type: " + dataTypeId + "\r\n");
	}

	if (!contentLength.isEmpty()){
		data.append("Content-Length: " + contentLength + "\r\n");
	}

	if (options.isConnectionClosing){
		connectionTokens.append(QByteArrayLiteral("close"));
	}

	if (!connectionTokens.isEmpty()){
		data.append("Connection: " + connectionTokens.join(", ") + "\r\n");
	}

	data.append("\r\n");

	if (isContentSent){
		data.append(contentData);
	}

	return true;
}


} // namespace imtrest


