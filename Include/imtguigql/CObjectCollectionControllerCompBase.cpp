#include <imtguigql/CObjectCollectionControllerCompBase.h>

// ACF includes
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtqml/CCommandDataEnumProviderComp.h>
#include <idoc/CStandardDocumentMetaInfo.h>
#include <imtbase/CCollectionFilter.h>


namespace imtguigql
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


// reimplemented (imtgql::IGqlRepresentationDataController)

imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::CreateResponse(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (m_modelIdsCompPtr.FindValue(gqlRequest.GetCommandId()) == -1){
		return nullptr;
	}

	const QList<imtgql::CGqlObject>* inputParamsPtr = gqlRequest.GetParams();
	const QList<imtgql::CGqlObject>* inputFieldsPtr = gqlRequest.GetFields();
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
	case OT_METAINFO:
		return GetMetaInfo(*inputParamsPtr, gqlObject, errorMessage);
	case OT_USER_OPERATION + 1:
		return GetTreeItemModel(*inputParamsPtr, gqlObject, errorMessage);
	case OT_USER_OPERATION + 2:
		return GetDependencies(*inputParamsPtr, gqlObject, errorMessage);
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
		if (fieldList->at(i).GetId() == "rename"){
			gqlObject = fieldList->at(i);
			operationType = OT_RENAME;
			return true;
		}
		if (fieldList->at(i).GetId() == "setDescription"){
			gqlObject = fieldList->at(i);
			operationType = OT_SET_DESCRIPTION;
			return true;
		}
		if (fieldList->at(i).GetId() == "metaInfo"){
			gqlObject = fieldList->at(i);
			operationType = OT_METAINFO;
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
		QString name, description;
		QByteArray objectId;

		istd::IChangeable* newObject = CreateObject(inputParams, objectId, name, description, errorMessage);
		if (newObject != nullptr){
			newObjectId = m_objectCollectionCompPtr->InsertNewObject("", name, description, newObject, objectId);
		}

		if (errorMessage.isEmpty() && newObjectId.isEmpty()){
			errorMessage = QObject::tr("Can not insert object: %1").arg(QString(objectId));
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

	QByteArray oldObjectId = inputParams.at(0).GetFieldArgumentValue("Id").toByteArray();
	QByteArray newObjectId;
	QString name, description;
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();
	}
	else{
		istd::IChangeable* savedObject = CreateObject(inputParams, newObjectId, name, description, errorMessage);
		if (savedObject != nullptr){
			if (m_objectCollectionCompPtr->SetObjectData(oldObjectId, *savedObject) == false){
				errorMessage = QObject::tr("Can not update object: %1").arg(QString(oldObjectId));
			}
		}
		else {
			errorMessage = QObject::tr("Can not create object for update: %1").arg(QString(oldObjectId));
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
		notificationModel->SetData("Name", name);
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
	QByteArray objectId = inputParams.at(0).GetFieldArgumentValue("Id").toByteArray();
	QString newName = inputParams.at(0).GetFieldArgumentValue("NewName").toString();

	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* dataModel = nullptr;

	if (!objectId.isEmpty()){
		dataModel = new imtbase::CTreeItemModel();
		imtbase::CTreeItemModel* itemsModel = new imtbase::CTreeItemModel();

		itemsModel->SetData("NewId", newName);
		itemsModel->SetData("NewName", newName);

		m_objectCollectionCompPtr->SetObjectName(objectId, newName);
		dataModel->SetExternTreeModel("item", itemsModel);
	}

	rootModel->SetExternTreeModel("data", dataModel);

	return rootModel;
}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::SetObjectDescription(
			const QList<imtgql::CGqlObject>& inputParams,
			const imtgql::CGqlObject& gqlObject,
			QString& errorMessage) const
{
	QByteArray objectId = inputParams.at(0).GetFieldArgumentValue("Id").toByteArray();
	QString description = inputParams.at(0).GetFieldArgumentValue("Description").toString();

	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();

	imtbase::CTreeItemModel* dataModel = nullptr;

	if (!objectId.isEmpty()){

		dataModel = new imtbase::CTreeItemModel();
		imtbase::CTreeItemModel* itemsModel = new imtbase::CTreeItemModel();

		itemsModel->SetData("Id", objectId);
		itemsModel->SetData("Description", description);

		dataModel->SetExternTreeModel("item", itemsModel);

		m_objectCollectionCompPtr->SetObjectDescription(objectId, description);
	}

	rootModel->SetExternTreeModel("data", dataModel);

	return rootModel;
}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::ListObjects(
			const QList<imtgql::CGqlObject>& inputParams,
			const imtgql::CGqlObject& gqlObject,
			QString& errorMessage) const
{
	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* dataModel = nullptr;
	imtbase::CTreeItemModel* itemsModel = nullptr;
	imtbase::CTreeItemModel* notificationModel = nullptr;

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
		notificationModel = new imtbase::CTreeItemModel();

		const imtgql::CGqlObject* viewParamsGql = nullptr;
		if (inputParams.size() > 0){
			viewParamsGql = inputParams.at(0).GetFieldArgumentObjectPtr("viewParams");
		}

		imtbase::CCollectionFilter m_filter;
		int offset = 0, count = -1;
		if (viewParamsGql != nullptr){
			offset = viewParamsGql->GetFieldArgumentValue("Offset").toInt();
			count = viewParamsGql->GetFieldArgumentValue("Count").toInt();

			QByteArray filterBA = viewParamsGql->GetFieldArgumentValue("FilterModel").toByteArray();
			if (!filterBA.isEmpty()){
				imtbase::CTreeItemModel generalModel;
				generalModel.Parse(filterBA);

				imtbase::CTreeItemModel* filterModel = generalModel.GetTreeItemModel("FilterIds");
				if (filterModel != nullptr){
					for (int i = 0; i < filterModel->GetItemsCount(); i++){
						QByteArray headerId = filterModel->GetData("Id").toByteArray();
						if (!headerId.isEmpty()){
							m_filter.SetFilteringInfoIds(QByteArrayList() << headerId);
						}
					}
				}

				QString filterText = generalModel.GetData("TextFilter").toString();
				if (!filterText.isEmpty()){
					m_filter.SetTextFilter(filterText);
				}

				imtbase::CTreeItemModel* sortModel = generalModel.GetTreeItemModel("Sort");
				if (sortModel != nullptr){
					QByteArray headerId = sortModel->GetData("HeaderId").toByteArray();
					QByteArray sortOrder = sortModel->GetData("SortOrder").toByteArray();
					if (!headerId.isEmpty() && !sortOrder.isEmpty()){
						m_filter.SetSortingOrder(sortOrder == "ASC" ? imtbase::ICollectionFilter::SO_ASC : imtbase::ICollectionFilter::SO_DESC);
						m_filter.SetSortingInfoIds(QByteArrayList() << headerId);
					}
				}
			}
		}
		iprm::CParamsSet filterParams;
		filterParams.SetEditableParameter("Filter", &m_filter);

		int pagesCount = qCeil(m_objectCollectionCompPtr->GetElementsCount(&filterParams) / (double)count);
		if (pagesCount < 0){
			pagesCount = 1;
		}

		notificationModel->SetData("PagesCount", pagesCount);

		imtbase::ICollectionInfo::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds(offset, count, &filterParams);
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
		dataModel->SetExternTreeModel("notification", notificationModel);
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
		errorMessage = QObject::tr("No object-ID could not be extracted from the request");

		return nullptr;
	}

	bool retVal = m_objectCollectionCompPtr->RemoveObject(objectId);
	if (!retVal){
		errorMessage = QObject::tr("Can not remove object: %1").arg(QString(objectId));
	}

	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* dataModel = nullptr;
	imtbase::CTreeItemModel* notificationModel = nullptr;

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsModel = rootModel->AddTreeModel("errors");
		errorsModel->SetData("message", errorMessage);
	}
	else{
		dataModel = new imtbase::CTreeItemModel();
		notificationModel = new imtbase::CTreeItemModel();
		notificationModel->SetData("Id", objectId);
		dataModel->SetExternTreeModel("removedNotification", notificationModel);
	}
	rootModel->SetExternTreeModel("data", dataModel);

	return rootModel;
}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::GetHeaders(
			const QList<imtgql::CGqlObject>& inputParams,
			const imtgql::CGqlObject& gqlObject,
			QString& errorMessage) const
{
	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* dataModel = nullptr;
	QByteArrayList fields;

	if (!m_headersProviderCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();
		imtbase::CTreeItemModel* errorsItemModel = rootModel->AddTreeModel("errors");
		errorsItemModel->SetData("message", errorMessage);
	}
	else{
		dataModel = new imtbase::CTreeItemModel();

		imtbase::CTreeItemModel* headersModel = m_headersProviderCompPtr->GetTreeItemModel(inputParams, fields);
		if (headersModel != nullptr){
			headersModel->SetIsArray(true);
			dataModel->SetExternTreeModel("headers", headersModel);
		}
		rootModel->SetExternTreeModel("data", dataModel);
	}

	return rootModel;
}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::GetTreeItemModel(
		const QList<imtgql::CGqlObject>& inputParams,
		const imtgql::CGqlObject& gqlObject,
		QString& errorMessage) const
{
	return nullptr;
}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::GetDependencies(
		const QList<imtgql::CGqlObject> &inputParams,
		const imtgql::CGqlObject &gqlObject,
		QString &errorMessage) const
{
	return nullptr;
}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::GetMetaInfo(
		const QList<imtgql::CGqlObject> &inputParams,
		const imtgql::CGqlObject &gqlObject,
		QString &errorMessage) const
{
	return nullptr;
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
							.toDateTime().toString("dd.MM.yyyy hh:mm:ss");
				}
				else if (informationId == QByteArray("ModificationTime")){
					elementInformation = metaInfo.GetMetaInfo(imtbase::IObjectCollection::MIT_LAST_OPERATION_TIME)
							.toDateTime().toString("dd.MM.yyyy hh:mm:ss");
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


istd::IChangeable* CObjectCollectionControllerCompBase::CreateObject(
			const QList<imtgql::CGqlObject>& inputParams,
			QByteArray& objectId,
			QString& name,
			QString& description,
			QString &errorMessage) const
{
	return nullptr;
}


} // namespace imtgql


