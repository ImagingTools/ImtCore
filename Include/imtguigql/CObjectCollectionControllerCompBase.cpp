#include <imtguigql/CObjectCollectionControllerCompBase.h>


// ImtCore includes
#include <imtqml/CCommandDataEnumProviderComp.h>
#include <iqtgui/CHierarchicalCommand.h>
#include <idoc/CStandardDocumentMetaInfo.h>
#include <imtgui/CObjectCollectionViewDelegate.h>


namespace imtguigql
{


// reimplemented (imtgql::IGqlRepresentationDataController)

imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::CreateResponse(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (m_modelIdsCompPtr.FindValue(gqlRequest.GetCommandId()) == -1){
		return nullptr;
	}

	const QList<imtgql::CGqlObject>* inputParamsPtr = gqlRequest.GetParams();
	imtgql::CGqlObject gqlObject;

	int operationType = OT_UNKNOWN;
	if (!GetOperationFromRequest(gqlRequest, gqlObject, errorMessage, operationType)){
		return nullptr;
	}

	Q_ASSERT(operationType != OT_UNKNOWN);

	switch (operationType){
	case OT_NEW:
		return InsertObject(*inputParamsPtr, gqlObject, errorMessage);
	case OT_GET:
		return GetObject(*inputParamsPtr, gqlObject, errorMessage);
	case OT_UPDATE:
		return UpdateObject(*inputParamsPtr, gqlObject, errorMessage);
	case OT_DELETE:
		return DeleteObject(*inputParamsPtr, gqlObject, errorMessage);
	case OT_RENAME:
		return RenameObject(*inputParamsPtr, gqlObject, errorMessage);
	case OT_SET_DESCRIPTION:
		return SetObjectDescription(*inputParamsPtr, gqlObject, errorMessage);
	case OT_LIST:
		return ListObjects(*inputParamsPtr, gqlObject, errorMessage);
	case OT_HEADERS:
		return GetHeaders(*inputParamsPtr, gqlObject, errorMessage);
	}

	return nullptr;
}


// protected methods

bool CObjectCollectionControllerCompBase::GetOperationFromRequest(
			const imtgql::CGqlRequest& gqlRequest, 
			imtgql::CGqlObject& gqlObject,
			QString& errorMessage,
			int& operationType) const
{
	const QList<imtgql::CGqlObject>* fieldList = gqlRequest.GetFields();
	if(fieldList == nullptr){
		return false;
	}

	int count = fieldList->count();
	for (int i = 0; i < count; i++){
		if (fieldList->at(i).GetId() == "headers"){
			gqlObject = fieldList->at(i);
			operationType = OT_HEADERS;
			return true;
		}
		if (fieldList->at(i).GetId() == "items"){
			gqlObject = fieldList->at(i);
			operationType = OT_LIST;
			return true;
		}
		if (fieldList->at(i).GetId() == "item"){
			gqlObject = fieldList->at(i);
			operationType = OT_GET;
			return true;
		}
		if (fieldList->at(i).GetId() == "addedNotification"){
			gqlObject = fieldList->at(i);
			operationType = OT_NEW;
			return true;
		}
		if (fieldList->at(i).GetId() == "updatedNotification"){
			gqlObject = fieldList->at(i);
			operationType = OT_UPDATE;
			return true;
		}
		if (fieldList->at(i).GetId() == "removedNotification"){
			gqlObject = fieldList->at(i);
			operationType = OT_DELETE;
			return true;
		}
	}
	return false;
}


QByteArray CObjectCollectionControllerCompBase::GetObjectIdFromInputParams(const QList<imtgql::CGqlObject>& inputParams) const
{
	int count = inputParams.count();
	for (int i = 0; i < count; i++){
		if (inputParams.at(i).GetFieldIds().contains("Id")){
			return inputParams.at(i).GetFieldArgumentValue("Id").toByteArray();
		}
	}
	return QByteArray();
}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::GetObject(
			const QList<imtgql::CGqlObject>& inputParams,
			const imtgql::CGqlObject& gqlObject,
			QString& errorMessage) const
{
	return nullptr;
}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::InsertObject(
			const QList<imtgql::CGqlObject>& inputParams,
			const imtgql::CGqlObject& gqlObject,
			QString& errorMessage) const
{
	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* dataModel = nullptr;
	imtbase::CTreeItemModel* notificationModel = nullptr;
	QByteArray newObjectId;

	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();
	}
	else{
		QString name, description, errorMessage;
		QByteArray objectId;
		istd::IChangeable* newObject = CreateObject(inputParams, objectId, name, description, errorMessage);
		if (newObject != nullptr){
			newObjectId = m_objectCollectionCompPtr->InsertNewObject("",name,description, newObject);
		}
	}

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsModel = rootModel->AddTreeModel("errors");
		errorsModel->SetData("message", errorMessage);
	}
	else{
		dataModel = new imtbase::CTreeItemModel();
		notificationModel = new imtbase::CTreeItemModel();
		notificationModel->SetData("Id", newObjectId);
		dataModel->SetExternTreeModel("addedNotification", notificationModel);
	}

	rootModel->SetExternTreeModel("data", dataModel);

	return rootModel;
}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::UpdateObject(
			const QList<imtgql::CGqlObject>& inputParams,
			const imtgql::CGqlObject& gqlObject,
			QString& errorMessage) const
{
	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* dataModel = nullptr;
	imtbase::CTreeItemModel* notificationModel = nullptr;
	QByteArray objectId;

	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();
	}
	else{
		QString name, description, errorMessage;
		QByteArray objectId;
		istd::IChangeable* savedObject = CreateObject(inputParams, objectId, name, description, errorMessage);
		if (savedObject != nullptr){
			if (m_objectCollectionCompPtr->SetObjectData(objectId, *savedObject) == false){
				errorMessage = QObject::tr("Can not update object: &1").arg(QString(objectId));
			}
		}
		else {
			errorMessage = QObject::tr("Can not create object for update: &1").arg(QString(objectId));
		}
	}

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsModel = rootModel->AddTreeModel("errors");
		errorsModel->SetData("message", errorMessage);
	}
	else{
		dataModel = new imtbase::CTreeItemModel();
		notificationModel = new imtbase::CTreeItemModel();
		notificationModel->SetData("Id", objectId);
		dataModel->SetExternTreeModel("updatedNotification", notificationModel);
	}

	rootModel->SetExternTreeModel("data", dataModel);

	return rootModel;
}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::RenameObject(
			const QList<imtgql::CGqlObject>& inputParams,
			const imtgql::CGqlObject& gqlObject,
			QString& errorMessage) const
{
	return nullptr;
}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::SetObjectDescription(
			const QList<imtgql::CGqlObject>& inputParams,
			const imtgql::CGqlObject& gqlObject,
			QString& errorMessage) const
{
	return nullptr;
}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::ListObjects(
			const QList<imtgql::CGqlObject>& inputParams,
			const imtgql::CGqlObject& gqlObject,
			QString& errorMessage) const
{
	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* dataModel = nullptr;
	imtbase::CTreeItemModel* itemsModel = nullptr;
	bool isSetResponce = false;

	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();
	}

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsItemModel = rootModel->AddTreeModel("errors");
		errorsItemModel->SetData("message", errorMessage);
	}
	else{
		dataModel = new imtbase::CTreeItemModel();
		itemsModel = new imtbase::CTreeItemModel();

		imtbase::ICollectionInfo::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds();
		imtbase::IObjectCollection::DataPtr dataPtr;
		for (const QByteArray& collectionId : collectionIds){
			int itemIndex = itemsModel->InsertNewItem();
			if (itemIndex >= 0){
				if (!SetupGqlItem(gqlObject, *itemsModel, itemIndex, collectionId, errorMessage)){
					return nullptr;
				}
			}
		}

		itemsModel->SetIsArray(true);

		dataModel->SetExternTreeModel("items", itemsModel);
	}

	rootModel->SetExternTreeModel("data", dataModel);

	return rootModel;
}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::DeleteObject(
			const QList<imtgql::CGqlObject>& inputParams,
			const imtgql::CGqlObject& gqlObject,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = "No collection component was set";

		return nullptr;
	}

	QByteArray objectId = GetObjectIdFromInputParams(inputParams);
	if (objectId.isEmpty()){
		errorMessage = "No object-ID could not be extracted from the request";

		return nullptr;
	}

	bool retVal = m_objectCollectionCompPtr->RemoveObject(objectId);
	if (retVal){
		// CREATE SUCCESS RESPONSE!

//		Q_ASSERT_X(false, "CObjectCollectionControllerCompBase::DeleteObject", "Not implemented");

	}
	else{
		errorMessage = QObject::tr("Can not remove object: %1").arg(QString(objectId));
	}

	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* dataModel = nullptr;
	imtbase::CTreeItemModel* notificationModel = nullptr;


	//	QByteArray objectId;

