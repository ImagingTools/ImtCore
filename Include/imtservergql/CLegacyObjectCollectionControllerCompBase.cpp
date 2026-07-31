// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CLegacyObjectCollectionControllerCompBase.h>


// std includes
#include <cmath>

// Qt includes
#include <QtCore/QFile>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QUuid>
#include <QtCore/QTemporaryDir>
#include <QtCore/QFileInfo>

// ACF includes
#include <iprm/CTextParam.h>
#include <iprm/CIdParam.h>
#include <iprm/CEnableableParam.h>
#include <istd/TDelPtr.h>
#include <iser/CJsonMemReadArchive.h>
#include <istd/TSingleFactory.h>

// ImtCore includes
#include <imtbase/IIdentifiable.h>
#include <imtbase/CMimeType.h>
#include <imtbase/CCollectionFilter.h>
#include <imtbase/CComplexCollectionFilter.h>
#include <imtbase/IObjectCollectionIterator.h>
#include <imtbase/COperationContext.h>
#include <imtbase/CTimeFilterParam.h>
#include <imtbase/CObjectCollection.h>
#include <imtbase/COperationDescription.h>
#include <imtgql/imtgql.h>
#include <imtcol/CComplexCollectionFilterRepresentationController.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/ComplexCollectionFilter.h>


namespace imtservergql
{


// reimplemented (icomp::CComponentBase)

void CLegacyObjectCollectionControllerCompBase::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	// for object collection thread running
	if (!m_objectCollectionCompPtr.EnsureInitialized()){
		qDebug() << "Invalid object collection component";
	}


	int count = m_replaceableFieldsAttrPtr.GetCount();
	count = qMax(count, m_replacementFieldsAttrPtr.GetCount());

	for (int i = 0; i < count; i++){
		QByteArray replaceableField = m_replaceableFieldsAttrPtr[i];
		QByteArray replacementField = m_replacementFieldsAttrPtr[i];
		if (!replaceableField.isEmpty() && ! replacementField.isEmpty()){
			m_fieldReplacementMap[replaceableField] = replacementField;
		}
	}
}


// reimplemented (imtservergql::CGqlRepresentationDataControllerComp)

QJsonObject CLegacyObjectCollectionControllerCompBase::CreateInternalResponse(
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
	case OT_ELEMENT_HISTORY:
		return GetObjectHistory(gqlRequest, errorMessage);
	case OT_ELEMENTS_COUNT:
		return GetElementsCount(gqlRequest, errorMessage);
	case OT_ELEMENT_IDS:
		return GetElementIds(gqlRequest, errorMessage);
	case OT_IMPORT:
		return ImportObject(gqlRequest, errorMessage);
	case OT_EXPORT:
		return ExportObject(gqlRequest, errorMessage);
	case OT_USER_OPERATION + 1:
		return GetTreeItemModel(gqlRequest, errorMessage);
	case OT_USER_OPERATION + 2:
		return GetDependencies(gqlRequest, errorMessage);
	}

	errorMessage = QString("Unable to create internal response. Operation is not supported");
	SendErrorMessage(0, errorMessage, "CLegacyObjectCollectionControllerCompBase");

	return QJsonObject();
}


// reimplemented (imtgql::IGqlRequestExtractor)

istd::IChangeableUniquePtr CLegacyObjectCollectionControllerCompBase::ExtractObject(const imtgql::CGqlRequest& gqlRequest, QByteArray& newObjectId, QString& errorMessage) const
{
	QString name;
	QString description;
	return CreateObjectFromRequest(gqlRequest, newObjectId, name, description, errorMessage);
}


// protected methods

void CLegacyObjectCollectionControllerCompBase::ReplaceComplexFilterFields(imtbase::IComplexCollectionFilter& filter) const
{
	for (const imtbase::IComplexCollectionFilter::FieldInfo& info: filter.GetFields()){
		imtbase::IComplexCollectionFilter::FieldInfo* editableInfoPtr = filter.GetEditableFieldInfo(info.id);
		if (editableInfoPtr != nullptr){
			if (m_fieldReplacementMap.contains(editableInfoPtr->id)){
				editableInfoPtr->id = m_fieldReplacementMap[editableInfoPtr->id];
			}
		}
	}

	std::function<void (imtbase::IComplexCollectionFilter::FilterExpression&)> ProcessGroupFilter = [&](imtbase::IComplexCollectionFilter::FilterExpression& groupFilter){
		for (imtbase::IComplexCollectionFilter::FieldFilter& fieldFilter : groupFilter.fieldFilters){
			if (m_fieldReplacementMap.contains(fieldFilter.fieldId)){
				fieldFilter.fieldId = m_fieldReplacementMap[fieldFilter.fieldId];
			}
		}

		for (imtbase::IComplexCollectionFilter::FilterExpression& groupFilterItem : groupFilter.filterExpressions){
			ProcessGroupFilter(groupFilterItem);
		}
	};

	imtbase::IComplexCollectionFilter::FilterExpression fieldsFilter = filter.GetFilterExpression();
	ProcessGroupFilter(fieldsFilter);
	filter.SetFilterExpression(fieldsFilter);
}


