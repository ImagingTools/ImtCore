// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrestdemo/CDemoBodyPolicyComp.h>


// Qt includes
#include <QtCore/QDir>

// ImtCore includes
#include <imtrest/CHttpRequest.h>
#include <imtrest/IProtocolEngine.h>
#include <imtrest/CHttpRangeTools.h>
#include <imtrestdemo/CDemoResumableUploadSink.h>
#include <imtrestdemo/CDemoUploadSink.h>


namespace imtrestdemo
{


// public methods

QString CDemoBodyPolicyComp::GetUploadFileName(const QByteArray& commandId)
{
	const QString name = QString::fromUtf8(commandId.mid(commandId.lastIndexOf('/') + 1));
	if (name.isEmpty() || name.startsWith('.') || name.contains('\\') || name.contains(':') || name.contains(QChar(0))){
		return QString();
	}

	// The names of the temporary files of the uploads that can be resumed
	if (name.endsWith(QLatin1String(CDemoResumableUploadSink::s_dataSuffix)) || name.endsWith(QLatin1String(CDemoResumableUploadSink::s_metaSuffix))){
		return QString();
	}

	return name;
}


// private methods

int CDemoBodyPolicyComp::ToEngineStatusCode(int httpStatusCode)
{
	typedef imtrest::IProtocolEngine Engine;

	switch (httpStatusCode){
	case 401: return Engine::SC_UNAUTHORIZED;
	case 403: return Engine::SC_FORBIDDEN;
	case 404: return Engine::SC_NOT_FOUND;
	case 409: return Engine::SC_CONFLICT;
	case 413: return Engine::SC_PAYLOAD_TOO_LARGE;
	case 500: return Engine::SC_INTERNAL_ERROR;
	case 503: return Engine::SC_SERVICE_UNAVAILABLE;
	default: return Engine::SC_BAD_REQUEST;
	}
}


// reimplemented (imtrest::IHttpRequestBodyPolicy)

imtrest::IHttpRequestBodyPolicy::BodyDecision CDemoBodyPolicyComp::DecideBody(const imtrest::CHttpRequest& requestHead) const
{
	typedef imtrest::IProtocolEngine Engine;

	if (!(*m_requiredTokenAttrPtr).isEmpty()){
		if (requestHead.GetHeaderValue("authorization") != "Bearer " + *m_requiredTokenAttrPtr){
			BodyDecision decision = CreateRejection(Engine::SC_UNAUTHORIZED);
			decision.rejectHeaders.insert("WWW-Authenticate", "Bearer realm=\"imtrestdemo\"");

			return decision;
		}
	}

	const QByteArray mode = *m_modeAttrPtr;
	if (mode == "reject"){
		return CreateRejection(ToEngineStatusCode(*m_rejectStatusCodeAttrPtr));
	}

	BodyDecision decision;
	decision.maxBodySize = *m_maxBodySizeAttrPtr;

	const imtrest::CHttpRequest::MethodType method = requestHead.GetMethodType();
	if (mode == "stream" && (method == imtrest::CHttpRequest::MT_PUT || method == imtrest::CHttpRequest::MT_POST)){
		const QString fileName = GetUploadFileName(requestHead.GetCommandId());
		if (fileName.isEmpty()){
			return CreateRejection(Engine::SC_BAD_REQUEST);
		}

		// A part of an upload that can be resumed
		const QByteArray contentRange = requestHead.GetHeaderValue("content-range");
		if (!contentRange.isEmpty()){
			return DecideRangedUpload(requestHead, fileName, contentRange);
		}

		std::shared_ptr<CDemoUploadSink> sinkPtr = std::make_shared<CDemoUploadSink>(QDir(*m_uploadDirectoryAttrPtr).absoluteFilePath(fileName));

		// The client should not send the body that cannot be saved
		if (!sinkPtr->Open()){
			return CreateRejection(Engine::SC_INTERNAL_ERROR);
		}

		decision.mode = BM_STREAM;
		decision.sinkPtr = sinkPtr;
	}

	return decision;
}


// private methods

imtrest::IHttpRequestBodyPolicy::BodyDecision CDemoBodyPolicyComp::DecideRangedUpload(
			const imtrest::CHttpRequest& requestHead,
			const QString& fileName,
			const QByteArray& contentRange) const
{
	typedef imtrest::IProtocolEngine Engine;

	imtrest::CHttpRangeTools::ContentRange range;
	if (!imtrest::CHttpRangeTools::ParseContentRange(contentRange, range) || !range.IsByteRange() || range.completeLength < 0){
		return CreateRejection(Engine::SC_BAD_REQUEST);
	}

	// The part has its length, and it is the length of the range: the part cannot be a different one
	bool isLengthValid = false;
	const qint64 length = requestHead.GetHeaderValue("content-length").toLongLong(&isLengthValid);
	if (!isLengthValid || length != range.GetLength()){
		return CreateRejection(Engine::SC_BAD_REQUEST);
	}

	std::shared_ptr<CDemoResumableUploadSink> sinkPtr = std::make_shared<CDemoResumableUploadSink>(*m_uploadDirectoryAttrPtr, fileName, range.first, range.completeLength);

	CDemoResumableUploadSink::SState state;
	const CDemoResumableUploadSink::OpenResult result = sinkPtr->Open(state);
	if (result == CDemoResumableUploadSink::OR_OK){
		BodyDecision decision;
		decision.mode = BM_STREAM;
		decision.sinkPtr = sinkPtr;

		return decision;
	}

	if (result == CDemoResumableUploadSink::OR_FAILED){
		return CreateRejection(Engine::SC_INTERNAL_ERROR);
	}

	// The client reads from the headers where to go on
	BodyDecision decision = CreateRejection(Engine::SC_CONFLICT);
	decision.rejectHeaders.insert("Upload-Offset", QByteArray::number(state.received));
	if (state.total >= 0){
		decision.rejectHeaders.insert("Upload-Length", QByteArray::number(state.total));
	}

	decision.rejectHeaders.insert("Upload-State", result == CDemoResumableUploadSink::OR_IN_PROGRESS ? "in-progress"
				: (result == CDemoResumableUploadSink::OR_TOTAL_MISMATCH ? "total-mismatch" : "offset-mismatch"));

	return decision;
}


imtrest::IHttpRequestBodyPolicy::BodyDecision CDemoBodyPolicyComp::CreateRejection(int statusCode)
{
	BodyDecision decision;
	decision.mode = BM_REJECT;
	decision.rejectStatusCode = statusCode;

	return decision;
}


} // namespace imtrestdemo
