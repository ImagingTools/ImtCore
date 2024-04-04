#include <imtgqlrepo/CGqlRemoteFileRepositoryComp.h>


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QUuid>

// Acf includes
#include <istd/CSystem.h>
#include <ilog/CMessage.h>
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imtgql/CGqlRequest.h>

// imtgqlrepo DDL includes
#include <GeneratedFiles/imtgqlrepo/DDL/Cpp/Globals.h>

// imtgqlrepo SDL includes
#include <GeneratedFiles/imtgqlrepo/SDL/CInsertRequest.h>
#include <GeneratedFiles/imtgqlrepo/SDL/CRemoveFileRequest.h>
#include <GeneratedFiles/imtgqlrepo/SDL/CFileDataRequest.h>
#include <GeneratedFiles/imtgqlrepo/SDL/CFileDataReply.h>
#include <GeneratedFiles/imtgqlrepo/SDL/CInsertFileResponse.h>


namespace imtgqlrepo
{


// public methods

// reimplemented (imtbase::ICollectionInfo)

int CGqlRemoteFileRepositoryComp::GetElementsCount(
			const iprm::IParamsSet* /*selectionParamsPtr*/,
			ilog::IMessageConsumer* logPtr) const
{
	FileMetaInfoList metaInfoList = ApiGetListOfObjects(logPtr);

	return metaInfoList.count();
}


CGqlRemoteFileRepositoryComp::Ids CGqlRemoteFileRepositoryComp::GetElementIds(
			int /*offset*/,
			int /*count*/,
			const iprm::IParamsSet* /*selectionParamsPtr*/,
			ilog::IMessageConsumer* logPtr) const
{
	CGqlRemoteFileRepositoryComp::Ids retVal;
	const FileMetaInfoList metaInfoList = ApiGetListOfObjects(logPtr);

	for (const sdl::CFileMetaInfo& metaInfo: metaInfoList){
		retVal << metaInfo.GetId();
	}

	return retVal;
}


QVariant CGqlRemoteFileRepositoryComp::GetElementInfo(
			const Id& elementId,
			int infoType,
			ilog::IMessageConsumer* logPtr) const
{

	/// \todo use \c GetFileInfo instead
	sdl::CFileMetaInfo fileMetaInfo = GetMetaInfoById(elementId, logPtr);

	switch (infoType){
	case EIT_DESCRIPTION:
		return fileMetaInfo.GetDescription();

	case EIT_NAME:
		return fileMetaInfo.GetName();

	case EIT_ENABLED:
		return true;
	}

	return QVariant();
}


idoc::MetaInfoPtr CGqlRemoteFileRepositoryComp::GetElementMetaInfo(
			const Id& elementId,
			ilog::IMessageConsumer* logPtr) const
{
	/// \todo use \c GetFileInfo instead
	sdl::CFileMetaInfo fileMetaInfo = GetMetaInfoById(elementId, logPtr);

	idoc::MetaInfoPtr metaInfoPtr(new idoc::CStandardDocumentMetaInfo);
	metaInfoPtr->SetMetaInfo(imtbase::IObjectCollection::MIT_LAST_OPERATION_TIME, fileMetaInfo.GetLastModified());
	metaInfoPtr->SetMetaInfo(imtbase::IObjectCollection::MIT_INSERTION_TIME, fileMetaInfo.GetAdded());
	metaInfoPtr->SetMetaInfo(imtbase::IObjectCollection::MIT_REVISION, fileMetaInfo.GetVersion());

	return metaInfoPtr;
}


bool CGqlRemoteFileRepositoryComp::SetElementName(
			const Id& /*elementId*/,
			const QString& /*name*/,
			ilog::IMessageConsumer* /*logPtr*/)
{
	return false;
}


bool CGqlRemoteFileRepositoryComp::SetElementDescription(
			const Id& /*elementId*/,
			const QString& /*description*/,
			ilog::IMessageConsumer* /*logPtr*/)
{
	return false;
}


bool CGqlRemoteFileRepositoryComp::SetElementEnabled(
			const Id& /*elementId*/,
			bool /*isEnabled*/,
			ilog::IMessageConsumer* /*logPtr*/)
{
	return false;
}


// reimplemented (imtbase::IObjectCollectionInfo)

idoc::MetaInfoPtr CGqlRemoteFileRepositoryComp::GetDataMetaInfo(const Id& objectId) const
{
	/// \todo use \c GetFileInfo instead
	sdl::CFileMetaInfo fileMetaInfo = GetMetaInfoById(objectId);

	idoc::MetaInfoPtr metaInfoPtr(new idoc::CStandardDocumentMetaInfo);
	metaInfoPtr->SetMetaInfo(imtbase::IObjectCollection::MIT_LAST_OPERATION_TIME, fileMetaInfo.GetLastModified());
	metaInfoPtr->SetMetaInfo(imtbase::IObjectCollection::MIT_INSERTION_TIME, fileMetaInfo.GetAdded());
	metaInfoPtr->SetMetaInfo(imtbase::IObjectCollection::MIT_REVISION, fileMetaInfo.GetVersion());

	return metaInfoPtr;
}


// reimplemented (imtrepo::IFileCollectionInfo)


bool CGqlRemoteFileRepositoryComp::GetFileInfo(
			const QByteArray& /*objectId*/,
			imtrepo::IFileCollectionItem& /*collectionItem*/) const
{
	return false;
}


CGqlRemoteFileRepositoryComp::FileCollectionLayout CGqlRemoteFileRepositoryComp::GetCollectionFileLayout() const
{
	return FCL_FOLDER;
}


QString CGqlRemoteFileRepositoryComp::GetCollectionRootFolder() const
{
	return QString();
}


// reimplemented (imtbase::IObjectCollection)

int CGqlRemoteFileRepositoryComp::GetOperationFlags(const Id& /*elementId*/) const
{
	return OF_SUPPORT_INSERT | OF_SUPPORT_DELETE;
}


CGqlRemoteFileRepositoryComp::Id CGqlRemoteFileRepositoryComp::InsertNewObject(
			const QByteArray& /*typeId*/,
			const QString& /*name*/,
			const QString& /*description*/,
			DataPtr /*defaultValuePtr*/,
			const Id& /*proposedElementId*/,
			const idoc::IDocumentMetaInfo* /*dataMetaInfoPtr*/,
			const idoc::IDocumentMetaInfo* /*elementMetaInfoPtr*/,
			const imtbase::IOperationContext* /*operationContextPtr*/)
{
	return Id();
}


bool CGqlRemoteFileRepositoryComp::RemoveElement(
			const Id& elementId,
			const imtbase::IOperationContext* /*operationContextPtr*/)
{
	const char* const logTag = "RemoveElement";
	if (!m_graphQlApiClientCompPtr.IsValid()){
		SendCriticalMessage("Unable to create request. API client is invalid", logTag);
		Q_ASSERT_X(false, logTag, "Unable to create request. API client is invalid");

		return false;
	}

	sdl::CRemoveFileRequest removeRequest;
	removeRequest.SetId(elementId);

	imtgql::CGqlObject removeRequestParam;
	const bool isAdded = removeRequest.AddMeToGraphQlObject(removeRequestParam);
	if (!isAdded){
		SendCriticalMessage("Unable to add request to ghaphQL request", "RemoveElement");
		Q_ASSERT_X(false, "RemoveElement", "Unable to add request to ghaphQL request");

		return false;
	}

	imtclientgql::IGqlClient::GqlRequestPtr requestPtr = CreateApiRequest(imtgql::IGqlRequest::RT_MUTATION, RequestTypeId::s_RemoveObject, {removeRequestParam});

	imtclientgql::IGqlClient::GqlResponsePtr responcePtr = m_graphQlApiClientCompPtr->SendRequest(requestPtr);
	if (responcePtr.isNull()){
		SendErrorMessage("Unable to exec request", logTag);

		return false;
	}

	if (!CheckResponceOnErrors(*responcePtr)){
		SendErrorMessage("Unable to remove file", logTag);

		return false;
	}


	return true;
}


// reimplemented (imtrepo::IFileObjectCollection)

QString CGqlRemoteFileRepositoryComp::GetFile(
			const QByteArray& objectId,
			const QString& targetFilePath) const
{
	const char* const logTag = "GetFile";

	sdl::CFileDataRequest fileDataRequest;
	fileDataRequest.SetId(objectId);

	imtgql::CGqlObject fileDataRequestParam;
	const bool isAdded = fileDataRequest.AddMeToGraphQlObject(fileDataRequestParam);
	if (!isAdded){
		SendCriticalMessage("Unable to add request to ghaphQL request", logTag);
		Q_ASSERT_X(false, logTag, "Unable to add request to ghaphQL request");

		return QString();
	}

	imtclientgql::IGqlClient::GqlRequestPtr requestPtr = CreateApiRequest(imtgql::IGqlRequest::RT_MUTATION, RequestTypeId::s_RemoveObject, {fileDataRequestParam});

	imtclientgql::IGqlClient::GqlResponsePtr responcePtr = m_graphQlApiClientCompPtr->SendRequest(requestPtr);
	if (responcePtr.isNull()){
		SendErrorMessage("Unable to exec request", logTag);

		return QString();
	}

	if (!CheckResponceOnErrors(*responcePtr)){
		SendErrorMessage("Unable to get file", logTag);

		return QString();
	}

	const QByteArray responceData = responcePtr->GetData();
	imtbase::CTreeItemModel treeModel;
	const bool isCreated = treeModel.CreateFromJson(responceData);
	if (!isCreated){
		SendErrorMessage("Unable to create model from data", logTag);

		return QString();
	}

	if (treeModel.GetItemsCount() != 1){
		SendErrorMessage("Invalid data received", logTag);

		return QString();
	}

	sdl::CFileDataReply fileDataReply;
	bool isFileDataReaded = sdl::CFileDataReply::ReadFromModel(fileDataReply, treeModel);
	if (!isFileDataReaded){
		SendErrorMessage("Unable to read reply data", logTag);

		return QString();
	}

	QString storedFilePath = targetFilePath;
	if (storedFilePath.isEmpty()){
		static QString tempPath = QDir::tempPath() + QStringLiteral("/ImtCore/GqlRemoteFileRepository/");
		storedFilePath = tempPath + QUuid::createUuid().toString(QUuid::WithoutBraces);
		if (!istd::CSystem::EnsurePathExists(tempPath)){
			SendCriticalMessage("Unable to create temp directory", logTag);

			return QString();
		}
	}

	QFile targetFile(storedFilePath);
	if (!targetFile.open(QIODevice::WriteOnly)){
		QString errorString = QString ("Unable to create temp directory '%1'").arg(storedFilePath);
		SendCriticalMessage(errorString, logTag);
		Q_ASSERT_X(false, logTag, errorString.toLocal8Bit());

		return QString();
	}

	const QByteArray encodedFileData = fileDataReply.GetData();

	const QByteArray::FromBase64Result decodeResult = QByteArray::fromBase64Encoding(encodedFileData);
	if (decodeResult.decodingStatus != QByteArray::Base64DecodingStatus::Ok){
		QString errorMessage = QString("Decoding error '%1'").arg(QString::number(int(decodeResult.decodingStatus)));
		targetFile.close();
		SendErrorMessage(errorMessage, logTag);
		I_CRITICAL();

		return nullptr;
	}

	targetFile.write(decodeResult.decoded);
	targetFile.close();

	return storedFilePath;
}


QByteArray CGqlRemoteFileRepositoryComp::InsertFile(
			const QString& filePath,
			const QByteArray& objectTypeId,
			const QString& objectName,
			const QString& objectDescription,
			const QByteArray& proposedObjectId,
			const idoc::IDocumentMetaInfo* /*dataMetaInfoPtr*/,
			const idoc::IDocumentMetaInfo* /*collectionItemMetaInfoPtr*/)
{
	const char* const logTag = "InsertFile";
	sdl::CInsertRequest insertRequest;
	insertRequest.SetFileType(objectTypeId);
	insertRequest.SetFileName(objectName);
	insertRequest.SetDescription(objectDescription);
	insertRequest.SetFileId(proposedObjectId);
	insertRequest.SetChecksumAlgorithm("MD5");
	insertRequest.SetChecksumValue(QString());
	insertRequest.SetVersion("1.0");

	QFile fileToInsert(filePath);
	if (!fileToInsert.open(QIODevice::ReadOnly)){
		SendCriticalMessage(QString("Unable to open file '%1'").arg(filePath), logTag);
		I_CRITICAL();

		return QByteArray();
	}
	QByteArray fileData = fileToInsert.readAll();
	fileData = fileData.toBase64();

	insertRequest.SetFileData(fileData);
	insertRequest.SetTotalSize(fileData.length());

	imtgql::CGqlObject insertRequestParam;
	const bool isAdded = insertRequest.AddMeToGraphQlObject(insertRequestParam);
	if (!isAdded){
		SendCriticalMessage("Unable to add request to ghaphQL request", logTag);
		Q_ASSERT_X(false, logTag, "Unable to add request to ghaphQL request");

		return QByteArray();
	}

	imtclientgql::IGqlClient::GqlRequestPtr requestPtr = CreateApiRequest(imtgql::IGqlRequest::RT_MUTATION, RequestTypeId::s_InsertObject, {insertRequestParam});
	imtclientgql::IGqlClient::GqlResponsePtr responcePtr = m_graphQlApiClientCompPtr->SendRequest(requestPtr);
	if (responcePtr.isNull()){
		SendErrorMessage("Unable to exec request", logTag);

		return QByteArray();
	}

	if (!CheckResponceOnErrors(*responcePtr)){
		SendErrorMessage("Unable to get file", logTag);

		return QByteArray();
	}

	const QByteArray responceData = responcePtr->GetData();
	imtbase::CTreeItemModel treeModel;
	const bool isCreated = treeModel.CreateFromJson(responceData);
	if (!isCreated){
		SendErrorMessage("Unable to create model from data", logTag);

		return QByteArray();
	}

	if (treeModel.GetItemsCount() != 1){
		SendErrorMessage("Invalid data received", logTag);

		return QByteArray();
	}

	sdl::CInsertFileResponse fileInsertRsponse;
	bool isFileDataReaded = sdl::CInsertFileResponse::ReadFromModel(fileInsertRsponse, treeModel);
	if (!isFileDataReaded){
		SendErrorMessage("Unable to read reply data", logTag);

		return QByteArray();
	}

	return fileInsertRsponse.GetId();
}


bool CGqlRemoteFileRepositoryComp::UpdateFile(
			const QString& /*filePath*/,
			const QByteArray& /*objectId*/)
{
	return false;
}


// protected methods

bool CGqlRemoteFileRepositoryComp::SendInfoMessage(
	const QString& message,
	const QString& messageSource,
	ilog::IMessageConsumer* slaveLogPtr) const
{
	bool retVal = SendLogMessage(istd::IInformationProvider::IC_INFO, 0, message, messageSource);
	if (slaveLogPtr != nullptr){
		auto messagePtr = istd::TSmartPtr<const istd::IInformationProvider>(new ilog::CMessage(istd::IInformationProvider::IC_INFO, 0, message, messageSource));
		slaveLogPtr->AddMessage(messagePtr);
	}

	return retVal;
}


bool CGqlRemoteFileRepositoryComp::SendWarningMessage(
	const QString& message,
	const QString& messageSource,
	ilog::IMessageConsumer* slaveLogPtr) const
{
	bool retVal = SendLogMessage(istd::IInformationProvider::IC_WARNING, 0, message, messageSource);
	if (slaveLogPtr != nullptr){
		auto messagePtr = istd::TSmartPtr<const istd::IInformationProvider>(new ilog::CMessage(istd::IInformationProvider::IC_WARNING, 0, message, messageSource));
		slaveLogPtr->AddMessage(messagePtr);
	}

	return retVal;
}


bool CGqlRemoteFileRepositoryComp::SendErrorMessage(
	const QString& message,
	const QString& messageSource,
	ilog::IMessageConsumer* slaveLogPtr) const
{
	bool retVal = SendLogMessage(istd::IInformationProvider::IC_ERROR, 0, message, messageSource);
	if (slaveLogPtr != nullptr){
		auto messagePtr = istd::TSmartPtr<const istd::IInformationProvider>(new ilog::CMessage(istd::IInformationProvider::IC_ERROR, 0, message, messageSource));
		slaveLogPtr->AddMessage(messagePtr);
	}

	return retVal;
}


bool CGqlRemoteFileRepositoryComp::SendCriticalMessage(
	const QString& message,
	const QString& messageSource,
	ilog::IMessageConsumer* slaveLogPtr) const
{
	bool retVal = SendLogMessage(istd::IInformationProvider::IC_CRITICAL, 0, message, messageSource);
	if (slaveLogPtr != nullptr){
		auto messagePtr = istd::TSmartPtr<const istd::IInformationProvider>(new ilog::CMessage(istd::IInformationProvider::IC_CRITICAL, 0, message, messageSource));
		slaveLogPtr->AddMessage(messagePtr);
	}

	return retVal;
}


// reimplemented (icomp::CComponentBase)

void CGqlRemoteFileRepositoryComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
}