bool CLegacyObjectCollectionControllerCompBase::GetOperationFromRequest(
		const imtgql::CGqlRequest& gqlRequest,
		imtgql::CGqlParamObject& /*gqlObject*/,
		QString& errorMessage,
		int& operationType) const
{
	const imtgql::CGqlFieldObject& fields = gqlRequest.GetFields();

	const QByteArrayList Ids = fields.GetFieldIds();
	for (const QByteArray& fieldId: Ids){
		if (fieldId == "headers"){
			operationType = OT_HEADERS;
			return true;
		}
		if (fieldId == "items"){
			operationType = OT_LIST;
			return true;
		}
		if (fieldId == "itemsCount"){
			operationType = OT_ELEMENTS_COUNT;
			return true;
		}
		if (fieldId == "itemIds"){
			operationType = OT_ELEMENT_IDS;
			return true;
		}
		if (fieldId == "item"){
			operationType = OT_GET;
			return true;
		}
		if (fieldId == "addedNotification"){
			operationType = OT_NEW;
			return true;
		}
		if (fieldId == "updatedNotification"){
			operationType = OT_UPDATE;
			return true;
		}
		if (fieldId == "updatedCollectionNotification"){
			operationType = OT_UPDATE_COLLECTION;
			return true;
		}
		if (fieldId == "removedNotification"){
			operationType = OT_DELETE;
			return true;
		}
		if (fieldId == "rename"){
			operationType = OT_RENAME;
			return true;
		}
		if (fieldId == "setDescription"){
			operationType = OT_SET_DESCRIPTION;
			return true;
		}
		if (fieldId == "info"){
			operationType = OT_INFO;
			return true;
		}
		if (fieldId == "metaInfo"){
			operationType = OT_METAINFO;
			return true;
		}
		if (fieldId == "dataMetaInfo"){
			operationType = OT_DATAMETAINFO;
			return true;
		}
		if (fieldId == "itemHistory"){
			operationType = OT_ELEMENT_HISTORY;
			return true;
		}
		if (fieldId == "import"){
			operationType = OT_IMPORT;
			return true;
		}
		if (fieldId == "export"){
			operationType = OT_EXPORT;
			return true;
		}
	}

	errorMessage = QString("Unable to get the operation type from the request");

	SendErrorMessage(0, errorMessage, "CLegacyObjectCollectionControllerCompBase");

	return false;
}


QByteArray CLegacyObjectCollectionControllerCompBase::GetObjectIdFromInputParams(const imtgql::CGqlParamObject& inputParams) const
{
	QByteArray retVal;

	if (inputParams.GetParamIds().contains(QByteArrayLiteral("Id"))){
		retVal = inputParams.GetParamArgumentValue(QByteArrayLiteral("Id")).toByteArray();
	}
	else if(inputParams.GetParamIds().contains(QByteArrayLiteral("input"))){
		const imtgql::CGqlParamObject* inputObject = inputParams.GetParamArgumentObjectPtr(QByteArrayLiteral("input"));
		if (inputObject != nullptr){
			return GetObjectIdFromInputParams(*inputObject);
		}
	}

	return retVal;
}


QByteArray CLegacyObjectCollectionControllerCompBase::GetObjectIdFromRequest(const imtgql::CGqlRequest& gqlRequest) const
{
	return GetObjectIdFromInputParams(gqlRequest.GetParams());
}


QJsonObject CLegacyObjectCollectionControllerCompBase::GetObject(
		const imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to get data object. Error: Attribute 'm_objectCollectionCompPtr' was not set").toUtf8();
		SendErrorMessage(0, errorMessage, "CLegacyObjectCollectionControllerCompBase");

		return QJsonObject();
	}

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		errorMessage = QString("Unable to get data object. Error: GraphQL input params is invalid.").toUtf8();
		SendErrorMessage(0, errorMessage, "CLegacyObjectCollectionControllerCompBase");

		return QJsonObject();
	}

	QByteArray objectId = inputParamPtr->GetParamArgumentValue("Id").toByteArray();
	QByteArray objectTypeId = GetObjectTypeIdFromRequest(gqlRequest);

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		errorMessage = QString("Unable to get data with ID: '%1'. The object does not exist.").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "CLegacyObjectCollectionControllerCompBase");

		return QJsonObject();
	}

	QJsonObject rootObj;
	QJsonObject dataObj;

	if (!CreateRepresentationFromObject(*dataPtr, objectTypeId, gqlRequest, dataObj, errorMessage)){
		if (errorMessage.isEmpty()){
			errorMessage = QString("Unable create object representation for the object with ID: '%1'.").arg(qPrintable(objectId));
		}

		SendErrorMessage(0, errorMessage, "CLegacyObjectCollectionControllerCompBase");

		return QJsonObject();
	}

	rootObj.insert(QStringLiteral("data"), dataObj);
	return rootObj;
}


QJsonObject CLegacyObjectCollectionControllerCompBase::InsertObject(
		const imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QT_TR_NOOP("Internal error");
		SendErrorMessage(0, "Internal error", "Object collection controller");

		return QJsonObject();
	}

	const imtgql::CGqlParamObject* gqlInputParamPtr = gqlRequest.GetParamObject("input");
	if (gqlInputParamPtr == nullptr){
		errorMessage = QString("Unable to insert an object. GraphQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return QJsonObject();
	}

	QByteArray typeId = GetObjectTypeIdFromRequest(gqlRequest);
	QString name = gqlInputParamPtr->GetParamArgumentValue("name").toString();
	QString description = gqlInputParamPtr->GetParamArgumentValue("description").toString();

	if (typeId.isEmpty()){
		typeId = "DocumentInfo";
	}

	QByteArray objectId;
	istd::IChangeableUniquePtr newObjectPtr = CreateObjectFromRequest(gqlRequest, objectId, name, description, errorMessage);
	if (!newObjectPtr.IsValid()){
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return QJsonObject();
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		errorMessage = QString("Object with ID: '%1' already exists").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return QJsonObject();
	}

	istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
	if (m_operationContextControllerCompPtr.IsValid()){
		operationContextPtr = m_operationContextControllerCompPtr->CreateOperationContext("Create", objectId, newObjectPtr.GetPtr());
	}

	QByteArray newObjectId = m_objectCollectionCompPtr->InsertNewObject(typeId, name, description, newObjectPtr.GetPtr(), objectId, nullptr, nullptr, operationContextPtr.GetPtr());
	if (newObjectId.isEmpty()){
		errorMessage = QString("Error when creating a new object. Object-ID: '%1'.").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "Object collection controller");

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


QJsonObject CLegacyObjectCollectionControllerCompBase::UpdateObject(
		const imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to update an object. Internal error.");
		SendErrorMessage(0, errorMessage, "CLegacyObjectCollectionControllerCompBase");

		return QJsonObject();
	}

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		errorMessage = QString("Unable to update an object. GraphQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return QJsonObject();
	}

	QByteArray objectId = inputParamPtr->GetParamArgumentValue("Id").toByteArray();
	QString name = inputParamPtr->GetParamArgumentValue("name").toString();
	QString description = inputParamPtr->GetParamArgumentValue("description").toString();

	istd::IChangeableUniquePtr savedObjectPtr = CreateObjectFromRequest(gqlRequest, objectId, name, description, errorMessage);
	if (!savedObjectPtr.IsValid()){
		if (errorMessage.isEmpty()){
			errorMessage = QString("Can not create object for update: '%1'").arg(qPrintable(objectId));
		}

		SendErrorMessage(0, errorMessage, "Object collection controller");

		return QJsonObject();
	}

	istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
	if (m_operationContextControllerCompPtr.IsValid()){
		operationContextPtr = m_operationContextControllerCompPtr->CreateOperationContext("Update", objectId, savedObjectPtr.GetPtr());
	}

	if (!m_objectCollectionCompPtr->SetObjectData(objectId, *savedObjectPtr.GetPtr(), istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr.GetPtr())){
		errorMessage = QString("Can not update object: '%1'").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return QJsonObject();
	}

	QJsonObject rootObj;
	QJsonObject dataObj;
	QJsonObject notificationObj;
	notificationObj.insert(QStringLiteral("Id"), QJsonValue::fromVariant(objectId));
	notificationObj.insert(QStringLiteral("Name"), QJsonValue::fromVariant(name));
	dataObj.insert(QStringLiteral("updatedNotification"), notificationObj);
	rootObj.insert(QStringLiteral("data"), dataObj);

	return rootObj;
}


