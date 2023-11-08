#include<imtrest/CHttpFileBufferComp.h>


//Qt includes
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>
#include <QtCore/QUuid>

// Acf includes
#include <iprm/CParamsSet.h>
#include <ifile/CFileNameParam.h>

// ImtCore includes
#include <imtrest/IProtocolEngine.h>


namespace imtrest
{


ConstResponsePtr CHttpFileBufferComp::OnGet(
			const QByteArray& commandId,
			const IRequest::CommandParams& commandParams,
			const HeadersMap& headers,
			const CHttpRequest& request) const
{

	int statusCode = 200;
	QByteArray payload;

	return ConstResponsePtr(request.GetProtocolEngine().CreateResponse(request, statusCode, payload, QByteArrayLiteral("application/json; charset=utf-8")));
}

ConstResponsePtr CHttpFileBufferComp::OnPost(
			const QByteArray& commandId,
			const IRequest::CommandParams& commandParams,
			const HeadersMap& headers,
			const CHttpRequest& request) const
{
	int statusCode = imtrest::IProtocolEngine::SC_OK;

	QString fileDirectoryPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "HttpFileBuffer";
	if (m_tempDirectoryPathCompPtr.IsValid()){
		fileDirectoryPath = m_tempDirectoryPathCompPtr->GetPath();
	}

	QFile tempFile(QDir::cleanPath(fileDirectoryPath + "/" + QUuid::createUuid().toString(QUuid::Id128)));
	Q_ASSERT(!tempFile.exists());
	if (!tempFile.open(QIODevice::WriteOnly)){
		statusCode = imtrest::IProtocolEngine::SC_INTERNAL_SERVER_ERROR;
		const QString errorString = QString(QT_TR_NOOP("Unable to save file: %1")).arg(tempFile.errorString());
		SendErrorMessage(0, errorString);
		I_CRITICAL();

		return CreateDefaultErrorResponse(errorString.toUtf8(), request);
	}

	quint64 writtenBytes = tempFile.write(request.GetBody());
	if (writtenBytes != request.GetBody().size()){
		statusCode = imtrest::IProtocolEngine::SC_INTERNAL_SERVER_ERROR;
		const QString errorString = QString(QT_TR_NOOP("Saved file is corrupted!: %1")).arg(tempFile.errorString());
		SendCriticalMessage(0, errorString);
		I_CRITICAL();

		return CreateDefaultErrorResponse(errorString.toUtf8(), request);
	}
	tempFile.close();

	istd::TDelPtr<iprm::CParamsSet> paramsSetPtr = new iprm::CParamsSet;
	ifile::CFileNameParam* fileNameParamPtr = new ifile::CFileNameParam;
	fileNameParamPtr->SetPath(tempFile.fileName());
	paramsSetPtr->SetEditableParameter("FilePath", fileNameParamPtr, true);

	imtbase::CObjectCollectionBase::DataPtr valuePtr(paramsSetPtr.GetPtr());

	// add TypeId, Name if error
	const QByteArray createdFileId = m_tempFileCollectionCompPtr->InsertNewObject(
		QByteArray(),
		QString(),
		QString(),
		valuePtr
		);

	if (createdFileId.isEmpty()){
		statusCode = imtrest::IProtocolEngine::SC_INTERNAL_SERVER_ERROR;
		SendErrorMessage(0, "Unable to create file");
		I_CRITICAL();

		return CreateDefaultErrorResponse("Unable to save file", request);
	}

	const QByteArray payload = createdFileId;

	return ConstResponsePtr(request.GetProtocolEngine().CreateResponse(request, statusCode, payload, QByteArrayLiteral("application/json; charset=utf-8")));
}



} // namespace imtrest


