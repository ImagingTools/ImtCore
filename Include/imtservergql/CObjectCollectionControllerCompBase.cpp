#include <imtservergql/CObjectCollectionControllerCompBase.h>


// std includes
#include <cmath>

// Qt includes
#include <QtCore/QFile>
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
#include <imtbase/CCollectionFilter.h>
#include <imtbase/CComplexCollectionFilter.h>
#include <imtbase/IObjectCollectionIterator.h>
#include <imtbase/COperationContext.h>
#include <imtbase/CTimeFilterParam.h>
#include <imtbase/CObjectCollection.h>
#include <imtbase/COperationDescription.h>
#include <imtbase/IRevisionController.h>
#include <imtgql/imtgql.h>
#include <imtcol/CComplexCollectionFilterRepresentationController.h>
#include <imtcol/CDocumentCollectionFilterRepresentationController.h>
#include <imtcol/CDocumentCollectionFilter.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/ComplexCollectionFilter.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/DocumentCollectionFilter.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/ImtCollection.h>


namespace imtservergql
{


// public methods

QMap<int, QByteArray> CObjectCollectionControllerCompBase::GetSupportedCommandIds() const
{
	static QMap<int, QByteArray> retVal;

	return retVal;
}


// reimplemented (icomp::CComponentBase)

void CObjectCollectionControllerCompBase::OnComponentCreated()
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


// reimplemented (sdl::imtbase::ImtCollection::CGraphQlHandlerCompBase)

sdl::imtbase::ImtCollection::CVisualStatus CObjectCollectionControllerCompBase::OnGetObjectVisualStatus(
			const sdl::imtbase::ImtCollection::CGetObjectVisualStatusGqlRequest& getObjectVisualStatusRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::imtbase::ImtCollection::CVisualStatus::V1_0 response;

	sdl::imtbase::ImtCollection::GetObjectVisualStatusRequestArguments arguments = getObjectVisualStatusRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		return sdl::imtbase::ImtCollection::CVisualStatus();
	}

	QByteArray objectId;
	if (arguments.input.Version_1_0->objectId){
		objectId = *arguments.input.Version_1_0->objectId;
	}

	QByteArray typeId;
	if (arguments.input.Version_1_0->typeId){
		typeId = *arguments.input.Version_1_0->typeId;
	}

	QString name = m_objectCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString();
	QString description = m_objectCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();

	int index = m_objectTypeIdAttrPtr.FindValue(typeId);
	if (index >= 0){
		if (index < m_objectIconPathsAttrPtr.GetCount()){
			response.icon = m_objectIconPathsAttrPtr[index];
		}
	}
	else{
		SendCriticalMessage(0, QString("Unknown type-ID provided '%1'").arg(qPrintable(typeId)));
	}

	response.objectId = objectId;
	response.text = name;
	response.description = description;

	sdl::imtbase::ImtCollection::CVisualStatus retVal;
	retVal.Version_1_0 = std::move(response);

	return retVal;
}


sdl::imtbase::ImtCollection::CRemoveElementSetPayload CObjectCollectionControllerCompBase::OnRemoveElementSet(
			const sdl::imtbase::ImtCollection::CRemoveElementSetGqlRequest& removeElementSetRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::imtbase::ImtCollection::CRemoveElementSetPayload::V1_0 response;

	sdl::imtbase::ImtCollection::RemoveElementSetRequestArguments arguments = removeElementSetRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		return sdl::imtbase::ImtCollection::CRemoveElementSetPayload();
	}
	
	istd::TDelPtr<iprm::CParamsSet> filterParamPtr;
	if (arguments.input.Version_1_0->filterModel.has_value()){
		sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilter::V1_0 filterModel = *arguments.input.Version_1_0->filterModel;

		istd::TDelPtr<imtbase::CComplexCollectionFilter> complexFilterPtr = new imtbase::CComplexCollectionFilter();
		if (!imtcol::CComplexCollectionFilterRepresentationController::ComplexCollectionFilterRepresentationToModel(filterModel, *complexFilterPtr, GetLogPtr())){
			errorMessage = QString("Unable to remove element set for collection '%1'. Error: SDL model parsing failed").arg(qPrintable(*m_collectionIdAttrPtr));
			return sdl::imtbase::ImtCollection::CRemoveElementSetPayload();
		}

		ReplaceComplexFilterFields(*complexFilterPtr);

		filterParamPtr.SetPtr(new iprm::CParamsSet);

		filterParamPtr->SetEditableParameter("ComplexFilter", complexFilterPtr.PopPtr(), true);
	}

	response.success = m_objectCollectionCompPtr->RemoveElementSet(filterParamPtr.GetPtr());

	sdl::imtbase::ImtCollection::CRemoveElementSetPayload retVal;
	retVal.Version_1_0 = std::move(response);

	return retVal;
}


sdl::imtbase::ImtCollection::CRestoreObjectsPayload CObjectCollectionControllerCompBase::OnRestoreObjects(
			const sdl::imtbase::ImtCollection::CRestoreObjectsGqlRequest& restoreObjectsRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::imtbase::ImtCollection::CRestoreObjectsPayload::V1_0 response;

	sdl::imtbase::ImtCollection::RestoreObjectsRequestArguments arguments = restoreObjectsRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		return sdl::imtbase::ImtCollection::CRestoreObjectsPayload();
	}

	QByteArrayList objectIds;
	if (arguments.input.Version_1_0->objectIds){
		objectIds = *arguments.input.Version_1_0->objectIds;
	}

	response.success = m_objectCollectionCompPtr->RestoreObjects(imtbase::ICollectionInfo::Ids(objectIds.constBegin(), objectIds.constEnd()));
	
	sdl::imtbase::ImtCollection::CRestoreObjectsPayload retVal;
	retVal.Version_1_0 = std::move(response);

	return retVal;
}


