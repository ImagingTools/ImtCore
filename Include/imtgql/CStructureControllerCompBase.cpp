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
    if (commandId == *m_structureIdAttrPtr + "UpdatedNotification"){
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
		const imtgql::CGqlRequest& /*gqlRequest*/,
		QString& /*errorMessage*/) const
{
	return nullptr;
}


imtbase::CTreeItemModel* CStructureControllerCompBase::SetNodeName(
    const imtgql::CGqlRequest& /*gqlRequest*/,
    QString& /*errorMessage*/) const
{
    return nullptr;
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
    const imtgql::CGqlRequest& /*gqlRequest*/,
    QString& /*errorMessage*/) const
{
    return nullptr;
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
	m_collectionStructureCompPtr->GetNodeIds();

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

		int elementsCount = m_collectionStructureCompPtr->GetNodeCount(&filterParams);

		int pagesCount = std::ceil(elementsCount / (double)count); /// count == 0
		if (pagesCount <= 0){
			pagesCount = 1;
		}

		notificationModel->SetData("PagesCount", pagesCount);
		notificationModel->SetData("TotalCount", elementsCount);

		istd::TDelPtr<imtbase::ICollectionStructureIterator> collectionStructureIterator(m_collectionStructureCompPtr->CreateCollectionStructureIterator(offset, count, &filterParams));
		if (collectionStructureIterator != nullptr){
			while (collectionStructureIterator->Next()){
				imtbase::ICollectionStructureInfo::NodeInfo nodeInfo = collectionStructureIterator->GetNodeInfo();
				int itemIndex = itemsModel->InsertNewItem();
				if (itemIndex >= 0){
					if (!SetupGqlItem(gqlRequest, *itemsModel, itemIndex, collectionStructureIterator.GetPtr(), errorMessage)){
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


bool CStructureControllerCompBase::SetupGqlItem(
	const imtgql::CGqlRequest& gqlRequest,
	imtbase::CTreeItemModel& model,
	int itemIndex,
	const imtbase::ICollectionStructureIterator* collectionStructureIterator,
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


QVariant CStructureControllerCompBase::GetObjectInformation(const QByteArray& /*informationId*/, const QByteArray& /*objectId*/) const
{
	return QVariant();
}


istd::IChangeable* CStructureControllerCompBase::CreateObject(
			const QList<imtgql::CGqlObject>& /*inputParams*/,
			QByteArray& /*objectId*/,
			QString& /*name*/,
			QString& /*description*/,
			QString& /*errorMessage*/) const
{
	return nullptr;
}


istd::IChangeable* CStructureControllerCompBase::CreateObject(
			const imtgql::CGqlRequest& gqlRequest,
			QByteArray& newObjectId,
			QString& name,
			QString& description,
			QString& errorMessage) const
{
	const QList<imtgql::CGqlObject> inputParams = gqlRequest.GetParams();

	return CreateObject(inputParams, newObjectId, name, description, errorMessage);
}


void CStructureControllerCompBase::PrepareFilters(
		const imtgql::CGqlRequest& gqlRequest,
		const imtgql::CGqlObject& viewParamsGql,
		iprm::CParamsSet& filterParams) const
{
    // istd::TDelPtr<imtbase::CCollectionFilter> collectionFilterPtr;
    // collectionFilterPtr.SetPtr(new imtbase::CCollectionFilter);

    // istd::TDelPtr<iprm::CParamsSet> objectFilterPtr;
    // objectFilterPtr.SetPtr(new iprm::CParamsSet);

    // QByteArray filterBA = viewParamsGql.GetFieldArgumentValue("FilterModel").toByteArray();
    // if (!filterBA.isEmpty()){
    // 	imtbase::CTreeItemModel generalModel;
    // 	generalModel.CreateFromJson(filterBA);

    // 	imtbase::CTreeItemModel* filterModel = generalModel.GetTreeItemModel("FilterIds");
    // 	if (filterModel != nullptr){
    // 		QByteArrayList filteringInfoIds;
    // 		for (int i = 0; i < filterModel->GetItemsCount(); i++){
    // 			QByteArray headerId = filterModel->GetData("Id", i).toByteArray();
    // 			if (!headerId.isEmpty()){
    // 				filteringInfoIds << headerId;
    // 			}
    // 		}
    // 		collectionFilterPtr->SetFilteringInfoIds(filteringInfoIds);
    // 	}

    // 	QString filterText = generalModel.GetData("TextFilter").toString();
    // 	if (!filterText.isEmpty()){
    // 		collectionFilterPtr->SetTextFilter(filterText);
    // 	}

    // 	imtbase::CTreeItemModel* sortModel = generalModel.GetTreeItemModel("Sort");
    // 	if (sortModel != nullptr){
    // 		QByteArray headerId = sortModel->GetData("HeaderId").toByteArray();
    // 		QByteArray sortOrder = sortModel->GetData("SortOrder").toByteArray();
    // 		if (!headerId.isEmpty() && !sortOrder.isEmpty()){
    // 			collectionFilterPtr->SetSortingOrder(sortOrder == "ASC" ? imtbase::ICollectionFilter::SO_ASC : imtbase::ICollectionFilter::SO_DESC);
    // 			collectionFilterPtr->SetSortingInfoIds(QByteArrayList() << headerId);
    // 		}
    // 	}

    // 	if (generalModel.ContainsKey("ObjectFilter")){
    // 		imtbase::CTreeItemModel* objectFilterModelPtr = generalModel.GetTreeItemModel("ObjectFilter");
    // 		SetObjectFilter(gqlRequest, *objectFilterModelPtr, *objectFilterPtr);
    // 	}
    // 	else{
    // 		imtbase::CTreeItemModel objectFilterModel;
    // 		SetObjectFilter(gqlRequest, objectFilterModel, *objectFilterPtr);
    // 	}
    // }

    // filterParams.SetEditableParameter("Filter", collectionFilterPtr.PopPtr());
    // filterParams.SetEditableParameter("ObjectFilter", objectFilterPtr.PopPtr());

    // this->SetAdditionalFilters(viewParamsGql, &filterParams);
}


void CStructureControllerCompBase::SetAdditionalFilters(const imtgql::CGqlObject& /*viewParamsGql*/, iprm::CParamsSet* /*filterParams*/) const
{
}


void CStructureControllerCompBase::SetObjectFilter(
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


