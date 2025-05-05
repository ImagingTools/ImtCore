#include <imtrest/CHttpTempFileStorageServletComp.h>


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QRegularExpression>

// ImtCore includes
#include <imtrest/CHttpResponse.h>
#include <imtrest/IProtocolEngine.h>


namespace imtrest
{


QByteArray CHttpTempFileStorageServletComp::s_plainTextReponseTypeId = QByteArrayLiteral("text/plain; charset = utf-8");
QByteArray CHttpTempFileStorageServletComp::s_octetStreamTypeId = QByteArrayLiteral("application/octet-stream");


// protected methods

// reimplemented (CHttpServletCompBase)

ConstResponsePtr CHttpTempFileStorageServletComp::OnPut(const QByteArray& commandId, const IRequest::CommandParams& commandParams, const HeadersMap& headers, const CHttpRequest& request) const
{
	ConstResponsePtr retVal;

	if (!m_fileManagerCompPtr.IsValid()){
		retVal = CreateResponse(
			request,
			IProtocolEngine::SC_INTERNAL_SERVER_ERROR,
			{},
			"Temp file manager not available",
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

	if (length == 0
		|| range.GetMaxValue() < 0
		|| range.GetLength() != length
		|| sessionId.isEmpty()
		|| fileId.isEmpty()
		|| fileSize <= 0
		|| fileName.isEmpty()
		|| fileData.size() != length){

		retVal = CreateResponse(request, IProtocolEngine::SC_BAD_REQUEST, responseHeaders, "Invalid request params", s_plainTextReponseTypeId);

		return retVal;
	}

	ProcessingStatus status = PutFileChunk(sessionId, fileId, fileName, fileSize, range, fileData);
	if (status != PS_OK){
		QByteArray message;

		switch (status){
		case PS_FAILED_TO_INIT_SESSION:
			message = "Failed to initialize file storage session";
			break;
		case PS_FAILED_TO_INIT_FILE:
			message = "Failed to initialize file int the storage";
			break;
		case PS_EMPTY_FILE_PATH:
			message = "Unable to retreive file path in the storage";
			break;
		case PS_FAILED_TO_OPEN:
			message = "Failed to open file";
			break;
		case PS_FAILED_TO_SEEK:
			message = "Failed to seek";
			break;
		case PS_FAILED_TO_WRITE:
			message = "Failed to write file";
			break;
		case PS_CHUNK_OUT_OF_RANGE:
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

	UpdateUploadProgress(fileId);

	double progressValue = GetFileProgress(fileId);
	IProtocolEngine::StatusCode statusCode = qFuzzyCompare(progressValue, 1.) ? IProtocolEngine::SC_CREATED : IProtocolEngine::SC_PARTIAL_CONTENT;
	retVal = CreateResponse(
		request,
		statusCode,
		{},
		"OK",
		s_plainTextReponseTypeId);

	return retVal;
}


// reimplemented (icomp::CComponentCreated)

void CHttpTempFileStorageServletComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	Q_ASSERT(m_fileManagerCompPtr.IsValid());
}


// private methods

CHttpTempFileStorageServletComp::RangeData CHttpTempFileStorageServletComp::GetRangeDataFromString(const QString& dataString) const
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


CHttpTempFileStorageServletComp::ProcessingStatus CHttpTempFileStorageServletComp::PutFileChunk(
	const QByteArray& sessionId,
	const QByteArray& fileId,
	const QString fileName,
	int fileSize,
	const istd::CIntRange& range,
	const QByteArray& data) const
{
	QString filePath = m_fileManagerCompPtr->GetPath(sessionId, fileId);
	if (filePath.isEmpty()){
		if (m_fileManagerCompPtr->BeginSession(sessionId).isEmpty()){
			return PS_FAILED_TO_INIT_SESSION;
		}

		if (m_fileManagerCompPtr->AddFileItem(sessionId, fileName, fileId) != fileId){
			return PS_FAILED_TO_INIT_FILE;
		}
	}

	filePath = m_fileManagerCompPtr->GetPath(sessionId, fileId);
	if (filePath.isEmpty()){
		return PS_EMPTY_FILE_PATH;
	}

	QFile file(filePath);
	if (!file.open(QFile::ReadWrite)){
		return PS_FAILED_TO_OPEN;
	}

	if (!file.seek(range.GetMinValue())){
		return PS_FAILED_TO_SEEK;
	}

	if (file.write(data) != data.size()){
		return PS_FAILED_TO_WRITE;
	}

	if (range.GetMinValue() < 0 || range.GetMaxValue() > fileSize){
		return PS_CHUNK_OUT_OF_RANGE;
	}

	m_uploadInfos[fileId].size = fileSize;
	AddFileChunk(fileId, range);

	file.close();

	return PS_OK;
}


CHttpTempFileStorageServletComp::ProcessingStatus CHttpTempFileStorageServletComp::AddFileChunk(const QByteArray& fileId, const istd::CIntRange& range) const
{
	FileInfo& info = m_uploadInfos[fileId];

	// Find intersections with uploaded chunks
	QVector<int> intersectsWith;
	for (int i = 0; i < m_uploadInfos[fileId].uploadedChunks.size(); i++){
		if (info.uploadedChunks[i].IsIntersectedBy(range)){
			intersectsWith.push_back(i);
		}
	}

	// Unite a chunk with intersecting chunks
	istd::CIntRange result = range;
	for (int index : intersectsWith){
		result.Unite(info.uploadedChunks[index]);
	}

	for (int i = intersectsWith.size() - 1; i >= 0; i--){
		info.uploadedChunks.remove(i);
	}

	info.uploadedChunks.push_back(result);

	return PS_OK;
}


double CHttpTempFileStorageServletComp::GetFileProgress(const QByteArray& fileId) const
{
	int size = m_uploadInfos[fileId].size;
	int uploaded = 0;

	Q_ASSERT(size != 0);

	for (const istd::CIntRange& chunk : m_uploadInfos[fileId].uploadedChunks){
		uploaded += chunk.GetLength();
	}

	return (double)uploaded / size;
}


void CHttpTempFileStorageServletComp::UpdateUploadProgress(const QByteArray& fileId) const
{
	if (m_uploadInfos.contains(fileId)){
		ibase::IProgressLogger* progressLoggerPtr = m_progressLoggerProviderCompPtr->GetProgressLogger(fileId);
		if (progressLoggerPtr != nullptr){
			double progress = GetFileProgress(fileId);

			progressLoggerPtr->OnProgress(progress);
		}
	}
}


CHttpResponse::Headers CHttpTempFileStorageServletComp::GetRequestHeaders(const CHttpRequest& request) const
{
	CHttpResponse::Headers headers;

	for (const QByteArray& headerId : request.GetHeaders()){
		headers[headerId] = request.GetHeaderValue(headerId);
	}

	return headers;
}


ConstResponsePtr CHttpTempFileStorageServletComp::CreateResponse(
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
		dataTypeId));

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

	// httpResponsePtr->SetHeaders(responseHeaders);

	return responsePtr;
}


} // namespace imtrest


