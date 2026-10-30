// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CHttpFileServletComp.h>


// STL includes
#include <memory>

// Qt includes
#include <QtCore/QFileInfo>

// ImtCore includes
#include <imtrest/CFileRangeBodySource.h>
#include <imtrest/CHttpDateTools.h>
#include <imtrest/CHttpRangeTools.h>
#include <imtrest/CHttpRequest.h>
#include <imtrest/CHttpResponse.h>
#include <imtrest/CHttpStreamingResponse.h>


namespace
{


typedef imtrest::CHttpFileServletComp::FileRepresentation FileRepresentation;
typedef imtrest::CHttpFileServletComp::RequestConditions RequestConditions;
typedef imtrest::IResponse::Headers Headers;


// Response without a body: the refusals and 304
imtrest::ConstResponsePtr CreateStatusResponse(const imtrest::IProtocolEngine& engine, int statusCode, const Headers& headers)
{
	std::unique_ptr<imtrest::CHttpResponse> responsePtr(new imtrest::CHttpResponse(statusCode, QByteArray(), QByteArray(), engine));
	responsePtr->SetHeaders(headers);

	return imtrest::ConstResponsePtr(responsePtr.release());
}


// The headers that describe the representation: they are the same for 200, 206 and 304 (RFC 9110, section 15.4.5)
Headers CreateRepresentationHeaders(const FileRepresentation& file)
{
	Headers headers = file.headers;

	if (!file.eTag.isEmpty()){
		headers.insert("ETag", file.eTag);
	}

	if (file.lastModified.isValid()){
		headers.insert("Last-Modified", imtrest::CHttpDateTools::ToImfFixdate(file.lastModified));
	}

	return headers;
}


struct SRangeSelection
{
	enum Kind
	{
		K_WHOLE,
		K_PART,
		K_NOT_SATISFIABLE
	};

	Kind kind = K_WHOLE;
	qint64 first = 0;
	qint64 length = 0;
};


// Which part of the file is sent for the headers Range and If-Range (RFC 9110, section 13.1.5 and 14.2)
SRangeSelection SelectRange(const RequestConditions& conditions, const imtrest::CHttpConditionalTools::Validators& validators, qint64 size)
{
	SRangeSelection retVal;
	retVal.length = size;

	// If the file is changed since the client got its first part, the client needs the whole file
	if (!conditions.ifRange.isEmpty() && !imtrest::CHttpConditionalTools::EvaluateIfRange(conditions.ifRange, validators)){
		return retVal;
	}

	QList<imtrest::CHttpRangeTools::ByteRange> ranges;

	switch (imtrest::CHttpRangeTools::ParseRange(conditions.range, size, ranges)){
	case imtrest::CHttpRangeTools::RS_NOT_SATISFIABLE:
		retVal.kind = SRangeSelection::K_NOT_SATISFIABLE;
		break;

	case imtrest::CHttpRangeTools::RS_SATISFIABLE:
		ranges = imtrest::CHttpRangeTools::CoalesceRanges(ranges);

		// Several ranges need multipart/byteranges, it is not supported: the whole file is allowed by RFC 9110, section 14.2
		if (ranges.size() == 1){
			retVal.kind = SRangeSelection::K_PART;
			retVal.first = ranges.first().first;
			retVal.length = ranges.first().GetLength();
		}
		break;

	case imtrest::CHttpRangeTools::RS_IGNORE:
		break;
	}

	return retVal;
}


// The body is read when it is sent, in the thread of the connection: the response keeps the path, not an open file
imtrest::ConstResponsePtr CreateBodyResponse(
			const imtrest::IProtocolEngine& engine,
			int statusCode,
			const FileRepresentation& file,
			qint64 first,
			qint64 length,
			const Headers& headers)
{
	const QString filePath = file.filePath;

	std::unique_ptr<imtrest::CHttpStreamingResponse> responsePtr(new imtrest::CHttpStreamingResponse(
				statusCode,
				file.dataTypeId.isEmpty() ? QByteArrayLiteral("application/octet-stream") : file.dataTypeId,
				engine,
				length,
				[filePath, first, length](){
		return imtrest::CFileRangeBodySource::CreateForFile(filePath, first, length);
	}));

	responsePtr->SetHeaders(headers);

	return imtrest::ConstResponsePtr(responsePtr.release());
}


} // namespace