// private methods

bool CGqlRemoteFileRepositoryComp::CheckResponceOnErrors(const imtgql::IGqlResponse& response, ilog::IMessageConsumer* slaveLogPtr) const
{
	const QStringList errorMessageList = response.GetErrorMessageList();
	if (errorMessageList.isEmpty()){
		return true;
	}

	SendErrorMessage("Errors occured during the request exec:", "CheckResponceOnErrors");
	for (const QString& errorMessage: errorMessageList){
		SendErrorMessage(QString("\t- %1").arg(errorMessage), "CheckResponceOnErrors");
	}

	return false;
}


imtclientgql::IGqlClient::GqlRequestPtr CGqlRemoteFileRepositoryComp::CreateApiRequest(
		imtgql::IGqlRequest::RequestType requestType,
		const QString& requestName,
		const QList<imtgql::CGqlObject> paramList,
		ilog::IMessageConsumer* slaveLogPtr) const
{
	imtgql::CGqlRequest* requestPtr = new imtgql::CGqlRequest(requestType, requestName.toUtf8());
	imtgql::CGqlObject requestIdField(requestName.toUtf8());
	requestPtr->AddField(requestIdField);

	for (const imtgql::CGqlObject& field: paramList){
		requestPtr->AddParam(field);
	}

	return imtclientgql::IGqlClient::GqlRequestPtr(requestPtr);
}


