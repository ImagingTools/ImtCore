#include <imtgqlrepo/CGqlFileRepositoryHandlerComp.h>


// STL includes
#include <cmath>

// Qt includes
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>

// Acf includes
#include <istd/CSystem.h>
#include <iprm/TParamsPtr.h>
#include <iprm/IIdParam.h>
#include <ifile/CFileNameParam.h>

// ImtCore includes
#include <imtbase/IObjectCollectionIterator.h>
#include <imtrepo/CFileCollectionItem.h>

// imtgqlrepo SDL includes
#include <GeneratedFiles/imtgqlrepo/SDL/CFileMetaInfo.h>
#include <GeneratedFiles/imtgqlrepo/SDL/CInsertRequest.h>


namespace imtgqlrepo
{


// public methods

bool CGqlFileRepositoryHandlerComp::CreateMetaInfoFromInsertRequest(const sdl::CInsertRequest& insertRequest, const QByteArray& typeId, idoc::MetaInfoPtr& metaInfoPtr) const
{
	const QByteArray sessionId = insertRequest.GetFileId();
	if (sessionId.isEmpty()){
		return false;
	}
	const QString tempFilePath = m_tempFileManagerCompPtr->GetPath(sessionId, sessionId);
	if (!QFile(tempFilePath).isReadable()){
		return false;
	}

	metaInfoPtr.SetPtr(new idoc::CStandardDocumentMetaInfo);

	QDateTime creationTime = QDateTime::currentDateTimeUtc();
	metaInfoPtr->SetMetaInfo(imtbase::IObjectCollection::MIT_LAST_OPERATION_TIME, creationTime);
	metaInfoPtr->SetMetaInfo(imtbase::IObjectCollection::MIT_INSERTION_TIME, creationTime);
	metaInfoPtr->SetMetaInfo(imtbase::IObjectCollection::MIT_REVISION, insertRequest.GetVersion());

	if (m_hashGeneratorCompPtr.IsValid()){
		QFile tempFile(tempFilePath);
		if (tempFile.open(QIODevice::ReadOnly)){
			const QByteArray fileData = tempFile.readAll();
			const QByteArray checksumm = m_hashGeneratorCompPtr->GenerateHash(fileData);
			metaInfoPtr->SetMetaInfo(idoc::IDocumentMetaInfo::MIT_CONTENT_CHECKSUM, checksumm);
		}
	}

	return true;
}


// reimplemented(imtbase::IMetaInfoCreator)

CGqlFileRepositoryHandlerComp::TypeIds CGqlFileRepositoryHandlerComp::GetSupportedTypeIds() const
{
	TypeIds supportedValues;
	const int typesCount = m_supportedTypeListAttrPtr.GetCount();
	for (int typeIndex = 0; typeIndex < typesCount; ++typeIndex){
		supportedValues << m_supportedTypeListAttrPtr[typeIndex];
	}

	return supportedValues;
}


bool CGqlFileRepositoryHandlerComp::CreateMetaInfo(const istd::IChangeable* dataPtr, const QByteArray& typeId, idoc::MetaInfoPtr& metaInfoPtr) const
{
	if (!GetSupportedTypeIds().contains(typeId)){
		SendErrorMessage(0, QString("Unsupported file type id: '%1'").arg(QString(typeId)), "CreateMetaInfo", __LINE__);

		I_CRITICAL();

		return false;
	}

	const imtgql::CGqlObject* dataParamsPtr = dynamic_cast<const imtgql::CGqlObject*>(dataPtr);
	if (dataParamsPtr == nullptr){
		SendErrorMessage(0, "Unable to find data object");

		I_CRITICAL();

		return false;
	}

	sdl::CInsertRequest insertRequest;
	if (!sdl::CInsertRequest::ReadFromGraphQlObject(insertRequest, *dataParamsPtr)){
		I_CRITICAL();

		return false;
	}

	return CreateMetaInfoFromInsertRequest(insertRequest, typeId, metaInfoPtr);
}


// protected methods

void CGqlFileRepositoryHandlerComp::SetErrorToModel(
	imtbase::CTreeItemModel& rootModel,
	imtbase::CTreeItemModel& dataModel,
	imtbase::CTreeItemModel& itemsModel,
	imtbase::CTreeItemModel& notificationModel,
	const QString& errorMessage)
{
	imtbase::CTreeItemModel* errorsItemModel = rootModel.AddTreeModel("errors");
	errorsItemModel->SetData("message", errorMessage);

	dataModel.SetExternTreeModel("items", &itemsModel);
	dataModel.SetExternTreeModel("notification", &notificationModel);

	rootModel.SetExternTreeModel("data", &dataModel);
}


// reimplemented (imtgql::CGqlRepresentationDataControllerComp)

imtbase::CTreeItemModel* CGqlFileRepositoryHandlerComp::GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	sdl::CFileMetaInfo fileMetaInfo;
	if (!CheckAndCreateClassFromRequest(gqlRequest, fileMetaInfo, errorMessage)){
		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel);
	istd::TDelPtr<imtbase::CTreeItemModel> dataModelPtr(new imtbase::CTreeItemModel);
	istd::TDelPtr<imtbase::CTreeItemModel> itemsModelPtr(new imtbase::CTreeItemModel);
	istd::TDelPtr<imtbase::CTreeItemModel> notificationModelPtr(new imtbase::CTreeItemModel);

