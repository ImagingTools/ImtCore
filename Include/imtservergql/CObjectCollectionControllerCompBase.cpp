// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
#include <istd/TSingleFactory.h>
#include <iser/CJsonMemWriteArchive.h>
#include <iser/CJsonMemReadArchive.h>
#include <iprm/TParamsPtr.h>
#include <iqt/iqt.h>

// ImtCore includes
#include <imtbase/imtbase.h>
#include <imtbase/CSearchResults.h>
#include <imtbase/IIdentifiable.h>
#include <imtbase/CCollectionFilter.h>
#include <imtbase/CComplexCollectionFilter.h>
#include <imtbase/IObjectCollectionIterator.h>
#include <imtbase/COperationContext.h>
#include <imtbase/CTimeFilterParam.h>
#include <imtbase/CObjectCollection.h>
#include <imtbase/COperationDescription.h>
#include <imtbase/IRevisionController.h>
#include <imtauth/CUserInfo.h>
#include <imtgql/imtgql.h>
#include <imtgql/IGqlRequestProvider.h>
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


// reimplemented (ISearchController)

QByteArray CObjectCollectionControllerCompBase::GetControllerId() const
{
	if (m_collectionIdAttrPtr.IsValid()){
		return *m_collectionIdAttrPtr;
	}

	return QByteArray();
}


QString CObjectCollectionControllerCompBase::GetControllerName() const
{
	if (m_collectionNameAttrPtr.IsValid()){
		return *m_collectionNameAttrPtr;
	}

	return QString();
}


const imtbase::ISearchResults* CObjectCollectionControllerCompBase::Search(const QString& text) const
{
	if (!m_headersProviderCompPtr.IsValid()){
		return nullptr;
	}

	QMap<int, QByteArray> commandIds = GetSupportedCommandIds();
	QByteArray listCommandId = commandIds.value(OT_LIST);
	if (listCommandId.isEmpty()){
		Q_ASSERT(false);
		return nullptr;
	}

	imtgql::CGqlRequest gqlRequest(imtgql::IGqlRequest::RT_QUERY, listCommandId);

	imtgql::IGqlRequestProvider* gqlRequestProviderPtr = QueryInterface<imtgql::IGqlRequestProvider>(const_cast<CObjectCollectionControllerCompBase*>(this));
	if (gqlRequestProviderPtr != nullptr){
		const imtgql::IGqlRequest* gqlRequestPtr = gqlRequestProviderPtr->GetGqlRequest();
		if (gqlRequestPtr != nullptr){
			istd::TUniqueInterfacePtr<imtgql::IGqlContext> gqlContextPtr;
			gqlContextPtr.MoveCastedPtr(gqlRequestPtr->GetRequestContext()->CloneMe());
			if (!gqlContextPtr.IsValid()){
				return nullptr;
			}

			gqlRequest.SetGqlContext(gqlContextPtr.PopInterfacePtr());
		}
	}

	QString errorMessage;
	bool ok = CheckPermissions(gqlRequest, errorMessage);
	if (!ok){
		return nullptr;
	}

	typename imtcol::ICollectionHeadersProvider::HeaderIds headerIds = m_headersProviderCompPtr->GetHeaderIds();

	sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilter::V1_0 complexFilter;

	sdl::imtbase::ComplexCollectionFilter::CGroupFilter::V1_0 groupFilter;
	groupFilter.logicalOperation = sdl::imtbase::ComplexCollectionFilter::LogicalOperation::Or;

	QList<sdl::imtbase::ComplexCollectionFilter::FieldFilterUnion> fieldList;
	for (const QByteArray& headerId : headerIds){
		typename imtcol::ICollectionHeadersProvider::HeaderInfo headerInfo;
		if (m_headersProviderCompPtr->GetHeaderInfo(headerId, headerInfo)){
			if (headerInfo.filterable){
				sdl::imtbase::ComplexCollectionFilter::CFieldFilter fieldFilter;
				fieldFilter.Version_1_0.Emplace();
				fieldFilter.Version_1_0->fieldId = headerInfo.headerId;
				fieldFilter.Version_1_0->filterValue = text;
				fieldFilter.Version_1_0->filterValueType = sdl::imtbase::ComplexCollectionFilter::ValueType::String;

				imtsdl::TElementList<sdl::imtbase::ComplexCollectionFilter::FilterOperation> filterOperations;
				filterOperations << sdl::imtbase::ComplexCollectionFilter::FilterOperation::Contains;
				fieldFilter.Version_1_0->filterOperations = filterOperations;

				fieldList << sdl::imtbase::ComplexCollectionFilter::FieldFilterUnion(fieldFilter);
			}
		}
	}

	groupFilter.fieldFilters.Emplace().FromList(fieldList);
	complexFilter.fieldsFilter = groupFilter;

	imtgql::CGqlParamObject input;
	imtgql::CGqlParamObject viewParams;
	viewParams.InsertParam("offset", 0);
	viewParams.InsertParam("count", -1);

	imtgql::CGqlParamObject complexFilterGqlOblect;
	if (complexFilter.WriteToGraphQlObject(complexFilterGqlOblect)){
		viewParams.InsertParam("filterModel", complexFilterGqlOblect);
	}

	input.InsertParam("viewParams", viewParams);
	gqlRequest.AddParam("input", input);

	imtgql::CGqlFieldObject items;
	items.InsertField("id");
	items.InsertField("name");
	items.InsertField("typeId");
	items.InsertField("description");
	gqlRequest.AddField("items", items);

	istd::TDelPtr<imtbase::CTreeItemModel> resultModelPtr = ListObjects(gqlRequest, errorMessage);
	if (!resultModelPtr.IsValid()){
		return nullptr;
	}

	imtbase::CTreeItemModel* dataModelPtr = resultModelPtr->GetTreeItemModel("data");
	if (dataModelPtr == nullptr){
		return nullptr;
	}

	imtbase::CTreeItemModel* itemsModelPtr = dataModelPtr->GetTreeItemModel("items");
	if (itemsModelPtr == nullptr){
		return nullptr;
	}

	imtbase::CSearchResults* searchResultsPtr = new imtbase::CSearchResults();
	for (int i = 0; i < itemsModelPtr->GetItemsCount(); i++){
		imtbase::ISearchResults::SearchResult searchResult;

		searchResult.contextId = itemsModelPtr->GetData("id", i).toByteArray();
		searchResult.resultName = itemsModelPtr->GetData("name", i).toString();
		searchResult.resultDescription = itemsModelPtr->GetData("description", i).toString();
		searchResult.contextTypeId = itemsModelPtr->GetData("typeId", i).toByteArray();

		QUrl url;
		url.setScheme("applink");

		url.setPath(*m_collectionIdAttrPtr + "/" + searchResult.contextTypeId + "/" + searchResult.contextId);
		searchResult.url = url;

		searchResultsPtr->AddSearchResult(searchResult);
	}

	return searchResultsPtr;
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

	RegisterFilterToSelectionParams(m_documentCollectionFilter, m_documentFilterRepresentationController);
	RegisterFilterToSelectionParams(m_complexCollectionFilter, m_complexCollectionFilterRepresentationController);
	RegisterFilterToSelectionParams(m_documentIdFilter, m_documentIdFilterRepresentationController);
}


// reimplemented (sdl::imtbase::ImtCollection::CGraphQlHandlerCompBase)

sdl::imtbase::ImtCollection::CDuplicateElementsPayload CObjectCollectionControllerCompBase::OnDuplicateElements(
			const sdl::imtbase::ImtCollection::CDuplicateElementsGqlRequest& duplicateElementsRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::imtbase::ImtCollection::CDuplicateElementsPayload response;

	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CObjectCollectionControllerCompBase");
		return response;
	}

	sdl::imtbase::ImtCollection::DuplicateElementsRequestArguments arguments = duplicateElementsRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		Q_ASSERT(false);
		return response;
	}

	QByteArrayList elementIds;
	if (arguments.input.Version_1_0->elementIds.HasValue()){
		elementIds = arguments.input.Version_1_0->elementIds->ToList();
	}

	QString name;
	if (arguments.input.Version_1_0->name.HasValue()){
		name = *arguments.input.Version_1_0->name;
	}

	if (elementIds.isEmpty()){
		errorMessage = QString("Unable to duplicate elements. Error: Element-IDs is empty");
		return response;
	}

	response.Version_1_0.Emplace();
	response.Version_1_0->success = false;

	istd::CChangeGroup changeGroup(m_objectCollectionCompPtr.GetPtr());

	int count = imtbase::narrow_cast<int>(elementIds.size());
	for (const QByteArray& elementId : elementIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_objectCollectionCompPtr->GetObjectData(elementId, dataPtr)){
			istd::IChangeableUniquePtr clonedObjectPtr = dataPtr->CloneMe();
			if (clonedObjectPtr.IsValid()){
				QString duplicateName;
				QByteArray typeId = m_objectCollectionCompPtr->GetObjectTypeId(elementId);

				if (count == 1){
					duplicateName = name;
				}
				else{
					QString elementName = m_objectCollectionCompPtr->GetElementInfo(elementId, imtbase::ICollectionInfo::EIT_NAME).toString();
					duplicateName = elementName + " Copy";
				}

				m_objectCollectionCompPtr->InsertNewObject(typeId, duplicateName, "", clonedObjectPtr.GetPtr());
			}
		}
	}

	response.Version_1_0->success = true;

	return response;
}


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


