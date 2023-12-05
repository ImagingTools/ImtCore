#include<imtrest/CHttpFileBufferComp.h>


//Qt includes
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>
#include <QtCore/QUuid>
#include <QtCore/QTimer>

// Acf includes
#include <istd/CSystem.h>
#include <iprm/CParamsSet.h>
#include <ifile/CFileNameParam.h>

// ImtCore includes
#include <imtrest/IProtocolEngine.h>


namespace imtrest
{


// protected methods

ConstResponsePtr CHttpFileBufferComp::CreateDefaultErrorResponse(const QByteArray& errorString, IProtocolEngine::StatusCode statusCode, const CHttpRequest& request) const
{
	SendErrorMessage(0, errorString);

	ConstResponsePtr retVal;

	retVal.SetPtr(request.GetProtocolEngine().CreateResponse(
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

	/// \todo add observe the model to clean temp files immediately
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
		const QString errorString = "Temp collection is invalid";
		I_CRITICAL();

		return CreateDefaultErrorResponse(errorString.toUtf8(), IProtocolEngine::SC_INTERNAL_SERVER_ERROR, request);
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

	const istd::IChangeable* objectPtr = m_tempFileCollectionCompPtr->GetObjectPtr(requestedFileId);
	if (objectPtr == nullptr){
		SendCriticalMessage(0, QString("Unable to find request with ID: '%1'").arg(QString(requestedFileId)));
		I_CRITICAL();

		return CreateDefaultErrorResponse(QT_TR_NOOP("Unable to find file"),IProtocolEngine::SC_NOT_FOUND, request);
	}

	const iprm::IParamsSet* paramsSetPtr = dynamic_cast<const iprm::IParamsSet*>(objectPtr);
	if (paramsSetPtr == nullptr){
		SendCriticalMessage(0, QString("Model has unsupported modelType"));
		I_CRITICAL();

		return CreateDefaultErrorResponse(QT_TR_NOOP("Model has unsupported modelType"), IProtocolEngine::SC_INTERNAL_ERROR, request);
	}

	iprm::TParamsPtr<ifile::IFileNameParam> fileNameParamPtr(paramsSetPtr, "FilePath");
	if (!fileNameParamPtr.IsValid()){
		SendCriticalMessage(0, QString("Model has unsupported modelType"));
		I_CRITICAL();

		return CreateDefaultErrorResponse(QT_TR_NOOP("Model has unsupported modelType"), IProtocolEngine::SC_INTERNAL_ERROR,request);
	}

	QString filePath = fileNameParamPtr->GetPath();

	QFile requestedFile(filePath);
	if (!requestedFile.open(QIODevice::ReadOnly)){
		SendCriticalMessage(0, QString("Unable to open file: '%1'. Error: '%2'").arg(filePath, requestedFile.errorString()));
		I_CRITICAL();

		return CreateDefaultErrorResponse(
					QString(QT_TR_NOOP("Unable to open file: '%1'. Error: '%2'")).arg(filePath, requestedFile.errorString()).toUtf8(),
					IProtocolEngine::SC_INTERNAL_ERROR,
					request);
	}

	/// \todo change it to HTTP-Range request
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
		const QString errorString = "Temp collection is invalid";
		I_CRITICAL();

		return CreateDefaultErrorResponse(errorString.toUtf8(), IProtocolEngine::SC_INTERNAL_SERVER_ERROR, request);
	}

	QString fileDirectoryPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "HttpFileBuffer";
	if (m_tempDirectoryPathCompPtr.IsValid()){
		fileDirectoryPath = m_tempDirectoryPathCompPtr->GetPath();
	}

	QFile tempFile(QDir::cleanPath(fileDirectoryPath + "/" + QUuid::createUuid().toString(QUuid::Id128)));
	Q_ASSERT(!tempFile.exists());
	if (!tempFile.open(QIODevice::WriteOnly)){
		const QString errorString = QString(QT_TR_NOOP("Unable to save file: %1")).arg(tempFile.errorString());
		I_CRITICAL();

		return CreateDefaultErrorResponse(errorString.toUtf8(), IProtocolEngine::SC_INTERNAL_SERVER_ERROR, request);
	}

	quint64 writtenBytes = tempFile.write(request.GetBody());
	if (writtenBytes != request.GetBody().size()){
		const QString errorString = QString(QT_TR_NOOP("Saved file is corrupted!: %1")).arg(tempFile.errorString());
		SendCriticalMessage(0, errorString);
		I_CRITICAL();

		return CreateDefaultErrorResponse(errorString.toUtf8(), IProtocolEngine::SC_INTERNAL_SERVER_ERROR, request);
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
		valuePtr
		);

	if (createdFileId.isEmpty()){
		I_CRITICAL();

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
				request, IProtocolEngine::SC_NOT_IMPLEMENTED, "This method is not implemented yet", QByteArrayLiteral("text/plain; charset=utf-8")));
}


} // namespace imtrest


