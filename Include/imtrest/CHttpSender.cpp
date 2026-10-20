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


struct SBodyFraming
{
	QByteArray contentLength;
	bool isChunked = false;
	imtrest::CHttpSender::BodyFraming framing = imtrest::CHttpSender::BF_NONE;
};


/**
	Decide how the end of the body is shown to the client.
	\param	bodySize	Size of the body of a streamed response, a negative value if it is not known. It is not used if the response
						is not streamed.
	\param	contentSize	Size of the data of a response that is not streamed.
*/
SBodyFraming DecideBodyFraming(
			int protocolStatusCode,
			const imtrest::CHttpSender::ResponseOptions& options,
			bool isStreamed,
			qint64 bodySize,
			qint64 contentSize,
			bool hasTransferEncoding,
			const QByteArray& ownContentLength)
{
	SBodyFraming retVal;

	const bool isInformational = (protocolStatusCode < 200);
	const bool isContentForbidden = isInformational || (protocolStatusCode == 204) || (protocolStatusCode == 304) || (protocolStatusCode == 205);

	if (isStreamed && !isContentForbidden){
		if (options.isBodyOmitted){
			retVal.contentLength = (bodySize >= 0) ? QByteArray::number(bodySize) : ownContentLength;
		}
		else if (bodySize >= 0){
			retVal.contentLength = QByteArray::number(bodySize);
			retVal.framing = (bodySize > 0) ? imtrest::CHttpSender::BF_CONTENT_LENGTH : imtrest::CHttpSender::BF_NONE;
		}
		else if (options.isChunkedAllowed){
			retVal.isChunked = true;
			retVal.framing = imtrest::CHttpSender::BF_CHUNKED;
		}
		else{
			retVal.framing = imtrest::CHttpSender::BF_UNTIL_CLOSE;
		}
	}
	else if (hasTransferEncoding || isInformational || (protocolStatusCode == 204)){
		// No length
	}
	else if (protocolStatusCode == 304){
		retVal.contentLength = ownContentLength;
	}
	else if (protocolStatusCode == 205){
		retVal.contentLength = QByteArrayLiteral("0");
	}
	else if (options.isBodyOmitted){
		retVal.contentLength = (contentSize == 0) ? ownContentLength : QByteArray::number(contentSize);
	}
	else{
		retVal.contentLength = QByteArray::number(contentSize);
	}

	return retVal;
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
	BodyFraming framing = BF_NONE;

	return BuildData(response, response.GetData(), options, StreamedBody(), data, framing);
}


bool CHttpSender::BuildStreamingResponseHead(const IResponse& response, qint64 bodySize, const ResponseOptions& options, QByteArray& head, BodyFraming& framing)
{
	StreamedBody streamedBody;
	streamedBody.isStreamed = true;
	streamedBody.size = (bodySize >= 0) ? bodySize : -1;

	return BuildData(response, QByteArray(), options, streamedBody, head, framing);
}


QByteArray CHttpSender::CreateChunk(const QByteArray& data)
{
	if (data.isEmpty()){
		return {};
	}

	const QByteArray sizeText = QByteArray::number(data.size(), 16);

	QByteArray retVal;
	retVal.reserve(sizeText.size() + data.size() + 4);
	retVal.append(sizeText);
	retVal.append("\r\n");
	retVal.append(data);
	retVal.append("\r\n");

	return retVal;
}


QByteArray CHttpSender::CreateLastChunk()
{
	return QByteArrayLiteral("0\r\n\r\n");
}


// private static methods

bool CHttpSender::BuildData(
			const IResponse& response,
			const QByteArray& contentData,
			const ResponseOptions& options,
			const StreamedBody& streamedBody,
			QByteArray& data,
			BodyFraming& framing)
{
	framing = BF_NONE;

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

		// The framing of a streamed body is decided by the sender
		if (streamedBody.isStreamed && (lowerName == "transfer-encoding")){
			continue;
		}

		hasDate = hasDate || (lowerName == "date");
		hasContentType = hasContentType || (lowerName == "content-type");
		hasTransferEncoding = hasTransferEncoding || ((lowerName == "transfer-encoding") && !value.trimmed().isEmpty());

		headersData += name + ": " + value + "\r\n";
	}

	// The response is the only source of the length of the representation for 304 and HEAD (RFC 9110, section 8.6)
	const SBodyFraming bodyFraming = DecideBodyFraming(
				protocolStatusCode,
				options,
				streamedBody.isStreamed,
				streamedBody.size,
				contentData.size(),
				hasTransferEncoding,
				ownContentLength);

	const QByteArray& contentLength = bodyFraming.contentLength;
	const bool isChunked = bodyFraming.isChunked;
	framing = bodyFraming.framing;

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

	if (isChunked){
		data.append("Transfer-Encoding: chunked\r\n");
	}

	if (options.isConnectionClosing || (framing == BF_UNTIL_CLOSE)){
		connectionTokens.append(QByteArrayLiteral("close"));
	}

	if (!connectionTokens.isEmpty()){
		data.append("Connection: " + connectionTokens.join(", ") + "\r\n");
	}

	data.append("\r\n");

	if (isContentSent && !streamedBody.isStreamed){
		data.append(contentData);
	}

	return true;
}


} // namespace imtrest