sdl::imtbase::ImtCollection::CRemoveElementsPayload CObjectCollectionControllerCompBase::OnRemoveElements(
			const sdl::imtbase::ImtCollection::CRemoveElementsGqlRequest& removeElementsRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtbase::ImtCollection::CRemoveElementsPayload::V1_0 response;

	sdl::imtbase::ImtCollection::RemoveElementsRequestArguments arguments = removeElementsRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		return sdl::imtbase::ImtCollection::CRemoveElementsPayload();
	}

	QByteArray collectionId;
	if (arguments.input.Version_1_0->collectionId.has_value()){
		collectionId = *arguments.input.Version_1_0->collectionId;
	}

	QByteArrayList elementIds;
	if (arguments.input.Version_1_0->elementIds.has_value()){
		elementIds = arguments.input.Version_1_0->elementIds->ToList();
	}

	if (elementIds.isEmpty()){
		errorMessage = QString("Unable to remove elements for collection: '%1'. Error: Element-IDs not provided").arg(QString::fromUtf8(collectionId));
		return sdl::imtbase::ImtCollection::CRemoveElementsPayload();
	}

	imtbase::ICollectionInfo::Ids allElementIds = m_objectCollectionCompPtr->GetElementIds();
	for (const QByteArray& elementId : elementIds){
		if (!allElementIds.contains(elementId)){
			errorMessage = QString("Unable to delete object. Object with ID '%1' does not exists").arg(QString(elementId));
			SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
			return sdl::imtbase::ImtCollection::CRemoveElementsPayload();
		}
	}

	if (!OnBeforeRemoveElements(elementIds, gqlRequest, errorMessage)){
		return sdl::imtbase::ImtCollection::CRemoveElementsPayload();
	}

	QByteArray typeId = m_objectCollectionCompPtr->GetObjectTypeId(elementIds[0]);

	istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
	if (m_operationContextControllerCompPtr.IsValid()){
		operationContextPtr = m_operationContextControllerCompPtr->CreateOperationContext("Remove", elementIds.toList().join(';'));
	}

	bool ok = m_objectCollectionCompPtr->RemoveElements(elementIds, operationContextPtr.GetPtr());
	if (ok){
		CreateUserActionLog(elementIds[0], typeId, "Delete", gqlRequest);
		OnAfterRemoveElements(elementIds, gqlRequest);
	}

	response.success = ok;

	sdl::imtbase::ImtCollection::CRemoveElementsPayload retVal;
	retVal.Version_1_0 = std::move(response);

	return retVal;
}


sdl::imtbase::ImtCollection::CRemoveElementSetPayload CObjectCollectionControllerCompBase::OnRemoveElementSet(
			const sdl::imtbase::ImtCollection::CRemoveElementSetGqlRequest& removeElementSetRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtbase::ImtCollection::CRemoveElementSetPayload::V1_0 response;

	sdl::imtbase::ImtCollection::RemoveElementSetRequestArguments arguments = removeElementSetRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		return sdl::imtbase::ImtCollection::CRemoveElementSetPayload();
	}

	iprm::CParamsSet filterParams;
	if (arguments.input.Version_1_0->selectionParams){
		if (filterParams.CopyFrom(m_selectionParams)){
			sdl::imtbase::ImtBaseTypes::CParamsSet::V1_0 paramsSet = *arguments.input.Version_1_0->selectionParams;
			if (!GetParamsSetFromRepresentation(paramsSet, filterParams)){
				errorMessage = QString("Unable to remove element set for collection '%1'. Error: Selection Params parsing failed").arg(QString::fromUtf8(*m_collectionIdAttrPtr));
				return sdl::imtbase::ImtCollection::CRemoveElementSetPayload();
			}
		}
	}

	imtbase::ICollectionInfo::Ids elementIds = m_objectCollectionCompPtr->GetElementIds(0, -1, &filterParams);
	if (!OnBeforeRemoveElements(elementIds, gqlRequest, errorMessage)){
		return sdl::imtbase::ImtCollection::CRemoveElementSetPayload();
	}

	QByteArray typeId = m_objectCollectionCompPtr->GetObjectTypeId(elementIds[0]);

	istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
	if (m_operationContextControllerCompPtr.IsValid()){
		operationContextPtr = m_operationContextControllerCompPtr->CreateOperationContext("Remove", elementIds.toList().join(';'));
	}

	bool ok = m_objectCollectionCompPtr->RemoveElementSet(&filterParams, operationContextPtr.GetPtr());
	if (ok){
		CreateUserActionLog(elementIds[0], typeId, "Delete", gqlRequest);
		OnAfterRemoveElements(elementIds, gqlRequest);
	}

	response.success = ok;

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
		objectIds = arguments.input.Version_1_0->objectIds->ToList();
	}

	istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
	if (m_operationContextControllerCompPtr.IsValid()){
		operationContextPtr = m_operationContextControllerCompPtr->CreateOperationContext("Restore", objectIds.toList().join(';'));
	}

	response.success = m_objectCollectionCompPtr->RestoreObjects(imtbase::ICollectionInfo::Ids(objectIds.constBegin(), objectIds.constEnd()), operationContextPtr.GetPtr());

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

	iprm::CParamsSet filterParams;
	if (arguments.input.Version_1_0->selectionParams){
		if (filterParams.CopyFrom(m_selectionParams)){
			sdl::imtbase::ImtBaseTypes::CParamsSet::V1_0 paramsSet = *arguments.input.Version_1_0->selectionParams;
			if (!GetParamsSetFromRepresentation(paramsSet, filterParams)){
				errorMessage = QString("Unable to restore object set for collection '%1'. Error: Selection Params parsing failed").arg(QString::fromUtf8(*m_collectionIdAttrPtr));
				return sdl::imtbase::ImtCollection::CRestoreObjectSetPayload();
			}
		}
	}

	imtbase::ICollectionInfo::Ids elementIds = m_objectCollectionCompPtr->GetElementIds(0, -1, &filterParams);

	istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
	if (m_operationContextControllerCompPtr.IsValid()){
		operationContextPtr = m_operationContextControllerCompPtr->CreateOperationContext("Restore", elementIds.toList().join(';'));
	}

	response.success = m_objectCollectionCompPtr->RestoreObjectSet(&filterParams, operationContextPtr.GetPtr());

	sdl::imtbase::ImtCollection::CRestoreObjectSetPayload retVal;
	retVal.Version_1_0 = std::move(response);

	return retVal;
}


sdl::imtbase::ImtCollection::CSetObjectNamePayload CObjectCollectionControllerCompBase::OnSetObjectName(
			const sdl::imtbase::ImtCollection::CSetObjectNameGqlRequest& setObjectNameRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtbase::ImtCollection::CSetObjectNamePayload::V1_0 response;
	response.success = false;

	sdl::imtbase::ImtCollection::SetObjectNameRequestArguments arguments = setObjectNameRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		Q_ASSERT(false);
		return sdl::imtbase::ImtCollection::CSetObjectNamePayload();
	}

	QByteArray objectId;
	if (arguments.input.Version_1_0->objectId){
		objectId = *arguments.input.Version_1_0->objectId;
	}

	QString newName;
	if (arguments.input.Version_1_0->name){
		newName = *arguments.input.Version_1_0->name;
	}

	if (!OnBeforeSetObjectName(objectId, newName, gqlRequest, errorMessage)){
		return sdl::imtbase::ImtCollection::CSetObjectNamePayload();
	}

	QString oldName = m_objectCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString();

	if (!m_objectCollectionCompPtr->SetElementName(objectId, newName)){
		errorMessage = QString("Unable to set name '%1' for element with ID: '%2'").arg(newName, QString::fromUtf8(objectId));
		return sdl::imtbase::ImtCollection::CSetObjectNamePayload();
	}

	OnAfterSetObjectName(objectId, oldName, newName, gqlRequest);

	sdl::imtbase::ImtCollection::CSetObjectNamePayload retVal;
	retVal.Version_1_0 = std::move(response);

	retVal.Version_1_0->objectId = objectId;
	retVal.Version_1_0->name = newName;
	retVal.Version_1_0->success = true;

	return retVal;
}


