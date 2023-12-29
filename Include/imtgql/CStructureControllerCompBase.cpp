#include <imtgql/CStructureControllerCompBase.h>


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
// #include <imtbase/CCollectionFilter.h>
#include <imtbase/ICollectionStructureIterator.h>
#include <imtbase/COperationContext.h>
// #include <imtbase/CObjectCollection.h>
#include <imtbase/COperationDescription.h>
#include <imtbase/ICollectionStructureController.h>
#include <imtgql/imtgql.h>


namespace imtgql
{


// reimplemented (imtgql::IGqlRequestHandler)

bool CStructureControllerCompBase::IsRequestSupported(const CGqlRequest& gqlRequest) const
{
    bool retVal = false;

    QByteArray commandId = gqlRequest.GetCommandId();

    for (int index = 0; index < m_commandIdsAttrPtr.GetCount(); index++){
        if (commandId == *m_structureIdAttrPtr + m_commandIdsAttrPtr[index]){
            retVal = true;

            break;
        }
    }

    return retVal;
}


// reimplemented (imtgql::CGqlRepresentationDataControllerComp)

imtbase::CTreeItemModel* CStructureControllerCompBase::CreateInternalResponse(
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
    case OT_INSERT_NEW_NODE:
        return InsertNewNode(gqlRequest, errorMessage);
    case OT_SET_NODE_NAME:
        return SetNodeName(gqlRequest, errorMessage);
    case OT_SET_NODE_DESCRIPTION:
        return SetNodeDescription(gqlRequest, errorMessage);
    case OT_SET_NODE_METAINFO:
        return SetNodeMetaInfo(gqlRequest, errorMessage);
    case OT_MOVE_NODE:
        return MoveNode(gqlRequest, errorMessage);
    case OT_REMOVE_NODE:
        return RemoveNode(gqlRequest, errorMessage);
    case OT_INSERT_NEW_OBJECT:
        return InsertNewObject(gqlRequest, errorMessage);
    case OT_MOVE_OBJECT:
        return MoveObject(gqlRequest, errorMessage);
    case OT_REMOVE_OBJECT:
        return RemoveObject(gqlRequest, errorMessage);
    case OT_GET_NODE_COUNT:
        return GetNodeCount(gqlRequest, errorMessage);
    case OT_GET_NODE_IDS:
        return GetNodeIds(gqlRequest, errorMessage);
    case OT_GET_NODE_INFO:
        return GetNodeInfo(gqlRequest, errorMessage);
    case OT_GET_OBJECT_PARENT_NODE_IDS:
        return GetObjectParentNodeIds(gqlRequest, errorMessage);
	case OT_GET_ELEMENTS:
		return GetElements(gqlRequest, errorMessage);
	}

	errorMessage = QString("Unable to create internal response. Operation is not supported");
    SendErrorMessage(0, errorMessage, "CStructureControllerCompBase");

	return nullptr;
}


// reimplemented (icomp::CComponentBase)

void CStructureControllerCompBase::OnComponentCreated()
{
    BaseClass::OnComponentCreated();
}


// protected methods

