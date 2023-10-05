#include <imtgql/CObjectCollectionControllerCompBase.h>


// STL includes
#include <cmath>

// ACF includes
#include <iprm/CTextParam.h>
#include <iprm/CEnableableParam.h>
#include <istd/TDelPtr.h>

// ImtCore includes
#include <imtbase/CCollectionFilter.h>
#include <imtbase/IObjectCollectionIterator.h>
#include <imtbase/COperationContext.h>


namespace imtgql
{


// reimplemented (icomp::CComponentBase)

void CObjectCollectionControllerCompBase::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	// for object collection thread running
	if (!m_objectCollectionCompPtr.EnsureInitialized()){
		qDebug() << "Invalid object collection component";
	}
}


// reimplemented (imtgql::CGqlRepresentationDataControllerComp)

imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::CreateInternalResponse(
		const imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	imtgql::CGqlObject gqlObject;

	int operationType = OT_UNKNOWN;
	if (!GetOperationFromRequest(gqlRequest, gqlObject, errorMessage, operationType)){
		return nullptr;
	}

	Q_ASSERT(operationType != OT_UNKNOWN);

	switch (operationType){
	case OT_NEW:
		return InsertObject(gqlRequest, errorMessage);
	case OT_GET:
		return GetObject(gqlRequest, errorMessage);
	case OT_UPDATE:
		return UpdateObject(gqlRequest, errorMessage);
	case OT_UPDATE_COLLECTION:
		return UpdateCollection(gqlRequest, errorMessage);
	case OT_DELETE:
		return DeleteObject(gqlRequest, errorMessage);
	case OT_RENAME:
		return RenameObject(gqlRequest, errorMessage);
	case OT_SET_DESCRIPTION:
		return SetObjectDescription(gqlRequest, errorMessage);
	case OT_LIST:
		return ListObjects(gqlRequest, errorMessage);
	case OT_HEADERS:
		return GetHeaders(gqlRequest, errorMessage);
	case OT_METAINFO:
		return GetMetaInfo(gqlRequest, errorMessage);
	case OT_INFO:
		return GetInfo(gqlRequest, errorMessage);
	case OT_DATAMETAINFO:
		return GetDataMetaInfo(gqlRequest, errorMessage);
	case OT_OBJECT_VIEW:
		return GetObjectView(gqlRequest, errorMessage);
	case OT_ELEMENTS_COUNT:
		return GetElementsCount(gqlRequest, errorMessage);
	case OT_ELEMENT_IDS:
		return GetElementIds(gqlRequest, errorMessage);
	case OT_USER_OPERATION + 1:
		return GetTreeItemModel(gqlRequest, errorMessage);
	case OT_USER_OPERATION + 2:
		return GetDependencies(gqlRequest, errorMessage);
	}

	return nullptr;
}


// protected methods