sdl::imtbase::ImtCollection::CSetObjectDescriptionPayload CObjectCollectionControllerCompBase::OnSetObjectDescription(
			const sdl::imtbase::ImtCollection::CSetObjectDescriptionGqlRequest& setObjectDescriptionRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtbase::ImtCollection::CSetObjectDescriptionPayload::V1_0 response;
	response.success = false;

	sdl::imtbase::ImtCollection::SetObjectDescriptionRequestArguments arguments = setObjectDescriptionRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		Q_ASSERT(false);
		return sdl::imtbase::ImtCollection::CSetObjectDescriptionPayload();
	}

	QByteArray objectId;
	if (arguments.input.Version_1_0->objectId){
		objectId = *arguments.input.Version_1_0->objectId;
	}

	QString description;
	if (arguments.input.Version_1_0->description){
		description = *arguments.input.Version_1_0->description;
	}

	if (!OnBeforeSetObjectDescription(objectId, description, gqlRequest, errorMessage)){
		return sdl::imtbase::ImtCollection::CSetObjectDescriptionPayload();
	}

	istd::CChangeGroup changeGroup(m_objectCollectionCompPtr.GetPtr());
	if (!m_objectCollectionCompPtr->SetElementDescription(objectId, description)){
		changeGroup.Reset();

		errorMessage = QString("Unable to set description '%1' for element with ID: '%2'").arg(description, QString::fromUtf8(objectId));
		return sdl::imtbase::ImtCollection::CSetObjectDescriptionPayload();
	}

	OnAfterSetObjectDescription(objectId, description, gqlRequest);

	sdl::imtbase::ImtCollection::CSetObjectDescriptionPayload retVal;
	retVal.Version_1_0 = std::move(response);

	retVal.Version_1_0->objectId = objectId;
	retVal.Version_1_0->description = description;
	retVal.Version_1_0->success = true;

	return retVal;
}


sdl::imtbase::ImtCollection::CExportObjectPayload CObjectCollectionControllerCompBase::OnExportObject(
			const sdl::imtbase::ImtCollection::CExportObjectGqlRequest& exportObjectRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::imtbase::ImtCollection::CExportObjectPayload::V1_0 response;

	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CObjectCollectionControllerCompBase");
		return sdl::imtbase::ImtCollection::CExportObjectPayload();
	}

	sdl::imtbase::ImtCollection::ExportObjectRequestArguments arguments = exportObjectRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		Q_ASSERT(false);
		return sdl::imtbase::ImtCollection::CExportObjectPayload();
	}

	QByteArray objectId;
	if (arguments.input.Version_1_0->objectId){
		objectId = *arguments.input.Version_1_0->objectId;
	}

	QString mimeType;
	if (arguments.input.Version_1_0->mimeType){
		mimeType = *arguments.input.Version_1_0->mimeType;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		errorMessage = QString("Unable to export the object with ID: '%1'. Error: Object does not exists").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		return sdl::imtbase::ImtCollection::CExportObjectPayload();
	}

	int index = GetMimeTypeIndex(mimeType);
	if (index < 0){
		errorMessage = "Mime type is invalid";
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		return sdl::imtbase::ImtCollection::CExportObjectPayload();
	}

	if (index >= m_importExportObjectFactCompPtr.GetCount()){
		errorMessage = "Import/Export object factory index out of range";
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		return sdl::imtbase::ImtCollection::CExportObjectPayload();
	}

	if (index >= m_filePersistenceCompPtr.GetCount()){
		errorMessage = "File persistence index out of range";
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		return sdl::imtbase::ImtCollection::CExportObjectPayload();
	}

	imtbase::CMimeType mime;
	if (!mime.FromString(mimeType)){
		errorMessage = QString("Unable to parse mime type");
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		return sdl::imtbase::ImtCollection::CExportObjectPayload();
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
		return sdl::imtbase::ImtCollection::CExportObjectPayload();
	}

	if (!ConvertObject(*dataPtr.GetPtr(), *objectPersistenceInstancePtr.GetPtr())){
		return sdl::imtbase::ImtCollection::CExportObjectPayload();
	}

	if (m_filePersistenceCompPtr[index]->SaveToFile(*objectPersistenceInstancePtr.GetPtr(), filePathTmp) != ifile::IFilePersistence::OS_OK){
		errorMessage = QString("Unable to export the object with ID: '%1'. Error: Saving data to the file '%1' failed").arg(qPrintable(objectId), filePathTmp);
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		return sdl::imtbase::ImtCollection::CExportObjectPayload();
	}

	QFile file(filePathTmp);
	if (!file.open(QIODevice::ReadOnly)){
		errorMessage = QString("Unable to export the object with ID: '%1'. Error: Unable to open file with name '%1'").arg(qPrintable(objectId), filePathTmp);
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		QFile::remove(filePathTmp);
		return sdl::imtbase::ImtCollection::CExportObjectPayload();
	}

	QByteArray data = file.readAll();
	file.close();

	response.objectId = objectId;
	response.fileData = data.toBase64();
	response.fileName = fileName;

	QFile::remove(filePathTmp);

	sdl::imtbase::ImtCollection::CExportObjectPayload retVal;
	retVal.Version_1_0 = std::move(response);

	return retVal;
}


sdl::imtbase::ImtCollection::CImportObjectPayload CObjectCollectionControllerCompBase::OnImportObject(
			const sdl::imtbase::ImtCollection::CImportObjectGqlRequest& importObjectRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::imtbase::ImtCollection::CImportObjectPayload::V1_0 response;
	response.success = false;

	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CObjectCollectionControllerCompBase");
		return sdl::imtbase::ImtCollection::CImportObjectPayload();
	}

	sdl::imtbase::ImtCollection::ImportObjectRequestArguments arguments = importObjectRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		Q_ASSERT(false);
		return sdl::imtbase::ImtCollection::CImportObjectPayload();
	}

	QByteArray objectData;
	if (arguments.input.Version_1_0->fileData){
		objectData = *arguments.input.Version_1_0->fileData;
	}

	QByteArray data = QByteArray::fromBase64(objectData);

	QByteArray typeId;
	if (arguments.input.Version_1_0->typeId){
		typeId = *arguments.input.Version_1_0->typeId;
	}

	QString mimeType;
	if (arguments.input.Version_1_0->mimeType){
		mimeType = *arguments.input.Version_1_0->mimeType;
	}

	QString name;
	if (arguments.input.Version_1_0->name){
		name = *arguments.input.Version_1_0->name;
	}

	QString description;
	if (arguments.input.Version_1_0->description){
		description = *arguments.input.Version_1_0->description;
	}

	int index = GetMimeTypeIndex(mimeType);
	if (index < 0){
		errorMessage = "Mime type is invalid";
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		return sdl::imtbase::ImtCollection::CImportObjectPayload();
	}
	if (index >= m_importExportObjectFactCompPtr.GetCount()){
		errorMessage = "Import/Export object factory index out of range";
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		return sdl::imtbase::ImtCollection::CImportObjectPayload();
	}
	if (index >= m_filePersistenceCompPtr.GetCount()){
		errorMessage = "File persistence index out of range";
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		return sdl::imtbase::ImtCollection::CImportObjectPayload();
	}

	istd::IChangeableUniquePtr objectPersistenceInstancePtr = m_importExportObjectFactCompPtr.CreateInstance(index);
	if (!objectPersistenceInstancePtr.IsValid()){
		errorMessage = QString("Unable to import object to the collection. Error: Object instance is invalid");
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return sdl::imtbase::ImtCollection::CImportObjectPayload();
	}

	imtbase::CMimeType mime;
	if (!mime.FromString(mimeType)){
		errorMessage = QString("Unable to parse mime type");
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return sdl::imtbase::ImtCollection::CImportObjectPayload();
	}

	QString extension = GetExtensionFromMimeType(mime);

	QTemporaryDir tempDir;
	QString filePathTmp = tempDir.path() + "/" + QUuid::createUuid().toString() + "." + extension;

	QFile file(filePathTmp);
	if (!file.open(QIODevice::WriteOnly)){
		SendErrorMessage(0, QString("Unable to open file with name '%1'").arg(filePathTmp), "CObjectCollectionControllerCompBase");
		return sdl::imtbase::ImtCollection::CImportObjectPayload();
	}

	file.write(data);
	file.close();

	if (m_filePersistenceCompPtr[index]->LoadFromFile(*objectPersistenceInstancePtr.GetPtr(), filePathTmp) != ifile::IFilePersistence::OS_OK){
		errorMessage = QString("Unable to import object to the collection");
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		QFile::remove(filePathTmp);

		return sdl::imtbase::ImtCollection::CImportObjectPayload();
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

		return sdl::imtbase::ImtCollection::CImportObjectPayload();
	}

	int typeIdIndex = GetObjectTypeIdIndex(typeId);
	Q_ASSERT_X(typeIdIndex >= 0, "Type ID is invalid", "CObjectCollectionControllerCompBase");

	istd::IChangeableUniquePtr collectionObjectInstancePtr = m_objectFactCompPtr.CreateInstance(typeIdIndex);
	if (!collectionObjectInstancePtr.IsValid()){
		errorMessage = QString("Unable to import object to the collection. Error: Object instance is invalid");
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		return sdl::imtbase::ImtCollection::CImportObjectPayload();
	}

	if (!ConvertObject(*objectPersistenceInstancePtr.GetPtr(), *collectionObjectInstancePtr.GetPtr())){
		return sdl::imtbase::ImtCollection::CImportObjectPayload();
	}

	QByteArray insertRetVal = m_objectCollectionCompPtr->InsertNewObject(typeId, name, description, collectionObjectInstancePtr.GetPtr(), objectUuid);
	if (insertRetVal.isEmpty()){
		errorMessage = QString("Unable to import object with ID: '%1' to the collection. Error: The object could not be inserted into the collection").arg(qPrintable(objectUuid));
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		QFile::remove(filePathTmp);
		return sdl::imtbase::ImtCollection::CImportObjectPayload();
	}

	response.objectId = objectUuid;
	response.success = true;
	QFile::remove(filePathTmp);

	sdl::imtbase::ImtCollection::CImportObjectPayload retVal;
	retVal.Version_1_0 = std::move(response);

	return retVal;
}