QJsonObject CLegacyObjectCollectionControllerCompBase::UpdateCollection(
		const imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to update collection. Component reference 'ObjectCollection' was not set");
		SendCriticalMessage(0, errorMessage, "CLegacyObjectCollectionControllerCompBase");

		return QJsonObject();
	}

	QJsonObject rootObj;

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		errorMessage = QString("Unable to update collection. GraphQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return QJsonObject();
	}

	QByteArray objectIdsJson = inputParamPtr->GetParamArgumentValue("Ids").toByteArray();
	if (objectIdsJson.isEmpty()){
		errorMessage = QString("Unable to update collection. Ids from input params is empty.");
		SendErrorMessage(0, errorMessage, "CLegacyObjectCollectionControllerCompBase");

		return QJsonObject();
	}

	imtbase::CTreeItemModel objectIdsModel;
	if (!objectIdsModel.CreateFromJson(objectIdsJson)){
		errorMessage = QString("Unable to create model from json: '%1'").arg(qPrintable(objectIdsJson));
		SendErrorMessage(0, errorMessage, "CLegacyObjectCollectionControllerCompBase");

		return QJsonObject();
	}

	for (int i = 0; i < objectIdsModel.GetItemsCount(); i++){
		QByteArray objectId = objectIdsModel.GetData("Id", i).toByteArray();
		if (!objectId.isEmpty()){
			QString name;
			QString description;

			istd::IChangeableUniquePtr savedObjectPtr = CreateObjectFromRequest(gqlRequest, objectId, name, description, errorMessage);
			if (savedObjectPtr.IsValid()){
				if (!m_objectCollectionCompPtr->SetObjectData(objectId, *savedObjectPtr)){
					errorMessage += QString("Could not update object: '%1'; ").arg(qPrintable(objectId));
					objectIdsModel.SetData("Failed", true, i);
				}
			}
		}
	}

	if (!errorMessage.isEmpty()){
		QJsonObject errorsObj;
		errorsObj.insert(QStringLiteral("message"), QJsonValue::fromVariant(errorMessage));
		rootObj.insert(QStringLiteral("errors"), errorsObj);
	}
	else{
		QJsonObject dataObj;
		QJsonArray notificationArray;

		for(int i = 0; i < objectIdsModel.GetItemsCount(); ++i){
			if(!objectIdsModel.ContainsKey("Failed", i)){
				QJsonObject itemObj;
				itemObj.insert(QStringLiteral("Id"), QJsonValue::fromVariant(objectIdsModel.GetData("Id", i)));
				notificationArray.append(itemObj);
			}
		}

		dataObj.insert(QStringLiteral("updatedCollectionNotification"), notificationArray);
		rootObj.insert(QStringLiteral("data"), dataObj);
	}

	return rootObj;
}


QJsonObject CLegacyObjectCollectionControllerCompBase::RenameObject(
		const imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to rename object. Component reference 'ObjectCollection' was not set");
		SendCriticalMessage(0, errorMessage);

		return QJsonObject();
	}

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		errorMessage = QString("Unable to rename object. GraphQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return QJsonObject();
	}

	QByteArray objectId = inputParamPtr->GetParamArgumentValue("Id").toByteArray();
	QString newName = inputParamPtr->GetParamArgumentValue("NewName").toString();

	if (!m_objectCollectionCompPtr->SetElementName(objectId, newName)){
		errorMessage = QString("Unable to set name '%1' for element with ID: '%2'").arg(qPrintable(newName)).arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return QJsonObject();
	}

	QJsonObject rootObj;
	QJsonObject dataObj;

	dataObj.insert(QStringLiteral("Id"), QJsonValue::fromVariant(objectId));
	dataObj.insert(QStringLiteral("Name"), QJsonValue::fromVariant(newName));

	rootObj.insert(QStringLiteral("data"), dataObj);
	return rootObj;
}


QJsonObject CLegacyObjectCollectionControllerCompBase::SetObjectDescription(
		const imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to set the object description. Component reference 'ObjectCollection' was not set");
		SendCriticalMessage(0, errorMessage);

		return QJsonObject();
	}

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		errorMessage = QString("Unable to set description for object. GraphQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return QJsonObject();
	}

	QByteArray objectId = inputParamPtr->GetParamArgumentValue("Id").toByteArray();
	QString description = inputParamPtr->GetParamArgumentValue("Description").toString();

	if (!m_objectCollectionCompPtr->SetElementDescription(objectId, description)){
		errorMessage = QString("Unable to set description '%1' for element with ID: '%2'").arg(description).arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return QJsonObject();
	}

	QJsonObject rootObj;
	QJsonObject dataObj;

	dataObj.insert(QStringLiteral("Id"), QJsonValue::fromVariant(objectId));
	dataObj.insert(QStringLiteral("Description"), QJsonValue::fromVariant(description));

	rootObj.insert(QStringLiteral("data"), dataObj);
	return rootObj;
}


