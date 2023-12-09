#include<imtrest/CHttpFileBufferComp.h>


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>
#include <QtCore/QUuid>
#include <QtCore/QTimer>

// ACF includes
#include <istd/CSystem.h>
#include <iprm/CParamsSet.h>
#include <ifile/CFileNameParam.h>

// ImtCore includes
#include <imtrest/IProtocolEngine.h>
#include <imtrepo/CFileCollectionItem.h>


namespace imtrest
{


// protected methods

ConstResponsePtr CHttpFileBufferComp::CreateDefaultErrorResponse(const QByteArray& errorString, IProtocolEngine::StatusCode statusCode, const CHttpRequest& request) const
{
	istd::IInformationProvider::InformationCategory informationCategory = istd::IInformationProvider::IC_ERROR;

	// Server-side errors are critical. Must NOT occurred
	switch (statusCode){
	case IProtocolEngine::StatusCode:: SC_INTERNAL_ERROR:
	case IProtocolEngine::StatusCode:: SC_INTERNAL_SERVER_ERROR:
	case IProtocolEngine::StatusCode:: SC_NOT_IMPLEMENTED:
	case IProtocolEngine::StatusCode:: SC_BAD_GATEWAY:
	case IProtocolEngine::StatusCode:: SC_SERVICE_UNAVAILABLE:
	case IProtocolEngine::StatusCode:: SC_GATEWAY_TIMEOUT:
	case IProtocolEngine::StatusCode:: SC_HTTP_VERSION_NOT_SUPPORTED:
	case IProtocolEngine::StatusCode:: SC_VARIANT_ALSO_NEGOTIATES:
	case IProtocolEngine::StatusCode:: SC_INSUFFICIENT_STORAGE:
	case IProtocolEngine::StatusCode:: SC_LOOP_DETECTED:
	case IProtocolEngine::StatusCode:: SC_BANDWIDTH_LIMIT_EXCEEDED:
	case IProtocolEngine::StatusCode:: SC_NOT_EXTENDED:
	case IProtocolEngine::StatusCode:: SC_NETWORK_AUTHENTICATION_REQUIRED:
	case IProtocolEngine::StatusCode:: SC_UNKNOWN_ERROR:
	case IProtocolEngine::StatusCode:: SC_WEB_SERVER_IS_DOWN:
	case IProtocolEngine::StatusCode:: SC_CONNECTION_TIMED_OUT:
	case IProtocolEngine::StatusCode:: SC_ORIGIN_IS_UNREACHABLE:
	case IProtocolEngine::StatusCode:: SC_A_TIMEOUT_OCCURRED:
	case IProtocolEngine::StatusCode:: SC_SSL_HANDSHAKE_FAILED:
	case IProtocolEngine::StatusCode:: SC_INVALID_SSL_CERTIFICATE:
		informationCategory = istd::IInformationProvider::IC_CRITICAL;
		Q_ASSERT_X(false, __FILE__, QString("Critical error! %1").arg(QString(errorString)).toLocal8Bit());

		break;
	default:
		break;
	}

	SendLogMessage(informationCategory, statusCode, errorString, QString());

	ConstResponsePtr retVal;

	retVal.SetPtr(
				request.GetProtocolEngine().CreateResponse(
				request,
				statusCode,
				errorString,
				QByteArray("text/plain; charset=utf-8")));

	return retVal;
}


// reimplemented (icomp::CComponentBase)

void CHttpFileBufferComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_tempDirectoryPathCompPtr.IsValid()){
		istd::CSystem::EnsurePathExists(m_tempDirectoryPathCompPtr->GetPath());
	}

	/// \todo add observe tempDirectoryPath to clean temp files
}


void CHttpFileBufferComp::OnComponentDestroyed()
{
	if (m_tempDirectoryPathCompPtr.IsValid()){
		istd::CSystem::RemoveDirectory(m_tempDirectoryPathCompPtr->GetPath());
	}

	BaseClass::OnComponentDestroyed();
}


// reimplemented (CHttpServletCompBase)

