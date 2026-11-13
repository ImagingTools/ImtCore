// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CHttpFileUploadServletComp.h>


// ImtCore includes
#include <imtrest/CHttpRangeTools.h>
#include <imtrest/IProtocolEngine.h>


namespace imtrest
{


QByteArray CHttpFileUploadServletComp::s_plainTextReponseTypeId = QByteArrayLiteral("text/plain; charset = utf-8");
QByteArray CHttpFileUploadServletComp::s_octetStreamTypeId = QByteArrayLiteral("application/octet-stream");


// public static methods

bool CHttpFileUploadServletComp::GetChunkRange(const QByteArray& contentRange, qint64 chunkSize, imtservergql::IFileUploadHandler::ByteRange& range, qint64& fileSize)
{
	CHttpRangeTools::ContentRange parsedRange;
	if (!CHttpRangeTools::ParseContentRange(contentRange, parsedRange)){
		return false;
	}

	if (!parsedRange.IsByteRange() || (parsedRange.completeLength <= 0) || (parsedRange.GetLength() != chunkSize)){
		return false;
	}

	range = imtservergql::IFileUploadHandler::ByteRange(parsedRange.first, parsedRange.last);
	fileSize = parsedRange.completeLength;

	return true;
}


// protected methods

// reimplemented (CHttpServletCompBase)

ConstResponsePtr CHttpFileUploadServletComp::OnPut(
			const QByteArray& /*commandId*/,
			const IRequest::CommandParams& /*commandParams*/,
			const HeadersMap& headers,
			const CHttpRequest& request) const
{
	Q_ASSERT(m_fileUploadHandlerPtr.IsValid());

	ConstResponsePtr retVal;

	if (!m_fileUploadHandlerPtr.IsValid()){
		retVal = CreateResponse(
					request,
					IProtocolEngine::SC_INTERNAL_SERVER_ERROR,
					{},
					"File upload handler unavailable",
					s_plainTextReponseTypeId);

		return retVal;
	}

	QByteArray contentType;
	QByteArray contentRange;
	int length = 0;
	QString fileName;
	QByteArray sessionId;
	QByteArray fileId;
	QByteArray fileData = request.GetBody();

	for (const QByteArray& header : headers.keys()){
		QByteArray value = contentType = headers[header];

		if (header.compare(QByteArrayLiteral("content-range"), Qt::CaseInsensitive) == 0){
			contentRange = value;
		}
		else if (header.compare(QByteArrayLiteral("content-length"), Qt::CaseInsensitive) == 0){
			length = value.toInt();
		}
		else if (header.compare(QByteArrayLiteral("x-session-id"), Qt::CaseInsensitive) == 0){
			sessionId = value;
		}
		else if (header.compare(QByteArrayLiteral("x-file-id"), Qt::CaseInsensitive) == 0){
			fileId = value;
		}
		else if (header.compare(QByteArrayLiteral("x-file-name"), Qt::CaseInsensitive) == 0){
			fileName = QString::fromUtf8(value);
		}
	}

	CHttpResponse::Headers responseHeaders = GetRequestHeaders(request);

	imtservergql::IFileUploadHandler::ByteRange range;
	qint64 fileSize = -1;
	const bool isRangeValid = GetChunkRange(contentRange, fileData.size(), range, fileSize);

	if (		length == 0 ||
				!isRangeValid ||
				sessionId.isEmpty() ||
				fileId.isEmpty() ||
				fileName.isEmpty() ||
				fileData.size() != length){
		retVal = CreateResponse(request, IProtocolEngine::SC_BAD_REQUEST, responseHeaders, "Invalid request params", s_plainTextReponseTypeId);

		return retVal;
	}

	imtservergql::IFileUploadHandler::FilelUploadStatus status = m_fileUploadHandlerPtr->ReceiveFileChunk(fileId, range, fileData);
	if (status >= imtservergql::IFileUploadHandler::FUS_INVALID_FILE_ID){
		QByteArray message;

		switch (status){
		case imtservergql::IFileUploadHandler::FUS_INVALID_FILE_ID:
			message = "Invalid file ID";
			break;
		case imtservergql::IFileUploadHandler::FUS_FAILED_TO_OPEN:
			message = "Failed to open file";
			break;
		case imtservergql::IFileUploadHandler::FUS_FAILED_TO_SEEK:
			message = "Failed to seek";
			break;
		case imtservergql::IFileUploadHandler::FUS_FAILED_TO_WRITE:
			message = "Failed to write file";
			break;
		case imtservergql::IFileUploadHandler::FUS_CHUNK_OUT_OF_RANGE:
			message = "Invalid file chunk range";
			break;
		}

		retVal = CreateResponse(
			request,
			IProtocolEngine::SC_INTERNAL_SERVER_ERROR,
			responseHeaders,
			message,
			s_plainTextReponseTypeId);

		return retVal;
	}

	IProtocolEngine::StatusCode statusCode = status > imtservergql::IFileUploadHandler::FUS_OK_COMPLETE ? IProtocolEngine::SC_CREATED : IProtocolEngine::SC_PARTIAL_CONTENT;
	retVal = CreateResponse(
				request,
				statusCode,
				{},
				"OK",
				s_plainTextReponseTypeId);

	return retVal;
}


// private methods

CHttpResponse::Headers CHttpFileUploadServletComp::GetRequestHeaders(const CHttpRequest& request) const
{
	CHttpResponse::Headers headers;

	for (const QByteArray& headerId : request.GetHeaders()){
		headers[headerId] = request.GetHeaderValue(headerId);
	}

	return headers;
}


ConstResponsePtr CHttpFileUploadServletComp::CreateResponse(
			const IRequest& request,
			int statusCode,
			const CHttpResponse::Headers& headers,
			const QByteArray& data,
			const QByteArray& dataTypeId) const
{
	const IProtocolEngine& engine = request.GetProtocolEngine();

	ConstResponsePtr responsePtr(engine.CreateResponse(
		request,
		statusCode,
		data,
		dataTypeId).PopInterfacePtr());

	CHttpResponse* httpResponsePtr = const_cast<CHttpResponse*>(dynamic_cast<const CHttpResponse*>(responsePtr.GetPtr()));
	Q_ASSERT(httpResponsePtr != nullptr);

	CHttpResponse::Headers responseHeaders = httpResponsePtr->GetHeaders();
	for (const QByteArray& headerId : headers.keys()){
		if (!responseHeaders.contains(headerId)){
			if (headerId.compare("content-length", Qt::CaseInsensitive)){
				responseHeaders[headerId] = QString::number(data.size()).toUtf8();
			}
			else{
				responseHeaders[headerId] = headers[headerId];
			}
		}
	}

	return responsePtr;
}


} // namespace imtrest