sdl::imtbase::ImtCollection::CGetObjectTypeIdPayload CObjectCollectionControllerCompBase::OnGetObjectTypeId(
			const sdl::imtbase::ImtCollection::CGetObjectTypeIdGqlRequest& getObjectTypeIdRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CObjectCollectionControllerCompBase");
		return sdl::imtbase::ImtCollection::CGetObjectTypeIdPayload();
	}

	sdl::imtbase::ImtCollection::GetObjectTypeIdRequestArguments arguments = getObjectTypeIdRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		Q_ASSERT(false);
		return sdl::imtbase::ImtCollection::CGetObjectTypeIdPayload();
	}

	QByteArray objectId;
	if (arguments.input.Version_1_0->objectId){
		objectId = *arguments.input.Version_1_0->objectId;
	}

	sdl::imtbase::ImtCollection::CGetObjectTypeIdPayload::V1_0 response;

	QByteArray typeId = m_objectCollectionCompPtr->GetObjectTypeId(objectId);
	response.typeId = typeId;

	sdl::imtbase::ImtCollection::CGetObjectTypeIdPayload retVal;
	retVal.Version_1_0 = std::move(response);

	return retVal;
}


sdl::imtbase::ImtCollection::CGetCollectionHeadersPayload CObjectCollectionControllerCompBase::OnGetCollectionHeaders(
			const sdl::imtbase::ImtCollection::CGetCollectionHeadersGqlRequest& /*getCollectionHeadersRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtbase::ImtCollection::CGetCollectionHeadersPayload::V1_0 response;

	if (!m_headersProviderCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'HeadersProvider' was not set", "CObjectCollectionControllerCompBase");
		return sdl::imtbase::ImtCollection::CGetCollectionHeadersPayload();
	}

	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr == nullptr){
		errorMessage = QString("Unable to get collection headers. Error: GraphQL context is invalid");
		return sdl::imtbase::ImtCollection::CGetCollectionHeadersPayload();
	}

	QByteArrayList userPermissions;
	const imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
	if (userInfoPtr != nullptr){
		userPermissions = userInfoPtr->GetPermissions();
	}

	QByteArray languageId = gqlContextPtr->GetLanguageId();

	QList<sdl::imtbase::ImtCollection::CHeaderInfo::V1_0> headersRepresentationList;
	imtcol::ICollectionHeadersProvider::HeaderIds headerIds = m_headersProviderCompPtr->GetHeaderIds();
	for (const QByteArray& headerId : headerIds){
		imtcol::ICollectionHeadersProvider::HeaderInfo headerInfo;
		if (m_headersProviderCompPtr->GetHeaderInfo(headerId, headerInfo)){
			// Check header permissions
			if (userInfoPtr != nullptr && !userInfoPtr->IsAdmin()){
				QByteArray headerPermission = headerInfo.permissionId;
				if (!headerPermission.isEmpty() && !userPermissions.contains(headerPermission)){
					continue;
				}
			}

			sdl::imtbase::ImtCollection::CHeaderInfo::V1_0 headerRepresentation;
			headerRepresentation.id = headerInfo.headerId;
			headerRepresentation.filterable = headerInfo.filterable;
			headerRepresentation.sortable = headerInfo.sortable;

			QString headerName = headerInfo.headerName;
			if (m_translationManagerCompPtr.IsValid()){
				headerName = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), headerName.toUtf8(), languageId, QByteArrayLiteral("Attribute"));
			}
			headerRepresentation.name = headerName;

			headersRepresentationList << headerRepresentation;
		}
	}

	response.headers.Emplace();
	response.headers->FromList(headersRepresentationList);

	sdl::imtbase::ImtCollection::CGetCollectionHeadersPayload retVal;
	retVal.Version_1_0 = std::move(response);

	return retVal;
}


sdl::imtbase::ImtCollection::CGetElementsCountPayload CObjectCollectionControllerCompBase::OnGetElementsCount(
			const sdl::imtbase::ImtCollection::CGetElementsCountGqlRequest& getElementsCountRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::imtbase::ImtCollection::CGetElementsCountPayload::V1_0 response;

	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CObjectCollectionControllerCompBase");
		return sdl::imtbase::ImtCollection::CGetElementsCountPayload();
	}

	sdl::imtbase::ImtCollection::GetElementsCountRequestArguments arguments = getElementsCountRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		Q_ASSERT(false);
		return sdl::imtbase::ImtCollection::CGetElementsCountPayload();
	}

	iprm::CParamsSet filterParams;
	if (arguments.input.Version_1_0->selectionParams){
		if (filterParams.CopyFrom(m_selectionParams)){
			sdl::imtbase::ImtBaseTypes::CParamsSet::V1_0 paramsSet = *arguments.input.Version_1_0->selectionParams;
			if (!GetParamsSetFromRepresentation(paramsSet, filterParams)){
				errorMessage = QString("Unable to get elements count for collection '%1'. Error: Selection Params parsing failed").arg(QString::fromUtf8(*m_collectionIdAttrPtr));
				return sdl::imtbase::ImtCollection::CGetElementsCountPayload();
			}
		}
	}

	response.count = m_objectCollectionCompPtr->GetElementsCount(&filterParams);

	sdl::imtbase::ImtCollection::CGetElementsCountPayload retVal;
	retVal.Version_1_0 = std::move(response);

	return retVal;
}


sdl::imtbase::ImtCollection::CGetElementIdsPayload CObjectCollectionControllerCompBase::OnGetElementIds(
			const sdl::imtbase::ImtCollection::CGetElementIdsGqlRequest& getElementIdsRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::imtbase::ImtCollection::CGetElementIdsPayload::V1_0 response;

	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CObjectCollectionControllerCompBase");
		return sdl::imtbase::ImtCollection::CGetElementIdsPayload();
	}

	sdl::imtbase::ImtCollection::GetElementIdsRequestArguments arguments = getElementIdsRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		Q_ASSERT(false);
		return sdl::imtbase::ImtCollection::CGetElementIdsPayload();
	}

	int offset = 0;
	if (arguments.input.Version_1_0->offset){
		offset = *arguments.input.Version_1_0->offset;
	}

	int count = -1;
	if (arguments.input.Version_1_0->count){
		count = *arguments.input.Version_1_0->count;
	}

	iprm::CParamsSet filterParams;
	if (arguments.input.Version_1_0->selectionParams){
		if (filterParams.CopyFrom(m_selectionParams)){
			sdl::imtbase::ImtBaseTypes::CParamsSet::V1_0 paramsSet = *arguments.input.Version_1_0->selectionParams;
			if (!GetParamsSetFromRepresentation(paramsSet, filterParams)){
				errorMessage = QString("Unable to get element IDs for collection '%1'. Error: Selection Params parsing failed").arg(QString::fromUtf8(*m_collectionIdAttrPtr));
				return sdl::imtbase::ImtCollection::CGetElementIdsPayload();
			}
		}
	}

	imtbase::IObjectCollection::Ids ids = m_objectCollectionCompPtr->GetElementIds(offset, count, &filterParams);
	response.elementIds.Emplace();
	response.elementIds->FromList(ids);

	sdl::imtbase::ImtCollection::CGetElementIdsPayload retVal;
	retVal.Version_1_0 = std::move(response);
	return retVal;
}


sdl::imtbase::ImtCollection::CInsertNewObjectPayload CObjectCollectionControllerCompBase::OnInsertNewObject(
			const sdl::imtbase::ImtCollection::CInsertNewObjectGqlRequest& insertNewObjectRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtbase::ImtCollection::CInsertNewObjectPayload::V1_0 response;

	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CObjectCollectionControllerCompBase");
		return sdl::imtbase::ImtCollection::CInsertNewObjectPayload();
	}

	sdl::imtbase::ImtCollection::InsertNewObjectRequestArguments arguments = insertNewObjectRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		Q_ASSERT(false);
		return sdl::imtbase::ImtCollection::CInsertNewObjectPayload();
	}

	QByteArray objectId;
	if (arguments.input.Version_1_0->proposedObjectId){
		objectId = *arguments.input.Version_1_0->proposedObjectId;
	}

	QByteArray typeId;
	if (arguments.input.Version_1_0->typeId){
		typeId = *arguments.input.Version_1_0->typeId;
	}

	QString name;
	if (arguments.input.Version_1_0->name){
		name = *arguments.input.Version_1_0->name;
	}

	QString description;
	if (arguments.input.Version_1_0->description){
		description = *arguments.input.Version_1_0->description;
	}

	QByteArray objectData;
	if (arguments.input.Version_1_0->objectData){
		objectData = (*arguments.input.Version_1_0->objectData).toUtf8();
	}

	istd::IChangeableUniquePtr objectPtr = nullptr;
	if (!objectData.isEmpty()){
		objectPtr = CreateObject(typeId);
		if (!DeSerializeObject(*objectPtr.GetPtr(), objectData)){
			errorMessage = QString("Unable to insert new object to collection '%1'. Error: Object serialization failed").arg(QString::fromUtf8(*m_collectionIdAttrPtr));
			return sdl::imtbase::ImtCollection::CInsertNewObjectPayload();
		}
	}

	istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
	if (m_operationContextControllerCompPtr.IsValid()){
		operationContextPtr = m_operationContextControllerCompPtr->CreateOperationContext("Create", objectId, objectPtr.GetPtr());
	}

	QByteArray result = m_objectCollectionCompPtr->InsertNewObject(typeId, name, description, objectPtr.GetPtr(), objectId, nullptr, nullptr, operationContextPtr.GetPtr());
	if (result.isEmpty()){
		errorMessage = QString("Unable to insert new object to collection '%1'").arg(QString::fromUtf8(*m_collectionIdAttrPtr));
		return sdl::imtbase::ImtCollection::CInsertNewObjectPayload();
	}

	CreateUserActionLog(objectId, typeId, "Create", gqlRequest);
	response.objectId = objectId;

	sdl::imtbase::ImtCollection::CInsertNewObjectPayload retVal;
	retVal.Version_1_0 = std::move(response);

	return retVal;
}


sdl::imtbase::ImtCollection::CSetObjectDataPayload CObjectCollectionControllerCompBase::OnSetObjectData(
			const sdl::imtbase::ImtCollection::CSetObjectDataGqlRequest& setObjectDataRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtbase::ImtCollection::CSetObjectDataPayload::V1_0 response;

	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CObjectCollectionControllerCompBase");
		return sdl::imtbase::ImtCollection::CSetObjectDataPayload();
	}

	sdl::imtbase::ImtCollection::SetObjectDataRequestArguments arguments = setObjectDataRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		Q_ASSERT(false);
		return sdl::imtbase::ImtCollection::CSetObjectDataPayload();
	}

	QByteArray objectId;
	if (arguments.input.Version_1_0->objectId){
		objectId = *arguments.input.Version_1_0->objectId;
	}

	QByteArray objectData;
	if (arguments.input.Version_1_0->objectData){
		objectData = (*arguments.input.Version_1_0->objectData).toUtf8();
	}

	QByteArray typeId = m_objectCollectionCompPtr->GetObjectTypeId(objectId);

	istd::IChangeableUniquePtr objectPtr = nullptr;
	if (!objectData.isEmpty()){
		objectPtr = CreateObject(typeId);
		if (!DeSerializeObject(*objectPtr.GetPtr(), objectData)){
			errorMessage = QString("Unable to set object data to collection '%1'. Error: Object serialization failed").arg(QString::fromUtf8(*m_collectionIdAttrPtr));
			return sdl::imtbase::ImtCollection::CSetObjectDataPayload();
		}
	}

	istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
	if (m_operationContextControllerCompPtr.IsValid()){
		operationContextPtr = m_operationContextControllerCompPtr->CreateOperationContext("Update", objectId, objectPtr.GetPtr());
	}

	bool ok = m_objectCollectionCompPtr->SetObjectData(objectId, *objectPtr.GetPtr(), istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr.GetPtr());
	if (ok){
		CreateUserActionLog(objectId, typeId, "Update", gqlRequest);
	}

	response.success = ok;

	sdl::imtbase::ImtCollection::CSetObjectDataPayload retVal;
	retVal.Version_1_0 = std::move(response);

	return retVal;
}


sdl::imtbase::ImtCollection::CGetObjectDataPayload CObjectCollectionControllerCompBase::OnGetObjectData(
			const sdl::imtbase::ImtCollection::CGetObjectDataGqlRequest& getObjectDataRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::imtbase::ImtCollection::CGetObjectDataPayload::V1_0 response;

	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CObjectCollectionControllerCompBase");
		return sdl::imtbase::ImtCollection::CGetObjectDataPayload();
	}

	sdl::imtbase::ImtCollection::GetObjectDataRequestArguments arguments = getObjectDataRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		Q_ASSERT(false);
		return sdl::imtbase::ImtCollection::CGetObjectDataPayload();
	}

	QByteArray objectId;
	if (arguments.input.Version_1_0->objectId){
		objectId = *arguments.input.Version_1_0->objectId;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		errorMessage = QString("Unable to get object data '%1'. Error: Object does not exists").arg(QString::fromUtf8(objectId));
		return sdl::imtbase::ImtCollection::CGetObjectDataPayload();
	}

	QByteArray objectData;
	if (!SerializeObject(*dataPtr.GetPtr(), objectData)){
		errorMessage = QString("Unable to get object data '%1'. Error: Object serializaion failed").arg(QString::fromUtf8(objectId));
		return sdl::imtbase::ImtCollection::CGetObjectDataPayload();
	}

	response.objectData = objectData;

	sdl::imtbase::ImtCollection::CGetObjectDataPayload retVal;
	retVal.Version_1_0 = std::move(response);

	return retVal;
}


sdl::imtbase::ImtCollection::CGetDataMetaInfoPayload CObjectCollectionControllerCompBase::OnGetDataMetaInfo(
			const sdl::imtbase::ImtCollection::CGetDataMetaInfoGqlRequest& getDataMetaInfoRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::imtbase::ImtCollection::CGetDataMetaInfoPayload::V1_0 response;

	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CObjectCollectionControllerCompBase");
		return sdl::imtbase::ImtCollection::CGetDataMetaInfoPayload();
	}

	sdl::imtbase::ImtCollection::GetDataMetaInfoRequestArguments arguments = getDataMetaInfoRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		Q_ASSERT(false);
		return sdl::imtbase::ImtCollection::CGetDataMetaInfoPayload();
	}

	QByteArray objectId;
	if (arguments.input.Version_1_0->objectId){
		objectId = *arguments.input.Version_1_0->objectId;
	}

	QByteArray metaInfoData;
	idoc::MetaInfoPtr metaInfo = m_objectCollectionCompPtr->GetDataMetaInfo(objectId);
	if (!SerializeObject(*metaInfo.GetPtr(), metaInfoData)){
		errorMessage = QString("Unable to get data meta info for object '%1'. Error: Meta Info serializaion failed").arg(QString::fromUtf8(objectId));
		return sdl::imtbase::ImtCollection::CGetDataMetaInfoPayload();
	}

	response.metaInfoData = metaInfoData;

	sdl::imtbase::ImtCollection::CGetDataMetaInfoPayload retVal;
	retVal.Version_1_0 = std::move(response);

	return retVal;
}


sdl::imtbase::ImtCollection::CGetElementInfoPayload CObjectCollectionControllerCompBase::OnGetElementInfo(
			const sdl::imtbase::ImtCollection::CGetElementInfoGqlRequest& getElementInfoRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::imtbase::ImtCollection::CGetElementInfoPayload::V1_0 response;

	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CObjectCollectionControllerCompBase");
		return sdl::imtbase::ImtCollection::CGetElementInfoPayload();
	}

	sdl::imtbase::ImtCollection::GetElementInfoRequestArguments arguments = getElementInfoRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		Q_ASSERT(false);
		return sdl::imtbase::ImtCollection::CGetElementInfoPayload();
	}

	QByteArray objectId;
	if (arguments.input.Version_1_0->elementId){
		objectId = *arguments.input.Version_1_0->elementId;
	}

	sdl::imtbase::ImtBaseTypes::CParameter::V1_0 parameterInfo;
	parameterInfo.id = objectId;

	QByteArray typeId = m_objectCollectionCompPtr->GetObjectTypeId(objectId);
	parameterInfo.typeId = typeId;

	QString name = m_objectCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toByteArray();
	parameterInfo.name = name;

	QString description = m_objectCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toByteArray();
	parameterInfo.description = description;

	bool enabled = m_objectCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_ENABLED).toBool();
	parameterInfo.enabled = enabled;

	response.elementInfo = parameterInfo;

	sdl::imtbase::ImtCollection::CGetElementInfoPayload retVal;
	retVal.Version_1_0 = std::move(response);

	return retVal;
}


sdl::imtbase::ImtCollection::CGetElementMetaInfoPayload CObjectCollectionControllerCompBase::OnGetElementMetaInfo(
			const sdl::imtbase::ImtCollection::CGetElementMetaInfoGqlRequest& getElementMetaInfoRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::imtbase::ImtCollection::CGetElementMetaInfoPayload::V1_0 response;

	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CObjectCollectionControllerCompBase");
		return sdl::imtbase::ImtCollection::CGetElementMetaInfoPayload();
	}

	sdl::imtbase::ImtCollection::GetElementMetaInfoRequestArguments arguments = getElementMetaInfoRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		Q_ASSERT(false);
		return sdl::imtbase::ImtCollection::CGetElementMetaInfoPayload();
	}

	QByteArray objectId;
	if (arguments.input.Version_1_0->elementId){
		objectId = *arguments.input.Version_1_0->elementId;
	}

	idoc::MetaInfoPtr metaInfo = m_objectCollectionCompPtr->GetElementMetaInfo(objectId);
	if (!metaInfo.IsValid()){
		errorMessage = QString("Unable to get element meta info for object '%1'. Error: Meta Info is invalid").arg(QString::fromUtf8(objectId));
		return sdl::imtbase::ImtCollection::CGetElementMetaInfoPayload();
	}

	QList<sdl::imtbase::ImtBaseTypes::CParameter::V1_0> parameterInfos;
	idoc::IDocumentMetaInfo::MetaInfoTypes metaInfoTypes = metaInfo->GetMetaInfoTypes();
	for (const int& infoType : metaInfoTypes){
		sdl::imtbase::ImtBaseTypes::CParameter::V1_0 parameterInfo;
		parameterInfo.id = metaInfo->GetMetaInfoId(infoType);
		parameterInfo.typeId = metaInfo->GetMetaInfoId(infoType);
		parameterInfo.name = metaInfo->GetMetaInfoName(infoType);
		parameterInfo.description = metaInfo->GetMetaInfoDescription(infoType);

		QVariant metaInfoValue = metaInfo->GetMetaInfo(infoType);
		parameterInfo.data = ConvertMetaInfoToString(infoType, metaInfoValue);

		parameterInfos << parameterInfo;
	}

	sdl::imtbase::ImtCollection::CElementMetaInfo::V1_0 elementMetaInfo;
	elementMetaInfo.infoParams.Emplace();
	elementMetaInfo.infoParams->FromList(parameterInfos);
	response.elementMetaInfo = elementMetaInfo;

	sdl::imtbase::ImtCollection::CGetElementMetaInfoPayload retVal;
	retVal.Version_1_0 = std::move(response);

	return retVal;
}


sdl::imtbase::ImtCollection::CCreateSubCollectionPayload CObjectCollectionControllerCompBase::OnCreateSubCollection(
			const sdl::imtbase::ImtCollection::CCreateSubCollectionGqlRequest& createSubCollectionRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::imtbase::ImtCollection::CCreateSubCollectionPayload::V1_0 response;

	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CObjectCollectionControllerCompBase");
		return sdl::imtbase::ImtCollection::CCreateSubCollectionPayload();
	}

	sdl::imtbase::ImtCollection::CreateSubCollectionRequestInfo requestInfo = createSubCollectionRequest.GetRequestInfo();

	sdl::imtbase::ImtCollection::CreateSubCollectionRequestArguments arguments = createSubCollectionRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		Q_ASSERT(false);
		return sdl::imtbase::ImtCollection::CCreateSubCollectionPayload();
	}

	int offset = 0;
	if (arguments.input.Version_1_0->offset){
		offset = *arguments.input.Version_1_0->offset;
	}

	int count = -1;
	if (arguments.input.Version_1_0->count){
		count = *arguments.input.Version_1_0->count;
	}

	iprm::CParamsSet filterParams;
	if (arguments.input.Version_1_0->selectionParams){
		if (filterParams.CopyFrom(m_selectionParams)){
			sdl::imtbase::ImtBaseTypes::CParamsSet::V1_0 paramsSet = *arguments.input.Version_1_0->selectionParams;
			if (!GetParamsSetFromRepresentation(paramsSet, filterParams)){
				errorMessage = QString("Unable to create sub collection '%1'. Error: Selection Params parsing failed").arg(QString::fromUtf8(*m_collectionIdAttrPtr));
				return sdl::imtbase::ImtCollection::CCreateSubCollectionPayload();
			}
		}
	}

	if (requestInfo.isItemsRequested){
		imtsdl::TElementList<sdl::imtbase::ImtCollection::CSubCollectionItem::V1_0> collectionItems;
		imtbase::ICollectionInfo::Ids ids = m_objectCollectionCompPtr->GetElementIds(offset, count, &filterParams);
		for (const imtbase::ICollectionInfo::Id& id: ids){
			sdl::imtbase::ImtCollection::CSubCollectionItem::V1_0 collectionItem;

			if (requestInfo.items.isItemInfoRequested){
				sdl::imtbase::ImtBaseTypes::CParameter::V1_0 parameterInfo;

				if (requestInfo.items.itemInfo.isIdRequested){
					parameterInfo.id = id;
				}

				if (requestInfo.items.itemInfo.isTypeIdRequested){
					parameterInfo.typeId = m_objectCollectionCompPtr->GetObjectTypeId(id);
				}

				if (requestInfo.items.itemInfo.isNameRequested){
					parameterInfo.name = m_objectCollectionCompPtr->GetElementInfo(id, imtbase::ICollectionInfo::EIT_NAME).toString();
				}

				if (requestInfo.items.itemInfo.isEnabledRequested){
					parameterInfo.enabled = m_objectCollectionCompPtr->GetElementInfo(id, imtbase::ICollectionInfo::EIT_ENABLED).toBool();
				}

				if (requestInfo.items.itemInfo.isDescriptionRequested){
					parameterInfo.description = m_objectCollectionCompPtr->GetElementInfo(id, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();
				}

				collectionItem.itemInfo = parameterInfo;
			}

			if (requestInfo.items.isMetaInfoRequested){
				QByteArray elementMetaInfoData;
				idoc::MetaInfoPtr metaInfo = m_objectCollectionCompPtr->GetElementMetaInfo(id);
				if (SerializeObject(*metaInfo.GetPtr(), elementMetaInfoData)){
					collectionItem.metaInfo = elementMetaInfoData;
				}
			}

			if (requestInfo.items.isDataMetaInfoRequested){
				QByteArray dataMetaInfo;
				idoc::MetaInfoPtr dataMetaInfoPtr = m_objectCollectionCompPtr->GetDataMetaInfo(id);
				if (SerializeObject(*dataMetaInfoPtr.GetPtr(), dataMetaInfo)){
					collectionItem.dataMetaInfo = dataMetaInfo;
				}
			}

			collectionItems << collectionItem;
		}

		response.items = collectionItems;
	}

	sdl::imtbase::ImtCollection::CCreateSubCollectionPayload retVal;
	retVal.Version_1_0 = std::move(response);

	return retVal;
}


// reimplemented (imtservergql::CGqlRepresentationDataControllerComp)

imtbase::CTreeItemModel* CObjectCollectionControllerCompBase::CreateInternalResponse(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	imtgql::CGqlParamObject gqlObject;
	int operationType = OT_UNKNOWN;
	if (!GetOperationFromRequest(gqlRequest, gqlObject, errorMessage, operationType)){
		return nullptr;
	}

	switch (operationType){
	case OT_NEW:
		return InsertObject(gqlRequest, errorMessage);
	case OT_GET:
		return GetObject(gqlRequest, errorMessage);
	case OT_UPDATE:
		return UpdateObject(gqlRequest, errorMessage);
	case OT_LIST:
		return ListObjects(gqlRequest, errorMessage);
	}

	return BaseClass::CreateInternalResponse(gqlRequest, errorMessage);
}


bool CObjectCollectionControllerCompBase::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	bool isSupported = BaseClass::IsRequestSupported(gqlRequest);
	if (isSupported){
		const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
		if (inputParamPtr == nullptr){
			return false;
		}

		if (m_collectionIdAttrPtr.IsValid() && *m_collectionIdAttrPtr != ""){
			QByteArray collectionId = inputParamPtr->GetParamArgumentValue("collectionId").toByteArray();
			return *m_collectionIdAttrPtr == collectionId;
		}
	}

	return false;
}


// reimplemented (imtgql::IGqlRequestExtractor)

istd::IChangeableUniquePtr CObjectCollectionControllerCompBase::ExtractObject(const imtgql::CGqlRequest& gqlRequest, QByteArray& newObjectId, QString& errorMessage) const
{
	return CreateObjectFromRequest(gqlRequest, newObjectId, errorMessage);
}


// protected methods

void CObjectCollectionControllerCompBase::ReplaceComplexFilterFields(imtbase::IComplexCollectionFilter& filter) const
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

	UpdateFieldsInfoFromHeaders(filter);
}


void CObjectCollectionControllerCompBase::UpdateFieldsInfoFromHeaders(imtbase::IComplexCollectionFilter& filter) const
{
	if (!m_headersProviderCompPtr.IsValid()){
		return;
	}

	imtcol::ICollectionHeadersProvider::HeaderIds headerIds = m_headersProviderCompPtr->GetHeaderIds();
	for (const QByteArray& headerId : headerIds){
		imtcol::ICollectionHeadersProvider::HeaderInfo headerInfo;
		if (m_headersProviderCompPtr->GetHeaderInfo(headerId, headerInfo)){
			imtbase::IComplexCollectionFilter::FieldInfo* fieldInfoPtr = filter.GetEditableFieldInfo(headerId);
			if (fieldInfoPtr != nullptr){
				if (headerInfo.filterable){
					if (!(fieldInfoPtr->metaInfo.flags & imtbase::IComplexCollectionFilter::SO_TEXT_FILTER)){
						fieldInfoPtr->metaInfo.flags &= imtbase::IComplexCollectionFilter::SO_TEXT_FILTER;
					}
				}

				if (headerInfo.sortable){
					if (!(fieldInfoPtr->metaInfo.flags & imtbase::IComplexCollectionFilter::SO_SORT)){
						fieldInfoPtr->metaInfo.flags &= imtbase::IComplexCollectionFilter::SO_SORT;
					}
				}

				if (headerInfo.fieldType == imtcol::ICollectionHeadersProvider::FT_ARRAY){
					fieldInfoPtr->metaInfo.type = imtbase::IComplexCollectionFilter::FT_ARRAY;
				}
			}
		}
	}
}


bool CObjectCollectionControllerCompBase::GetOperationFromRequest(
			const imtgql::CGqlRequest& gqlRequest,
			imtgql::CGqlParamObject& /*gqlObject*/,
			QString& /*errorMessage*/,
			int& operationType) const
{
	const imtgql::CGqlFieldObject& fields = gqlRequest.GetFields();

	const QByteArrayList ids = fields.GetFieldIds();
	for (const QByteArray& fieldId: ids){
		if (fieldId == "items"){
			operationType = OT_LIST;
			break;
		}
		if (fieldId == "item"){
			operationType = OT_GET;
			break;
		}
		if (fieldId == "addedNotification"){
			operationType = OT_NEW;
			break;
		}
		if (fieldId == "updatedNotification"){
			operationType = OT_UPDATE;
			break;
		}
	}

	return true;
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
		errorMessage = QString("Unable to get document. Error: Document does not exists");
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

	CreateUserActionLog(objectId, typeId, "Create", gqlRequest);

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

	if (!inputParamPtr->ContainsParam("id")){
		errorMessage = "Bad request. Unable to update object. Parameter 'id' missing.";

		return nullptr;
	}

	QByteArray objectId = inputParamPtr->GetParamArgumentValue("id").toByteArray();
	QString name = GetObjectNameFromRequest(gqlRequest);
	QString description = inputParamPtr->GetParamArgumentValue("description").toString();

	imtbase::IObjectCollection::DataPtr savedObjectPtr;
	if (!m_objectCollectionCompPtr->GetObjectData(objectId, savedObjectPtr)){
		errorMessage = QString("Unable to find object with id '%1'").arg(qPrintable(objectId));

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

	if (name.length() > 0){
		QString currentName = m_objectCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString();
		if (currentName != name){
			m_objectCollectionCompPtr->SetElementName(objectId, name);
		}
	}

	QString currentDescription = m_objectCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();
	if (currentDescription != description){
		m_objectCollectionCompPtr->SetElementDescription(objectId, description);
	}

	QByteArray typeId = m_objectCollectionCompPtr->GetObjectTypeId(objectId);
	CreateUserActionLog(objectId, typeId, "Update", gqlRequest);

	sdl::imtbase::ImtCollection::CUpdatedNotificationPayload::V1_0 response;
	response.id = objectId;

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

	if (!response.WriteToModel(*dataModelPtr, 0)){
		errorMessage = QString("Unable to update object '%1'. Error: Unable to write notification data to the model").arg(qPrintable(objectId));
		return nullptr;
	}

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
	else if (inputParamsPtr->ContainsParam("selectionParams")){
		const imtgql::CGqlParamObject* selectionParamsPtr = inputParamsPtr->GetParamArgumentObjectPtr("selectionParams");
		if (selectionParamsPtr != nullptr){
			offset = inputParamsPtr->GetParamArgumentValue("offset").toInt();
			count = inputParamsPtr->GetParamArgumentValue("count").toInt();
			PrepareFilters(gqlRequest, *selectionParamsPtr, filterParams);
		}
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

	const imtgql::CGqlParamObject* inputParamsPtr = gqlRequest.GetParamObject("input");
	if (inputParamsPtr == nullptr){
		errorMessage = QString("Unable to rename object. GraphQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return nullptr;
	}

	iprm::CParamsSet filterParams;
	const imtgql::CGqlParamObject* viewParamsPtr = inputParamsPtr->GetParamArgumentObjectPtr("viewParams");
	if (viewParamsPtr != nullptr){
		PrepareFilters(gqlRequest, *viewParamsPtr, filterParams);
	}
	else if (inputParamsPtr->ContainsParam("selectionParams")){
		const imtgql::CGqlParamObject* selectionParamsPtr = inputParamsPtr->GetParamArgumentObjectPtr("selectionParams");
		if (selectionParamsPtr != nullptr){
			PrepareFilters(gqlRequest, *selectionParamsPtr, filterParams);
		}
	}

	int elementsCount = m_objectCollectionCompPtr->GetElementsCount(&filterParams);

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");
	dataModelPtr->SetData("itemsCount", elementsCount);

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
	response.elementIds.Emplace();
	response.elementIds->FromList(objectIds);

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
		retVal = removeElementsInput.elementIds->ToList();
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
	const imtgql::CGqlFieldObject& fields = gqlRequest.GetFields();
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
			const imtgql::CGqlParamObject& inputParamsGql,
			iprm::CParamsSet& filterParams) const
{
	this->SetAdditionalFilters(gqlRequest, inputParamsGql, &filterParams);

	const imtgql::CGqlParamObject* complexFilterModelPtr = inputParamsGql.GetParamArgumentObjectPtr("filterModel");
	const imtgql::CGqlParamObject* documentFilterModelPtr = inputParamsGql.GetParamArgumentObjectPtr("documentFilterModel");

	sdl::imtbase::ImtBaseTypes::CParamsSet::V1_0 paramsSet;
	if (inputParamsGql.ContainsParam("parameters") && paramsSet.ReadFromGraphQlObject(inputParamsGql)){
		for (sdl::imtbase::ImtBaseTypes::CParameter::V1_0 parameter: paramsSet.parameters->ToList()){
			if (parameter.id){
				QByteArray parameterId = *parameter.id;
				const iser::ISerializable* parameterPtr = m_selectionParams.GetParameter(parameterId);
				if (parameterPtr != nullptr){
					iser::ISerializableUniquePtr filterParameterPtr;
					filterParameterPtr.MoveCastedPtr(parameterPtr->CloneMe());
					if (filterParameterPtr.IsValid()){
						if (parameterId == m_complexCollectionFilterRepresentationController.GetTypeId()){
							imtbase::CComplexCollectionFilter* complexFilterPtr = dynamic_cast<imtbase::CComplexCollectionFilter*>(filterParameterPtr.GetPtr());
							if (complexFilterPtr != nullptr){
								ReplaceComplexFilterFields(*complexFilterPtr);
								SetAdditionalFilters(gqlRequest, *complexFilterPtr);
							}
						}

						filterParams.SetEditableParameter(parameterId, filterParameterPtr);
					}
				}
			}
		}
		if (!GetParamsSetFromRepresentation(paramsSet, filterParams)){
			SendErrorMessage(0, QString("Unable to read filter params set for collection '%1'. Error: Selection Params parsing failed").arg(QString::fromUtf8(*m_collectionIdAttrPtr)));
		}
	}

	if (complexFilterModelPtr != nullptr){
		sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilter::V1_0 complexFilterSdl;
		bool isComplexFilterOk = complexFilterSdl.ReadFromGraphQlObject(*complexFilterModelPtr);
		if (isComplexFilterOk){
			istd::TDelPtr<imtbase::CComplexCollectionFilter> complexFilterPtr = new imtbase::CComplexCollectionFilter();
			if (m_complexCollectionFilterRepresentationController.GetDataModelFromSdlRepresentation(*complexFilterPtr, complexFilterSdl)){
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


bool CObjectCollectionControllerCompBase::CreateCollectionFilterFromViewParamsSdl(
			const sdl::imtbase::ImtCollection::CCollectionViewParams::V1_0& viewParams,
			int& offset,
			int& count,
			iprm::CParamsSet& filterParams) const
{
	offset = 0;
	if (viewParams.offset){
		offset = *viewParams.offset;
	}

	count = -1;
	if (viewParams.count){
		count = *viewParams.count;
	}

	if (viewParams.filterModel){
		sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilter::V1_0 sdlFilter = *viewParams.filterModel;
		if (!CreateCollectionFilterFromSdl(sdlFilter, filterParams)){
			return false;
		}
	}

	if (viewParams.documentFilterModel){
		sdl::imtbase::DocumentCollectionFilter::CDocumentCollectionFilter::V1_0 documentFilterModel = *viewParams.documentFilterModel;
		if (!CreateDocumentFilterFromSdl(documentFilterModel, filterParams)){
			return false;
		}
	}

	return true;
}


bool CObjectCollectionControllerCompBase::CreateCollectionFilterFromSdl(
			sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilter::V1_0& collectionFilter,
			iprm::CParamsSet& filterParams) const
{
	istd::TDelPtr<imtbase::CComplexCollectionFilter> complexFilterPtr = new imtbase::CComplexCollectionFilter();
	if (m_complexCollectionFilterRepresentationController.GetDataModelFromSdlRepresentation(*complexFilterPtr, collectionFilter)){
		ReplaceComplexFilterFields(*complexFilterPtr);
		filterParams.SetEditableParameter("ComplexFilter", complexFilterPtr.PopPtr(), true);

		return true;
	}

	return false;
}


bool CObjectCollectionControllerCompBase::CreateDocumentFilterFromSdl(
			sdl::imtbase::DocumentCollectionFilter::CDocumentCollectionFilter::V1_0& documentFilter,
			iprm::CParamsSet& filterParams) const
{
	istd::TDelPtr<imtcol::CDocumentCollectionFilter> documentFilterPtr = new imtcol::CDocumentCollectionFilter();
	if (imtcol::CDocumentCollectionFilterRepresentationController::DocumentCollectionFilterRepresentationToModel(
					documentFilter,
					*documentFilterPtr,
					GetLogPtr())){
		filterParams.SetEditableParameter("DocumentFilter", documentFilterPtr.PopPtr(), true);

		return true;
	}

	return  false;
}


QString CObjectCollectionControllerCompBase::ConvertMetaInfoToString(int /*infoType*/, const QVariant& metaInfoValue) const
{
	return metaInfoValue.toString();
}


istd::IChangeableUniquePtr CObjectCollectionControllerCompBase::CreateObject(const QByteArray& typeId) const
{
	int index = GetObjectTypeIdIndex(typeId);
	if (m_objectFactCompPtr.IsValid() && index < m_objectFactCompPtr.GetCount()){
		return m_objectFactCompPtr.CreateInstance(index);
	}

	return nullptr;
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


bool CObjectCollectionControllerCompBase::GetParamsSetFromRepresentation(sdl::imtbase::ImtBaseTypes::CParamsSet::V1_0& representation, iprm::IParamsSet& paramsSet) const
{
	QJsonObject jsonObject;
	if (!representation.WriteToJsonObject(jsonObject)){
		return false;
	}

	if (!m_paramSetRepresentationController.GetDataModelFromRepresentation(jsonObject, paramsSet)){
		return false;
	}

	if (paramsSet.GetParamIds().contains("ComplexFilter")){
		imtbase::IComplexCollectionFilter* complexFilterParamPtr = dynamic_cast<imtbase::IComplexCollectionFilter*>(paramsSet.GetEditableParameter("ComplexFilter"));
		if (complexFilterParamPtr != nullptr){
			ReplaceComplexFilterFields(*complexFilterParamPtr);
		}
	}

	return true;
}


bool CObjectCollectionControllerCompBase::RegisterFilterToSelectionParams(
			iser::ISerializable& filterParam,
			const imtserverapp::IJsonRepresentationController& representationController)
{
	imtserverapp::IJsonRepresentationController::RepresentationInfo info = representationController.GetRepresentationInfo();
	if (!m_selectionParams.SetEditableParameter(info.modelId, &filterParam)){
		return false;
	}

	return m_paramSetRepresentationController.RegisterSubController(representationController);
}


bool CObjectCollectionControllerCompBase::OnBeforeRemoveElements(
			const QByteArrayList& /*elementIds*/,
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	return true;
}


void CObjectCollectionControllerCompBase::OnAfterRemoveElements(
			const QByteArrayList& /*elementIds*/,
			const imtgql::CGqlRequest& /*gqlRequest*/) const
{
}


bool CObjectCollectionControllerCompBase::OnBeforeSetObjectName(
			const QByteArray& /*objectId*/,
			QString& /*newName*/,
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	return true;
}


void CObjectCollectionControllerCompBase::OnAfterSetObjectName(
			const QByteArray& /*objectId*/,
			const QString& /* oldName */,
			const QString& /*newName*/,
			const imtgql::CGqlRequest& /*gqlRequest*/) const
{
}


bool CObjectCollectionControllerCompBase::OnBeforeSetObjectDescription(
			const QByteArray& /*objectId*/,
			QString& /*description*/,
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	return true;
}


void CObjectCollectionControllerCompBase::OnAfterSetObjectDescription(
			const QByteArray& /*objectId*/,
			const QString& /*description*/,
			const imtgql::CGqlRequest& /*gqlRequest*/) const
{
}


QByteArray CObjectCollectionControllerCompBase::ExtractObjectIdFromGetObjectTypeIdGqlRequest(const imtgql::CGqlRequest& gqlRequest) const
{
	sdl::imtbase::ImtCollection::CGetObjectTypeIdGqlRequest getObjectTypeIdGqlRequest(gqlRequest, false);
	if (getObjectTypeIdGqlRequest.IsValid()){
		auto arguments = getObjectTypeIdGqlRequest.GetRequestedArguments();
		if (arguments.input.Version_1_0.HasValue()){
			if (arguments.input.Version_1_0->objectId.HasValue()){
				return *arguments.input.Version_1_0->objectId;
			}
		}
	}

	return QByteArray();
}


QByteArray CObjectCollectionControllerCompBase::ExtractObjectIdFromGetObjectDataGqlRequest(const imtgql::CGqlRequest& gqlRequest) const
{
	sdl::imtbase::ImtCollection::CGetObjectDataGqlRequest getObjectDataGqlRequest(gqlRequest, false);
	if (getObjectDataGqlRequest.IsValid()){
		auto arguments = getObjectDataGqlRequest.GetRequestedArguments();
		if (arguments.input.Version_1_0.HasValue()){
			if (arguments.input.Version_1_0->objectId.HasValue()){
				return *arguments.input.Version_1_0->objectId;
			}
		}
	}

	return QByteArray();
}

bool CObjectCollectionControllerCompBase::SerializeObject(
			istd::IChangeable& object,
			QByteArray& objectData) const
{
	objectData.clear();

	iser::ISerializable* serializableObject = dynamic_cast<iser::ISerializable*>(&object);
	if (serializableObject == nullptr){
		return false;
	}

	istd::TDelPtr<iser::CJsonMemWriteArchive> archivePtr;
	if (m_versionInfoCompPtr.IsValid()){
		archivePtr.SetPtr(new iser::CJsonMemWriteArchive(m_versionInfoCompPtr.GetPtr()));
	}
	else{
		archivePtr.SetPtr(new iser::CJsonMemWriteArchive());
	}

	if (!serializableObject->Serialize(*archivePtr.GetPtr())){
		return false;
	}

	objectData = archivePtr->GetData();

	return true;
}


bool CObjectCollectionControllerCompBase::DeSerializeObject(
			istd::IChangeable& object,
			const QByteArray& objectData) const
{
	iser::ISerializable* serializableObject = dynamic_cast<iser::ISerializable*>(&object);
	if (serializableObject == nullptr){
		return false;
	}

	iser::CJsonMemReadArchive archive(objectData);
	return serializableObject->Serialize(archive);
}


bool CObjectCollectionControllerCompBase::CreateUserActionLog(
			const QByteArray& objectId,
			const QByteArray& objectTypeId,
			const QByteArray& actionTypeId,
			const imtgql::CGqlRequest& gqlRequest) const
{
	if (!m_userActionManagerCompPtr.IsValid()){
		return false;
	}

	if (!m_objectCollectionCompPtr.IsValid()){
		return false;
	}

	imtauth::IUserRecentAction::TargetInfo targetInfo;
	targetInfo.id = objectId;
	if (objectTypeId.isEmpty()){
		targetInfo.typeId = m_objectCollectionCompPtr->GetObjectTypeId(objectId);
	}
	else{
		targetInfo.typeId = objectTypeId;
	}

	const iprm::IOptionsList* optionsListPtr = m_objectCollectionCompPtr->GetObjectTypesInfo();
	if (optionsListPtr != nullptr){
		int optionsCount = optionsListPtr->GetOptionsCount();
		for (int i = 0; i < optionsCount; ++i){
			QByteArray optionId = optionsListPtr->GetOptionId(i);
			if (optionId == targetInfo.typeId){
				targetInfo.typeName = optionsListPtr->GetOptionName(i);
				break;
			}
		}
	}

	if (actionTypeId == "Delete"){
		targetInfo.name = "";
	}
	else{
		if (targetInfo.name.isEmpty()){
			targetInfo.name = m_objectCollectionCompPtr->GetElementInfo(targetInfo.id, imtbase::ICollectionInfo::ElementInfoType::EIT_NAME).toString();
		}

		if (targetInfo.name.isEmpty()){
			targetInfo.name = targetInfo.id;
		}
	}

	targetInfo.source = *m_collectionIdAttrPtr;

	imtauth::IUserRecentAction::ActionTypeInfo actionTypeInfo;
	actionTypeInfo.id = actionTypeId;
	if (actionTypeId == "Create"){
		actionTypeInfo.name = QT_TR_NOOP(QStringLiteral("Create"));
		actionTypeInfo.description = QT_TR_NOOP(QStringLiteral("Object created"));
	}
	else if (actionTypeId == "Update"){
		actionTypeInfo.name = QT_TR_NOOP(QStringLiteral("Update"));
		actionTypeInfo.description = QT_TR_NOOP(QStringLiteral("Object changed"));
	}
	else if (actionTypeId == "Delete"){
		actionTypeInfo.name = QT_TR_NOOP(QStringLiteral("Delete"));
		actionTypeInfo.description = QT_TR_NOOP(QStringLiteral("Objects was deleted"));
	}

	imtauth::IUserRecentAction::UserInfo userInfo;
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr != nullptr){
		const imtauth::CIdentifiableUserInfo* userInfoPtr = dynamic_cast<const imtauth::CIdentifiableUserInfo*>(gqlContextPtr->GetUserInfo());
		if (userInfoPtr != nullptr){
			userInfo.id = userInfoPtr->GetObjectUuid();
			userInfo.name = userInfoPtr->GetName();
		}
	}

	return m_userActionManagerCompPtr->CreateUserAction(userInfo, actionTypeInfo, targetInfo);
}


} // namespace imtservergql


