// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrestdemo/CDemoUploadServletComp.h>


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>

// ImtCore includes
#include <imtrestdemo/CDemoBodyPolicyComp.h>
#include <imtrestdemo/CDemoResumableUploadSink.h>
#include <imtrestdemo/CDemoUploadSink.h>
#include <imtrestdemo/DemoServletTools.h>


namespace imtrestdemo
{


// protected methods

// reimplemented (imtrest::CHttpServletCompBase)

imtrest::ConstResponsePtr CDemoUploadServletComp::OnGet(
			const QByteArray& commandId,
			const imtrest::IRequest::CommandParams& /*commandParams*/,
			const HeadersMap& /*headers*/,
			const imtrest::CHttpRequest& request) const
{
	typedef imtrest::IProtocolEngine Engine;

	// /upload/<name>: how much of the file the server has (the client asks it to resume an upload that was broken)
	if (commandId != "/upload" && commandId != "/upload/"){
		return CreateStateResponse(request, CDemoBodyPolicyComp::GetUploadFileName(commandId));
	}

	QJsonArray files;
	QJsonArray uploading;

	const QFileInfoList entries = QDir(*m_uploadDirectoryAttrPtr).entryInfoList(QDir::Files | QDir::NoDotAndDotDot, QDir::Name);
	for (const QFileInfo& entry : entries){
		const QString fileName = entry.fileName();

		// The temporary files of the uploads that are not finished are not files of the server
		if (fileName.endsWith(QLatin1String(CDemoResumableUploadSink::s_metaSuffix))){
			continue;
		}

		if (fileName.endsWith(QLatin1String(CDemoResumableUploadSink::s_dataSuffix))){
			const QString name = fileName.left(fileName.size() - int(qstrlen(CDemoResumableUploadSink::s_dataSuffix)));
			const CDemoResumableUploadSink::SState state = CDemoResumableUploadSink::ReadState(*m_uploadDirectoryAttrPtr, name);

			QJsonObject part;
			part.insert("name", name);
			part.insert("received", double(state.received));
			part.insert("total", double(state.total));
			part.insert("inProgress", state.isInProgress);
			uploading.append(part);

			continue;
		}

		QJsonObject file;
		file.insert("name", fileName);
		file.insert("size", double(entry.size()));
		files.append(file);
	}

	QJsonObject result;
	result.insert("files", files);
	result.insert("uploading", uploading);

	return DemoServletTools::CreateJsonResponse(request.GetProtocolEngine(), imtrest::IProtocolEngine::SC_OK, result);
}


imtrest::ConstResponsePtr CDemoUploadServletComp::OnPut(
			const QByteArray& /*commandId*/,
			const imtrest::IRequest::CommandParams& /*commandParams*/,
			const HeadersMap& /*headers*/,
			const imtrest::CHttpRequest& request) const
{
	return SaveFile(request);
}


imtrest::ConstResponsePtr CDemoUploadServletComp::OnPost(
			const QByteArray& /*commandId*/,
			const imtrest::IRequest::CommandParams& /*commandParams*/,
			const HeadersMap& /*headers*/,
			const imtrest::CHttpRequest& request) const
{
	return SaveFile(request);
}


imtrest::ConstResponsePtr CDemoUploadServletComp::OnDelete(
			const QByteArray& /*commandId*/,
			const imtrest::IRequest::CommandParams& /*commandParams*/,
			const HeadersMap& /*headers*/,
			const imtrest::CHttpRequest& request) const
{
	typedef imtrest::IProtocolEngine Engine;

	const QString name = CDemoBodyPolicyComp::GetUploadFileName(request.GetCommandId());
	if (name.isEmpty()){
		return DemoServletTools::CreateTextResponse(request.GetProtocolEngine(), Engine::SC_BAD_REQUEST, "The name of the file is not allowed");
	}

	// The file and the part of it that is not received to the end
	const bool isFileRemoved = QFile::remove(QDir(*m_uploadDirectoryAttrPtr).absoluteFilePath(name));
	const bool isPartRemoved = CDemoResumableUploadSink::RemoveUpload(*m_uploadDirectoryAttrPtr, name);
	if (!isFileRemoved && !isPartRemoved){
		return DemoServletTools::CreateTextResponse(request.GetProtocolEngine(), Engine::SC_NOT_FOUND, "There is no such file");
	}

	return DemoServletTools::CreateResponse(request.GetProtocolEngine(), Engine::SC_NO_CONTENT, QByteArray(), QByteArray());
}


// private methods

imtrest::ConstResponsePtr CDemoUploadServletComp::SaveFile(const imtrest::CHttpRequest& request) const
{
	typedef imtrest::IProtocolEngine Engine;

	// A part of an upload that can be resumed
	const std::shared_ptr<CDemoResumableUploadSink> resumableSinkPtr = std::dynamic_pointer_cast<CDemoResumableUploadSink>(request.GetBodySink());
	if (resumableSinkPtr != nullptr){
		return CreatePartResponse(request, *resumableSinkPtr);
	}

	// The body is already written to the file by the policy, here its result is only read
	const std::shared_ptr<CDemoUploadSink> sinkPtr = std::dynamic_pointer_cast<CDemoUploadSink>(request.GetBodySink());
	if (sinkPtr == nullptr){
		return DemoServletTools::CreateTextResponse(request.GetProtocolEngine(), Engine::SC_INTERNAL_SERVER_ERROR, "The upload policy is not configured for this path");
	}

	const QString name = QFileInfo(sinkPtr->GetFilePath()).fileName();

	QJsonObject result;
	result.insert("name", name);
	result.insert("size", double(sinkPtr->GetReceivedSize()));
	result.insert("sha256", QString::fromLatin1(sinkPtr->GetSha256()));

	imtrest::IResponse::Headers headers;
	headers.insert("Location", "/files/uploaded/" + QUrl::toPercentEncoding(name));

	return DemoServletTools::CreateJsonResponse(request.GetProtocolEngine(), Engine::SC_CREATED, result, headers);
}


imtrest::ConstResponsePtr CDemoUploadServletComp::CreatePartResponse(const imtrest::CHttpRequest& request, const CDemoResumableUploadSink& sink) const
{
	typedef imtrest::IProtocolEngine Engine;

	const QString name = QFileInfo(sink.GetFilePath()).fileName();

	QJsonObject result;
	result.insert("name", name);
	result.insert("received", double(sink.GetFileSize()));
	result.insert("total", double(sink.GetTotal()));
	result.insert("complete", sink.IsComplete());

	imtrest::IResponse::Headers headers;
	headers.insert("Upload-Offset", QByteArray::number(sink.GetFileSize()));
	headers.insert("Upload-Length", QByteArray::number(sink.GetTotal()));

	if (!sink.IsComplete()){
		// The part is accepted, the file is not complete yet
		return DemoServletTools::CreateJsonResponse(request.GetProtocolEngine(), Engine::SC_ACCEPTED, result, headers);
	}

	result.insert("size", double(sink.GetFileSize()));
	result.insert("sha256", QString::fromLatin1(sink.GetSha256()));
	headers.insert("Location", "/files/uploaded/" + QUrl::toPercentEncoding(name));

	return DemoServletTools::CreateJsonResponse(request.GetProtocolEngine(), Engine::SC_CREATED, result, headers);
}


imtrest::ConstResponsePtr CDemoUploadServletComp::CreateStateResponse(const imtrest::CHttpRequest& request, const QString& name) const
{
	typedef imtrest::IProtocolEngine Engine;

	if (name.isEmpty()){
		return DemoServletTools::CreateTextResponse(request.GetProtocolEngine(), Engine::SC_BAD_REQUEST, "The name of the file is not allowed");
	}

	QJsonObject result;
	result.insert("name", name);

	imtrest::IResponse::Headers headers;

	const CDemoResumableUploadSink::SState state = CDemoResumableUploadSink::ReadState(*m_uploadDirectoryAttrPtr, name);
	const QFileInfo fileInfo(QDir(*m_uploadDirectoryAttrPtr).absoluteFilePath(name));
	if (state.exists){
		result.insert("received", double(state.received));
		result.insert("total", double(state.total));
		result.insert("complete", false);
		result.insert("inProgress", state.isInProgress);
		headers.insert("Upload-Offset", QByteArray::number(state.received));
		if (state.total >= 0){
			headers.insert("Upload-Length", QByteArray::number(state.total));
		}
	}
	else if (fileInfo.isFile()){
		result.insert("received", double(fileInfo.size()));
		result.insert("total", double(fileInfo.size()));
		result.insert("complete", true);
		result.insert("inProgress", false);
		headers.insert("Upload-Offset", QByteArray::number(fileInfo.size()));
		headers.insert("Upload-Length", QByteArray::number(fileInfo.size()));
	}
	else{
		result.insert("error", "There is no such upload");

		return DemoServletTools::CreateJsonResponse(request.GetProtocolEngine(), Engine::SC_NOT_FOUND, result);
	}

	return DemoServletTools::CreateJsonResponse(request.GetProtocolEngine(), Engine::SC_OK, result, headers);
}


} // namespace imtrestdemo