	const QByteArray fileId = fileMetaInfo.GetId();
	imtrepo::CFileCollectionItem fileItem;
	if (!m_fileObjectCollectionCompPtr->GetFileInfo(fileId, fileItem)){
		errorMessage = QT_TR_NOOP(QString("400: File with id '%1' does not exsists").arg(QString(fileId)));
		SetErrorToModel(*rootModelPtr, *dataModelPtr.PopPtr(), *itemsModelPtr.PopPtr(), *notificationModelPtr.PopPtr(), errorMessage);
		SendErrorMessage(400, errorMessage);
		I_CRITICAL();

		return rootModelPtr.PopPtr();
	}

	QFile requestedFile(fileItem.GetFilePath());
	if (!requestedFile.open(QIODevice::ReadOnly)){
		errorMessage = QT_TR_NOOP(QString("400: Unable to open file '%1'. Error: %2").arg(QString(requestedFile.fileName()), requestedFile.errorString()));
		SetErrorToModel(*rootModelPtr, *dataModelPtr.PopPtr(), *itemsModelPtr.PopPtr(), *notificationModelPtr.PopPtr(), errorMessage);
		SendErrorMessage(400, errorMessage);
		I_CRITICAL();

		return rootModelPtr.PopPtr();
	}

	QByteArray fileData = requestedFile.readAll().toBase64();
	dataModelPtr->SetData("Id", fileId);
	dataModelPtr->SetData("Data", fileData);

	rootModelPtr->SetExternTreeModel("data", dataModelPtr.PopPtr());

