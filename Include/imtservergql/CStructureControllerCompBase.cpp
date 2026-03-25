// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CStructureControllerCompBase.h>


// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

// ACF includes
#include <iprm/CTextParam.h>
#include <istd/TDelPtr.h>
#include <iser/CJsonMemReadArchive.h>
#include <istd/TSingleFactory.h>

// ImtCore includes
#include <imtbase/IHierarchicalStructureIterator.h>
#include <imtbase/COperationContext.h>
#include <imtbase/IStructuredObjectCollectionController.h>
#include <imtgql/imtgql.h>


namespace imtservergql
{


// reimplemented (imtgql::IGqlRequestHandler)

bool CStructureControllerCompBase::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
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


// reimplemented (imtservergql::CGqlRepresentationDataControllerComp)

QJsonObject CStructureControllerCompBase::CreateInternalResponse(
	const imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	imtgql::CGqlParamObject gqlObject;

	int operationType = OT_UNKNOWN;
	if (!GetOperationFromRequest(gqlRequest, gqlObject, errorMessage, operationType)){
		return QJsonObject();
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
		return GetItemInfo(gqlRequest, errorMessage);
	case OT_GET_OBJECT_PARENT_NODE_IDS:
		return GetObjectParentNodeIds(gqlRequest, errorMessage);
	case OT_GET_ELEMENTS:
		return GetElements(gqlRequest, errorMessage);
	}

	errorMessage = QString("Unable to create internal response. Operation is not supported");
	SendErrorMessage(0, errorMessage, "CStructureControllerCompBase");

	return QJsonObject();
}


// protected methods

bool CStructureControllerCompBase::GetOperationFromRequest(
	const imtgql::CGqlRequest& gqlRequest,
	imtgql::CGqlParamObject& /*gqlObject*/,
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
	if (commandId == *m_structureIdAttrPtr + "GetItemInfo"){
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


QByteArray CStructureControllerCompBase::GetObjectIdFromInputParams(const QList<imtgql::CGqlParamObject>& inputParams) const
{
	qsizetype count = inputParams.size();
	for (int i = 0; i < count; i++){
		if (inputParams.at(i).GetParamIds().contains("Id")){
			return inputParams.at(i).GetParamArgumentValue("Id").toByteArray();
		}
	}
	return QByteArray();
}


QJsonObject CStructureControllerCompBase::InsertNewNode(
		const imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	if (!m_collectionStructureCompPtr.IsValid()){
		errorMessage = QString("Unable to insert new node. Component reference 'CollectionStructure' was not set");

		SendCriticalMessage(0, errorMessage);

		return QJsonObject();
	}

	const imtgql::CGqlParamObject& inputParams = gqlRequest.GetParams();

	QByteArray nodeId;
	QByteArray parentNodeId;
	QString newName;
	QString description;
	nodeId = inputParams.GetParamArgumentValue("Id").toByteArray();
	parentNodeId = inputParams.GetParamArgumentValue("ParentNodeId").toByteArray();
	newName = inputParams.GetParamArgumentValue("NewName").toString();
	description = inputParams.GetParamArgumentValue("Description").toString();

	QJsonObject rootObj;
	QJsonObject dataObj;

	if (!nodeId.isEmpty()){
		nodeId = m_collectionStructureCompPtr->InsertNewNode(newName, description, nodeId, parentNodeId, nullptr);
		dataObj.insert(QStringLiteral("Id"), QJsonValue::fromVariant(nodeId));
		dataObj.insert(QStringLiteral("ParentNodeId"), QJsonValue::fromVariant(parentNodeId));
		dataObj.insert(QStringLiteral("successful"), QJsonValue::fromVariant(!nodeId.isEmpty()));
	}

	rootObj.insert(QStringLiteral("data"), dataObj);
	return rootObj;
}


QJsonObject CStructureControllerCompBase::SetNodeName(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!m_collectionStructureCompPtr.IsValid()){
		errorMessage = QString("Unable to rename object. Component reference 'CollectionStructure' was not set");

		SendCriticalMessage(0, errorMessage);

		return QJsonObject();
	}

	const imtgql::CGqlParamObject& inputParams = gqlRequest.GetParams();

	QByteArray nodeId;
	QString newName;

	nodeId = inputParams.GetParamArgumentValue("Id").toByteArray();
	newName = inputParams.GetParamArgumentValue("NewName").toString();

	QJsonObject rootObj;
	QJsonObject dataObj;

	if (!nodeId.isEmpty()){
		dataObj.insert(QStringLiteral("Id"), QJsonValue::fromVariant(nodeId));
		dataObj.insert(QStringLiteral("Name"), QJsonValue::fromVariant(newName));
		bool successful = m_collectionStructureCompPtr->SetNodeName(nodeId, newName);
		dataObj.insert(QStringLiteral("successful"), QJsonValue::fromVariant(successful));
	}

	rootObj.insert(QStringLiteral("data"), dataObj);
	return rootObj;
}


QJsonObject CStructureControllerCompBase::SetNodeDescription(
	const imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	return QJsonObject();
}


QJsonObject CStructureControllerCompBase::SetNodeMetaInfo(
	const imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	return QJsonObject();
}


QJsonObject CStructureControllerCompBase::MoveNode(
	const imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	return QJsonObject();
}


QJsonObject CStructureControllerCompBase::RemoveNode(
	const imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	return QJsonObject();
}


QJsonObject CStructureControllerCompBase::InsertNewObject(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid() || !m_collectionStructureCompPtr.IsValid() || !m_gqlRequestExtractorCompPtr.IsValid()){
		errorMessage = QT_TR_NOOP("Internal error");
		SendErrorMessage(0, "Internal error", "Structure controller");

		return QJsonObject();
	}

	QByteArray objectId;
	QString name;
	QString description;
	QByteArray typeId;
	QByteArray nodeId;
	QByteArray selectIndex;
	const imtgql::CGqlParamObject& inputParams = gqlRequest.GetParams();
	const imtgql::CGqlParamObject* additionObject = nullptr;
	const imtgql::CGqlParamObject* inputObject = inputParams.GetParamArgumentObjectPtr("input");
	if (inputObject != nullptr){
		additionObject = inputObject->GetParamArgumentObjectPtr("addition");
	}

	if (additionObject != nullptr){
		nodeId = additionObject->GetParamArgumentValue("nodeId").toByteArray();
		selectIndex = additionObject->GetParamArgumentValue("selectIndex").toByteArray();
	}

	if (typeId.isEmpty()){
		typeId = "DocumentInfo";
	}

	istd::IChangeableUniquePtr newObjectPtr = m_gqlRequestExtractorCompPtr->ExtractObject(gqlRequest, objectId, errorMessage);
	if (!newObjectPtr.IsValid()){
		SendErrorMessage(0, "Unable to create object from gql request", "Object collection controller");

		return QJsonObject();
	}

	istd::TDelPtr<imtbase::IOperationContext> operationContextPtr;

	if (m_operationContextControllerCompPtr.IsValid()){
		operationContextPtr.SetPtr(m_operationContextControllerCompPtr->CreateOperationContext("Create", objectId, newObjectPtr.GetPtr()));
	}

	imtbase::IStructuredObjectCollectionController* collectionStructureController = nullptr; // connect via I_REF 
	if (collectionStructureController == nullptr){
		return QJsonObject();
	}

	QByteArray newObjectId =  collectionStructureController->InsertNewObjectIntoCollection(m_objectCollectionCompPtr.GetPtr(), nodeId, typeId, name, description, newObjectPtr.GetPtr(), objectId, nullptr, nullptr, operationContextPtr.GetPtr());
	if (newObjectId.isEmpty()){
		errorMessage = QT_TR_NOOP(QString("Can not insert object: %1").arg(qPrintable(objectId)));
		SendErrorMessage(0, QString("Can not insert object: %1").arg(qPrintable(objectId)), "Object collection controller");

		return QJsonObject();
	}

	QJsonObject rootObj;
	QJsonObject dataObj;
	QJsonObject notificationObj;

	notificationObj.insert(QStringLiteral("Id"), QJsonValue::fromVariant(newObjectId));
	notificationObj.insert(QStringLiteral("Name"), QJsonValue::fromVariant(name));

	dataObj.insert(QStringLiteral("addedNotification"), notificationObj);
	rootObj.insert(QStringLiteral("data"), dataObj);

	return rootObj;
}


QJsonObject CStructureControllerCompBase::MoveObject(
	const imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	return QJsonObject();
}


QJsonObject CStructureControllerCompBase::RemoveObject(
	const imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	return QJsonObject();
}


QJsonObject CStructureControllerCompBase::GetNodeCount(
	const imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	return QJsonObject();
}


QJsonObject CStructureControllerCompBase::GetNodeIds(
	const imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	if (!m_collectionStructureCompPtr.IsValid()){
		return QJsonObject();
	}

	return QJsonObject();
}


QJsonObject CStructureControllerCompBase::GetItemInfo(
	const imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	return QJsonObject();
}


QJsonObject CStructureControllerCompBase::GetObjectParentNodeIds(
	const imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	return QJsonObject();
}


QJsonObject CStructureControllerCompBase::GetNodes(
	const imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	if (!m_collectionStructureCompPtr.IsValid()){
		errorMessage = QString("Unable to list nodes. Component reference 'CollectionStructure' was not set");

		SendCriticalMessage(0, errorMessage);

		return QJsonObject();
	}

	const imtgql::CGqlParamObject& inputParams = gqlRequest.GetParams();

	QJsonObject rootObj;
	QJsonObject dataObj;
	QJsonArray itemsArray;
	QJsonObject notificationObj;

	if (!errorMessage.isEmpty()){
		QJsonObject errorsObj;
		errorsObj.insert(QStringLiteral("message"), QJsonValue::fromVariant(errorMessage));
		rootObj.insert(QStringLiteral("errors"), errorsObj);
	}
	else{
		const imtgql::CGqlParamObject* viewParamsGql = nullptr;
		const imtgql::CGqlParamObject* inputObject = inputParams.GetParamArgumentObjectPtr("input");
		if (inputObject != nullptr){
			viewParamsGql = inputObject->GetParamArgumentObjectPtr("viewParams");
		}

		iprm::CParamsSet filterParams;
		int offset = 0;
		int count = -1;

		if (viewParamsGql != nullptr){
			offset = viewParamsGql->GetParamArgumentValue("offset").toInt();
			count = viewParamsGql->GetParamArgumentValue("count").toInt();
			// PrepareFilters(gqlRequest, *viewParamsGql, filterParams);
		}

		int itemCount = m_collectionStructureCompPtr->GetItemCount(&filterParams);

		int pagesCount = std::ceil(itemCount / (double)count); /// count == 0
		if (pagesCount <= 0){
			pagesCount = 1;
		}

		notificationObj.insert(QStringLiteral("PagesCount"), QJsonValue::fromVariant(pagesCount));
		notificationObj.insert(QStringLiteral("TotalCount"), QJsonValue::fromVariant(itemCount));

		istd::TDelPtr<imtbase::IHierarchicalStructureIterator> collectionStructureIterator(m_collectionStructureCompPtr->CreateHierarchicalStructureIterator(offset, count, &filterParams));
		if (collectionStructureIterator != nullptr){
			while (collectionStructureIterator->Next()){
				imtbase::IHierarchicalStructureInfo::ItemInfo nodeInfo = collectionStructureIterator->GetItemInfo();
				QJsonObject itemObj;
				if (!SetupNodeItem(gqlRequest, itemObj, collectionStructureIterator.GetPtr(), errorMessage)){
					SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

					return QJsonObject();
				}
				itemsArray.append(itemObj);
			}
		}

		dataObj.insert(QStringLiteral("items"), itemsArray);
		dataObj.insert(QStringLiteral("notification"), notificationObj);
	}

	rootObj.insert(QStringLiteral("data"), dataObj);

	return rootObj;
}


QJsonObject CStructureControllerCompBase::GetElements(
	const imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	return QJsonObject();
}


bool CStructureControllerCompBase::SetupNodeItem(
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QJsonObject& /*itemObj*/,
			const imtbase::IHierarchicalStructureIterator* /*collectionStructureIterator*/,
			QString& /*errorMessage*/) const
{
	return false;
}


bool CStructureControllerCompBase::SetupObjectItem(
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QJsonObject& /*itemObj*/,
			const imtbase::IObjectCollectionIterator* /*objectCollectionIterator*/,
			QString& /*errorMessage*/) const
{
	return false;
}


QByteArrayList CStructureControllerCompBase::GetInformationIds(const imtgql::CGqlRequest& gqlRequest, const QByteArray& objectId) const
{
	QByteArrayList retVal;

	const imtgql::CGqlFieldObject& fields = gqlRequest.GetFields();
	const imtgql::CGqlFieldObject* findObject = fields.GetFieldArgumentObjectPtr(objectId);
	if (findObject != nullptr){
		retVal= findObject->GetFieldIds();
	}

	return retVal;
}


} // namespace imtservergql