bool CObjectCollectionControllerCompBase::GetOperationFromRequest(
		const imtgql::CGqlRequest& gqlRequest,
		imtgql::CGqlObject& gqlObject,
		QString& /*errorMessage*/,
		int& operationType) const
{
	const QList<imtgql::CGqlObject> fieldList = gqlRequest.GetFields();

	int count = fieldList.count();
	for (int i = 0; i < count; i++){
		if (fieldList.at(i).GetId() == "headers"){
			gqlObject = fieldList.at(i);
			operationType = OT_HEADERS;
			return true;
		}
		if (fieldList.at(i).GetId() == "items"){
			gqlObject = fieldList.at(i);
			operationType = OT_LIST;
			return true;
		}
		if (fieldList.at(i).GetId() == "itemsCount"){
			gqlObject = fieldList.at(i);
			operationType = OT_ELEMENTS_COUNT;
			return true;
		}
		if (fieldList.at(i).GetId() == "itemIds"){
			gqlObject = fieldList.at(i);
			operationType = OT_ELEMENT_IDS;
			return true;
		}
		if (fieldList.at(i).GetId() == "item"){
			gqlObject = fieldList.at(i);
			operationType = OT_GET;
			return true;
		}
		if (fieldList.at(i).GetId() == "addedNotification"){
			gqlObject = fieldList.at(i);
			operationType = OT_NEW;
			return true;
		}
		if (fieldList.at(i).GetId() == "updatedNotification"){
			gqlObject = fieldList.at(i);
			operationType = OT_UPDATE;
			return true;
		}
		if (fieldList.at(i).GetId() == "updatedCollectionNotification"){
			gqlObject = fieldList.at(i);
			operationType = OT_UPDATE_COLLECTION;
			return true;
		}
		if (fieldList.at(i).GetId() == "removedNotification"){
			gqlObject = fieldList.at(i);
			operationType = OT_DELETE;
			return true;
		}
		if (fieldList.at(i).GetId() == "rename"){
			gqlObject = fieldList.at(i);
			operationType = OT_RENAME;
			return true;
		}
		if (fieldList.at(i).GetId() == "setDescription"){
			gqlObject = fieldList.at(i);
			operationType = OT_SET_DESCRIPTION;
			return true;
		}
		if (fieldList.at(i).GetId() == "info"){
			gqlObject = fieldList.at(i);
			operationType = OT_INFO;
			return true;
		}
		if (fieldList.at(i).GetId() == "metaInfo"){
			gqlObject = fieldList.at(i);
			operationType = OT_METAINFO;
			return true;
		}
		if (fieldList.at(i).GetId() == "dataMetaInfo"){
			gqlObject = fieldList.at(i);
			operationType = OT_DATAMETAINFO;
			return true;
		}
		if (fieldList.at(i).GetId() == "objectView"){
			gqlObject = fieldList.at(i);
			operationType = OT_OBJECT_VIEW;
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
		const imtgql::CGqlRequest& /*gqlRequest*/,
		QString& /*errorMessage*/) const
{
	return nullptr;
}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::InsertObject(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QT_TR_NOOP("Internal error");
		SendErrorMessage(0, "Internal error", "Object collection controller");

		return nullptr;
	}

	QByteArray objectId;
	QString name;
	QString description;

	istd::IChangeable* newObjectPtr = CreateObject(gqlRequest, objectId, name, description, errorMessage);
	if (newObjectPtr == nullptr){
		SendErrorMessage(0, "Unable to create object from gql request", "Object collection controller");

		return nullptr;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		errorMessage = QT_TR_NOOP("Object with this ID already exists");
		SendErrorMessage(0, QString("Object with ID: %1 already exists").arg(qPrintable(objectId)), "Object collection controller");

		return nullptr;
	}

	imtbase::IOperationContext* operationContextPtr = CreateOperationContext(gqlRequest, QString("Created the object"));
	QByteArray newObjectId = m_objectCollectionCompPtr->InsertNewObject("DocumentInfo", name, description, newObjectPtr, objectId, nullptr, nullptr, operationContextPtr);
	if (newObjectId.isEmpty()){
		errorMessage = QT_TR_NOOP(QString("Can not insert object: %1").arg(qPrintable(objectId)));
		SendErrorMessage(0, QString("Can not insert object: %1").arg(qPrintable(objectId)), "Object collection controller");

		return nullptr;
	}

	imtbase::CTreeItemModel* rootModelPtr(new imtbase::CTreeItemModel());

	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");
	Q_ASSERT(dataModelPtr != nullptr);

	imtbase::CTreeItemModel* notificationModelPtr = dataModelPtr->AddTreeModel("addedNotification");
	Q_ASSERT(notificationModelPtr != nullptr);

	notificationModelPtr->SetData("Id", newObjectId);
	notificationModelPtr->SetData("Name", name);

	return rootModelPtr;
}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::UpdateObject(
		const imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	imtbase::CTreeItemModel* dataModel = nullptr;
	imtbase::CTreeItemModel* notificationModel = nullptr;

	const QList<imtgql::CGqlObject> inputParams = gqlRequest.GetParams();

	QByteArray oldObjectId;
	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParam("input");
	if (inputParamPtr != nullptr){
		oldObjectId = inputParamPtr->GetFieldArgumentValue("Id").toByteArray();
	}

	QByteArray newObjectId;
	QString name;
	QString description;

	istd::IChangeable* savedObject = CreateObject(gqlRequest, newObjectId, name, description, errorMessage);
	if (savedObject != nullptr){
		imtbase::IOperationContext* operationContextPtr = CreateOperationContext(gqlRequest, QString("Updated the object"));
		if (!m_objectCollectionCompPtr->SetObjectData(oldObjectId, *savedObject, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr)){
			errorMessage = QObject::tr("Can not update object: %1").arg(qPrintable(oldObjectId));
		}
	}
	else {
		if (errorMessage.isEmpty()){
			errorMessage = QObject::tr("Can not create object for update: %1").arg(qPrintable(oldObjectId));
		}
	}

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsModel = rootModelPtr->AddTreeModel("errors");
		errorsModel->SetData("message", errorMessage);
	}
	else{
		dataModel = new imtbase::CTreeItemModel();
		notificationModel = new imtbase::CTreeItemModel();
		notificationModel->SetData("Id", newObjectId);
		notificationModel->SetData("Name", name);
		dataModel->SetExternTreeModel("updatedNotification", notificationModel);
	}
	rootModelPtr->SetExternTreeModel("data", dataModel);

	return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::UpdateCollection(
		const imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	imtbase::CTreeItemModel* dataModel = nullptr;
	imtbase::CTreeItemModel* notificationModel = nullptr;

	const QList<imtgql::CGqlObject> inputParams = gqlRequest.GetParams();

	QByteArray objectIdsJson;

	if (inputParams.size() > 0){
		objectIdsJson = inputParams.at(0).GetFieldArgumentValue("Ids").toByteArray();
	}

	if (!objectIdsJson.isEmpty()){
		imtbase::CTreeItemModel objectIdsModel;
		if (objectIdsModel.CreateFromJson(objectIdsJson)){
			for (int i = 0; i < objectIdsModel.GetItemsCount(); i++){
				QString name, description;

				QByteArray objectId = objectIdsModel.GetData("Id", i).toByteArray();
				if (!objectId.isEmpty()){
					istd::IChangeable* savedObject = CreateObject(gqlRequest, objectId, name, description, errorMessage);
					if (!m_objectCollectionCompPtr->SetObjectData(objectId, *savedObject)){
						errorMessage += QObject::tr("Could not update object: %1; ").arg(qPrintable(objectId));
						objectIdsModel.SetData("Failed", true, i);
					}
				}
			}
			if (!errorMessage.isEmpty()){
				imtbase::CTreeItemModel* errorsModel = rootModelPtr->AddTreeModel("errors");
				errorsModel->SetData("message", errorMessage);
			}
			else{
				dataModel = new imtbase::CTreeItemModel();
				notificationModel = new imtbase::CTreeItemModel();

				for(int i = 0; i < objectIdsModel.GetItemsCount(); ++i){
					int index = notificationModel->InsertNewItem();
					if(!objectIdsModel.ContainsKey("Failed", i)){
						notificationModel->SetData("Id", objectIdsModel.GetData("Id", i), index);
					}
				}

				dataModel->SetExternTreeModel("updatedCollectionNotification", notificationModel);
			}
		}
	}

	rootModelPtr->SetExternTreeModel("data", dataModel);

	return rootModelPtr.PopPtr();
}

imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::RenameObject(
		const imtgql::CGqlRequest& gqlRequest,
		QString& /*errorMessage*/) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return nullptr;
	}

	const QList<imtgql::CGqlObject> inputParams = gqlRequest.GetParams();

	QByteArray objectId;
	QString newName;
	if (inputParams.size() > 0){
		objectId = inputParams.at(0).GetFieldArgumentValue("Id").toByteArray();
		newName = inputParams.at(0).GetFieldArgumentValue("NewName").toString();
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModel = rootModelPtr->AddTreeModel("data");

	if (!objectId.isEmpty()){
		dataModel->SetData("Id", objectId);
		dataModel->SetData("Name", newName);

		m_objectCollectionCompPtr->SetElementName(objectId, newName);
	}

	return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::SetObjectDescription(
		const imtgql::CGqlRequest& gqlRequest,
		QString& /*errorMessage*/) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return nullptr;
	}

	const QList<imtgql::CGqlObject> inputParams = gqlRequest.GetParams();

	QByteArray objectId;
	QString description;

	if (inputParams.size() > 0){
		objectId = inputParams.at(0).GetFieldArgumentValue("Id").toByteArray();
		description = inputParams.at(0).GetFieldArgumentValue("Description").toString();
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

	if (!objectId.isEmpty()){
		dataModelPtr->SetData("Id", objectId);
		dataModelPtr->SetData("Description", description);
		m_objectCollectionCompPtr->SetElementDescription(objectId, description);
	}

	return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::ListObjects(
		const imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return nullptr;
	}

	const QList<imtgql::CGqlObject> inputParams = gqlRequest.GetParams();

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	imtbase::CTreeItemModel* dataModel = nullptr;
	imtbase::CTreeItemModel* itemsModel = nullptr;
	imtbase::CTreeItemModel* notificationModel = nullptr;

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsItemModel = rootModelPtr->AddTreeModel("errors");
		errorsItemModel->SetData("message", errorMessage);
	}
	else{
		dataModel = new imtbase::CTreeItemModel();
		itemsModel = new imtbase::CTreeItemModel();
		notificationModel = new imtbase::CTreeItemModel();

		const imtgql::CGqlObject* viewParamsGql = nullptr;
		if (inputParams.size() > 0){
			viewParamsGql = inputParams.at(0).GetFieldArgumentObjectPtr("viewParams");
		}

		iprm::CParamsSet filterParams;

		int offset = 0, count = -1;

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

		istd::TDelPtr<imtbase::IObjectCollectionIterator> objectCollectionIterator(m_objectCollectionCompPtr->CreateObjectCollectionIterator(offset, count, &filterParams));
		if (objectCollectionIterator != nullptr){
			while (objectCollectionIterator->Next()){
				imtbase::IObjectCollection::DataPtr objectDataPtr;
				if (objectCollectionIterator->GetObjectData(objectDataPtr)){
					int itemIndex = itemsModel->InsertNewItem();
					if (itemIndex >= 0){
						if (!SetupGqlItem(gqlRequest, *itemsModel, itemIndex, objectCollectionIterator.GetPtr(), errorMessage)){
							return nullptr;
						}
					}
				}
				else{
					return nullptr;
				}
			}
		}

		itemsModel->SetIsArray(true);


		dataModel->SetExternTreeModel("items", itemsModel);
		dataModel->SetExternTreeModel("notification", notificationModel);
	}

	rootModelPtr->SetExternTreeModel("data", dataModel);

	return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::GetElementsCount(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return nullptr;
	}

	const QList<imtgql::CGqlObject> inputParams = gqlRequest.GetParams();

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	imtbase::CTreeItemModel* dataModel = nullptr;

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsItemModel = rootModelPtr->AddTreeModel("errors");
		errorsItemModel->SetData("message", errorMessage);
	}
	else{
		dataModel = new imtbase::CTreeItemModel();

		const imtgql::CGqlObject* viewParamsGql = nullptr;
		if (inputParams.size() > 0){
			viewParamsGql = inputParams.at(0).GetFieldArgumentObjectPtr("viewParams");
		}

		iprm::CParamsSet filterParams;

		if (viewParamsGql != nullptr){
			PrepareFilters(gqlRequest, *viewParamsGql, filterParams);
		}

		int elementsCount = m_objectCollectionCompPtr->GetElementsCount(&filterParams);

		dataModel->SetData("itemsCount", elementsCount);
	}

	rootModelPtr->SetExternTreeModel("data", dataModel);

	return rootModelPtr.PopPtr();

}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::GetElementIds(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return nullptr;
	}

	const QList<imtgql::CGqlObject> inputParams = gqlRequest.GetParams();

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	imtbase::CTreeItemModel* dataModel = nullptr;

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsItemModel = rootModelPtr->AddTreeModel("errors");
		errorsItemModel->SetData("message", errorMessage);
	}
	else{
		dataModel = new imtbase::CTreeItemModel();

		const imtgql::CGqlObject* viewParamsGql = nullptr;
		if (inputParams.size() > 0){
			viewParamsGql = inputParams.at(0).GetFieldArgumentObjectPtr("viewParams");
		}

		iprm::CParamsSet filterParams;

		int offset = 0, count = -1;

		if (viewParamsGql != nullptr){
			offset = viewParamsGql->GetFieldArgumentValue("Offset").toInt();
			count = viewParamsGql->GetFieldArgumentValue("Count").toInt();
			PrepareFilters(gqlRequest, *viewParamsGql, filterParams);
		}


		QByteArray dataIds;

		imtbase::ICollectionInfo::Ids ids = m_objectCollectionCompPtr->GetElementIds(offset, count, &filterParams);
		for (int i = 0; i < ids.count(); i++){
			if (i > 0){
				dataIds += ";";
			}
			dataIds += ids[i];
		}

		dataModel->SetData("itemIds", dataIds);
	}

	rootModelPtr->SetExternTreeModel("data", dataModel);

	return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::DeleteObject(
		const imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = "No collection component was set";

		return nullptr;
	}

	const QList<imtgql::CGqlObject> inputParams = gqlRequest.GetParams();

	QByteArray objectId = GetObjectIdFromInputParams(inputParams);
	if (objectId.isEmpty()){
		errorMessage = QObject::tr("No object-ID could not be extracted from the request");

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	imtbase::IOperationContext* operationContextPtr = CreateOperationContext(gqlRequest, QString("Removed the object"));
	if (m_objectCollectionCompPtr->RemoveElement(objectId, operationContextPtr)){
		imtbase::CTreeItemModel* dataModel = new imtbase::CTreeItemModel();
		imtbase::CTreeItemModel* notificationModel = new imtbase::CTreeItemModel();

		notificationModel->SetData("Id", objectId);
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


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::GetHeaders(
		const imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	if (!m_headersProviderCompPtr.IsValid()){
		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	imtbase::CTreeItemModel* headersModelPtr = m_headersProviderCompPtr->CreateResponse(gqlRequest, errorMessage);
	if (headersModelPtr != nullptr){
		rootModelPtr->SetExternTreeModel("data", headersModelPtr);
	}

	return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::GetTreeItemModel(
		const imtgql::CGqlRequest& /*gqlRequest*/,
		QString& /*errorMessage*/) const
{
	return nullptr;
}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::GetDependencies(
		const imtgql::CGqlRequest& /*gqlRequest*/,
		QString& /*errorMessage*/) const
{
	return nullptr;
}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::GetMetaInfo(
		const imtgql::CGqlRequest& /*gqlRequest*/,
		QString& /*errorMessage*/) const
{
	return nullptr;
}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::GetInfo(
		const imtgql::CGqlRequest& /*gqlRequest*/,
		QString& /*errorMessage*/) const
{
	return nullptr;
}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::GetDataMetaInfo(
		const imtgql::CGqlRequest& /*gqlRequest*/,
		QString& /*errorMessage*/) const
{
	return nullptr;
}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::GetObjectView(
		const imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	if (!m_objectViewProviderCompPtr.IsValid()){
		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	imtbase::CTreeItemModel* objectViewModelPtr = m_objectViewProviderCompPtr->CreateResponse(gqlRequest, errorMessage);
	if (objectViewModelPtr != nullptr){
		rootModelPtr->SetExternTreeModel("data", objectViewModelPtr);
	}

	return rootModelPtr.PopPtr();
}


imtbase::IOperationContext* CObjectCollectionControllerCompBase::CreateOperationContext(const imtgql::CGqlRequest& gqlRequest, const QString& operationDescription) const
{
	imtgql::IGqlContext* requestContextPtr = gqlRequest.GetRequestContext();
	if (requestContextPtr == nullptr){
		return nullptr;
	}

	imtauth::IUserInfo* userInfoPtr = requestContextPtr->GetUserInfo();
	if (userInfoPtr == nullptr){
		return nullptr;
	}

	imtbase::IOperationContext::IdentifableObjectInfo objectInfo;
	objectInfo.id = userInfoPtr->GetId();
	objectInfo.name = userInfoPtr->GetName();

	istd::TDelPtr<imtbase::COperationContext> operationContextPtr;
	operationContextPtr.SetPtr(new imtbase::COperationContext);

	operationContextPtr->SetOperationOwnerId(objectInfo);
	operationContextPtr->SetOperationDescription(operationDescription);

	return operationContextPtr.PopPtr();
}


bool CObjectCollectionControllerCompBase::SetupGqlItem(
		const imtgql::CGqlRequest& gqlRequest,
		imtbase::CTreeItemModel& model,
		int itemIndex,
		const QByteArray& collectionId,
		QString& /*errorMessage*/) const
{
	bool retVal = true;

	QByteArrayList informationIds = GetInformationIds(gqlRequest, "items");

	if (!informationIds.isEmpty()){
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
			else{
				idoc::MetaInfoPtr elementMetaInfo = m_objectCollectionCompPtr->GetElementMetaInfo(collectionId);
				if (elementMetaInfo.IsValid()){
					if (informationId == QByteArray("Added")){
						elementInformation = elementMetaInfo->GetMetaInfo(imtbase::IObjectCollection::MIT_INSERTION_TIME)
								.toDateTime().toString("dd.MM.yyyy hh:mm:ss");
					}
					else if (informationId == QByteArray("LastModified")){
						elementInformation = elementMetaInfo->GetMetaInfo(imtbase::IObjectCollection::MIT_LAST_OPERATION_TIME)
								.toDateTime().toString("dd.MM.yyyy hh:mm:ss");
					}
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


bool CObjectCollectionControllerCompBase::SetupGqlItem(
		const imtgql::CGqlRequest& gqlRequest,
		imtbase::CTreeItemModel& model,
		int itemIndex,
		const imtbase::IObjectCollectionIterator* objectCollectionIterator,
		QString&/*errorMessage*/) const
{
	if (objectCollectionIterator == nullptr){
		return false;
	}
	
	bool retVal = true;
	QByteArray collectionId = objectCollectionIterator->GetObjectId();

	QByteArrayList informationIds = GetInformationIds(gqlRequest, "items");

	if (!informationIds.isEmpty()){
		idoc::MetaInfoPtr elementMetaInfo = objectCollectionIterator->GetDataMetaInfo();

		for (QByteArray informationId : informationIds){
			QVariant elementInformation;

			if(informationId == "Id"){
				elementInformation = QString(collectionId);
			}
			else if(informationId == "Name"){
				elementInformation = objectCollectionIterator->GetElementInfo("Name");
			}
			else if(informationId == "Description"){
				elementInformation = objectCollectionIterator->GetElementInfo("Description");
			}
			else{
				if (elementMetaInfo.IsValid()){
					if (informationId == QByteArray("Added")){
						elementInformation = elementMetaInfo->GetMetaInfo(imtbase::IObjectCollection::MIT_INSERTION_TIME)
								.toDateTime().toString("dd.MM.yyyy hh:mm:ss");
					}
					else if (informationId == QByteArray("LastModified")){
						elementInformation = elementMetaInfo->GetMetaInfo(imtbase::IObjectCollection::MIT_LAST_OPERATION_TIME)
								.toDateTime().toString("dd.MM.yyyy hh:mm:ss");
					}
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


QByteArrayList CObjectCollectionControllerCompBase::GetInformationIds(const imtgql::CGqlRequest& gqlRequest, const QByteArray& objectId) const
{
	const QList<imtgql::CGqlObject> fieldList = gqlRequest.GetFields();
	int count = fieldList.count();
	for (int i = 0; i < count; i++){
		if (fieldList.at(i).GetId() == objectId){
			return fieldList.at(i).GetFieldIds();
		}
	}

	return QByteArrayList();
}


QVariant CObjectCollectionControllerCompBase::GetObjectInformation(const QByteArray& /*informationId*/, const QByteArray& /*objectId*/) const
{
	return QVariant();
}


istd::IChangeable* CObjectCollectionControllerCompBase::CreateObject(
		const QList<imtgql::CGqlObject>& /*inputParams*/,
		QByteArray& /*objectId*/,
		QString& /*name*/,
		QString& /*description*/,
		QString& /*errorMessage*/) const
{
	return nullptr;
}


istd::IChangeable* CObjectCollectionControllerCompBase::CreateObject(
		const imtgql::CGqlRequest& gqlRequest,
		QByteArray& newObjectId,
		QString& name,
		QString& description,
		QString& errorMessage) const
{
	const QList<imtgql::CGqlObject> inputParams = gqlRequest.GetParams();

	return CreateObject(inputParams, newObjectId, name, description, errorMessage);
}


void CObjectCollectionControllerCompBase::PrepareFilters(
		const imtgql::CGqlRequest& gqlRequest,
		const imtgql::CGqlObject& viewParamsGql,
		iprm::CParamsSet& filterParams) const
{
	istd::TDelPtr<imtbase::CCollectionFilter> collectionFilterPtr;
	collectionFilterPtr.SetPtr(new imtbase::CCollectionFilter);

	istd::TDelPtr<iprm::CParamsSet> objectFilterPtr;
	objectFilterPtr.SetPtr(new iprm::CParamsSet);

	QByteArray filterBA = viewParamsGql.GetFieldArgumentValue("FilterModel").toByteArray();
	if (!filterBA.isEmpty()){
		imtbase::CTreeItemModel generalModel;
		generalModel.CreateFromJson(filterBA);

		imtbase::CTreeItemModel* filterModel = generalModel.GetTreeItemModel("FilterIds");
		if (filterModel != nullptr){
			QByteArrayList filteringInfoIds;
			for (int i = 0; i < filterModel->GetItemsCount(); i++){
				QByteArray headerId = filterModel->GetData("Id", i).toByteArray();
				if (!headerId.isEmpty()){
					filteringInfoIds << headerId;
				}
			}
			collectionFilterPtr->SetFilteringInfoIds(filteringInfoIds);
		}

		QString filterText = generalModel.GetData("TextFilter").toString();
		if (!filterText.isEmpty()){
			collectionFilterPtr->SetTextFilter(filterText);
		}

		imtbase::CTreeItemModel* sortModel = generalModel.GetTreeItemModel("Sort");
		if (sortModel != nullptr){
			QByteArray headerId = sortModel->GetData("HeaderId").toByteArray();
			QByteArray sortOrder = sortModel->GetData("SortOrder").toByteArray();
			if (!headerId.isEmpty() && !sortOrder.isEmpty()){
				collectionFilterPtr->SetSortingOrder(sortOrder == "ASC" ? imtbase::ICollectionFilter::SO_ASC : imtbase::ICollectionFilter::SO_DESC);
				collectionFilterPtr->SetSortingInfoIds(QByteArrayList() << headerId);
			}
		}

		if (generalModel.ContainsKey("ObjectFilter")){
			imtbase::CTreeItemModel* objectFilterModelPtr = generalModel.GetTreeItemModel("ObjectFilter");
			SetObjectFilter(gqlRequest, *objectFilterModelPtr, *objectFilterPtr);
		}
		else{
			imtbase::CTreeItemModel objectFilterModel;
			SetObjectFilter(gqlRequest, objectFilterModel, *objectFilterPtr);
		}
	}

	filterParams.SetEditableParameter("Filter", collectionFilterPtr.PopPtr());
	filterParams.SetEditableParameter("ObjectFilter", objectFilterPtr.PopPtr());

	this->SetAdditionalFilters(viewParamsGql, &filterParams);
}



void CObjectCollectionControllerCompBase::SetAdditionalFilters(const imtgql::CGqlObject& /*viewParamsGql*/, iprm::CParamsSet* /*filterParams*/) const
{
}


void CObjectCollectionControllerCompBase::SetObjectFilter(
		const imtgql::CGqlRequest& /*gqlRequest*/,
		const imtbase::CTreeItemModel& objectFilterModel,
		iprm::CParamsSet& filterParams) const
{
	QByteArray key;
	if (objectFilterModel.ContainsKey("Key")){
		key = objectFilterModel.GetData("Key").toByteArray();
	}

	istd::TDelPtr<iprm::CTextParam> textParamPtr(new iprm::CTextParam());
	if (objectFilterModel.ContainsKey("Value")){
		QString value = objectFilterModel.GetData("Value").toString();
		textParamPtr->SetText(value);
	}

	istd::TDelPtr<iprm::CEnableableParam> enableableParamPtr(new iprm::CEnableableParam());
	if (objectFilterModel.ContainsKey("IsEqual")){
		bool isEqual = objectFilterModel.GetData("IsEqual").toBool();
		enableableParamPtr->SetEnabled(isEqual);

		istd::TDelPtr<iprm::CParamsSet> paramsSetPtr(new iprm::CParamsSet());
		paramsSetPtr->SetEditableParameter("Value", textParamPtr.PopPtr());
		paramsSetPtr->SetEditableParameter("IsEqual", enableableParamPtr.PopPtr());

		filterParams.SetEditableParameter(key, paramsSetPtr.PopPtr());
	}
	else{
		filterParams.SetEditableParameter(key, textParamPtr.PopPtr());
	}
}


} // namespace imtgql