namespace imtrest
{


// public static methods

QByteArray CHttpFileServletComp::CreateDefaultETag(qint64 size, const QDateTime& lastModified)
{
	const qint64 modifiedMs = lastModified.isValid() ? lastModified.toMSecsSinceEpoch() : 0;

	return CHttpConditionalTools::CreateEntityTag(QByteArray::number(modifiedMs, 16) + "-" + QByteArray::number(size, 16));
}


ConstResponsePtr CHttpFileServletComp::CreateFileResponse(
			const IProtocolEngine& engine,
			bool isHeadRequest,
			const RequestConditions& conditions,
			const FileRepresentation& file)
{
	CHttpConditionalTools::Validators validators;
	validators.eTag = file.eTag;
	validators.lastModified = file.lastModified;

	switch (CHttpConditionalTools::EvaluatePreconditions(isHeadRequest ? QByteArrayLiteral("HEAD") : QByteArrayLiteral("GET"), conditions.preconditions, validators)){
	case CHttpConditionalTools::PR_NOT_MODIFIED:
		return CreateStatusResponse(engine, IProtocolEngine::SC_NOT_MODIFIED, CreateRepresentationHeaders(file));

	case CHttpConditionalTools::PR_PRECONDITION_FAILED:
		return CreateStatusResponse(engine, IProtocolEngine::SC_PRECONDITION_FAILED, Headers());

	case CHttpConditionalTools::PR_BAD_REQUEST:
		return CreateStatusResponse(engine, IProtocolEngine::SC_BAD_REQUEST, Headers());

	case CHttpConditionalTools::PR_PROCEED:
		break;
	}

	Headers headers = CreateRepresentationHeaders(file);
	headers.insert("Accept-Ranges", "bytes");

	// Range is defined for GET only (RFC 9110, section 14.2)
	SRangeSelection selection;
	selection.length = file.size;
	if (!isHeadRequest && !conditions.range.isEmpty()){
		selection = SelectRange(conditions, validators, file.size);
	}

	if (selection.kind == SRangeSelection::K_NOT_SATISFIABLE){
		// The unsatisfied range tells the size of the file (RFC 9110, section 15.5.17)
		headers.insert("Content-Range", "bytes */" + QByteArray::number(file.size));

		return CreateStatusResponse(engine, IProtocolEngine::SC_RANGE_NOT_SATISFIABLE, headers);
	}

	if (selection.kind == SRangeSelection::K_PART){
		headers.insert("Content-Range", CHttpRangeTools::CreateContentRange(selection.first, selection.length, file.size));

		return CreateBodyResponse(engine, IProtocolEngine::SC_PARTIAL_CONTENT, file, selection.first, selection.length, headers);
	}

	return CreateBodyResponse(engine, IProtocolEngine::SC_OK, file, 0, file.size, headers);
}


ConstResponsePtr CHttpFileServletComp::ProcessFileRequest(const IHttpFileResolver& resolver, const imtrest::CHttpRequest& request, bool isHeadRequest)
{
	const IProtocolEngine& engine = request.GetProtocolEngine();

	IHttpFileResolver::FileDescription description;
	const IProtocolEngine::StatusCode resolveStatus = resolver.ResolveFile(request, description);
	if (resolveStatus != IProtocolEngine::SC_OK){
		return CreateStatusResponse(engine, resolveStatus, description.headers);
	}

	const QFileInfo fileInfo(description.filePath);
	if (!fileInfo.isFile()){
		return CreateStatusResponse(engine, IProtocolEngine::SC_NOT_FOUND, description.headers);
	}

	if (!fileInfo.isReadable()){
		return CreateStatusResponse(engine, IProtocolEngine::SC_FORBIDDEN, description.headers);
	}

	FileRepresentation file;
	file.filePath = description.filePath;
	file.size = fileInfo.size();
	file.lastModified = fileInfo.lastModified();
	file.eTag = description.eTag.isEmpty() ? CreateDefaultETag(file.size, file.lastModified) : description.eTag;
	file.dataTypeId = description.dataTypeId;
	file.headers = description.headers;

	RequestConditions conditions;
	conditions.preconditions.ifMatch = request.GetHeaderValue("if-match");
	conditions.preconditions.ifNoneMatch = request.GetHeaderValue("if-none-match");
	conditions.preconditions.ifModifiedSince = request.GetHeaderValue("if-modified-since");
	conditions.preconditions.ifUnmodifiedSince = request.GetHeaderValue("if-unmodified-since");
	conditions.ifRange = request.GetHeaderValue("if-range");
	conditions.range = request.GetHeaderValue("range");

	return CreateFileResponse(engine, isHeadRequest, conditions, file);
}


// protected methods

// reimplemented (CHttpServletCompBase)

ConstResponsePtr CHttpFileServletComp::OnGet(
			const QByteArray& /*commandId*/,
			const imtrest::IRequest::CommandParams& /*commandParams*/,
			const HeadersMap& /*headers*/,
			const imtrest::CHttpRequest& request) const
{
	return ProcessRequestWithResolver(request, false);
}


ConstResponsePtr CHttpFileServletComp::OnHead(
			const QByteArray& /*commandId*/,
			const imtrest::IRequest::CommandParams& /*commandParams*/,
			const HeadersMap& /*headers*/,
			const imtrest::CHttpRequest& request) const
{
	return ProcessRequestWithResolver(request, true);
}


// private methods

ConstResponsePtr CHttpFileServletComp::ProcessRequestWithResolver(const imtrest::CHttpRequest& request, bool isHeadRequest) const
{
	if (!m_fileResolverCompPtr.IsValid()){
		// A mistake of the assembly: the client gets the answer, and the server goes on
		SendErrorMessage(0, QStringLiteral("The file resolver is not set"));

		return CreateStatusResponse(request.GetProtocolEngine(), IProtocolEngine::SC_INTERNAL_SERVER_ERROR, Headers());
	}

	return ProcessFileRequest(*m_fileResolverCompPtr, request, isHeadRequest);
}


} // namespace imtrest


