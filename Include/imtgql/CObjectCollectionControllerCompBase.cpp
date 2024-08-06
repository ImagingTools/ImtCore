#include <imtgql/CObjectCollectionControllerCompBase.h>


// std includes
#include <cmath>

// ACF includes
#include <iprm/CTextParam.h>
#include <iprm/CIdParam.h>
#include <iprm/CEnableableParam.h>
#include <istd/TDelPtr.h>
#include <iser/CJsonMemReadArchive.h>
#include <istd/TSingleFactory.h>

// ImtCore includes
#include <imtbase/CCollectionFilter.h>
#include <imtbase/IObjectCollectionIterator.h>
#include <imtbase/COperationContext.h>
#include <imtbase/CTimeFilterParam.h>
#include <imtbase/CObjectCollection.h>
#include <imtbase/COperationDescription.h>
#include <imtgql/imtgql.h>


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
	case OT_ELEMENT_HISTORY:
		return GetObjectHistory(gqlRequest, errorMessage);
	case OT_ELEMENTS_COUNT:
		return GetElementsCount(gqlRequest, errorMessage);
	case OT_ELEMENT_IDS:
		return GetElementIds(gqlRequest, errorMessage);
	case OT_USER_OPERATION + 1:
		return GetTreeItemModel(gqlRequest, errorMessage);
	case OT_USER_OPERATION + 2:
		return GetDependencies(gqlRequest, errorMessage);
	}

	errorMessage = QString("Unable to create internal response. Operation is not supported");
	SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

	return nullptr;
}


// reimplemented (imtgql::IGqlRequestExtractor)

istd::IChangeable* CObjectCollectionControllerCompBase::ExtractObject(const imtgql::CGqlRequest& gqlRequest, QByteArray& newObjectId, QString& name, QString& description, QString& errorMessage) const
{
	return CreateObject(gqlRequest, newObjectId, name, description, errorMessage);
}


// protected methods