QJsonObject CLegacyObjectCollectionControllerCompBase::ListObjects(
		const imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to list objects. Component reference 'ObjectCollection' was not set");
		SendCriticalMessage(0, errorMessage);

		return QJsonObject();
	}

	QJsonObject rootObj;
	QJsonObject dataObj;

	iprm::CParamsSet filterParams;

	int offset = 0;
	int count = -1;

	const imtgql::CGqlParamObject* viewParamsPtr = nullptr;
	const imtgql::CGqlParamObject* inputParamsPtr = gqlRequest.GetParamObject("input");
	if (inputParamsPtr != nullptr){
		viewParamsPtr = inputParamsPtr->GetParamArgumentObjectPtr("viewParams");
	}

	if (viewParamsPtr != nullptr){
		offset = viewParamsPtr->GetParamArgumentValue("offset").toInt();
		count = viewParamsPtr->GetParamArgumentValue("count").toInt();

		PrepareFilters(gqlRequest, *viewParamsPtr, filterParams);
	}

	if (count == 0){
		count = -1;
	}

	istd::TDelPtr<imtbase::IObjectCollectionIterator> objectCollectionIterator(
				m_objectCollectionCompPtr->CreateObjectCollectionIterator(QByteArray(), offset, count, &filterParams));
	if (objectCollectionIterator == nullptr){
		errorMessage = QString("Object collection iterator creation failed");
		SendErrorMessage(0, errorMessage, "CLegacyObjectCollectionControllerCompBase");

		return QJsonObject();
	}

	// GetElementsCount() returns the total count across all pages (from COUNT(*) OVER()),
	// not just the number of records in the current page.
	int elementsCount = objectCollectionIterator->GetElementsCount();

	int pagesCount = std::ceil(elementsCount / (double)count);
	if (pagesCount <= 0){
		pagesCount = 1;
	}

	QJsonObject notificationObj;
	notificationObj.insert(QStringLiteral("PagesCount"), QJsonValue::fromVariant(pagesCount));
	notificationObj.insert(QStringLiteral("TotalCount"), QJsonValue::fromVariant(elementsCount));
	dataObj.insert(QStringLiteral("notification"), notificationObj);

	QJsonArray itemsArray;
	while (objectCollectionIterator->Next()){
		QJsonObject itemObj;
		if (!SetupGqlItem(gqlRequest, itemObj, objectCollectionIterator.GetPtr(), errorMessage)){
			errorMessage = QString("Unable to get object data from object collection iterator.");
			SendErrorMessage(0, errorMessage, "CLegacyObjectCollectionControllerCompBase");

			return QJsonObject();
		}
		itemsArray.append(itemObj);
	}

	dataObj.insert(QStringLiteral("items"), itemsArray);
	rootObj.insert(QStringLiteral("data"), dataObj);

	return rootObj;
}


QJsonObject CLegacyObjectCollectionControllerCompBase::GetElementsCount(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to get the element count. Component reference 'ObjectCollection' was not set");
		SendCriticalMessage(0, errorMessage);

		return QJsonObject();
	}

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		errorMessage = QString("Unable to rename object. GraphQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return QJsonObject();
	}

	iprm::CParamsSet filterParams;
	const imtgql::CGqlParamObject* viewParamsPtr = inputParamPtr->GetParamArgumentObjectPtr("viewParams");
	if (viewParamsPtr != nullptr){
		PrepareFilters(gqlRequest, *viewParamsPtr, filterParams);
	}

	int elementsCount = m_objectCollectionCompPtr->GetElementsCount(&filterParams);

	QJsonObject rootObj;
	QJsonObject dataObj;
	dataObj.insert(QStringLiteral("itemsCount"), QJsonValue::fromVariant(elementsCount));
	rootObj.insert(QStringLiteral("data"), dataObj);

	return rootObj;
}


QJsonObject CLegacyObjectCollectionControllerCompBase::GetElementIds(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to get the element IDs. Component reference 'ObjectCollection' was not set");
		SendCriticalMessage(0, errorMessage);

		return QJsonObject();
	}

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		errorMessage = QString("Unable to rename object. GraphQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return QJsonObject();
	}

	int offset = 0;
	int count = -1;
	iprm::CParamsSet filterParams;
	const imtgql::CGqlParamObject* viewParamsPtr = inputParamPtr->GetParamArgumentObjectPtr("viewParams");
	if (viewParamsPtr != nullptr){
		offset = viewParamsPtr->GetParamArgumentValue("offset").toInt();
		count = viewParamsPtr->GetParamArgumentValue("count").toInt();

		PrepareFilters(gqlRequest, *viewParamsPtr, filterParams);
	}

	QJsonObject rootObj;
	QJsonObject dataObj;

	QByteArray dataIds;
	imtbase::ICollectionInfo::Ids ids = m_objectCollectionCompPtr->GetElementIds(offset, count, &filterParams);
	for (int i = 0; i < ids.count(); i++){
		if (i > 0){
			dataIds += ";";
		}
		dataIds += ids[i];
	}

	dataObj.insert(QStringLiteral("itemIds"), QJsonValue::fromVariant(dataIds));
	rootObj.insert(QStringLiteral("data"), dataObj);

	return rootObj;
}


