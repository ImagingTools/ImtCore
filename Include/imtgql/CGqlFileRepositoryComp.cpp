#include <imtgql/CGqlFileRepositoryComp.h>


// STL includes
#include <cmath>

// Qt includes
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>

// Acf includes
#include <iprm/TParamsPtr.h>
#include <iprm/IIdParam.h>
#include <ifile/CFileNameParam.h>

// ImtCore includes
#include <imtbase/IObjectCollectionIterator.h>
#include <imtrepo/CFileCollectionItem.h>


namespace imtgql
{


// public methods

// reimplemented(imtbase::IMetaInfoCreator)

CGqlFileRepositoryComp::TypeIds CGqlFileRepositoryComp::GetSupportedTypeIds() const
{
	TypeIds supportedValues;
	const int typesCount = m_supportedTypeListAttrPtr.GetCount();
	for (int typeIndex = 0; typeIndex < typesCount; ++typeIndex){
		supportedValues << m_supportedTypeListAttrPtr[typeIndex];
	}

	return supportedValues;
}


bool CGqlFileRepositoryComp::CreateMetaInfo(const istd::IChangeable* dataPtr, const QByteArray& typeId, idoc::MetaInfoPtr& metaInfoPtr) const
{
	if (!GetSupportedTypeIds().contains(typeId)){
		SendErrorMessage(0, QString("Unsupported file type id: '%1'").arg(QString(typeId)), "CreateMetaInfo", __LINE__);

		I_CRITICAL();

		return false;
	}

	const imtgql::CGqlObject* dataParams = dynamic_cast<const imtgql::CGqlObject*>(dataPtr);
	if (dataParams == nullptr){
		SendErrorMessage(0, "Unable to find data object");

		I_CRITICAL();

		return false;
	}

	metaInfoPtr.SetPtr(new idoc::CStandardDocumentMetaInfo);

	QDateTime creationTime = QDateTime::currentDateTimeUtc();
	metaInfoPtr->SetMetaInfo(imtbase::IObjectCollection::MIT_LAST_OPERATION_TIME, creationTime);
	metaInfoPtr->SetMetaInfo(imtbase::IObjectCollection::MIT_INSERTION_TIME, creationTime);

	const QString tempFilePath = GetFilePathFromRequestQueue((*dataParams)["Id"].toByteArray());
	if (m_hashGeneratorCompPtr.IsValid()){
		QFile tempFile(tempFilePath);
		if (tempFile.open(QIODevice::ReadOnly)){
			const QByteArray fileData = tempFile.readAll();
			const QByteArray checksumm = m_hashGeneratorCompPtr->GenerateHash(fileData);
			metaInfoPtr->SetMetaInfo(idoc::IDocumentMetaInfo::MIT_CONTENT_CHECKSUM, checksumm);
		}
	}

	metaInfoPtr->SetMetaInfo(imtbase::IObjectCollection::MIT_REVISION, (*dataParams)["Revision"].toLongLong());

	return true;
}


// protected methods

void CGqlFileRepositoryComp::SetErrorToModel(
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



QString CGqlFileRepositoryComp::GetFilePathFromRequestQueue(const QByteArray& queueRequestId) const
{
	QString filePath;
	if (!m_tempRequestCollectionCompPtr.IsValid()){
		SendCriticalMessage(0, "Request collection is invalid");

		I_CRITICAL();

		return filePath;
	}

	const istd::IChangeable* objectPtr = m_tempRequestCollectionCompPtr->GetObjectPtr(queueRequestId);
	if (objectPtr == nullptr){
		SendCriticalMessage(0, QString("Unable to find request with ID: '%1'").arg(QString(queueRequestId)));

		I_CRITICAL();

		return filePath;
	}

	const iprm::IParamsSet* paramsSetPtr = dynamic_cast<const iprm::IParamsSet*>(objectPtr);
	if (paramsSetPtr == nullptr){
		SendCriticalMessage(0, QString("Model has unsupported modelType"));

		I_CRITICAL();

		return filePath;
	}

	iprm::TParamsPtr<ifile::IFileNameParam> fileNameParamPtr(paramsSetPtr, "FilePath");
	if (fileNameParamPtr.IsValid()){
		filePath = fileNameParamPtr->GetPath();
	}

	return filePath;
}


QByteArray CGqlFileRepositoryComp::CreateNewFilePathInRequestQueue(const QString& filePath) const
{
//	m_tempRequestCollectionController

	return "";
}


// reimplemented (imtgql::CGqlRepresentationDataControllerComp)


imtbase::CTreeItemModel* CGqlFileRepositoryComp::GetObject(const CGqlRequest& gqlRequest, QString& errorMessage) const
{
	/// \todo implement insertion into the gqlModel file's data
	I_CRITICAL();

	return nullptr;
}


imtbase::CTreeItemModel* CGqlFileRepositoryComp::InsertObject(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	const QList<imtgql::CGqlObject> inputParams = gqlRequest.GetParams();
	const imtgql::CGqlObject* dataParamsPtr = nullptr;
	for(const imtgql::CGqlObject& gqlObject: inputParams){
		dataParamsPtr = gqlObject.GetFieldArgumentObjectPtr("Data");
		if (dataParamsPtr != nullptr){
			break;
		}
	}

	if (dataParamsPtr == nullptr) {
		errorMessage = QT_TR_NOOP("Unable to extract parameter object from GraphQL-request");

		SendErrorMessage(0, errorMessage);

		I_CRITICAL();

		return nullptr;
	}

	idoc::MetaInfoPtr metaInfoPtr;
	if (!CreateMetaInfo(dataParamsPtr, QByteArray(), metaInfoPtr)){
		errorMessage = QT_TR_NOOP("Unable to create meta information for the data object");

		SendErrorMessage(0, errorMessage);

		I_CRITICAL();

		return nullptr;
	}

	const QByteArray typeId = (*dataParamsPtr)["Type"].toByteArray();
	const QString name = (*dataParamsPtr)["Name"].toString();
	const QString description = (*dataParamsPtr)["Description"].toString();
	const QByteArray insertionType = (*dataParamsPtr)["InsertionType"].toByteArray();

	QString tempFilePath;
	if (insertionType == QByteArrayLiteral("FromQueue")){
		tempFilePath = GetFilePathFromRequestQueue((*dataParamsPtr)["Id"].toByteArray());
	}
	// From GQL-Reauest (use base-64 encoding) (default)
	else {
//		tempFilePath =
		QFile tempFile(tempFilePath);
		if (!tempFile.open(QIODevice::WriteOnly)){
			errorMessage = QT_TR_NOOP("Unable to create file");
			SendErrorMessage(0, errorMessage);

			I_CRITICAL();

			return nullptr;
		}

		const QByteArray::FromBase64Result decodeResult = QByteArray::fromBase64Encoding((*dataParamsPtr)["FileData"].toByteArray());
		if (decodeResult.decodingStatus != QByteArray::Base64DecodingStatus::Ok){
			errorMessage = QT_TR_NOOP(QString("Decoding error '%1'").arg(QString::number(int(decodeResult.decodingStatus))));
			SendErrorMessage(0, errorMessage);

			I_CRITICAL();

			return nullptr;
		}

		const qint64 writtenBytes = tempFile.write(decodeResult.decoded);
		if (writtenBytes != decodeResult.decoded.size()){
			SendErrorMessage(0, QString("Unsable to remove file: '%1'. error: '%2'. Written: %3 OF %4").arg(
									tempFilePath,
									tempFile.errorString(),
									QString::number(writtenBytes),
									QString::number(decodeResult.decoded.size())));
			SendErrorMessage(0, errorMessage);

			I_CRITICAL();

			return nullptr;
		}

		tempFile.close();
//		CreateNewFilePathInRequestQueue();
	}

	const QByteArray createdFileId = m_fileObjectCollectionCompPtr->InsertFile(
				tempFilePath,
				typeId,
				name,
				description,
				QByteArray(),
				metaInfoPtr.GetPtr());

	QFile tempFile(tempFilePath);
	if (tempFile.exists()){
		if (!tempFile.remove()){
			SendErrorMessage(0, QString("Unsable to remove file: '%1'. error: '%2'").arg(tempFilePath, tempFile.errorString()));
		}
	}

	if(createdFileId.isEmpty()){
		errorMessage = QT_TR_NOOP("Unable to import file");
		SendErrorMessage(0, errorMessage);

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


imtbase::CTreeItemModel* CGqlFileRepositoryComp::GetMetaInfo(const CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_fileObjectCollectionCompPtr.IsValid()){
		errorMessage = QString("500: File collection is not set");
		SendErrorMessage(500, errorMessage, "CGqlFileRepositoryComp::GetMetaInfo");

		return nullptr;
	}

	const QByteArray fileId = GetObjectIdFromInputParams(gqlRequest.GetParams());
	imtrepo::CFileCollectionItem item;
	const bool isFileInfoExists = m_fileObjectCollectionCompPtr->GetFileInfo(fileId, item);
	if (!isFileInfoExists){
		errorMessage = QT_TR_NOOP(QString("400: Unable to get a file with ID: '%1'.").arg(qPrintable(fileId)));
		SendErrorMessage(400, errorMessage, "CGqlFileRepositoryComp::GetObject");

		return nullptr;
	}

	idoc::MetaInfoPtr fileMetaInfoPtr = item.GetContentsMetaInfo();
	if (!fileMetaInfoPtr.IsValid()){
		errorMessage = QT_TR_NOOP(QString("500: Unable to get a meta info for file with ID: '%1'.").arg(qPrintable(fileId)));
		SendErrorMessage(500, errorMessage, "CGqlFileRepositoryComp::GetObject");

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");
	dataModelPtr->SetData("Id", QString(fileId));
	dataModelPtr->SetData("Type", item.GetTypeId());
	dataModelPtr->SetData("Name", item.GetName());

	// add metainfo
	imtbase::CTreeItemModel* metaInfoModePtr = dataModelPtr->AddTreeModel("MetaInfo");
	const idoc::CStandardDocumentMetaInfo::MetaInfoTypes metaInfoTypes = fileMetaInfoPtr->GetMetaInfoTypes();
	for (const int& metaInfoType: qAsConst(metaInfoTypes)){
		QVariant metaInfoValue = fileMetaInfoPtr->GetMetaInfo(metaInfoType);
		if (metaInfoValue.isValid()){
			metaInfoModePtr->SetData(QByteArray::number(metaInfoType), metaInfoValue);
		}
	}

	return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CGqlFileRepositoryComp::ListObjects(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to get list objects. Internal error.");

		SendErrorMessage(0, errorMessage, "CGqlFileRepositoryComp::ListObjects");

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


imtbase::CTreeItemModel* CGqlFileRepositoryComp::DeleteObject(
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


bool CGqlFileRepositoryComp::SetupGqlItem(
			const imtgql::CGqlRequest& gqlRequest,
			imtbase::CTreeItemModel& model,
			int itemIndex,
			const QByteArray& objectId,
			QString& errorMessage) const
{
	bool retVal = false;

	const QByteArrayList metaInfoIdList = GetInformationIds(gqlRequest, "items");
	idoc::MetaInfoPtr elementMetaInfoPtr = m_objectCollectionCompPtr->GetDataMetaInfo(objectId);
	if (!elementMetaInfoPtr.IsValid()){
		errorMessage.append("\nUnable to get file metainfo");

		SendErrorMessage(0, QString("Unable to get metainfo from file: '%1'").arg(QString(objectId)), "SetupGqlItem", __LINE__);

		return retVal;
	}

	for (const QByteArray& metaInfoId: metaInfoIdList){
		QVariant elementInformation;

		if (metaInfoId == QByteArrayLiteral("Id")){
			elementInformation = objectId;
		}
		else if (metaInfoId == QByteArrayLiteral("Name")){
			m_objectCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME);
		}
		else if (metaInfoId == QByteArrayLiteral("Description")){
			m_objectCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_DESCRIPTION);
		}
		else if (metaInfoId == QByteArrayLiteral("Added")){
			QDateTime creationTime = elementMetaInfoPtr->GetMetaInfo(imtbase::IObjectCollection::MIT_LAST_OPERATION_TIME).toDateTime().toUTC();
			elementInformation = creationTime.toString(Qt::ISODate);
		}
		else if (metaInfoId == QByteArrayLiteral("LastModified")){
			QDateTime modificationTime = elementMetaInfoPtr->GetMetaInfo(imtbase::IObjectCollection::MIT_LAST_OPERATION_TIME).toDateTime().toUTC();
			elementInformation = modificationTime.toString(Qt::ISODate);
		}
		else if (metaInfoId == QByteArrayLiteral("ChecksumAlgorithm")){
			elementInformation = QByteArrayLiteral("MD5");
		}
		else if (metaInfoId == QByteArrayLiteral("ChecksumValue")){
			elementInformation = elementMetaInfoPtr->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_CONTENT_CHECKSUM);
		}
		else if (metaInfoId == QByteArrayLiteral("Version")){
			elementInformation = elementMetaInfoPtr->GetMetaInfo(imtbase::IObjectCollection::MIT_REVISION);
		}
		else {
			elementInformation = GetObjectInformation(metaInfoId, objectId);
		}
		if (!elementInformation.isNull()){
			retVal = retVal && model.SetData(metaInfoId, elementInformation, itemIndex);
		}
	}

	return retVal;
}


// reimplemented (icomp::CComponentBase)

void CGqlFileRepositoryComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (!m_fileObjectCollectionCompPtr.IsValid()){
		SendCriticalMessage(0, "Invalid file object collection component");

		I_CRITICAL();
	}

	if (!m_tempRequestCollectionCompPtr.IsValid()){
		SendCriticalMessage(0, "Invalid request collection component");

		I_CRITICAL();
	}
}

} // namespace imtgql