bool CStructureControllerCompBase::GetOperationFromRequest(
			const imtgql::CGqlRequest& gqlRequest,
			imtgql::CGqlObject& gqlObject,
			QString& errorMessage,
			int& operationType) const
{
    QByteArray commandId = gqlRequest.GetCommandId();

    if (commandId == *m_structureIdAttrPtr + "InsertNewNode"){
        operationType = OT_INSERT_NEW_NODE;
        return true;
    }
    if (commandId == *m_structureIdAttrPtr + "SetNodeName"){
        operationType = OT_SET_NODE_NAME;
        return true;
    }
    if (commandId == *m_structureIdAttrPtr + "SetNodeDescription"){
        operationType = OT_SET_NODE_DESCRIPTION;
        return true;
    }
    if (commandId == *m_structureIdAttrPtr + "SetNodeMetaInfo"){
        operationType = OT_SET_NODE_METAINFO;
        return true;
    }
    if (commandId == *m_structureIdAttrPtr + "MoveNode"){
        operationType = OT_MOVE_NODE;
        return true;
    }
    if (commandId == *m_structureIdAttrPtr + "RemoveNode"){
        operationType = OT_REMOVE_NODE;
        return true;
    }
	if (commandId == *m_structureIdAttrPtr + "InsertNewObject"){
        operationType = OT_INSERT_NEW_OBJECT;
        return true;
    }
    if (commandId == *m_structureIdAttrPtr + "MoveObject"){
        operationType = OT_MOVE_OBJECT;
        return true;
    }
    if (commandId == *m_structureIdAttrPtr + "RemoveObject"){
        operationType = OT_REMOVE_OBJECT;
        return true;
    }
    if (commandId == *m_structureIdAttrPtr + "GetNodeCount"){
        operationType = OT_GET_NODE_COUNT;
        return true;
    }
    if (commandId == *m_structureIdAttrPtr + "GetNodeIds"){
        operationType = OT_GET_NODE_IDS;
        return true;
    }
    if (commandId == *m_structureIdAttrPtr + "GetNodeInfo"){
        operationType = OT_GET_NODE_INFO;
        return true;
    }
    if (commandId == *m_structureIdAttrPtr + "GetObjectParentNodeIds"){
        operationType = OT_GET_OBJECT_PARENT_NODE_IDS;
        return true;
    }
	if (commandId == *m_structureIdAttrPtr + "GetNodes"){
		operationType = OT_GET_NODES;
		return true;
	}
	if (commandId == *m_structureIdAttrPtr + "GetElements"){
		operationType = OT_GET_ELEMENTS;
		return true;
	}


	errorMessage = QString("Unable to get the operation type from the request");

    SendErrorMessage(0, errorMessage, "CStructureControllerCompBase");

	return false;
}


QByteArray CStructureControllerCompBase::GetObjectIdFromInputParams(const QList<imtgql::CGqlObject>& inputParams) const
{
	int count = inputParams.count();
	for (int i = 0; i < count; i++){
		if (inputParams.at(i).GetFieldIds().contains("Id")){
			return inputParams.at(i).GetFieldArgumentValue("Id").toByteArray();
		}
	}
	return QByteArray();
}