QJsonObject CLegacyObjectCollectionControllerCompBase::DeleteObject(
		const imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to remove the object from the collection. Component reference 'ObjectCollection' was not set");
		SendCriticalMessage(0, errorMessage);

		return QJsonObject();
	}

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		errorMessage = QString("Unable to delete object. GraphQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return QJsonObject();
	}

	QByteArray objectId = inputParamPtr->GetParamArgumentValue("Id").toByteArray();
	if (objectId.isEmpty()){
		errorMessage = QString("No object-ID could not be extracted from the request");
		SendErrorMessage(0, errorMessage, "CLegacyObjectCollectionControllerCompBase");
		return QJsonObject();
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr);

	istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
	if (m_operationContextControllerCompPtr.IsValid()){
		operationContextPtr = m_operationContextControllerCompPtr->CreateOperationContext("Remove", objectId, dataPtr.GetPtr());
	}
	
	imtbase::ICollectionInfo::Ids elementIds;
	elementIds << objectId;

	if (!m_objectCollectionCompPtr->RemoveElements(elementIds, operationContextPtr.GetPtr())){
		errorMessage = QString("Can't remove object with ID: '%1'").arg(QString(objectId));
		SendErrorMessage(0, errorMessage, "CLegacyObjectCollectionControllerCompBase");

		return QJsonObject();
	}

	QJsonObject rootObj;
	QJsonObject dataObj;
	QJsonObject notificationObj;
	notificationObj.insert(QStringLiteral("Id"), QJsonValue::fromVariant(objectId));
	dataObj.insert(QStringLiteral("removedNotification"), notificationObj);
	rootObj.insert(QStringLiteral("data"), dataObj);

	return rootObj;
}


QJsonObject CLegacyObjectCollectionControllerCompBase::GetHeaders(
		const imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	if (!m_headersProviderCompPtr.IsValid()){
		errorMessage = QString("Unable to get headers. Component reference 'HeadersProvider' was not set");
		SendCriticalMessage(0, errorMessage);

		return QJsonObject();
	}

	QJsonObject rootObj;

	imtgql::CGqlRequest headersRequest;
	const imtgql::IGqlContext* gqlContext = gqlRequest.GetRequestContext();
	if(gqlContext != nullptr){
		istd::IChangeableUniquePtr clonePtr = gqlContext->CloneMe();
		if(clonePtr.IsValid()){
			imtgql::IGqlContext* headerGqlContext = dynamic_cast<imtgql::IGqlContext*>(clonePtr.GetPtr());

			headersRequest.SetGqlContext(headerGqlContext);
		}
	}

	QJsonObject headersObj = m_headersProviderCompPtr->CreateResponse(headersRequest, errorMessage);
	if (!headersObj.isEmpty()){
		rootObj.insert(QStringLiteral("data"), headersObj);
	}

	return rootObj;
}


QJsonObject CLegacyObjectCollectionControllerCompBase::GetTreeItemModel(
		const imtgql::CGqlRequest& /*gqlRequest*/,
		QString& /*errorMessage*/) const
{
	return QJsonObject();
}


QJsonObject CLegacyObjectCollectionControllerCompBase::GetDependencies(
		const imtgql::CGqlRequest& /*gqlRequest*/,
		QString& /*errorMessage*/) const
{
	return QJsonObject();
}


QJsonObject CLegacyObjectCollectionControllerCompBase::GetMetaInfo(
		const imtgql::CGqlRequest& /*gqlRequest*/,
		QString& /*errorMessage*/) const
{
	return QJsonObject();
}


QJsonObject CLegacyObjectCollectionControllerCompBase::GetInfo(
		const imtgql::CGqlRequest& /*gqlRequest*/,
		QString& /*errorMessage*/) const
{
	return QJsonObject();
}


QJsonObject CLegacyObjectCollectionControllerCompBase::GetDataMetaInfo(
		const imtgql::CGqlRequest& /*gqlRequest*/,
		QString& /*errorMessage*/) const
{
	return QJsonObject();
}


QJsonObject CLegacyObjectCollectionControllerCompBase::GetObjectHistory(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to get the object history. Component reference 'ObjectCollection' was not set");
		SendCriticalMessage(0, errorMessage);

		return QJsonObject();
	}

	if (!m_documentChangeGeneratorCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'DocumentChangeGenerator' was not set", "CLegacyObjectCollectionControllerCompBase");
		return QJsonObject();
	}

	const imtgql::CGqlParamObject* gqlInputParamsPtr = gqlRequest.GetParamObject("input");
	if (gqlInputParamsPtr == nullptr){
		errorMessage = QString("Unable to get object history: GraphQL-parameters not set");
		SendErrorMessage(0, errorMessage);

		return QJsonObject();
	}

	QByteArray objectId = gqlInputParamsPtr->GetParamArgumentValue("Id").toByteArray();
	if (objectId.isEmpty()){
		errorMessage = QString("Unable to get history for an object with empty ID");
		SendErrorMessage(0, errorMessage);

		return QJsonObject();
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

	QJsonObject rootObj;
	QJsonArray dataArray;

	istd::TDelPtr<imtbase::IObjectCollectionIterator> objectCollectionIterator(
					m_objectCollectionCompPtr->CreateObjectCollectionIterator(QByteArray(), 0, -1, &filterParams));
	if (objectCollectionIterator == nullptr){
		errorMessage = QString("Unable to get history for an object with ID: '%1'. Error when trying to create collection iterator.").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "CLegacyObjectCollectionControllerCompBase");

		return QJsonObject();
	}

	while (objectCollectionIterator->Next()){
		QByteArray ownerId = objectCollectionIterator->GetElementInfo("OwnerId").toByteArray();
		QString ownerName = objectCollectionIterator->GetElementInfo("OwnerName").toString();
		QByteArray operationsDescriptionJson = objectCollectionIterator->GetElementInfo("OperationDescription").toByteArray();
		QDateTime lastModified =  objectCollectionIterator->GetElementInfo("LastModified").toDateTime();
		lastModified.setTimeZone(QTimeZone::utc());

		if (!ownerId.isEmpty() && !operationsDescriptionJson.isEmpty()){
			imtbase::CObjectCollection changeCollection;

			typedef istd::TSingleFactory<istd::IChangeable, imtbase::COperationDescription> FactoryOperationDescriptionImpl;
			changeCollection.RegisterFactory<FactoryOperationDescriptionImpl>("OperationInfo");

			iser::CJsonMemReadArchive archive(operationsDescriptionJson);
			if (changeCollection.Serialize(archive)){
				QString operationDescription = m_documentChangeGeneratorCompPtr->GetOperationDescription(changeCollection, languageId);
				if (!operationDescription.isEmpty()){
					QJsonObject itemObj;
					itemObj.insert(QStringLiteral("OwnerId"), QString::fromUtf8(ownerId));
					itemObj.insert(QStringLiteral("OwnerName"), ownerName);
					itemObj.insert(QStringLiteral("OperationDescription"), operationDescription);
					itemObj.insert(QStringLiteral("Time"), lastModified.toLocalTime().toString("dd.MM.yyyy hh:mm:ss"));
					dataArray.append(itemObj);
				}
			}
		}
	}

	rootObj.insert(QStringLiteral("data"), dataArray);
	return rootObj;
}