	return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CGqlFileRepositoryHandlerComp::InsertObject(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::CInsertRequest insertRequest;
	if (!CheckAndCreateClassFromRequest(gqlRequest, insertRequest, errorMessage)){
		return nullptr;
	}

	QByteArray newFileId = QUuid::createUuid().toString(QUuid::Id128).toLatin1();
	const QByteArray typeId = !insertRequest.GetFileType().isEmpty() ? insertRequest.GetFileType() : m_supportedTypeListAttrPtr[0];
	const QString name = insertRequest.GetFileName();
	const QString description = insertRequest.GetDescription();
	const QByteArray insertionType = insertRequest.GetInsertionType();

	idoc::MetaInfoPtr metaInfoPtr;
	if (!CreateMetaInfo(&gqlRequest.GetParams()[0], typeId, metaInfoPtr)){
		errorMessage = QT_TR_NOOP("500: Unable to create meta information for the data object");
		SendErrorMessage(500, errorMessage);
		I_CRITICAL();

		return nullptr;
	}


	QString tempFilePath;
	if (insertionType == QByteArrayLiteral("FromQueue") && !insertRequest.GetFileId().isEmpty()){
		newFileId = insertRequest.GetFileId();
		tempFilePath = m_tempFileManagerCompPtr->GetPath(newFileId, newFileId);
	}
	// From GQL-Request (use base-64 encoding) (default)
	else {
		const QByteArray tempFileSesionId = m_tempFileManagerCompPtr->BeginSession(QString(), newFileId);
		const QByteArray tempFileId = m_tempFileManagerCompPtr->AddFileItem(tempFileSesionId, name, newFileId);
		tempFilePath = m_tempFileManagerCompPtr->GetPath(tempFileSesionId, tempFileId);
		QFile tempFile(tempFilePath);
		if (!tempFile.open(QIODevice::WriteOnly)){
			errorMessage = QT_TR_NOOP("500: Unable to create file");
			SendErrorMessage(500, errorMessage);
			I_CRITICAL();

			return nullptr;
		}

		const QByteArray::FromBase64Result decodeResult = QByteArray::fromBase64Encoding(insertRequest.GetFileData());
		if (decodeResult.decodingStatus != QByteArray::Base64DecodingStatus::Ok){
			errorMessage = QT_TR_NOOP(QString("400: Decoding error '%1'").arg(QString::number(int(decodeResult.decodingStatus))));
			SendErrorMessage(400, errorMessage);
			tempFile.close();
			m_tempFileManagerCompPtr->FinishSession(tempFileSesionId);

			I_CRITICAL();

			return nullptr;
		}

		const qint64 writtenBytes = tempFile.write(decodeResult.decoded);
		if (writtenBytes != decodeResult.decoded.size()){
			SendErrorMessage(500, QString("500: Writtem file corrupted: '%1'. error: '%2'. Written: %3 OF %4").arg(
									tempFilePath,
									tempFile.errorString(),
									QString::number(writtenBytes),
									QString::number(decodeResult.decoded.size())));
			tempFile.close();
			m_tempFileManagerCompPtr->FinishSession(tempFileSesionId);

			I_CRITICAL();

			return nullptr;
		}

		if (m_hashGeneratorCompPtr.IsValid()){
			const QByteArray checksumm = m_hashGeneratorCompPtr->GenerateHash(decodeResult.decoded);
			metaInfoPtr->SetMetaInfo(idoc::IDocumentMetaInfo::MIT_CONTENT_CHECKSUM, checksumm);
		}
	}

	const QByteArray createdFileId = m_fileObjectCollectionCompPtr->InsertFile(
				tempFilePath,
				typeId,
				name,
				description,
				QByteArray(),
				metaInfoPtr.GetPtr());

	m_tempFileManagerCompPtr->FinishSession(newFileId);

	if(createdFileId.isEmpty()){
		errorMessage = QT_TR_NOOP("500: Unable to import file");
		SendErrorMessage(500, errorMessage);
		I_CRITICAL();

		return nullptr;
	}

	imtbase::CTreeItemModel* itemsModel = new imtbase::CTreeItemModel;
	itemsModel->SetData("Id", createdFileId);

	imtbase::CTreeItemModel* dataModel = new imtbase::CTreeItemModel;
	dataModel->SetExternTreeModel("items", itemsModel);

	imtbase::CTreeItemModel* rootModelPtr = new imtbase::CTreeItemModel;
	rootModelPtr->SetExternTreeModel("data", dataModel);

	return rootModelPtr;
}


imtbase::CTreeItemModel* CGqlFileRepositoryHandlerComp::GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to get list objects. Internal error.");
		SendErrorMessage(500, errorMessage, "CGqlFileRepositoryComp::ListObjects");

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModel = new imtbase::CTreeItemModel;
	imtbase::CTreeItemModel* itemsModel = new imtbase::CTreeItemModel;
	imtbase::CTreeItemModel* notificationModel = new imtbase::CTreeItemModel;

	const QByteArray elementId = GetObjectIdFromInputParams(gqlRequest.GetParams());
	int itemIndex = itemsModel->InsertNewItem();
	if (itemIndex >= 0){
		if (!SetupGqlItem(gqlRequest, *itemsModel, itemIndex, elementId, errorMessage)){
			SetErrorToModel(*rootModelPtr, *dataModel, *itemsModel, *notificationModel, errorMessage);

			return rootModelPtr.PopPtr();
		}
	}