ConstResponsePtr CHttpFileBufferComp::OnGet(
			const QByteArray& commandId,
			const IRequest::CommandParams& commandParams,
			const HeadersMap& headers,
			const CHttpRequest& request) const
{
	if (!m_tempFileCollectionCompPtr.IsValid()){
		return CreateDefaultErrorResponse("Temp collection is invalid", IProtocolEngine::SC_INTERNAL_SERVER_ERROR, request);
	}

	QByteArray requestedFileId = commandId;
	QByteArray commandIdBase = *m_commandIdAttrPtr;
	if (commandIdBase.endsWith('*')){
		commandIdBase.chop(1);
	}

	requestedFileId = requestedFileId.replace(commandIdBase, "");

	while (requestedFileId.startsWith('/')){
		requestedFileId.remove(0,1);
	}

	while (requestedFileId.endsWith('/')){
		requestedFileId.chop(1);
	}

	QString filePath;

	// try to use direct access to the file from collection
	if (m_fileObjectCollectionCompPtr.IsValid()){
		imtrepo::CFileCollectionItem fileInfo;
		bool isFileInfoExists = m_fileObjectCollectionCompPtr->GetFileInfo(requestedFileId, fileInfo);
		if (!isFileInfoExists){
			I_CRITICAL();

			return CreateDefaultErrorResponse(QT_TR_NOOP(QString("Unable to find file with ID: '%1'").arg(QString(requestedFileId)).toUtf8()),IProtocolEngine::SC_NOT_FOUND, request);
		}

		filePath = fileInfo.GetFilePath();
	}
	else {
		const istd::IChangeable* objectPtr = m_tempFileCollectionCompPtr->GetObjectPtr(requestedFileId);
		if (objectPtr == nullptr){
			I_CRITICAL();

			return CreateDefaultErrorResponse(QT_TR_NOOP(QString("Unable to find request with ID: '%1'").arg(QString(requestedFileId)).toUtf8()),IProtocolEngine::SC_NOT_FOUND, request);
		}

		const iprm::IParamsSet* paramsSetPtr = dynamic_cast<const iprm::IParamsSet*>(objectPtr);
		if (paramsSetPtr == nullptr){
			return CreateDefaultErrorResponse(QT_TR_NOOP("Model has unsupported modelType"), IProtocolEngine::SC_INTERNAL_ERROR, request);
		}

		iprm::TParamsPtr<ifile::IFileNameParam> fileNameParamPtr(paramsSetPtr, "FilePath");
		if (!fileNameParamPtr.IsValid()){
			return CreateDefaultErrorResponse(QT_TR_NOOP("Model has unsupported modelType"), IProtocolEngine::SC_INTERNAL_ERROR,request);
		}

		filePath = fileNameParamPtr->GetPath();
	}

	QFile requestedFile(filePath);
	if (!requestedFile.open(QIODevice::ReadOnly)){
		return CreateDefaultErrorResponse(
					(QString(QT_TR_NOOP("Unable to open file: [%1]'%2'. Error: '%3'")).arg(QString(requestedFileId), filePath, requestedFile.errorString())).toUtf8(),
					IProtocolEngine::SC_INTERNAL_ERROR,
					request);
	}

	/// \todo change it to HTTP-Range request expect: \code "Range: bytes=0-1023" and reply: \code Content-Range: <unit> <range-start>-<range-end>/<size> \note don't forget status code \code SC_PARTIAL_CONTENT
	qint64 offset = 0;
	if (commandParams.contains("Offset")){
		I_CRITICAL();

		bool isNumber = false;
		offset = commandParams["Offset"].toLongLong(&isNumber);
		if (!isNumber){
			return CreateDefaultErrorResponse(
						QString(QT_TR_NOOP("Offset has wrong value '%1'")).arg(QString(commandParams["Offset"])).toUtf8(),
						IProtocolEngine::SC_BAD_REQUEST,
						request);
		}
	}

	/// \todo change it to HTTP-Range request
	qint64 limit = requestedFile.size() - offset;
	if (commandParams.contains("Limit")){
		I_CRITICAL();

		bool isNumber = false;
		limit = commandParams["Limit"].toLongLong(&isNumber);
		if (!isNumber){
			return CreateDefaultErrorResponse(
						QString(QT_TR_NOOP("Limit has wrong value '%1'")).arg(QString(commandParams["Limit"])).toUtf8(),
						IProtocolEngine::SC_BAD_REQUEST,
						request);
		}
	}

	requestedFile.seek(offset);
	const QByteArray payload = requestedFile.read(limit);

	/// \todo add Content-Disposition header to define file name \code Content-Disposition: attachment; filename="filename.jpg"

	return ConstResponsePtr(request.GetProtocolEngine().CreateResponse(request, IProtocolEngine::SC_OK, payload, QByteArrayLiteral("application/octet-stream; charset=utf-8")));
}