CGqlRemoteFileRepositoryComp::FileMetaInfoList CGqlRemoteFileRepositoryComp::ApiGetListOfObjects(ilog::IMessageConsumer* slaveLogPtr) const
{
	const QString logTag = QStringLiteral("ApiGetListOfObjects");
	if (!m_graphQlApiClientCompPtr.IsValid()){
		SendErrorMessage("Unable to create request. API client is invalid", logTag, slaveLogPtr);
		Q_ASSERT_X(false, "CreateApiRequest", "Unable to create request. API client is invalid");

		return FileMetaInfoList();
	}

	FileMetaInfoList retVal;

	imtclientgql::IGqlClient::GqlRequestPtr requestPtr = CreateApiRequest(imtgql::IGqlRequest::RT_QUERY, RequestTypeId::s_GetObjects, {}, slaveLogPtr);
	if (requestPtr.isNull()){
		SendErrorMessage("Unable to create request", logTag, slaveLogPtr);

		return retVal;
	}

	imtclientgql::IGqlClient::GqlResponsePtr responcePtr = m_graphQlApiClientCompPtr->SendRequest(requestPtr);
	if (responcePtr.isNull()){
		SendErrorMessage("Unable to exec request", logTag, slaveLogPtr);

		return retVal;
	}

	const QByteArray responceData = responcePtr->GetData();
	imtbase::CTreeItemModel treeModel;
	const bool isCreated = treeModel.CreateFromJson(responceData);
	if (!isCreated){
		SendErrorMessage("Unable to create model from data", logTag, slaveLogPtr);
		Q_ASSERT_X(false, "CreateApiRequest", "Unable to create model from data");

		return retVal;
	}

	for (int i = 0; i < treeModel.GetItemsCount(); ++i){
		sdl::CFileMetaInfo metaInfo;
		const bool isReaded = sdl::CFileMetaInfo::ReadFromModel(metaInfo, treeModel);
		if (isReaded){
			retVal << metaInfo;
		}
		else {
			SendErrorMessage("Unable to read from model data", logTag, slaveLogPtr);
			Q_ASSERT_X(false, "CreateApiRequest", "Unable to read from model data");

			retVal.clear();

			break;
		}
	}

	return retVal;
}


sdl::CFileMetaInfo CGqlRemoteFileRepositoryComp::GetMetaInfoById(
			const Id& elementId,
			ilog::IMessageConsumer* slaveLogPtr) const
{
	const FileMetaInfoList fileMetaInfoList = ApiGetListOfObjects(slaveLogPtr);

	for (const sdl::CFileMetaInfo& fileMetaInfo: fileMetaInfoList){
		if (fileMetaInfo.GetId() == elementId){
			return fileMetaInfo;
		}
	}

	SendErrorMessage(QString("Unable to find element by ID '%1'").arg(elementId), "GetMetaInfoById", slaveLogPtr);

	return sdl::CFileMetaInfo();
}


} // namespace imtgqlrepo