bool CObjectCollectionControllerCompBase::GetOperationFromRequest(
		const imtgql::CGqlRequest& gqlRequest,
		imtgql::CGqlObject& gqlObject,
		QString& errorMessage,
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
		if (fieldList.at(i).GetId() == "itemHistory"){
			gqlObject = fieldList.at(i);
			operationType = OT_ELEMENT_HISTORY;
			return true;
		}
	}

	errorMessage = QString("Unable to get the operation type from the request");

	SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

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

	const imtgql::CGqlObject* gqlInputParamPtr = gqlRequest.GetParam("input");
	if (gqlInputParamPtr == nullptr){
		errorMessage = QString("Unable to insert an object. GraphQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return nullptr;
	}

	QByteArray typeId = gqlInputParamPtr->GetFieldArgumentValue("typeId").toByteArray();
	QString name = gqlInputParamPtr->GetFieldArgumentValue("name").toString();
	QString description = gqlInputParamPtr->GetFieldArgumentValue("description").toString();

	const imtgql::CGqlObject* additionalParamsPtr = gqlInputParamPtr->GetFieldArgumentObjectPtr("addition");
	if (additionalParamsPtr != nullptr){
		typeId = additionalParamsPtr->GetFieldArgumentValue("typeId").toByteArray();
	}

	if (typeId.isEmpty()){
		typeId = "DocumentInfo";
	}

	QByteArray objectId;
	istd::IChangeable* newObjectPtr = CreateObject(gqlRequest, objectId, name, description, errorMessage);
	if (newObjectPtr == nullptr){
		errorMessage = QString("Unable to create object from GraphQL request");
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return nullptr;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		errorMessage = QString("Object with ID: '%1' already exists").arg(objectId);
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return nullptr;
	}

	istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;

	if (m_operationContextControllerCompPtr.IsValid()){
		operationContextPtr = m_operationContextControllerCompPtr->CreateOperationContext(imtbase::IDocumentChangeGenerator::OT_CREATE, gqlRequest);
	}

	QByteArray newObjectId = m_objectCollectionCompPtr->InsertNewObject(typeId, name, description, newObjectPtr, objectId, nullptr, nullptr, operationContextPtr.GetPtr());
	if (newObjectId.isEmpty()){
		errorMessage = QString("Error when creating a new object. Object-ID: '%1'.").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "Object collection controller");

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
		errorMessage = QString("Unable to update an object. Internal error.");
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return nullptr;
	}

	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParam("input");
	if (inputParamPtr == nullptr){
		errorMessage = QString("Unable to update an object. GraphQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return nullptr;
	}

	QByteArray oldObjectId = inputParamPtr->GetFieldArgumentValue("Id").toByteArray();
	QString name = inputParamPtr->GetFieldArgumentValue("name").toString();
	QString description = inputParamPtr->GetFieldArgumentValue("description").toString();

	QByteArray newObjectId;
	istd::IChangeable* savedObjectPtr = CreateObject(gqlRequest, newObjectId, name, description, errorMessage);
	if (savedObjectPtr == nullptr){
		errorMessage = QString("Can not create object for update: '%1'").arg(qPrintable(oldObjectId));
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return nullptr;
	}

	istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
	if (m_operationContextControllerCompPtr.IsValid()){
		operationContextPtr = m_operationContextControllerCompPtr->CreateOperationContext(imtbase::IDocumentChangeGenerator::OT_UPDATE, gqlRequest, oldObjectId, savedObjectPtr);
	}

	if (!m_objectCollectionCompPtr->SetObjectData(oldObjectId, *savedObjectPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr.GetPtr())){
		errorMessage = QString("Can not update object: '%1'").arg(qPrintable(oldObjectId));
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");
	imtbase::CTreeItemModel* notificationModelPtr = dataModelPtr->AddTreeModel("updatedNotification");
	notificationModelPtr->SetData("Id", newObjectId);
	notificationModelPtr->SetData("Name", name);

	return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::UpdateCollection(
		const imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to update collection. Component reference 'ObjectCollection' was not set");
		SendCriticalMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	imtbase::CTreeItemModel* dataModel = nullptr;
	imtbase::CTreeItemModel* notificationModel = nullptr;

	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParam("input");
	if (inputParamPtr == nullptr){
		errorMessage = QString("Unable to update collection. GraphQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return nullptr;
	}

	QByteArray objectIdsJson = inputParamPtr->GetFieldArgumentValue("Ids").toByteArray();
	if (objectIdsJson.isEmpty()){
		errorMessage = QString("Unable to update collection. Ids from input params is empty.");
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return nullptr;
	}

	imtbase::CTreeItemModel objectIdsModel;
	if (!objectIdsModel.CreateFromJson(objectIdsJson)){
		errorMessage = QString("Unable to create model from json: '%1'").arg(objectIdsJson);
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return nullptr;
	}

	for (int i = 0; i < objectIdsModel.GetItemsCount(); i++){
		QByteArray objectId = objectIdsModel.GetData("Id", i).toByteArray();
		if (!objectId.isEmpty()){
			QString name;
			QString description;

			istd::IChangeable* savedObjectPtr = CreateObject(gqlRequest, objectId, name, description, errorMessage);
			if (savedObjectPtr != nullptr){
				if (!m_objectCollectionCompPtr->SetObjectData(objectId, *savedObjectPtr)){
					errorMessage += QString("Could not update object: '%1'; ").arg(qPrintable(objectId));
					objectIdsModel.SetData("Failed", true, i);
				}
			}
		}
	}

	// ???
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

	rootModelPtr->SetExternTreeModel("data", dataModel);

	return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::RenameObject(
		const imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to rename object. Component reference 'ObjectCollection' was not set");
		SendCriticalMessage(0, errorMessage);

		return nullptr;
	}

	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParam("input");
	if (inputParamPtr == nullptr){
		errorMessage = QString("Unable to rename object. GraphQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return nullptr;
	}

	QByteArray objectId = inputParamPtr->GetFieldArgumentValue("Id").toByteArray();
	QString newName = inputParamPtr->GetFieldArgumentValue("NewName").toString();

	if (!m_objectCollectionCompPtr->SetElementName(objectId, newName)){
		errorMessage = QString("Unable to set name '%1' for element with ID: '%2'").arg(newName).arg(objectId);
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModel = rootModelPtr->AddTreeModel("data");

	dataModel->SetData("Id", objectId);
	dataModel->SetData("Name", newName);

	return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::SetObjectDescription(
		const imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to set the object description. Component reference 'ObjectCollection' was not set");
		SendCriticalMessage(0, errorMessage);

		return nullptr;
	}

	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParam("input");
	if (inputParamPtr == nullptr){
		errorMessage = QString("Unable to set description for object. GraphQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return nullptr;
	}

	QByteArray objectId = inputParamPtr->GetFieldArgumentValue("Id").toByteArray();
	QString description = inputParamPtr->GetFieldArgumentValue("Description").toString();

	if (!m_objectCollectionCompPtr->SetElementDescription(objectId, description)){
		errorMessage = QString("Unable to set description '%1' for element with ID: '%2'").arg(description).arg(objectId);
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

	dataModelPtr->SetData("Id", objectId);
	dataModelPtr->SetData("Description", description);

	return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::ListObjects(
		const imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to list objects. Component reference 'ObjectCollection' was not set");
		SendCriticalMessage(0, errorMessage);

		return nullptr;
	}

	const imtgql::CGqlObject* inputParamsPtr = gqlRequest.GetParam("input");
	if (inputParamsPtr == nullptr){
		errorMessage = QString("GraphQL input params is invalid");
		SendCriticalMessage(0, errorMessage);

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

	iprm::CParamsSet filterParams;

	int offset = 0;
	int count = -1;

	const imtgql::CGqlObject* viewParamsPtr = inputParamsPtr->GetFieldArgumentObjectPtr("viewParams");
	if (viewParamsPtr != nullptr){
		offset = viewParamsPtr->GetFieldArgumentValue("Offset").toInt();
		count = viewParamsPtr->GetFieldArgumentValue("Count").toInt();

		PrepareFilters(gqlRequest, *viewParamsPtr, filterParams);
	}

	if (count == 0){
		count = -1;
	}

	int elementsCount = m_objectCollectionCompPtr->GetElementsCount(&filterParams);

	int pagesCount = std::ceil(elementsCount / (double)count);
	if (pagesCount <= 0){
		pagesCount = 1;
	}

	imtbase::CTreeItemModel* notificationModelPtr = dataModelPtr->AddTreeModel("notification");
	notificationModelPtr->SetData("PagesCount", pagesCount);
	notificationModelPtr->SetData("TotalCount", elementsCount);

	istd::TDelPtr<imtbase::IObjectCollectionIterator> objectCollectionIterator(m_objectCollectionCompPtr->CreateObjectCollectionIterator(offset, count, &filterParams));
	if (objectCollectionIterator == nullptr){
		errorMessage = QString("Object collection iterator creation failed");
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return nullptr;
	}

	imtbase::CTreeItemModel* itemsModelPtr = dataModelPtr->AddTreeModel("items");
	while (objectCollectionIterator->Next()){
		imtbase::IObjectCollection::DataPtr objectDataPtr;
		if (objectCollectionIterator->GetObjectData(objectDataPtr)){
			int itemIndex = itemsModelPtr->InsertNewItem();
			if (itemIndex >= 0){
				if (!SetupGqlItem(gqlRequest, *itemsModelPtr, itemIndex, objectCollectionIterator.GetPtr(), errorMessage)){
					SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

					return nullptr;
				}
			}
		}
		else{
			errorMessage = QString("Unable to get object data from object collection iterator.");
			SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

			return nullptr;
		}
	}

	return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::GetElementsCount(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to get the element count. Component reference 'ObjectCollection' was not set");
		SendCriticalMessage(0, errorMessage);

		return nullptr;
	}

	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParam("input");
	if (inputParamPtr == nullptr){
		errorMessage = QString("Unable to rename object. GraphQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return nullptr;
	}

	iprm::CParamsSet filterParams;
	const imtgql::CGqlObject* viewParamsPtr = inputParamPtr->GetFieldArgumentObjectPtr("viewParams");
	if (viewParamsPtr != nullptr){
		PrepareFilters(gqlRequest, *viewParamsPtr, filterParams);
	}

	int elementsCount = m_objectCollectionCompPtr->GetElementsCount(&filterParams);

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");
	dataModelPtr->SetData("itemsCount", elementsCount);

	return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::GetElementIds(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to get the element IDs. Component reference 'ObjectCollection' was not set");
		SendCriticalMessage(0, errorMessage);

		return nullptr;
	}

	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParam("input");
	if (inputParamPtr == nullptr){
		errorMessage = QString("Unable to rename object. GraphQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return nullptr;
	}

	int offset = 0, count = -1;
	iprm::CParamsSet filterParams;
	const imtgql::CGqlObject* viewParamsPtr = inputParamPtr->GetFieldArgumentObjectPtr("viewParams");
	if (viewParamsPtr != nullptr){
		offset = viewParamsPtr->GetFieldArgumentValue("Offset").toInt();
		count = viewParamsPtr->GetFieldArgumentValue("Count").toInt();

		PrepareFilters(gqlRequest, *viewParamsPtr, filterParams);
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

	QByteArray dataIds;
	imtbase::ICollectionInfo::Ids ids = m_objectCollectionCompPtr->GetElementIds(offset, count, &filterParams);
	for (int i = 0; i < ids.count(); i++){
		if (i > 0){
			dataIds += ";";
		}
		dataIds += ids[i];
	}

	dataModelPtr->SetData("itemIds", dataIds);

	return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::DeleteObject(
		const imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to remove the object from the collection. Component reference 'ObjectCollection' was not set");
		SendCriticalMessage(0, errorMessage);

		return nullptr;
	}

	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParam("input");
	if (inputParamPtr == nullptr){
		errorMessage = QString("Unable to delete object. GraphQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return nullptr;
	}

	QByteArray objectId = inputParamPtr->GetFieldArgumentValue("Id").toByteArray();
	if (objectId.isEmpty()){
		errorMessage = QString("No object-ID could not be extracted from the request");
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return nullptr;
	}

	istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
	if (m_operationContextControllerCompPtr.IsValid()){
		operationContextPtr = m_operationContextControllerCompPtr->CreateOperationContext(imtbase::IDocumentChangeGenerator::OT_REMOVE, gqlRequest);
	}

	if (!m_objectCollectionCompPtr->RemoveElement(objectId, operationContextPtr.GetPtr())){
		errorMessage = QString("Can't remove object with ID: '%1'").arg(QString(objectId));
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");
	imtbase::CTreeItemModel* notificationModelPtr = dataModelPtr->AddTreeModel("removedNotification");
	notificationModelPtr->SetData("Id", objectId);

	return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::GetHeaders(
		const imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	if (!m_headersProviderCompPtr.IsValid()){
		errorMessage = QString("Unable to get headers. Component reference 'HeadersProvider' was not set");
		SendCriticalMessage(0, errorMessage);

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
		errorMessage = QString("Unable to get the object view. Component reference 'ObjectViewProvider' was not set");
		SendCriticalMessage(0, errorMessage);

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	imtbase::CTreeItemModel* objectViewModelPtr = m_objectViewProviderCompPtr->CreateResponse(gqlRequest, errorMessage);
	if (objectViewModelPtr != nullptr){
		rootModelPtr->SetExternTreeModel("data", objectViewModelPtr);
	}

	return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::GetObjectHistory(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to get the object history. Component reference 'ObjectCollection' was not set");
		SendCriticalMessage(0, errorMessage);

		return nullptr;
	}

	const imtgql::CGqlObject* gqlInputParamsPtr = gqlRequest.GetParam("input");
	if (gqlInputParamsPtr == nullptr){
		errorMessage = QString("Unable to get object history: GraphQL-parameters not set");
		SendErrorMessage(0, errorMessage);

		return nullptr;
	}

	QByteArray objectId = gqlInputParamsPtr->GetFieldArgumentValue("Id").toByteArray();
	if (objectId.isEmpty()){
		errorMessage = QString("Unable to get history for an object with empty ID");
		SendErrorMessage(0, errorMessage);

		return nullptr;
	}

	QByteArray languageId;
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr != nullptr){
		languageId = gqlContextPtr->GetLanguageId();
	}

	iprm::CParamsSet filterParams;

	iprm::CIdParam idParam;
	idParam.SetId(objectId);

	filterParams.SetEditableParameter("Id", &idParam);

	iprm::CEnableableParam enableableParam;
	enableableParam.SetEnabled(true);

	filterParams.SetEditableParameter("IsHistory", &enableableParam);

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");
	Q_ASSERT(dataModelPtr != nullptr);

	istd::TDelPtr<imtbase::IObjectCollectionIterator> objectCollectionIterator(m_objectCollectionCompPtr->CreateObjectCollectionIterator(0, -1, &filterParams));
	if (objectCollectionIterator == nullptr){
		errorMessage = QString("Unable to get history for an object with ID: '%1'. Error when trying to create collection iterator.").arg(objectId);
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return nullptr;
	}

	while (objectCollectionIterator->Next()){
		QByteArray ownerId = objectCollectionIterator->GetElementInfo("OwnerId").toByteArray();
		QString ownerName = objectCollectionIterator->GetElementInfo("OwnerName").toString();
		QByteArray operationsDescriptionJson = objectCollectionIterator->GetElementInfo("OperationDescription").toByteArray();
		QDateTime lastModified =  objectCollectionIterator->GetElementInfo("LastModified").toDateTime();
		lastModified.setTimeSpec(Qt::UTC);

		if (!ownerId.isEmpty() && !operationsDescriptionJson.isEmpty()){
			imtbase::CObjectCollection changeCollection;

			typedef istd::TSingleFactory<istd::IChangeable, imtbase::COperationDescription> FactoryOperationDescriptionImpl;
			changeCollection.RegisterFactory<FactoryOperationDescriptionImpl>("OperationInfo");

			iser::CJsonMemReadArchive archive(operationsDescriptionJson);
			if (changeCollection.Serialize(archive)){
				QString operationDescription = m_documentChangeGeneratorCompPtr->GetOperationDescription(changeCollection, languageId);
				if (!operationDescription.isEmpty()){
					int index = dataModelPtr->InsertNewItem();

					dataModelPtr->SetData("OwnerId", ownerId, index);
					dataModelPtr->SetData("OwnerName", ownerName, index);
					dataModelPtr->SetData("OperationDescription", operationDescription, index);
					dataModelPtr->SetData("Time", lastModified.toLocalTime().toString("dd.MM.yyyy hh:mm:ss"), index);
				}
			}
		}
	}

	return rootModelPtr.PopPtr();
}


bool CObjectCollectionControllerCompBase::SetupGqlItem(
		const imtgql::CGqlRequest& gqlRequest,
		imtbase::CTreeItemModel& model,
		int itemIndex,
		const QByteArray& collectionId,
		QString& /*errorMessage*/) const
{
	QByteArrayList informationIds = GetInformationIds(gqlRequest, "items");
	if (informationIds.isEmpty()){
		return false;
	}

	bool retVal = true;

	for (const QByteArray& informationId : informationIds){
		QVariant elementInformation;

		if(informationId == QByteArray("Id")){
			elementInformation = QString(collectionId);
		}
		else if(informationId == QByteArray("Name")){
			elementInformation = m_objectCollectionCompPtr->GetElementInfo(collectionId, imtbase::ICollectionInfo::EIT_NAME);
		}
		else if(informationId == QByteArray("Description")){
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

	return retVal;
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

	QByteArrayList informationIds = GetInformationIds(gqlRequest, "items");
	if (informationIds.isEmpty()){
		return false;
	}

	bool retVal = true;

	QByteArray collectionId = objectCollectionIterator->GetObjectId();

	idoc::MetaInfoPtr elementMetaInfo = objectCollectionIterator->GetDataMetaInfo();

	for (const QByteArray& informationId : informationIds){
		QVariant elementInformation;

		if(informationId == QByteArray("Id")){
			elementInformation = QString(collectionId);
		}
		else if(informationId == QByteArray("Name")){
			elementInformation = objectCollectionIterator->GetElementInfo("Name");
		}
		else if(informationId == QByteArray("Description")){
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

	return retVal;
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
	imtbase::CCollectionFilter* collectionFilterPtr = new imtbase::CCollectionFilter();

	iprm::CParamsSet* objectFilterPtr = new iprm::CParamsSet();

	QByteArray filterBA = viewParamsGql.GetFieldArgumentValue("FilterModel").toByteArray();
	if (filterBA.isEmpty()){
		return;
	}

	imtbase::CTreeItemModel generalModel;
	if (!generalModel.CreateFromJson(filterBA)){
		return;
	}

	imtbase::CTreeItemModel* filterIdsModelPtr = generalModel.GetTreeItemModel("FilterIds");
	if (filterIdsModelPtr != nullptr){
		QByteArrayList filteringInfoIds;
		for (int i = 0; i < filterIdsModelPtr->GetItemsCount(); i++){
			QByteArray headerId = filterIdsModelPtr->GetData("Id", i).toByteArray();
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

	imtbase::CTreeItemModel* sortModelPtr = generalModel.GetTreeItemModel("Sort");
	if (sortModelPtr != nullptr){
		QByteArray headerId = sortModelPtr->GetData("HeaderId").toByteArray();
		QByteArray sortOrder = sortModelPtr->GetData("SortOrder").toByteArray();
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

	if (generalModel.ContainsKey("TimeFilter")){
		imtbase::CTreeItemModel* timeRangeFilterModelPtr = generalModel.GetTreeItemModel("TimeFilter");
		if (timeRangeFilterModelPtr != nullptr){
			imtbase::CTimeFilterParam* timeFilterParamPtr = new imtbase::CTimeFilterParam();

			if (m_timeFilterParamRepresentationController.GetDataModelFromRepresentation(*timeRangeFilterModelPtr, *timeFilterParamPtr)){
				filterParams.SetEditableParameter("TimeFilter", timeFilterParamPtr, true);
			}
			else{
				SendWarningMessage(0, QString("Unable to create time range filter param from representation model"));
			}
		}
	}

	filterParams.SetEditableParameter("Filter", collectionFilterPtr, true);
	filterParams.SetEditableParameter("ObjectFilter", objectFilterPtr, true);

	this->SetAdditionalFilters(gqlRequest, viewParamsGql, &filterParams);
}


void CObjectCollectionControllerCompBase::SetAdditionalFilters(const imtgql::CGqlRequest& /*gqlRequest*/, const imtgql::CGqlObject& /*viewParamsGql*/, iprm::CParamsSet* /*filterParams*/) const
{
}


void CObjectCollectionControllerCompBase::SetObjectFilter(
		const imtgql::CGqlRequest& /*gqlRequest*/,
		const imtbase::CTreeItemModel& objectFilterModel,
		iprm::CParamsSet& filterParams) const
{
	QStringList keys = objectFilterModel.GetKeys();

	for (const QString& key : keys){
		QByteArray value = objectFilterModel.GetData(key.toUtf8()).toByteArray();

		iprm::CTextParam* idParamPtr = new iprm::CTextParam();
		idParamPtr->SetText(value);

		filterParams.SetEditableParameter(key.toUtf8(), idParamPtr, true);
	}
}


} // namespace imtgql