imtbase::CTreeItemModel* CStructureControllerCompBase::InsertNewNode(
		const imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	if (!m_collectionStructureCompPtr.IsValid()){
		errorMessage = QString("Unable to insert new node. Component reference 'CollectionStructure' was not set");

		SendCriticalMessage(0, errorMessage);

		return nullptr;
	}

	const QList<imtgql::CGqlObject> inputParams = gqlRequest.GetParams();

	QByteArray nodeId;
	QByteArray parentNodeId;
	QString newName;
	QString description;
	if (inputParams.size() > 0){
		nodeId = inputParams.at(0).GetFieldArgumentValue("Id").toByteArray();
		parentNodeId = inputParams.at(0).GetFieldArgumentValue("ParentNodeId").toByteArray();
		newName = inputParams.at(0).GetFieldArgumentValue("NewName").toString();
		description = inputParams.at(0).GetFieldArgumentValue("Description").toString();
	}

	istd::TDelPtr<imtbase::IOperationContext> operationContextPtr;

	if (m_operationContextControllerCompPtr.IsValid()){
		operationContextPtr.SetPtr(m_operationContextControllerCompPtr->CreateOperationContext(imtbase::IDocumentChangeGenerator::OT_CREATE, gqlRequest));
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModel = rootModelPtr->AddTreeModel("data");

	if (!nodeId.isEmpty()){
		nodeId = m_collectionStructureCompPtr->InsertNewNode(newName, description, nodeId, parentNodeId, nullptr, operationContextPtr.GetPtr());
		dataModel->SetData("Id", nodeId);
		dataModel->SetData("ParentNodeId", parentNodeId);
		dataModel->SetData("successful", !nodeId.isEmpty());
	}

	return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CStructureControllerCompBase::SetNodeName(
	const imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	if (!m_collectionStructureCompPtr.IsValid()){
		errorMessage = QString("Unable to rename object. Component reference 'CollectionStructure' was not set");

		SendCriticalMessage(0, errorMessage);

		return nullptr;
	}

	const QList<imtgql::CGqlObject> inputParams = gqlRequest.GetParams();

	QByteArray nodeId;
	QString newName;
	if (inputParams.size() > 0){
		nodeId = inputParams.at(0).GetFieldArgumentValue("Id").toByteArray();
		newName = inputParams.at(0).GetFieldArgumentValue("NewName").toString();
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModel = rootModelPtr->AddTreeModel("data");

	if (!nodeId.isEmpty()){
		dataModel->SetData("Id", nodeId);
		dataModel->SetData("Name", newName);
		bool successful = m_collectionStructureCompPtr->SetNodeName(nodeId, newName);
		dataModel->SetData("successful", successful);
	}

	return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CStructureControllerCompBase::SetNodeDescription(
    const imtgql::CGqlRequest& /*gqlRequest*/,
    QString& /*errorMessage*/) const
{
    return nullptr;
}


imtbase::CTreeItemModel* CStructureControllerCompBase::SetNodeMetaInfo(
    const imtgql::CGqlRequest& /*gqlRequest*/,
    QString& /*errorMessage*/) const
{
    return nullptr;
}


imtbase::CTreeItemModel* CStructureControllerCompBase::MoveNode(
    const imtgql::CGqlRequest& /*gqlRequest*/,
    QString& /*errorMessage*/) const
{
    return nullptr;
}


imtbase::CTreeItemModel* CStructureControllerCompBase::RemoveNode(
    const imtgql::CGqlRequest& /*gqlRequest*/,
    QString& /*errorMessage*/) const
{
    return nullptr;
}


imtbase::CTreeItemModel* CStructureControllerCompBase::InsertNewObject(
	const imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid() || !m_collectionStructureCompPtr.IsValid() || !m_gqlRequestExtractorCompPtr.IsValid()){
		errorMessage = QT_TR_NOOP("Internal error");
		SendErrorMessage(0, "Internal error", "Structure controller");

		return nullptr;
	}

	QByteArray objectId;
	QString name;
	QString description;
	QByteArray typeId;
	QByteArray nodeId;
	QByteArray selectIndex;
	const QList<imtgql::CGqlObject> params = gqlRequest.GetParams();

	if (params.size() > 0){
		// typeId = params.at(0).GetFieldArgumentValue("typeId").toByteArray();
		// name = params.at(0).GetFieldArgumentValue("name").toByteArray();
		// description = params.at(0).GetFieldArgumentValue("description").toString();
		const CGqlObject* additionObject = params.at(0).GetFieldArgumentObjectPtr("addition");
		if (additionObject != nullptr){
			nodeId = additionObject->GetFieldArgumentValue("nodeId").toByteArray();
			selectIndex = additionObject->GetFieldArgumentValue("selectIndex").toByteArray();
		}
	}

	if (typeId.isEmpty()){
		typeId = "DocumentInfo";
	}

	istd::IChangeable* newObjectPtr = m_gqlRequestExtractorCompPtr->ExtractObject(gqlRequest, objectId, name, description, errorMessage);
	if (newObjectPtr == nullptr){
		SendErrorMessage(0, "Unable to create object from gql request", "Object collection controller");

		return nullptr;
	}

	// imtbase::IObjectCollection::DataPtr dataPtr;
	// if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
	// 	errorMessage = QT_TR_NOOP("Object with this ID already exists");
	// 	SendErrorMessage(0, QString("Object with ID: %1 already exists").arg(qPrintable(objectId)), "Object collection controller");

	// 	return nullptr;
	// }

	istd::TDelPtr<imtbase::IOperationContext> operationContextPtr;

	if (m_operationContextControllerCompPtr.IsValid()){
		operationContextPtr.SetPtr(m_operationContextControllerCompPtr->CreateOperationContext(imtbase::IDocumentChangeGenerator::OT_CREATE, gqlRequest));
	}
	imtbase::ICollectionStructureController* collectionStructureController = m_collectionStructureCompPtr->GetHierarchicalStructureController();
	if (collectionStructureController == nullptr){
		return nullptr;
	}

	QByteArray newObjectId =  collectionStructureController->InsertNewObjectIntoCollection(m_objectCollectionCompPtr.GetPtr(), nodeId, typeId, name, description, newObjectPtr, objectId, nullptr, nullptr, operationContextPtr.GetPtr());
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


imtbase::CTreeItemModel* CStructureControllerCompBase::MoveObject(
    const imtgql::CGqlRequest& /*gqlRequest*/,
    QString& /*errorMessage*/) const
{
    return nullptr;
}


imtbase::CTreeItemModel* CStructureControllerCompBase::RemoveObject(
    const imtgql::CGqlRequest& /*gqlRequest*/,
    QString& /*errorMessage*/) const
{
    return nullptr;
}


imtbase::CTreeItemModel* CStructureControllerCompBase::GetNodeCount(
    const imtgql::CGqlRequest& /*gqlRequest*/,
    QString& /*errorMessage*/) const
{
    return nullptr;
}


imtbase::CTreeItemModel* CStructureControllerCompBase::GetNodeIds(
    const imtgql::CGqlRequest& /*gqlRequest*/,
    QString& /*errorMessage*/) const
{
	if (!m_collectionStructureCompPtr.IsValid()){
		return nullptr;
	}

	return nullptr;
}


imtbase::CTreeItemModel* CStructureControllerCompBase::GetNodeInfo(
    const imtgql::CGqlRequest& /*gqlRequest*/,
    QString& /*errorMessage*/) const
{
    return nullptr;
}


imtbase::CTreeItemModel* CStructureControllerCompBase::GetObjectParentNodeIds(
    const imtgql::CGqlRequest& /*gqlRequest*/,
    QString& /*errorMessage*/) const
{
    return nullptr;
}


imtbase::CTreeItemModel* CStructureControllerCompBase::GetNodes(
	const imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	if (!m_collectionStructureCompPtr.IsValid()){
		errorMessage = QString("Unable to list nodes. Component reference 'CollectionStructure' was not set");

		SendCriticalMessage(0, errorMessage);

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
			// PrepareFilters(gqlRequest, *viewParamsGql, filterParams);
		}

		int nodesCount = m_collectionStructureCompPtr->GetNodeCount(&filterParams);

		int pagesCount = std::ceil(nodesCount / (double)count); /// count == 0
		if (pagesCount <= 0){
			pagesCount = 1;
		}

		notificationModel->SetData("PagesCount", pagesCount);
		notificationModel->SetData("TotalCount", nodesCount);

		istd::TDelPtr<imtbase::ICollectionStructureIterator> collectionStructureIterator(m_collectionStructureCompPtr->CreateCollectionStructureIterator(offset, count, &filterParams));
		if (collectionStructureIterator != nullptr){
			while (collectionStructureIterator->Next()){
				imtbase::ICollectionStructureInfo::NodeInfo nodeInfo = collectionStructureIterator->GetNodeInfo();
				int itemIndex = itemsModel->InsertNewItem();
				if (itemIndex >= 0){
					if (!SetupNodeItem(gqlRequest, *itemsModel, itemIndex, collectionStructureIterator.GetPtr(), errorMessage)){
						SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

						return nullptr;
					}
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


imtbase::CTreeItemModel* CStructureControllerCompBase::GetElements(
	const imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	return nullptr;
}


bool CStructureControllerCompBase::SetupNodeItem(
			const imtgql::CGqlRequest& gqlRequest,
			imtbase::CTreeItemModel& model,
			int itemIndex,
			const imtbase::ICollectionStructureIterator* collectionStructureIterator,
			QString& errorMessage) const
{
	return false;
}


bool CStructureControllerCompBase::SetupObjectItem(
			const imtgql::CGqlRequest& gqlRequest,
			imtbase::CTreeItemModel& model,
			int itemIndex,
			const imtbase::IObjectCollectionIterator* objectCollectionIterator,
			QString& errorMessage) const
{
	return false;
}


QByteArrayList CStructureControllerCompBase::GetInformationIds(const imtgql::CGqlRequest& gqlRequest, const QByteArray& objectId) const
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



} // namespace imtgql