sdl::imtbase::ImtCollection::CRestoreObjectSetPayload CObjectCollectionControllerCompBase::OnRestoreObjectSet(
			const sdl::imtbase::ImtCollection::CRestoreObjectSetGqlRequest& restoreObjectSetRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::imtbase::ImtCollection::CRestoreObjectSetPayload::V1_0 response;

	sdl::imtbase::ImtCollection::RestoreObjectSetRequestArguments arguments = restoreObjectSetRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		return sdl::imtbase::ImtCollection::CRestoreObjectSetPayload();
	}

	istd::TDelPtr<iprm::CParamsSet> filterParamPtr;
	if (arguments.input.Version_1_0->filterModel.has_value()){
		sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilter::V1_0 filterModel = *arguments.input.Version_1_0->filterModel;

		istd::TDelPtr<imtbase::CComplexCollectionFilter> complexFilterPtr = new imtbase::CComplexCollectionFilter();
		if (!imtcol::CComplexCollectionFilterRepresentationController::ComplexCollectionFilterRepresentationToModel(filterModel, *complexFilterPtr, GetLogPtr())){
			errorMessage = QString("Unable to restore object set for collection '%1'. Error: SDL model parsing failed").arg(qPrintable(*m_collectionIdAttrPtr));
			return sdl::imtbase::ImtCollection::CRestoreObjectSetPayload();
		}

		ReplaceComplexFilterFields(*complexFilterPtr);

		filterParamPtr.SetPtr(new iprm::CParamsSet);

		filterParamPtr->SetEditableParameter("ComplexFilter", complexFilterPtr.PopPtr(), true);
	}

	response.success = m_objectCollectionCompPtr->RestoreObjectSet(filterParamPtr.GetPtr());

	sdl::imtbase::ImtCollection::CRestoreObjectSetPayload retVal;
	retVal.Version_1_0 = std::move(response);

	return retVal;
}


// reimplemented (imtservergql::CGqlRepresentationDataControllerComp)

imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::CreateInternalResponse(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (sdl::imtbase::ImtCollection::CGetObjectVisualStatusGqlRequest::GetCommandId() == gqlRequest.GetCommandId()){
		return BaseClass::CreateInternalResponse(gqlRequest, errorMessage);
	}

	if (sdl::imtbase::ImtCollection::CRemoveElementSetGqlRequest::GetCommandId() == gqlRequest.GetCommandId()){
		return BaseClass::CreateInternalResponse(gqlRequest, errorMessage);
	}

	if (sdl::imtbase::ImtCollection::CRestoreObjectSetGqlRequest::GetCommandId() == gqlRequest.GetCommandId()){
		return BaseClass::CreateInternalResponse(gqlRequest, errorMessage);
	}

	if (sdl::imtbase::ImtCollection::CRestoreObjectsGqlRequest::GetCommandId() == gqlRequest.GetCommandId()){
		return BaseClass::CreateInternalResponse(gqlRequest, errorMessage);
	}

	imtgql::CGqlParamObject gqlObject;

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
	case OT_ELEMENTS_COUNT:
		return GetElementsCount(gqlRequest, errorMessage);
	case OT_ELEMENT_IDS:
		return GetElementIds(gqlRequest, errorMessage);
	case OT_IMPORT:
		return ImportObject(gqlRequest, errorMessage);
	case OT_EXPORT:
		return ExportObject(gqlRequest, errorMessage);
	case OT_OBJECT_TYPE_ID:
		return GetObjectTypeId(gqlRequest, errorMessage);
	default:
		Q_ASSERT(false);
		break;
	}

	errorMessage = QString("Unable to create internal response. Operation is not supported");
	SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

	return nullptr;
}


bool CObjectCollectionControllerCompBase::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	using namespace sdl::imtbase::ImtCollection;

	const QByteArray requestCommandId = gqlRequest.GetCommandId();

	{
		if (CGetObjectVisualStatusGqlRequest::GetCommandId() == requestCommandId){
			CGetObjectVisualStatusGqlRequest req(gqlRequest, false);
			if(req.IsValid()){
				const auto& argsOpt = req.GetRequestedArguments().input.Version_1_0;
				if (!argsOpt.has_value()) {
					Q_ASSERT(false);

					return false;
				}

				const auto& typeIdOpt = argsOpt->typeId;
				const QByteArray& typeId = typeIdOpt.value_or(QByteArray());

				return m_objectTypeIdAttrPtr.FindValue(typeId) >= 0;
			}
		}
	}

	auto handleCollectionRequest = [&](auto&& request) -> bool {
		if (request.GetCommandId() == requestCommandId && request.IsValid()) {
			const auto& argsOpt = request.GetRequestedArguments().input.Version_1_0;
			if (!argsOpt.has_value()) {
				Q_ASSERT(false);
				return false;
			}
			const auto& collectionIdOpt = argsOpt->collectionId;
			const QByteArray collectionId = collectionIdOpt.value_or(QByteArray());
			if (m_collectionIdAttrPtr.IsValid()){
				return *m_collectionIdAttrPtr == collectionId;
			}
		}

		return false;
	};

	if (handleCollectionRequest(CRemoveElementSetGqlRequest(gqlRequest, false)) ||
		handleCollectionRequest(CRestoreObjectSetGqlRequest(gqlRequest, false)) ||
		handleCollectionRequest(CRestoreObjectsGqlRequest(gqlRequest, false))){
		return true;
	}

	return BaseClass::IsRequestSupported(gqlRequest);
}


// reimplemented (imtgql::IGqlRequestExtractor)

istd::IChangeableUniquePtr CObjectCollectionControllerCompBase::ExtractObject(const imtgql::CGqlRequest& gqlRequest, QByteArray& newObjectId, QString& errorMessage) const
{
	return CreateObjectFromRequest(gqlRequest, newObjectId, errorMessage);
}


// protected methods