	itemsModel->SetIsArray(true);
	dataModel->SetExternTreeModel("items", itemsModel);
	dataModel->SetExternTreeModel("notification", notificationModel);
	rootModelPtr->SetExternTreeModel("data", dataModel);

	return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CGqlFileRepositoryHandlerComp::ListObjects(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to get list objects. Internal error.");
		SendErrorMessage(500, errorMessage, "CGqlFileRepositoryComp::ListObjects");

		return nullptr;
	}
	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModel = new imtbase::CTreeItemModel;
	imtbase::CTreeItemModel* itemsModel = new imtbase::CTreeItemModel;
	imtbase::CTreeItemModel* notificationModel = new imtbase::CTreeItemModel;

	if (!errorMessage.isEmpty()){
		SetErrorToModel(*rootModelPtr, *dataModel, *itemsModel, *notificationModel, errorMessage);
		return rootModelPtr.PopPtr();
	}

	const imtgql::CGqlObject* viewParamsGql = nullptr;
	const QList<imtgql::CGqlObject> inputParams = gqlRequest.GetParams();
	if (inputParams.size() > 0){
		viewParamsGql = inputParams.at(0).GetFieldArgumentObjectPtr("viewParams");
	}

	iprm::CParamsSet filterParams;

	int offset = 0;
	int count = -1;

	if (viewParamsGql != nullptr){
		offset = viewParamsGql->GetFieldArgumentValue("Offset").toInt();
		count = viewParamsGql->GetFieldArgumentValue("Count").toInt();
		PrepareFilters(gqlRequest, *viewParamsGql, filterParams);
	}

	int elementsCount = m_objectCollectionCompPtr->GetElementsCount(&filterParams);
	int pagesCount = std::ceil(elementsCount / (double)count);
	if (pagesCount <= 0){
		pagesCount = 1;
	}

	notificationModel->SetData("PagesCount", pagesCount);
	notificationModel->SetData("TotalCount", elementsCount);

	imtbase::IObjectCollection::Ids elementIds = m_objectCollectionCompPtr->GetElementIds(offset, count, &filterParams);
	for (const imtbase::IObjectCollection::Id& elementId: std::as_const(elementIds)){
		int itemIndex = itemsModel->InsertNewItem();
		if (itemIndex >= 0){
			if (!SetupGqlItem(gqlRequest, *itemsModel, itemIndex, elementId, errorMessage)){
				SetErrorToModel(*rootModelPtr, *dataModel, *itemsModel, *notificationModel, errorMessage);

				return rootModelPtr.PopPtr();
			}
		}
		else{
			errorMessage = QString("Unable to insert new item to model.");
			SetErrorToModel(*rootModelPtr, *dataModel, *itemsModel, *notificationModel, errorMessage);

			return rootModelPtr.PopPtr();
		}
	}

	itemsModel->SetIsArray(true);
	dataModel->SetExternTreeModel("items", itemsModel);
	dataModel->SetExternTreeModel("notification", notificationModel);
	rootModelPtr->SetExternTreeModel("data", dataModel);

	return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CGqlFileRepositoryHandlerComp::DeleteObject(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = "No collection component was set";
		SendErrorMessage(0, errorMessage, "CGqlFileRepositoryComp");

		return nullptr;
	}

	const QList<imtgql::CGqlObject> inputParams = gqlRequest.GetParams();
	QByteArray objectId = GetObjectIdFromInputParams(inputParams);
	if (objectId.isEmpty()){
		errorMessage = QObject::tr("No object-ID could not be extracted from the request");
		SendErrorMessage(0, errorMessage, "CGqlFileRepositoryComp");

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::IOperationContext* operationContextPtr = nullptr;
	if (m_operationContextControllerCompPtr.IsValid()){
		operationContextPtr = m_operationContextControllerCompPtr->CreateOperationContext(imtbase::IDocumentChangeGenerator::OT_REMOVE, gqlRequest);
	}
	if (m_objectCollectionCompPtr->RemoveElement(objectId, operationContextPtr)){
		imtbase::CTreeItemModel* notificationModel = new imtbase::CTreeItemModel();
		notificationModel->SetData("Id", objectId);

		imtbase::CTreeItemModel* dataModel = new imtbase::CTreeItemModel();
		dataModel->SetExternTreeModel("removedNotification", notificationModel);

		rootModelPtr->SetExternTreeModel("data", dataModel);
	}
	else{
		errorMessage = QObject::tr("Can't remove object: %1").arg(QString(objectId));
	}

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsModel = rootModelPtr->AddTreeModel("errors");
		errorsModel->SetData("message", errorMessage);
	}