QJsonObject CLegacyObjectCollectionControllerCompBase::ImportObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CLegacyObjectCollectionControllerCompBase");
		return QJsonObject();
	}

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		errorMessage = QT_TR_NOOP("Unable to import the object. GQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "CLegacyObjectCollectionControllerCompBase");
		return QJsonObject();
	}

	QByteArray objectData = inputParamPtr->GetParamArgumentValue("fileData").toByteArray();
	QByteArray data = QByteArray::fromBase64(objectData);
	QByteArray typeId = inputParamPtr->GetParamArgumentValue("typeId").toByteArray();
	QString mimeType = inputParamPtr->GetParamArgumentValue("mimeType").toString();
	QString name = inputParamPtr->GetParamArgumentValue("name").toString();
	QString description = inputParamPtr->GetParamArgumentValue("description").toString();

	int index = GetMimeTypeIndex(mimeType);
	if (index < 0){
		errorMessage = "Mime type is invalid at CLegacyObjectCollectionControllerCompBase";
		return QJsonObject();
	}
	if (index >= m_importExportObjectFactCompPtr.GetCount()){
		errorMessage = "Import/Export object factory index out of range at CLegacyObjectCollectionControllerCompBase";
		return QJsonObject();
	}
	if (index >= m_filePersistenceCompPtr.GetCount()){
		errorMessage = "File persistence index out of range at CLegacyObjectCollectionControllerCompBase";
		return QJsonObject();
	}

	istd::IChangeableUniquePtr objectPersistenceInstancePtr = m_importExportObjectFactCompPtr.CreateInstance(index);
	if (!objectPersistenceInstancePtr.IsValid()){
		errorMessage = QString("Unable to import object to the collection. Error: Object instance is invalid");
		SendErrorMessage(0, errorMessage, "CLegacyObjectCollectionControllerCompBase");

		return QJsonObject();
	}

	imtbase::CMimeType mime;
	if (!mime.FromString(mimeType)){
		errorMessage = QString("Unable to parse mime type");
		SendErrorMessage(0, errorMessage, "CLegacyObjectCollectionControllerCompBase");

		return QJsonObject();
	}

	QString extension = mime.GetSuffix();

	QTemporaryDir tempDir;
	QString filePathTmp = tempDir.path() + "/" + QUuid::createUuid().toString(QUuid::WithoutBraces) + "." + extension;

	QFile file(filePathTmp);
	if (!file.open(QIODevice::WriteOnly)){
		SendErrorMessage(0, QString("Unable to open file with name '%1'").arg(filePathTmp), "CLegacyObjectCollectionControllerCompBase");
		return QJsonObject();
	}

	file.write(data);
	file.close();

	if (m_filePersistenceCompPtr[index]->LoadFromFile(*objectPersistenceInstancePtr.GetPtr(), filePathTmp) != ifile::IFilePersistence::OS_OK){
		errorMessage = QString("Unable to import object to the collection");
		SendErrorMessage(0, errorMessage, "CLegacyObjectCollectionControllerCompBase");
		QFile::remove(filePathTmp);

		return QJsonObject();
	}

	QByteArray objectUuid = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	imtbase::IIdentifiable* identifiableObjectPtr = dynamic_cast<imtbase::IIdentifiable*>(objectPersistenceInstancePtr.GetPtr());
	if (identifiableObjectPtr != nullptr){
		objectUuid = identifiableObjectPtr->GetObjectUuid();
	}

	if (m_objectCollectionCompPtr->GetElementIds().contains(objectUuid)){
		errorMessage = QString("Unable to import object with ID: '%1' to the collection. Error: The object already exists inside the collection").arg(qPrintable(objectUuid));
		SendErrorMessage(0, errorMessage, "CLegacyObjectCollectionControllerCompBase");
		QFile::remove(filePathTmp);

		return QJsonObject();
	}

	int typeIdIndex = GetObjectTypeIdIndex(typeId);
	Q_ASSERT_X(typeIdIndex >= 0, "Type ID is invalid", "CLegacyObjectCollectionControllerCompBase");

	istd::IChangeableUniquePtr collectionObjectInstancePtr = m_objectFactCompPtr.CreateInstance(typeIdIndex);
	if (!collectionObjectInstancePtr.IsValid()){
		errorMessage = QString("Unable to import object to the collection. Error: Object instance is invalid");
		SendErrorMessage(0, errorMessage, "CLegacyObjectCollectionControllerCompBase");

		return QJsonObject();
	}

	if (!ConvertObject(*objectPersistenceInstancePtr.GetPtr(), *collectionObjectInstancePtr.GetPtr(), errorMessage)){
		if (errorMessage.isEmpty()){
			errorMessage = QString("Unable to import object to the collection. Error: Object conversion failed");
		}
		SendErrorMessage(0, errorMessage, "CLegacyObjectCollectionControllerCompBase");

		return QJsonObject();
	}

	QByteArray retVal = m_objectCollectionCompPtr->InsertNewObject(typeId, name, description, collectionObjectInstancePtr.GetPtr(), objectUuid);
	if (retVal.isEmpty()){
		errorMessage = QString("Unable to import object with ID: '%1' to the collection. Error: The object could not be inserted into the collection").arg(qPrintable(objectUuid));
		SendErrorMessage(0, errorMessage, "CLegacyObjectCollectionControllerCompBase");
		QFile::remove(filePathTmp);

		return QJsonObject();
	}

	QJsonObject rootObj;
	rootObj.insert(QStringLiteral("id"), QString::fromUtf8(objectUuid));
	rootObj.insert(QStringLiteral("status"), QStringLiteral("ok"));
	QFile::remove(filePathTmp);

	return rootObj;
}