//	if (!m_objectCollectionCompPtr.IsValid()){
//		errorMessage = QObject::tr("Internal error").toUtf8();
//	}
//	else{
//		QString name, description, errorMessage;
//		QByteArray objectId;
//		istd::IChangeable* savedObject = CreateObject(inputParams, objectId, name, description, errorMessage);
//		if (savedObject != nullptr){
//			if (m_objectCollectionCompPtr->SetObjectData(objectId, *savedObject) == false){
//				errorMessage = QObject::tr("Can not update object: &1").arg(QString(objectId));
//			}
//		}
//		else {
//			errorMessage = QObject::tr("Can not create object for update: &1").arg(QString(objectId));
//		}
//	}

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsModel = rootModel->AddTreeModel("errors");
		errorsModel->SetData("message", errorMessage);
	}
	else{
		dataModel = new imtbase::CTreeItemModel();
		notificationModel = new imtbase::CTreeItemModel();
		notificationModel->SetData("Id", objectId);
		dataModel->SetExternTreeModel("updatedNotification", notificationModel);
	}

	rootModel->SetExternTreeModel("data", dataModel);

	return rootModel;

	return nullptr;
}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::GetHeaders(
			const QList<imtgql::CGqlObject>& inputParams,
			const imtgql::CGqlObject& gqlObject,
			QString& errorMessage) const
{
	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* dataModel = nullptr;
	imtbase::CTreeItemModel* itemsModel = nullptr;
	bool isSetResponce = false;
	QByteArrayList fields;

	if (!m_viewDelegateCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();
	}

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsItemModel = rootModel->AddTreeModel("errors");
		errorsItemModel->SetData("message", errorMessage);
	}
	else{
		dataModel = new imtbase::CTreeItemModel();
		itemsModel = new imtbase::CTreeItemModel();
		const imtbase::ICollectionInfo& fieldCollection = m_viewDelegateCompPtr->GetSummaryInformationTypes();
		QVector<QByteArray> fieldIds = fieldCollection.GetElementIds();

		for (QByteArray fieldId : fieldIds){
			QString headerName = fieldCollection.GetElementInfo(fieldId, imtbase::ICollectionInfo::EIT_NAME).toString();
			int index = itemsModel->InsertNewItem();
			itemsModel->SetData("Name", headerName,index);
			itemsModel->SetData("Id", QString(fieldId),index);
		}
		itemsModel->SetIsArray(true);

		dataModel->SetExternTreeModel("headers", itemsModel);
	}

	rootModel->SetExternTreeModel("data", dataModel);

	return rootModel;
}