void CObjectCollectionControllerCompBase::ReplaceComplexFilterFields(imtbase::IComplexCollectionFilter& filter) const
{
	imtbase::IComplexCollectionFilter::FieldSortingInfoList sortingInfoList = filter.GetSortingInfo();
	for (imtbase::IComplexCollectionFilter::FieldSortingInfo& fieldSortingInfo : sortingInfoList){
		if (m_fieldReplacementMap.contains(fieldSortingInfo.fieldId)){
			fieldSortingInfo.fieldId = m_fieldReplacementMap[fieldSortingInfo.fieldId];
		}
	}
	filter.SetSortingInfo(sortingInfoList);
	
	std::function<void (imtbase::IComplexCollectionFilter::GroupFilter&)> ProcessGroupFilter = [&](imtbase::IComplexCollectionFilter::GroupFilter& groupFilter){
		for (imtbase::IComplexCollectionFilter::FieldFilter& fieldFilter : groupFilter.fieldFilters){
			if (m_fieldReplacementMap.contains(fieldFilter.fieldId)){
				fieldFilter.fieldId = m_fieldReplacementMap[fieldFilter.fieldId];
			}
		}
		
		for (imtbase::IComplexCollectionFilter::GroupFilter& groupFilterItem : groupFilter.groupFilters){
			ProcessGroupFilter(groupFilterItem);
		}
	};
	
	imtbase::IComplexCollectionFilter::GroupFilter fieldsFilter = filter.GetFieldsFilter();
	ProcessGroupFilter(fieldsFilter);
	filter.SetFieldsFilter(fieldsFilter);

	QByteArrayList textFilterFieldsList = filter.GetTextFilterFieldsList();
	QByteArrayList retValFieldsList = textFilterFieldsList;

	for (int i = 0; i < textFilterFieldsList.size(); i++){
		if (m_fieldReplacementMap.contains(textFilterFieldsList[i])){
			retValFieldsList[i] = m_fieldReplacementMap[textFilterFieldsList[i]];
		}
	}

	filter.SetTextFilterFieldsList(retValFieldsList);
}


bool CObjectCollectionControllerCompBase::GetOperationFromRequest(
			const imtgql::CGqlRequest& gqlRequest,
			imtgql::CGqlParamObject& /*gqlObject*/,
			QString& errorMessage,
			int& operationType) const
{
	const imtgql::CGqlFieldObject fields = gqlRequest.GetFields();

	const QByteArrayList ids = fields.GetFieldIds();
	for (const QByteArray& fieldId: ids){
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
		if (fieldId == "import"){
			operationType = OT_IMPORT;
			return true;
		}
		if (fieldId == "export"){
			operationType = OT_EXPORT;
			return true;
		}
		if (fieldId == "objectTypeId"){
			operationType = OT_OBJECT_TYPE_ID;
			return true;
		}
	}

	errorMessage = QString("Unable to get the operation type from the request");

	SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

	return false;
}


QByteArray CObjectCollectionControllerCompBase::GetObjectIdFromInputParams(const imtgql::CGqlParamObject& inputParams) const
{
	QByteArray retVal;

	if (inputParams.GetParamIds().contains(QByteArrayLiteral("id"))){
		retVal = inputParams.GetParamArgumentValue(QByteArrayLiteral("id")).toByteArray();
	}
	else if(inputParams.GetParamIds().contains(QByteArrayLiteral("input"))){
		const imtgql::CGqlParamObject* inputObject = inputParams.GetParamArgumentObjectPtr(QByteArrayLiteral("input"));
		if (inputObject != nullptr){
			return GetObjectIdFromInputParams(*inputObject);
		}
	}

	return retVal;
}