QJsonObject CLegacyObjectCollectionControllerCompBase::ExportObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CLegacyObjectCollectionControllerCompBase");
		return QJsonObject();
	}

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		errorMessage = QT_TR_NOOP("Unable to import the object. GQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "CLegacyObjectCollectionControllerCompBase");
		return QJsonObject();
	}

	QByteArray objectId = inputParamPtr->GetParamArgumentValue("id").toByteArray();
	QString mimeType = inputParamPtr->GetParamArgumentValue("mimeType").toString();
	QString objectName = m_objectCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString();

	if (objectName.isEmpty()){
		objectName = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		errorMessage = QString("Unable to export the object with ID: '%1'. Error: Object does not exists").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "CLegacyObjectCollectionControllerCompBase");

		return QJsonObject();
	}

	int index = GetMimeTypeIndex(mimeType);
	if (index < 0){
		errorMessage = "Mime type is invalid at CLegacyObjectCollectionControllerCompBase";
		return QJsonObject();
	}
	if (index >= m_importExportObjectFactCompPtr.GetCount()){
		errorMessage = "Import/Export object factory index out of range at CLegacyObjectCollectionControllerCompBase";
		return QJsonObject();
	}
	if (index >= m_filePersistenceCompPtr.GetCount()){
		errorMessage = "File persistence index out of range at CLegacyObjectCollectionControllerCompBase";
		return QJsonObject();
	}

	imtbase::CMimeType mime;
	if (!mime.FromString(mimeType)){
		errorMessage = QString("Unable to parse mime type");
		SendErrorMessage(0, errorMessage, "CLegacyObjectCollectionControllerCompBase");

		return QJsonObject();
	}

	QString extension = mime.GetSuffix();

	QTemporaryDir tempDir;
	QString fileName = objectName + "." + extension;
	QString filePathTmp = tempDir.path() + "/" + fileName;

	istd::IChangeableUniquePtr objectPersistenceInstancePtr = m_importExportObjectFactCompPtr.CreateInstance(index);
	if (!objectPersistenceInstancePtr.IsValid()){
		errorMessage = QString("Unable to import object to the collection. Error: Object persistence instance is invalid");
		SendErrorMessage(0, errorMessage, "CLegacyObjectCollectionControllerCompBase");

		return QJsonObject();
	}

	if (!ConvertObject(*dataPtr.GetPtr(), *objectPersistenceInstancePtr.GetPtr(), errorMessage)){
		if (errorMessage.isEmpty()){
			errorMessage = QString("Unable to export the object with ID: '%1'. Error: Object conversion failed").arg(qPrintable(objectId));
		}
		SendErrorMessage(0, errorMessage, "CLegacyObjectCollectionControllerCompBase");

		return QJsonObject();
	}

	if (m_filePersistenceCompPtr[index]->SaveToFile(*objectPersistenceInstancePtr.GetPtr(), filePathTmp) != ifile::IFilePersistence::OS_OK){
		errorMessage = QString("Unable to export the object with ID: '%1'. Error: Saving data to the file '%1' failed").arg(qPrintable(objectId)).arg(filePathTmp);
		SendErrorMessage(0, errorMessage, "CLegacyObjectCollectionControllerCompBase");

		return QJsonObject();
	}

	QFile file(filePathTmp);
	if (!file.open(QIODevice::ReadOnly)){
		errorMessage = QString("Unable to export the object with ID: '%1'. Error: Unable to open file with name '%1'").arg(qPrintable(objectId)).arg(filePathTmp);
		SendErrorMessage(0, errorMessage, "CLegacyObjectCollectionControllerCompBase");
		QFile::remove(filePathTmp);

		return QJsonObject();
	}

	QByteArray data = file.readAll();
	file.close();
	QFile::remove(filePathTmp);

	QJsonObject rootObj;
	rootObj.insert(QStringLiteral("fileData"), QString::fromLatin1(data.toBase64()));
	rootObj.insert(QStringLiteral("fileName"), fileName);

	return rootObj;
}


bool CLegacyObjectCollectionControllerCompBase::ConvertObject(const istd::IChangeable& source, istd::IChangeable& target, QString& errorMessage) const
{
	Q_UNUSED(errorMessage);
	return target.CopyFrom(source);
}


int CLegacyObjectCollectionControllerCompBase::GetObjectTypeIdIndex(const QByteArray& typeId) const
{
	for (int i = 0; i < m_objectTypeIdAttrPtr.GetCount(); i++){
		if (m_objectTypeIdAttrPtr[i] == typeId){
			return i;
		}
	}

	return -1;
}


int CLegacyObjectCollectionControllerCompBase::GetMimeTypeIndex(const QString& mimeType) const
{
	for (int i = 0; i < m_mimeTypeAttrPtr.GetCount(); i++){
		if (m_mimeTypeAttrPtr[i] == mimeType){
			return i;
		}
	}

	return -1;
}


