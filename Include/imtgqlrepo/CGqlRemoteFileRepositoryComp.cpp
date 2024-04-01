#include <imtgqlrepo/CGqlRemoteFileRepositoryComp.h>


// Qt includes

// Acf includes
#include <ilog/CMessage.h>
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imtgql/CGqlRequest.h>

#include <GeneratedFiles/imtgqlrepo/DDL/Cpp/Globals.h>


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
	/// \todo implement it
	I_CRITICAL();

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
	return OF_SUPPORT_INSERT | OF_SUPPORT_DELETE | OF_SUPPORT_USING;
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
			const Id& /*elementId*/,
			const imtbase::IOperationContext* /*operationContextPtr*/)
{
	/// \todo implement it
	I_CRITICAL();

	return false;
}


// reimplemented (imtrepo::IFileObjectCollection)

QString CGqlRemoteFileRepositoryComp::GetFile(
			const QByteArray& /*objectId*/,
			const QString& /*targetFilePath*/) const
{
	/// \todo implement it
	I_CRITICAL();

	return QString();
}


QByteArray CGqlRemoteFileRepositoryComp::InsertFile(
			const QString& /*filePath*/,
			const QByteArray& /*objectTypeId*/,
			const QString& /*objectName*/,
			const QString& /*objectDescription*/,
			const QByteArray& /*proposedObjectId*/,
			const idoc::IDocumentMetaInfo* /*dataMetaInfoPtr*/,
			const idoc::IDocumentMetaInfo* /*collectionItemMetaInfoPtr*/)
{
	/// \todo implement it
	I_CRITICAL();

	return QByteArray();
}


bool CGqlRemoteFileRepositoryComp::UpdateFile(
			const QString& /*filePath*/,
			const QByteArray& /*objectId*/)
{
	/// \todo implement it
	I_CRITICAL();

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

imtclientgql::IGqlClient::GqlRequestPtr CGqlRemoteFileRepositoryComp::CreateApiRequest(
		const QString& requestName,
		ilog::IMessageConsumer* slaveLogPtr	) const
{
	imtgql::CGqlRequest* requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, requestName.toUtf8());
	imtgql::CGqlObject requestIdField(requestName.toUtf8());
	requestPtr->AddField(requestIdField);

	return imtclientgql::IGqlClient::GqlRequestPtr(requestPtr);
}


CGqlRemoteFileRepositoryComp::FileMetaInfoList CGqlRemoteFileRepositoryComp::ApiGetListOfObjects(ilog::IMessageConsumer* slaveLogPtr) const
{
	const QString logTag = QStringLiteral("ApiGetListOfObjects");

	FileMetaInfoList retVal;

	imtclientgql::IGqlClient::GqlRequestPtr requestPtr = CreateApiRequest(RequestTypeId::s_GetObjects, slaveLogPtr);
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