ConstResponsePtr CHttpFileBufferComp::OnPost(
			const QByteArray& commandId,
			const IRequest::CommandParams& commandParams,
			const HeadersMap& headers,
			const CHttpRequest& request) const
{
	if (!m_tempFileCollectionCompPtr.IsValid()){
		return CreateDefaultErrorResponse("Temp collection is invalid", IProtocolEngine::SC_INTERNAL_SERVER_ERROR, request);
	}

	QString fileDirectoryPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "HttpFileBuffer";
	if (m_tempDirectoryPathCompPtr.IsValid()){
		fileDirectoryPath = m_tempDirectoryPathCompPtr->GetPath();
	}

	QFile tempFile(QDir::cleanPath(fileDirectoryPath + "/" + QUuid::createUuid().toString(QUuid::Id128)));
	Q_ASSERT(!tempFile.exists());
	if (!tempFile.open(QIODevice::WriteOnly)){
		return CreateDefaultErrorResponse(QString(QT_TR_NOOP("Unable to save file: %1")).arg(tempFile.errorString()).toUtf8(), IProtocolEngine::SC_INTERNAL_SERVER_ERROR, request);
	}

	quint64 writtenBytes = tempFile.write(request.GetBody());
	if (writtenBytes != request.GetBody().size()){
		return CreateDefaultErrorResponse(QString(QT_TR_NOOP("Saved file is corrupted!: %1")).arg(tempFile.errorString()).toUtf8(), IProtocolEngine::SC_INTERNAL_SERVER_ERROR, request);
	}
	tempFile.close();

	istd::TDelPtr<iprm::CParamsSet> paramsSetPtr = new iprm::CParamsSet;
	ifile::CFileNameParam* fileNameParamPtr = new ifile::CFileNameParam;
	fileNameParamPtr->SetPath(tempFile.fileName());
	paramsSetPtr->SetEditableParameter("FilePath", fileNameParamPtr, true);

	imtbase::CObjectCollectionBase::DataPtr valuePtr(paramsSetPtr.GetPtr());

	/// \todo get name from request
	const QByteArray createdFileId = m_tempFileCollectionCompPtr->InsertNewObject(
				"File",
				tempFile.fileName(),
				QString(),
				valuePtr);

	if (createdFileId.isEmpty()){
		return CreateDefaultErrorResponse("Unable to save file", IProtocolEngine::SC_INTERNAL_SERVER_ERROR, request);
	}

	const QByteArray payload = createdFileId;

	return ConstResponsePtr(request.GetProtocolEngine().CreateResponse(request, IProtocolEngine::SC_OK, payload, QByteArrayLiteral("application/json; charset=utf-8")));
}


ConstResponsePtr CHttpFileBufferComp::OnHead(
			const QByteArray& commandId,
			const IRequest::CommandParams& commandParams,
			const HeadersMap& headers,
			const CHttpRequest& request) const
{
	/// \todo add file info data return + range-based request headers
	return ConstResponsePtr(request.GetProtocolEngine().CreateResponse(
				request,
				IProtocolEngine::SC_NOT_IMPLEMENTED,
				"This method is not implemented yet",
				QByteArrayLiteral("text/plain; charset=utf-8")));
}


} // namespace imtrest