	return rootModelPtr.PopPtr();
}


bool CGqlFileRepositoryHandlerComp::SetupGqlItem(
			const imtgql::CGqlRequest& /*gqlRequest*/,
			imtbase::CTreeItemModel& model,
			int /*itemIndex*/,
			const QByteArray& objectId,
			QString& errorMessage) const
{
	idoc::MetaInfoPtr elementMetaInfoPtr = m_objectCollectionCompPtr->GetDataMetaInfo(objectId);
	if (!elementMetaInfoPtr.IsValid()){
		errorMessage.append("\nUnable to get file metainfo");

		SendErrorMessage(0, QString("Unable to get metainfo from file: '%1'").arg(QString(objectId)), "SetupGqlItem", __LINE__);

		return false;
	}

	sdl::CFileMetaInfo fileMetaInfo;
	fileMetaInfo.SetId(objectId);
	fileMetaInfo.SetName(m_objectCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString());
	fileMetaInfo.SetDescription(m_objectCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString());
	QDateTime creationTime = elementMetaInfoPtr->GetMetaInfo(imtbase::IObjectCollection::MIT_LAST_OPERATION_TIME).toDateTime().toUTC();
	fileMetaInfo.SetAdded(creationTime.toString(Qt::ISODate));
	QDateTime modificationTime = elementMetaInfoPtr->GetMetaInfo(imtbase::IObjectCollection::MIT_LAST_OPERATION_TIME).toDateTime().toUTC();
	fileMetaInfo.SetLastModified(modificationTime.toString(Qt::ISODate));
	fileMetaInfo.SetChecksumAlgorithm(QStringLiteral("MD5"));
	fileMetaInfo.SetChecksumValue(elementMetaInfoPtr->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_CONTENT_CHECKSUM).toString());
	fileMetaInfo.SetVersion(elementMetaInfoPtr->GetMetaInfo(imtbase::IObjectCollection::MIT_REVISION).toString());

	bool retVal = fileMetaInfo.AddMeToModel(model, 0);
	Q_ASSERT_X(retVal, "SetupGqlItem", "Unable to add metainfo to model");

	return retVal;
}


// reimplemented (icomp::CComponentBase)

void CGqlFileRepositoryHandlerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (!m_fileObjectCollectionCompPtr.IsValid()){
		SendCriticalMessage(0, "Invalid file object collection component");

		I_CRITICAL();
	}

	if (!m_tempFileManagerCompPtr.IsValid()){
		SendCriticalMessage(0, "Invalid request collection component");

		I_CRITICAL();
	}
}


void CGqlFileRepositoryHandlerComp::OnComponentDestroyed()
{
	if (m_tempFileManagerCompPtr.IsValid()){
		m_tempFileManagerCompPtr->ResetAllSessions();
	}

	BaseClass::OnComponentDestroyed();
}


// private methods

template<class T>
bool CGqlFileRepositoryHandlerComp::CheckAndCreateClassFromRequest(const imtgql::CGqlRequest& gqlRequest, T& object, QString& errorMessage) const
{
	QList<imtgql::CGqlObject> requestParams = gqlRequest.GetParams();
	if (requestParams.count() != 1){
		errorMessage = QT_TR_NOOP(QString("400: invalid data format"));
		SendErrorMessage(400, errorMessage);
		I_CRITICAL();

		return false;
	}

	imtgql::CGqlObject dataObject = requestParams[0];
	if (!T::ReadFromGraphQlObject(object, dataObject)){
		errorMessage = QT_TR_NOOP(QString("400: invalid data provided"));
		SendErrorMessage(400, errorMessage);
		I_CRITICAL();

		return false;
	}

	return true;
}



} // namespace imtgqlrepo

