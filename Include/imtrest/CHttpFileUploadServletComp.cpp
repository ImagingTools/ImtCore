// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CHttpFileUploadServletComp.h>


// Qt includes
#include <QtCore/QRegularExpression>

// ImtCore includes
#include <imtrest/IProtocolEngine.h>


namespace imtrest
{


QByteArray CHttpFileUploadServletComp::s_plainTextReponseTypeId = QByteArrayLiteral("text/plain; charset = utf-8");
QByteArray CHttpFileUploadServletComp::s_octetStreamTypeId = QByteArrayLiteral("application/octet-stream");


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
	istd::CIntRange range(-1, -1);
	int length = 0;
	int fileSize = -1;
	QString fileName;
	QByteArray sessionId;
	QByteArray fileId;
	QByteArray fileData = request.GetBody();

	for (const QByteArray& header : headers.keys()){
		QByteArray value = contentType = headers[header];

		if (header.compare(QByteArrayLiteral("content-range"), Qt::CaseInsensitive) == 0){
			RangeData rangeData = GetRangeDataFromString(value);

			range.SetMinValue(rangeData.startIndex);
			range.SetMaxValue(rangeData.endIndex);
			fileSize = rangeData.totalSize;
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

	if (		length == 0 ||
				range.GetMaxValue() < 0 ||
				range.GetLength() != length ||
				sessionId.isEmpty() ||
				fileId.isEmpty() ||
				fileSize <= 0 ||
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

CHttpFileUploadServletComp::RangeData CHttpFileUploadServletComp::GetRangeDataFromString(const QString& dataString) const
{
	static QRegularExpression regExp("^(?<units>\\S{1,}){0,1}\\s+(?<start>\\d{1,})-(?<end>\\d{1,})?(\\/?(?<total>\\d{1,}))?");

	QRegularExpressionMatch regExpMatch = regExp.match(dataString);
	if (!regExpMatch.hasMatch()){
		return RangeData();
	}

	RangeData retVal;
	retVal.units = regExpMatch.captured("units");
	QString capturedStartData = regExpMatch.captured("start");
	if (!capturedStartData.isEmpty()){
		[[maybe_unused]] bool isNumber = false;
		retVal.startIndex = capturedStartData.toLongLong(&isNumber);
		Q_ASSERT(isNumber);
	}

	QString capturedEndData = regExpMatch.captured("end");
	if (!capturedEndData.isEmpty()){
		[[maybe_unused]] bool isNumber = false;
		retVal.endIndex = capturedEndData.toLongLong(&isNumber);
		Q_ASSERT(isNumber);
	}

	QString capturedTotalData = regExpMatch.captured("total");
	if (!capturedTotalData.isEmpty()){
		[[maybe_unused]] bool isNumber = false;
		retVal.totalSize = capturedTotalData.toLongLong(&isNumber);
		Q_ASSERT(isNumber);
	}

	return retVal;
}


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