bool CLegacyObjectCollectionControllerCompBase::SetupGqlItem(
		const imtgql::CGqlRequest& gqlRequest,
		QJsonObject& itemObj,
		const QByteArray& collectionId,
		QString& /*errorMessage*/) const
{
	QByteArrayList informationIds = GetInformationIds(gqlRequest, "items");
	if (informationIds.isEmpty()){
		return false;
	}

	for (const QByteArray& informationId : informationIds){
		QVariant elementInformation;

		if(informationId == QByteArray("Id")){
			elementInformation = QString::fromLatin1(collectionId);
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

		itemObj.insert(QString::fromUtf8(informationId.constData()), QJsonValue::fromVariant(elementInformation));
	}

	return true;
}


bool CLegacyObjectCollectionControllerCompBase::SetupGqlItem(
		const imtgql::CGqlRequest& gqlRequest,
		QJsonObject& itemObj,
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

	QByteArray collectionId = objectCollectionIterator->GetObjectId();

	idoc::MetaInfoPtr elementMetaInfo = objectCollectionIterator->GetDataMetaInfo();

	for (const QByteArray& informationId : informationIds){
		QVariant elementInformation;

		if(informationId == QByteArray("Id")){
			elementInformation = QString::fromLatin1(collectionId);
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

		itemObj.insert(QString::fromUtf8(informationId.constData()), QJsonValue::fromVariant(elementInformation));
	}

	return true;
}


QByteArrayList CLegacyObjectCollectionControllerCompBase::GetInformationIds(const imtgql::CGqlRequest& gqlRequest, const QByteArray& objectId) const
{
	QByteArrayList retVal;
	const imtgql::CGqlFieldObject& fields = gqlRequest.GetFields();
	const imtgql::CGqlFieldObject* findObject = fields.GetFieldArgumentObjectPtr(objectId);
	if (findObject != nullptr){
		retVal =findObject->GetFieldIds();
	}

	return retVal;
}


QVariant CLegacyObjectCollectionControllerCompBase::GetObjectInformation(const QByteArray& /*informationId*/, const QByteArray& /*objectId*/) const
{
	return QVariant();
}


QByteArray CLegacyObjectCollectionControllerCompBase::GetObjectTypeIdFromRequest(const imtgql::CGqlRequest& gqlRequest) const
{
	const imtgql::CGqlParamObject* gqlInputParamPtr = gqlRequest.GetParamObject("input");
	if (gqlInputParamPtr != nullptr){
		QByteArray typeId = gqlInputParamPtr->GetParamArgumentValue("typeId").toByteArray();

		if (typeId.isEmpty()){
			const imtgql::CGqlParamObject* additionalParamsPtr = gqlInputParamPtr->GetParamArgumentObjectPtr("addition");
			if (additionalParamsPtr != nullptr){
				typeId = additionalParamsPtr->GetParamArgumentValue("typeId").toByteArray();
			}
		}

		return typeId;
	}

	return QByteArray();
}


bool CLegacyObjectCollectionControllerCompBase::CreateRepresentationFromObject(
			const istd::IChangeable& /*data*/,
			const QByteArray& /*objectTypeId*/,
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QJsonObject& /*dataObj*/,
			QString& /*errorMessage*/) const
{
	return false;
}


istd::IChangeableUniquePtr CLegacyObjectCollectionControllerCompBase::CreateObjectFromInputParams(
		const QList<imtgql::CGqlParamObject>& /*inputParams*/,
		QByteArray& /*objectId*/,
		QString& /*name*/,
		QString& /*description*/,
		QString& /*errorMessage*/) const
{
	return nullptr;
}


istd::IChangeableUniquePtr CLegacyObjectCollectionControllerCompBase::CreateObjectFromRequest(
		const imtgql::CGqlRequest& gqlRequest,
		QByteArray& newObjectId,
		QString& name,
		QString& description,
		QString& errorMessage) const
{
	QList<imtgql::CGqlParamObject> inputParams;
	inputParams.append(gqlRequest.GetParams());

	return CreateObjectFromInputParams(inputParams, newObjectId, name, description, errorMessage);
}


void CLegacyObjectCollectionControllerCompBase::PrepareFilters(
		const imtgql::CGqlRequest& gqlRequest,
		const imtgql::CGqlParamObject& viewParamsGql,
		iprm::CParamsSet& filterParams) const
{
	this->SetAdditionalFilters(gqlRequest, viewParamsGql, &filterParams);

	const imtgql::CGqlParamObject* complexFilterModelPtr = viewParamsGql.GetParamArgumentObjectPtr("filterModel");
	if (complexFilterModelPtr != nullptr){
		sdl::V1_0::imtbase::CComplexCollectionFilter complexFilterSdl;
		bool isComplexFilterOk = complexFilterSdl.ReadFromGraphQlObject(*complexFilterModelPtr);

		if (isComplexFilterOk){
			istd::TDelPtr<imtbase::CComplexCollectionFilter> complexFilterPtr = new imtbase::CComplexCollectionFilter();
			if (imtcol::CComplexCollectionFilterRepresentationController::ComplexCollectionFilterRepresentationToModel(complexFilterSdl, *complexFilterPtr, GetLogPtr())){
				ReplaceComplexFilterFields(*complexFilterPtr);
				filterParams.SetEditableParameter("ComplexFilter", complexFilterPtr.PopPtr(), true);
			}
		}

		return;
	}

	QByteArray filterBA = viewParamsGql.GetParamArgumentValue("FilterModel").toByteArray();

	imtbase::CTreeItemModel generalModel;
	if (!generalModel.CreateFromJson(filterBA)){
		return;
	}

	iprm::CParamsSet* objectFilterPtr				= new iprm::CParamsSet();
	imtbase::CCollectionFilter* collectionFilterPtr = new imtbase::CCollectionFilter();

	imtbase::CTreeItemModel* filterIdsModelPtr = generalModel.GetTreeItemModel("FilterIds");
	if (filterIdsModelPtr != nullptr){
		QByteArrayList filteringInfoIds;
		for (int i = 0; i < filterIdsModelPtr->GetItemsCount(); i++){
			QByteArray headerId = filterIdsModelPtr->GetData("id", i).toByteArray();
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

	filterParams.SetEditableParameter("Filter", collectionFilterPtr, true);
	filterParams.SetEditableParameter("ObjectFilter", objectFilterPtr, true);
}


void CLegacyObjectCollectionControllerCompBase::SetAdditionalFilters(const imtgql::CGqlRequest& /*gqlRequest*/, const imtgql::CGqlParamObject& /*viewParamsGql*/, iprm::CParamsSet* /*filterParamsPtr*/) const
{
}


void CLegacyObjectCollectionControllerCompBase::SetObjectFilter(
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


istd::IChangeableUniquePtr CLegacyObjectCollectionControllerCompBase::CreateObject(const QByteArray& typeId)
{
	int index = GetObjectTypeIdIndex(typeId);
	if (m_objectFactCompPtr.IsValid() && index < m_objectFactCompPtr.GetCount()){
		return m_objectFactCompPtr.CreateInstance(index);
	}

	return nullptr;
}


} // namespace imtservergql