bool CObjectCollectionControllerCompBase::SetupGqlItem(
			const imtgql::CGqlObject& gqlObject,
			imtbase::CTreeItemModel& model,
			int itemIndex,
			const QByteArray& collectionId,
			QString& errorMessage) const
{
	bool retVal = true;
	QByteArrayList informationIds = GetInformationIds(gqlObject);
	if (!informationIds.isEmpty()){
		idoc::CStandardDocumentMetaInfo metaInfo;
		for (QByteArray informationId : informationIds){
			QVariant elementInformation;
			if(informationId == "Id"){
				elementInformation = QString(collectionId);
			}
			else if(informationId == "Name"){
				elementInformation = m_objectCollectionCompPtr->GetElementInfo(collectionId, imtbase::ICollectionInfo::EIT_NAME);
			}
			else if(informationId == "Description"){
				elementInformation = m_objectCollectionCompPtr->GetElementInfo(collectionId, imtbase::ICollectionInfo::EIT_DESCRIPTION);
			}
			else if (m_objectCollectionCompPtr->GetCollectionItemMetaInfo(collectionId, metaInfo)){
				if (informationId == QByteArray("Added")){
					elementInformation = metaInfo.GetMetaInfo(imtbase::IObjectCollection::MIT_INSERTION_TIME)
							.toDateTime().toString(imtgui::CObjectCollectionViewDelegate::s_dateTimeFormat);
				}
				else if (informationId == QByteArray("ModificationTime")){
					elementInformation = metaInfo.GetMetaInfo(imtbase::IObjectCollection::MIT_LAST_OPERATION_TIME)
							.toDateTime().toString(imtgui::CObjectCollectionViewDelegate::s_dateTimeFormat);
				}
			}

			if(elementInformation.isNull()){
				elementInformation = GetObjectInformation(informationId, collectionId);
			}
			if (elementInformation.isNull()){
				elementInformation = "";
			}

			retVal = retVal && model.SetData(informationId, elementInformation, itemIndex);
		}

		return true;
	}

	return false;
}


QByteArrayList CObjectCollectionControllerCompBase::GetInformationIds(const imtgql::CGqlObject& gqlObject) const
{
	return gqlObject.GetFieldIds();
}


QVariant CObjectCollectionControllerCompBase::GetObjectInformation(const QByteArray& informationId, const QByteArray& objectId) const
{
	return QVariant();
}


istd::IChangeable* CObjectCollectionControllerCompBase::CreateObject(const QList<imtgql::CGqlObject>& inputParams, QByteArray& objectId,
																	 QString& name, QString& description, QString &errorMessage) const
{
	return nullptr;
}


} // namespace imtgql