QByteArray CObjectCollectionControllerCompBase::GetObjectIdFromRequest(const imtgql::CGqlRequest& gqlRequest) const
{
	return GetObjectIdFromInputParams(gqlRequest.GetParams());
}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::GetObject(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to get data object. Error: Attribute 'm_objectCollectionCompPtr' was not set").toUtf8();
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return nullptr;
	}

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		errorMessage = QString("Unable to get data object. Error: GraphQL input params is invalid.").toUtf8();
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return nullptr;
	}

	QByteArray objectId = inputParamPtr->GetParamArgumentValue("id").toByteArray();
	QByteArray objectTypeId = GetObjectTypeIdFromRequest(gqlRequest);

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		errorMessage = QString("Unable to get data with ID: '%1'. The object does not exist.").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

	if (!CreateRepresentationFromObject(*dataPtr, objectTypeId, gqlRequest, *dataModelPtr, errorMessage)){
		if (errorMessage.isEmpty()){
			errorMessage = QString("Unable create object representation for the object with ID: '%1'.").arg(qPrintable(objectId));
		}

		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return nullptr;
	}

	return rootModelPtr.PopPtr();
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

	const imtgql::CGqlParamObject* gqlInputParamPtr = gqlRequest.GetParamObject("input");
	if (gqlInputParamPtr == nullptr){
		errorMessage = QString("Unable to insert an object. GraphQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return nullptr;
	}

	QByteArray typeId = GetObjectTypeIdFromRequest(gqlRequest);
	QString name = GetObjectNameFromRequest(gqlRequest);
	QString description = gqlInputParamPtr->GetParamArgumentValue("description").toString();

	if (typeId.isEmpty()){
		typeId = "DocumentInfo";
	}

	QByteArray objectId;
	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr != nullptr){
		objectId = inputParamPtr->GetParamArgumentValue("id").toByteArray();
	}

	QByteArray objectIdFromRepresentation;
	istd::IChangeableUniquePtr newObjectPtr = CreateObjectFromRequest(gqlRequest, objectIdFromRepresentation, errorMessage);
	if (!newObjectPtr.IsValid()){
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return nullptr;
	}

	if (!objectIdFromRepresentation.isEmpty()){
		objectId = objectIdFromRepresentation;
	}

	imtbase::ICollectionInfo::Ids elementIds = m_objectCollectionCompPtr->GetElementIds();
	if (elementIds.contains(objectId)){
		errorMessage = QString("Object with ID: '%1' already exists").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return nullptr;
	}

	istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
	if (m_operationContextControllerCompPtr.IsValid()){
		operationContextPtr = m_operationContextControllerCompPtr->CreateOperationContext("Create", objectId, newObjectPtr.GetPtr());
	}

	QByteArray newObjectId = m_objectCollectionCompPtr->InsertNewObject(typeId, name, description, newObjectPtr.GetPtr(), objectId, nullptr, nullptr, operationContextPtr.GetPtr());
	if (newObjectId.isEmpty()){
		errorMessage = QString("Error when creating a new object. Object-ID: '%1'.").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return nullptr;
	}

	sdl::imtbase::ImtCollection::CAddedNotificationPayload::V1_0 response;
	response.id = newObjectId;

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");
	Q_ASSERT(dataModelPtr != nullptr);

	if (!response.WriteToModel(*dataModelPtr)){
		errorMessage = QString("Unable to insert object '%1'. Error: Unable to write notification data to the model").arg(qPrintable(newObjectId));
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		return nullptr;
	}

	return rootModelPtr.PopPtr();
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

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		errorMessage = QString("Unable to update an object. GraphQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return nullptr;
	}

	QByteArray objectId = inputParamPtr->GetParamArgumentValue("id").toByteArray();
	QString name = GetObjectNameFromRequest(gqlRequest);

	imtbase::IObjectCollection::DataPtr savedObjectPtr;
	if (!m_objectCollectionCompPtr->GetObjectData(objectId, savedObjectPtr)){
		errorMessage = QString("Can't get object from collection: '%1'").arg(qPrintable(objectId));

		return nullptr;
	}

	Q_ASSERT(savedObjectPtr.IsValid());

	if (!UpdateObjectFromRequest(gqlRequest, *savedObjectPtr, errorMessage)){
		if (errorMessage.isEmpty()){
			errorMessage = QString("Can't update object in the collection: '%1'").arg(qPrintable(objectId));
		}

		return nullptr;
	}

	istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
	if (m_operationContextControllerCompPtr.IsValid()){
		operationContextPtr = m_operationContextControllerCompPtr->CreateOperationContext("Update", objectId, savedObjectPtr.GetPtr());
	}

	if (!m_objectCollectionCompPtr->SetObjectData(objectId, *savedObjectPtr.GetPtr(), istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr.GetPtr())){
		errorMessage = QString("Can not update object: '%1'").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return nullptr;
	}

	const bool isNameUpdateRquired = inputParamPtr->GetParamIds().contains("name");
	if (isNameUpdateRquired){
		QString currentName = m_objectCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString();
		if (currentName != name){
			m_objectCollectionCompPtr->SetElementName(objectId, name);
		}
	}

	sdl::imtbase::ImtCollection::CUpdatedNotificationPayload::V1_0 response;
	response.id = objectId;

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

	if (!response.WriteToModel(*dataModelPtr, false)){
		errorMessage = QString("Unable to update object '%1'. Error: Unable to write notification data to the model").arg(qPrintable(objectId));
		return nullptr;
	}

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

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		errorMessage = QString("Unable to update collection. GraphQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return nullptr;
	}

	QByteArray objectIdsJson = inputParamPtr->GetParamArgumentValue("Ids").toByteArray();
	if (objectIdsJson.isEmpty()){
		errorMessage = QString("Unable to update collection. Ids from input params is empty.");
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return nullptr;
	}

	imtbase::CTreeItemModel objectIdsModel;
	if (!objectIdsModel.CreateFromJson(objectIdsJson)){
		errorMessage = QString("Unable to create model from json: '%1'").arg(qPrintable(objectIdsJson));
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return nullptr;
	}

	for (int i = 0; i < objectIdsModel.GetItemsCount(); i++){
		QByteArray objectId = objectIdsModel.GetData("id", i).toByteArray();
		if (!objectId.isEmpty()){
			istd::IChangeableUniquePtr savedObjectPtr = CreateObjectFromRequest(gqlRequest, objectId, errorMessage);
			if (savedObjectPtr.IsValid()){
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
				notificationModel->SetData("id", objectIdsModel.GetData("id", i), index);
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

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		errorMessage = QString("Unable to rename object. GraphQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return nullptr;
	}

	QByteArray objectId = inputParamPtr->GetParamArgumentValue("id").toByteArray();
	QString newName = inputParamPtr->GetParamArgumentValue("newName").toString();

	if (!m_objectCollectionCompPtr->SetElementName(objectId, newName)){
		errorMessage = QString("Unable to set name '%1' for element with ID: '%2'").arg(newName, qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModel = rootModelPtr->AddTreeModel("data");

	dataModel->SetData("id", objectId);
	dataModel->SetData("name", newName);

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

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		errorMessage = QString("Unable to set description for object. GraphQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return nullptr;
	}

	QByteArray objectId = inputParamPtr->GetParamArgumentValue("id").toByteArray();
	QString description = inputParamPtr->GetParamArgumentValue("description").toString();

	if (!m_objectCollectionCompPtr->SetElementDescription(objectId, description)){
		errorMessage = QString("Unable to set description '%1' for element with ID: '%2'").arg(description, qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

	dataModelPtr->SetData("id", objectId);
	dataModelPtr->SetData("description", description);

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

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

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

	int elementsCount = m_objectCollectionCompPtr->GetElementsCount(&filterParams);

	int pagesCount = std::ceil(elementsCount / (double)count);
	if (pagesCount <= 0){
		pagesCount = 1;
	}

	imtbase::CTreeItemModel* notificationModelPtr = dataModelPtr->AddTreeModel("notification");
	notificationModelPtr->SetData("pagesCount", pagesCount);
	notificationModelPtr->SetData("totalCount", elementsCount);

	istd::TDelPtr<imtbase::IObjectCollectionIterator> objectCollectionIterator(
		m_objectCollectionCompPtr->CreateObjectCollectionIterator(QByteArray(), offset, count, &filterParams));
	if (objectCollectionIterator == nullptr){
		errorMessage = QString("Object collection iterator creation failed");
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return nullptr;
	}

	imtbase::CTreeItemModel* itemsModelPtr = dataModelPtr->AddTreeModel("items");
	itemsModelPtr->SetIsArray(true);
	while (objectCollectionIterator->Next()){
		imtbase::IObjectCollection::DataPtr objectDataPtr;
		int itemIndex = itemsModelPtr->InsertNewItem();
		if (itemIndex >= 0){
			if (!SetupGqlItem(gqlRequest, *itemsModelPtr, itemIndex, objectCollectionIterator.GetPtr(), errorMessage)){
				SendWarningMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
			}
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

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		errorMessage = QString("Unable to rename object. GraphQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return nullptr;
	}

	iprm::CParamsSet filterParams;
	const imtgql::CGqlParamObject* viewParamsPtr = inputParamPtr->GetParamArgumentObjectPtr("viewParams");
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

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		errorMessage = QString("Unable to rename object. GraphQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return nullptr;
	}

	int offset = 0, count = -1;
	iprm::CParamsSet filterParams;
	const imtgql::CGqlParamObject* viewParamsPtr = inputParamPtr->GetParamArgumentObjectPtr("viewParams");
	if (viewParamsPtr != nullptr){
		offset = viewParamsPtr->GetParamArgumentValue("offset").toInt();
		count = viewParamsPtr->GetParamArgumentValue("count").toInt();

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

	imtbase::ICollectionInfo::Ids objectIds = ExtractObjectIdsForRemoval(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return nullptr;
	}

	imtbase::ICollectionInfo::Ids elementIds = m_objectCollectionCompPtr->GetElementIds();
	for (const QByteArray& objectId : objectIds){
		if (!elementIds.contains(objectId)){
			errorMessage = QString("Unable to delete object. Object with ID '%1' does not exists").arg(QString(objectId));
			SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
			return nullptr;
		}
	}

	istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
	if (m_operationContextControllerCompPtr.IsValid()){
		operationContextPtr = m_operationContextControllerCompPtr->CreateOperationContext("Remove", objectIds.toList().join(';'));
	}

	if (!m_objectCollectionCompPtr->RemoveElements(objectIds, operationContextPtr.GetPtr())){
		errorMessage = QString("Can't remove object with ID: '%1'").arg(QString(objectIds.toList().join(';')));
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return nullptr;
	}

	sdl::imtbase::ImtCollection::CRemovedNotificationPayload::V1_0 response;
	response.elementIds = QByteArrayList(objectIds.constBegin(), objectIds.constEnd());

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");
	Q_ASSERT(dataModelPtr != nullptr);

	if (!response.WriteToModel(*dataModelPtr)){
		errorMessage = QString("Unable to delete object '%1'. Error: Unable to write notification data to the model").arg(qPrintable(objectIds.toList().join(';')));
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		return nullptr;
	}

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

	imtgql::CGqlRequest headersRequest;
	const imtgql::IGqlContext* gqlContext = gqlRequest.GetRequestContext();
	if(gqlContext != nullptr){
		const istd::IChangeable* clonePtr = gqlContext->CloneMe();
		if(clonePtr != nullptr){
			const imtgql::IGqlContext* headerGqlContext = dynamic_cast<const imtgql::IGqlContext*>(clonePtr);
			headersRequest.SetGqlContext(headerGqlContext);
		}
	}

	imtbase::CTreeItemModel* headersModelPtr = m_headersProviderCompPtr->CreateResponse(headersRequest, errorMessage);
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


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::GetObjectTypeId(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CObjectCollectionControllerCompBase");
		return nullptr;
	}

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		errorMessage = QT_TR_NOOP("Unable to import the object. GQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		return nullptr;
	}

	QByteArray objectId = inputParamPtr->GetParamArgumentValue("id").toByteArray();

	QByteArray typeId = m_objectCollectionCompPtr->GetObjectTypeId(objectId);

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	rootModelPtr->SetData("typeId", typeId);

	return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::ImportObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CObjectCollectionControllerCompBase");
		return nullptr;
	}

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		errorMessage = QT_TR_NOOP("Unable to import the object. GQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		return nullptr;
	}

	QByteArray objectData = inputParamPtr->GetParamArgumentValue("fileData").toByteArray();
	QByteArray data = QByteArray::fromBase64(objectData);
	QByteArray typeId = inputParamPtr->GetParamArgumentValue("typeId").toByteArray();
	QString mimeType = inputParamPtr->GetParamArgumentValue("mimeType").toString();
	QString name = inputParamPtr->GetParamArgumentValue("name").toString();
	QString description = inputParamPtr->GetParamArgumentValue("description").toString();

	int index = GetMimeTypeIndex(mimeType);
	if (index < 0){
		errorMessage = "Mime type is invalid";
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		return nullptr;
	}
	if (index >= m_importExportObjectFactCompPtr.GetCount()){
		errorMessage = "Import/Export object factory index out of range";
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		return nullptr;
	}
	if (index >= m_filePersistenceCompPtr.GetCount()){
		errorMessage = "File persistence index out of range";
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		return nullptr;
	}

	istd::IChangeableUniquePtr objectPersistenceInstancePtr = m_importExportObjectFactCompPtr.CreateInstance(index);
	if (!objectPersistenceInstancePtr.IsValid()){
		errorMessage = QString("Unable to import object to the collection. Error: Object instance is invalid");
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return nullptr;
	}

	imtbase::CMimeType mime;
	if (!mime.FromString(mimeType)){
		errorMessage = QString("Unable to parse mime type");
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return nullptr;
	}

	QString extension = GetExtensionFromMimeType(mime);

	QTemporaryDir tempDir;
	QString filePathTmp = tempDir.path() + "/" + QUuid::createUuid().toString() + "." + extension;

	QFile file(filePathTmp);
	if (!file.open(QIODevice::WriteOnly)){
		SendErrorMessage(0, QString("Unable to open file with name '%1'").arg(filePathTmp), "CObjectCollectionControllerCompBase");
		return nullptr;
	}

	file.write(data);
	file.close();

	if (m_filePersistenceCompPtr[index]->LoadFromFile(*objectPersistenceInstancePtr.GetPtr(), filePathTmp) != ifile::IFilePersistence::OS_OK){
		errorMessage = QString("Unable to import object to the collection");
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		QFile::remove(filePathTmp);

		return nullptr;
	}

	QByteArray objectUuid = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	imtbase::IIdentifiable* identifiableObjectPtr = dynamic_cast<imtbase::IIdentifiable*>(objectPersistenceInstancePtr.GetPtr());
	if (identifiableObjectPtr != nullptr){
		objectUuid = identifiableObjectPtr->GetObjectUuid();
	}

	if (m_objectCollectionCompPtr->GetElementIds().contains(objectUuid)){
		errorMessage = QString("Unable to import object with ID: '%1' to the collection. Error: The object already exists inside the collection").arg(qPrintable(objectUuid));
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		QFile::remove(filePathTmp);

		return nullptr;
	}

	int typeIdIndex = GetObjectTypeIdIndex(typeId);
	Q_ASSERT_X(typeIdIndex >= 0, "Type ID is invalid", "CObjectCollectionControllerCompBase");
	
	istd::IChangeableUniquePtr collectionObjectInstancePtr = m_objectFactCompPtr.CreateInstance(typeIdIndex);
	if (!collectionObjectInstancePtr.IsValid()){
		errorMessage = QString("Unable to import object to the collection. Error: Object instance is invalid");
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		
		return nullptr;
	}

	if (!ConvertObject(*objectPersistenceInstancePtr.GetPtr(), *collectionObjectInstancePtr.GetPtr())){
		return nullptr;
	}

	QByteArray retVal = m_objectCollectionCompPtr->InsertNewObject(typeId, name, description, collectionObjectInstancePtr.GetPtr(), objectUuid);
	if (retVal.isEmpty()){
		errorMessage = QString("Unable to import object with ID: '%1' to the collection. Error: The object could not be inserted into the collection").arg(qPrintable(objectUuid));
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		QFile::remove(filePathTmp);

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	rootModelPtr->SetData("id", objectUuid);
	rootModelPtr->SetData("status", "ok");
	QFile::remove(filePathTmp);

	return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::ExportObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CObjectCollectionControllerCompBase");
		return nullptr;
	}

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		errorMessage = QT_TR_NOOP("Unable to import the object. GQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		return nullptr;
	}

	QByteArray objectId = inputParamPtr->GetParamArgumentValue("id").toByteArray();
	QString mimeType = inputParamPtr->GetParamArgumentValue("mimeType").toString();

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		errorMessage = QString("Unable to export the object with ID: '%1'. Error: Object does not exists").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return nullptr;
	}

	int index = GetMimeTypeIndex(mimeType);
	if (index < 0){
		errorMessage = "Mime type is invalid";
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return nullptr;
	}
	if (index >= m_importExportObjectFactCompPtr.GetCount()){
		errorMessage = "Import/Export object factory index out of range";
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return nullptr;
	}
	if (index >= m_filePersistenceCompPtr.GetCount()){
		errorMessage = "File persistence index out of range";
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return nullptr;
	}

	imtbase::CMimeType mime;
	if (!mime.FromString(mimeType)){
		errorMessage = QString("Unable to parse mime type");
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return nullptr;
	}

	QString extension = GetExtensionFromMimeType(mime);
	QString objectName = GetExportFileName(objectId);

	QTemporaryDir tempDir;
	QString fileName = objectName + "." + extension;
	QString filePathTmp = tempDir.path() + "/" + fileName;

	istd::IChangeableUniquePtr objectPersistenceInstancePtr = m_importExportObjectFactCompPtr.CreateInstance(index);
	if (!objectPersistenceInstancePtr.IsValid()){
		errorMessage = QString("Unable to import object to the collection. Error: Object persistence instance is invalid");
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return nullptr;
	}

	if (!ConvertObject(*dataPtr.GetPtr(), *objectPersistenceInstancePtr.GetPtr())){
		return nullptr;
	}

	if (m_filePersistenceCompPtr[index]->SaveToFile(*objectPersistenceInstancePtr.GetPtr(), filePathTmp) != ifile::IFilePersistence::OS_OK){
		errorMessage = QString("Unable to export the object with ID: '%1'. Error: Saving data to the file '%1' failed").arg(qPrintable(objectId), filePathTmp);
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return nullptr;
	}

	QFile file(filePathTmp);
	if (!file.open(QIODevice::ReadOnly)){
		errorMessage = QString("Unable to export the object with ID: '%1'. Error: Unable to open file with name '%1'").arg(qPrintable(objectId), filePathTmp);
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		QFile::remove(filePathTmp);

		return nullptr;
	}

	QByteArray data = file.readAll();
	file.close();
	QFile::remove(filePathTmp);

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	rootModelPtr->SetData("fileData", data.toBase64());
	rootModelPtr->SetData("fileName", fileName);

	return rootModelPtr.PopPtr();
}


bool CObjectCollectionControllerCompBase::ConvertObject(const istd::IChangeable& source, istd::IChangeable& target) const
{
	return target.CopyFrom(source);
}


int CObjectCollectionControllerCompBase::GetObjectTypeIdIndex(const QByteArray& typeId) const
{
	for (int i = 0; i < m_objectTypeIdAttrPtr.GetCount(); i++){
		if (m_objectTypeIdAttrPtr[i] == typeId){
			return i;
		}
	}

	return -1;
}


int CObjectCollectionControllerCompBase::GetMimeTypeIndex(const QString& mimeType) const
{
	for (int i = 0; i < m_mimeTypeAttrPtr.GetCount(); i++){
		if (m_mimeTypeAttrPtr[i] == mimeType){
			return i;
		}
	}

	return -1;
}


QString CObjectCollectionControllerCompBase::GetExtensionFromMimeType(const imtbase::CMimeType& mimeType) const
{
	return mimeType.GetSuffix();
}


QString CObjectCollectionControllerCompBase::GetExportFileName(const QByteArray& objectId) const
{
	QString objectName = m_objectCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString();
	if (objectName.isEmpty()){
		objectName = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	}

	return objectName;
}


bool CObjectCollectionControllerCompBase::UpdateObjectFromRequest(
			const imtgql::CGqlRequest& /*gqlRequest*/,
			istd::IChangeable& /*object*/,
			QString& /*errorMessage*/) const
{
	return false;
}


QVariant CObjectCollectionControllerCompBase::GetInputArgumentFromRequest(
			const imtgql::CGqlRequest& gqlRequest,
			const QString& argumentKey) const
{
	const imtgql::CGqlParamObject* inputParamObjectPtr = gqlRequest.GetParamObject("input");
	if (inputParamObjectPtr == nullptr){
		return QVariant();
	}

	return inputParamObjectPtr->GetParamArgumentValue(argumentKey.toUtf8());
}


imtbase::ICollectionInfo::Ids CObjectCollectionControllerCompBase::ExtractObjectIdsForRemoval(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	imtbase::ICollectionInfo::Ids retVal;

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		errorMessage = QString("Failed to delete objects: 'input' parameter is missing or invalid.");
		SendErrorMessage(0, errorMessage, "Object collection controller");
		return retVal;
	}

	sdl::imtbase::ImtCollection::CRemoveElementsInput::V1_0 removeElementsInput;
	if (!removeElementsInput.ReadFromGraphQlObject(*inputParamPtr)){
		errorMessage = QString("Failed to delete objects: unable to parse 'input' parameters.");
		SendErrorMessage(0, errorMessage, "Object collection controller");
		return retVal;
	}

	if (removeElementsInput.elementIds.has_value()){
		retVal = imtbase::ICollectionInfo::Ids((*removeElementsInput.elementIds).constBegin(), (*removeElementsInput.elementIds).constEnd());
	}
	else{
		errorMessage = QString("Failed to delete objects: 'elementIds' list is empty.");
		SendErrorMessage(0, errorMessage, "Object collection controller");
	}

	return retVal;
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

		if(informationId == QByteArray("id")){
			elementInformation = QString(collectionId);
		}
		else if(informationId == QByteArray("name")){
			elementInformation = m_objectCollectionCompPtr->GetElementInfo(collectionId, imtbase::ICollectionInfo::EIT_NAME);
		}
		else if(informationId == QByteArray("description")){
			elementInformation = m_objectCollectionCompPtr->GetElementInfo(collectionId, imtbase::ICollectionInfo::EIT_DESCRIPTION);
		}
		else{
			idoc::MetaInfoPtr elementMetaInfo = m_objectCollectionCompPtr->GetElementMetaInfo(collectionId);
			if (elementMetaInfo.IsValid()){
				if (informationId == QByteArray("added")){
					elementInformation = elementMetaInfo->GetMetaInfo(imtbase::IObjectCollection::MIT_INSERTION_TIME)
					.toDateTime().toString("dd.MM.yyyy hh:mm:ss");
				}
				else if (informationId == QByteArray("lastModified")){
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

		if(informationId == QByteArray("id")){
			elementInformation = QString(collectionId);
		}
		else if(informationId == QByteArray("name")){
			elementInformation = objectCollectionIterator->GetElementInfo("Name");
		}
		else if(informationId == QByteArray("description")){
			elementInformation = objectCollectionIterator->GetElementInfo("Description");
		}
		else{
			if (elementMetaInfo.IsValid()){
				if (informationId == QByteArray("added")){
					elementInformation = elementMetaInfo->GetMetaInfo(imtbase::IObjectCollection::MIT_INSERTION_TIME)
					.toDateTime().toString("dd.MM.yyyy hh:mm:ss");
				}
				else if (informationId == QByteArray("lastModified")){
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
	QByteArrayList retVal;
	const imtgql::CGqlFieldObject fields = gqlRequest.GetFields();
	const imtgql::CGqlFieldObject* findObject = fields.GetFieldArgumentObjectPtr(objectId);
	if (findObject != nullptr){
		retVal =findObject->GetFieldIds();
	}

	return retVal;
}


QVariant CObjectCollectionControllerCompBase::GetObjectInformation(const QByteArray& /*informationId*/, const QByteArray& /*objectId*/) const
{
	return QVariant();
}


QByteArray CObjectCollectionControllerCompBase::GetObjectTypeIdFromRequest(const imtgql::CGqlRequest& gqlRequest) const
{
	const imtgql::CGqlParamObject* gqlInputParamPtr = gqlRequest.GetParamObject("input");
	if (gqlInputParamPtr != nullptr){
		QVariant typeId = gqlInputParamPtr->GetParamArgumentValue("typeId");
		if (!typeId.isNull()){
			return typeId.toByteArray();
		}
	}

	return QByteArray();
}


bool CObjectCollectionControllerCompBase::CreateRepresentationFromObject(
			const istd::IChangeable& /*data*/,
			const QByteArray& /*objectTypeId*/,
			const imtgql::CGqlRequest& /*gqlRequest*/,
			imtbase::CTreeItemModel& /*dataModel*/,
			QString& /*errorMessage*/) const
{
	return false;
}


istd::IChangeableUniquePtr CObjectCollectionControllerCompBase::CreateObjectFromInputParams(
			const QList<imtgql::CGqlParamObject>& /*inputParams*/,
			QByteArray& /*objectId*/,
			QString& /*errorMessage*/) const
{
	return nullptr;
}


istd::IChangeableUniquePtr CObjectCollectionControllerCompBase::CreateObjectFromRequest(
			const imtgql::CGqlRequest& gqlRequest,
			QByteArray& newObjectId,
			QString& errorMessage) const
{
	QList<imtgql::CGqlParamObject> inputParams;
	inputParams.append(gqlRequest.GetParams());

	return CreateObjectFromInputParams(inputParams, newObjectId, errorMessage);
}


void CObjectCollectionControllerCompBase::PrepareFilters(
			const imtgql::CGqlRequest& gqlRequest,
			const imtgql::CGqlParamObject& viewParamsGql,
			iprm::CParamsSet& filterParams) const
{
	this->SetAdditionalFilters(gqlRequest, viewParamsGql, &filterParams);

	const imtgql::CGqlParamObject* complexFilterModelPtr = viewParamsGql.GetParamArgumentObjectPtr("filterModel");
	if (complexFilterModelPtr != nullptr){
		sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilter::V1_0 complexFilterSdl;
		bool isComplexFilterOk = complexFilterSdl.ReadFromGraphQlObject(*complexFilterModelPtr);
		if (isComplexFilterOk){
			istd::TDelPtr<imtbase::CComplexCollectionFilter> complexFilterPtr = new imtbase::CComplexCollectionFilter();
			if (imtcol::CComplexCollectionFilterRepresentationController::ComplexCollectionFilterRepresentationToModel(complexFilterSdl, *complexFilterPtr, GetLogPtr())){
				ReplaceComplexFilterFields(*complexFilterPtr);
				SetAdditionalFilters(gqlRequest, *complexFilterPtr);

				filterParams.SetEditableParameter("ComplexFilter", complexFilterPtr.PopPtr(), true);
			}
			else{
				SendErrorMessage(0, QString("Unable to create collection filter from SDL representation"));
			}
		}
		else{
			SendErrorMessage(0, QString("Unable to read SDL filter model from GraphQL object"));
		}
	}

	const imtgql::CGqlParamObject* documentFilterModelPtr = viewParamsGql.GetParamArgumentObjectPtr("documentFilterModel");
	if (documentFilterModelPtr != nullptr){
		sdl::imtbase::DocumentCollectionFilter::CDocumentCollectionFilter::V1_0 documentFilterSdl;
		bool isDocumentFilterOk = documentFilterSdl.ReadFromGraphQlObject(*documentFilterModelPtr);
		if (isDocumentFilterOk){
			istd::TDelPtr<imtcol::CDocumentCollectionFilter> documentFilterPtr = new imtcol::CDocumentCollectionFilter();
			if (imtcol::CDocumentCollectionFilterRepresentationController::DocumentCollectionFilterRepresentationToModel(documentFilterSdl, *documentFilterPtr, GetLogPtr())){
				filterParams.SetEditableParameter("DocumentFilter", documentFilterPtr.PopPtr(), true);
			}
		}
	}
}


void CObjectCollectionControllerCompBase::SetAdditionalFilters(
			const imtgql::CGqlRequest& /*gqlRequest*/,
			const imtgql::CGqlParamObject& /*viewParamsGql*/,
			iprm::CParamsSet* /*filterParamsPtr*/) const
{
}


void CObjectCollectionControllerCompBase::SetAdditionalFilters(
			const imtgql::CGqlRequest& /*gqlRequest*/,
			imtbase::CComplexCollectionFilter& /*complexFilter*/) const
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


istd::IChangeableUniquePtr CObjectCollectionControllerCompBase::CreateObject(const QByteArray& typeId) const
{
	int index = GetObjectTypeIdIndex(typeId);
	if (m_objectFactCompPtr.IsValid() && index < m_objectFactCompPtr.GetCount()){
		return m_objectFactCompPtr.CreateInstance(index);
	}

	return nullptr;
}


QString CObjectCollectionControllerCompBase::GetObjectNameFromRequest(const imtgql::CGqlRequest& gqlRequest) const
{
	const imtgql::CGqlParamObject* gqlInputParamPtr = gqlRequest.GetParamObject("input");
	if (gqlInputParamPtr == nullptr){
		SendErrorMessage(0, "'input' parameter not found", "CObjectCollectionControllerCompBase::GetObjectNameFromRequest");

		return QString();
	}
	QString name = gqlInputParamPtr->GetParamArgumentValue("name").toString();

	return name;
}


bool CObjectCollectionControllerCompBase::DoUpdateObjectFromRequest(
			const imtgql::CGqlRequest& gqlRequest,
			istd::IChangeable& object,
			QByteArray& objectId,
			QString& errorMessage) const
{
	if(UpdateObjectFromRequest(gqlRequest, object, errorMessage)){
		return true;
	}

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		errorMessage = QString("Unable to update an object. GraphQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return false;
	}

	QString createErrorMessage;
	istd::IChangeableUniquePtr savedObjectPtr = CreateObjectFromRequest(gqlRequest, objectId, createErrorMessage);
	if (!savedObjectPtr.IsValid()){
		if (errorMessage.isEmpty()){
			errorMessage = QString("Can not create object for update: '%1'").arg(qPrintable(objectId));
		}

		SendErrorMessage(0, errorMessage, "Object collection controller");

		return false;
	}

	return object.CopyFrom(*savedObjectPtr);
}


} // namespace imtservergql


