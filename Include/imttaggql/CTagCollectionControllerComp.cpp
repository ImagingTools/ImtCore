// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imttaggql/CTagCollectionControllerComp.h>


// Qt includes
#include <QtCore/QRegularExpression>
#include <QtCore/QUuid>

// ACF includes
#include <iprm/CParamsSet.h>
#include <istd/TDelPtr.h>

// ImtCore includes
#include <imtauth/CTenantFilterParam.h>
#include <imtbase/COperationContext.h>
#include <imtbase/IObjectCollectionIterator.h>
#include <imtcol/CDocumentIdFilter.h>
#include <imttag/CTagNameLookup.h>
#include <imttaggql/imttaggql.h>
#include <GeneratedFiles/imttagsdl/SDL/1.0/CPP/Tags.h>


namespace imttaggql
{


static const QString s_defaultColor = QStringLiteral("ededed");


static QJsonObject CreateNotificationResponse(const QByteArray& objectId, bool isAdded)
{
	QJsonObject dataObj;
	bool isWritten = false;
	if (isAdded){
		sdl::V1_0::imtbase::CAddedNotificationPayload response;
		response.id = objectId;
		isWritten = response.WriteToJsonObject(dataObj);
	}
	else{
		sdl::V1_0::imtbase::CUpdatedNotificationPayload response;
		response.id = objectId;
		isWritten = response.WriteToJsonObject(dataObj);
	}

	if (!isWritten){
		return QJsonObject();
	}

	QJsonObject rootObj;
	rootObj.insert(QStringLiteral("data"), dataObj);

	return rootObj;
}


// protected methods

bool CTagCollectionControllerComp::FillTagFromRepresentation(
			const sdl::V1_0::imttag::CTagData& representation,
			imttag::ITag& tag,
			QString& errorMessage) const
{
	if (representation.name){
		tag.SetName(imttag::CTagNameLookup::NormalizeName(*representation.name));
	}

	if (tag.GetName().isEmpty()){
		errorMessage = QStringLiteral("Tag name must not be empty");

		return false;
	}

	if (representation.color){
		QString color = representation.color->trimmed().toLower();
		if (color.startsWith('#')){
			color.remove(0, 1);
		}

		static const QRegularExpression colorExpression(QStringLiteral("^[0-9a-f]{6}$"));
		if (!colorExpression.match(color).hasMatch()){
			errorMessage = QStringLiteral("Tag color '%1' is not a 6-digit hex color").arg(*representation.color);

			return false;
		}

		tag.SetColor(color);
	}

	if (tag.GetColor().isEmpty()){
		tag.SetColor(s_defaultColor);
	}

	if (representation.description){
		tag.SetDescription(*representation.description);
	}

	if (representation.isSystem){
		tag.SetSystem(*representation.isSystem);
	}

	return true;
}


bool CTagCollectionControllerComp::IsTagVisible(const QByteArray& tagId, const imtgql::CGqlRequest& gqlRequest) const
{
	if (gqlRequest.GetRequestContext() == nullptr){
		return true;
	}

	imtcol::CDocumentIdFilter* documentIdFilterPtr = new imtcol::CDocumentIdFilter;
	documentIdFilterPtr->SetDocumentIds(QByteArrayList() << tagId);
	documentIdFilterPtr->SetConditionType(imtcol::IDocumentIdFilter::CT_IN);

	imtauth::CTenantFilterParam* tenantFilterPtr = new imtauth::CTenantFilterParam;
	tenantFilterPtr->SetTenantId(GetRequestTenantId(gqlRequest));

	iprm::CParamsSet filterParams;
	filterParams.SetEditableParameter("DocumentIdFilter", documentIdFilterPtr, true);
	filterParams.SetEditableParameter("TenantFilter", tenantFilterPtr, true);

	return m_objectCollectionCompPtr->GetElementIds(0, -1, &filterParams).contains(tagId);
}


bool CTagCollectionControllerComp::IsNameAvailable(
			const QString& name,
			bool isSystem,
			const QByteArray& excludedTagId,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	// A system name must be free in every tenant, a tenant name among the system tags and its own.
	const QByteArray tenantId = GetRequestTenantId(gqlRequest);
	const QByteArrayList foundTagIds = imttag::CTagNameLookup::FindTagIds(
				*m_objectCollectionCompPtr,
				name,
				isSystem ? nullptr : &tenantId,
				excludedTagId);
	if (!foundTagIds.isEmpty()){
		errorMessage = QStringLiteral("A tag named '%1' already exists").arg(name);

		return false;
	}

	return true;
}


bool CTagCollectionControllerComp::CheckWriteAccess(const imttag::ITag& tag, const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (tag.IsSystem() && !IsSuperuserRequest(gqlRequest)){
		errorMessage = QStringLiteral("Only the superuser can change system tags");

		return false;
	}

	return true;
}


imtbase::IObjectCollection::DataPtr CTagCollectionControllerComp::GetTagData(const QByteArray& tagId) const
{
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_objectCollectionCompPtr->GetObjectData(tagId, dataPtr) || (dynamic_cast<const imttag::ITag*>(dataPtr.GetPtr()) == nullptr)){
		return imtbase::IObjectCollection::DataPtr();
	}

	return dataPtr;
}


// reimplemented (imtservergql::CObjectCollectionControllerCompBase)

QJsonObject CTagCollectionControllerComp::InsertObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QStringLiteral("Tag collection is not configured");

		return QJsonObject();
	}

	QByteArray tagId;
	istd::IChangeableUniquePtr objectPtr = CreateObjectFromRequest(gqlRequest, tagId, errorMessage);
	imttag::ITag* tagPtr = dynamic_cast<imttag::ITag*>(objectPtr.GetPtr());
	if (tagPtr == nullptr){
		if (errorMessage.isEmpty()){
			errorMessage = QStringLiteral("Tag could not be created from the request");
		}

		return QJsonObject();
	}

	if (tagId.isEmpty()){
		tagId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	}

	tagPtr->SetId(tagId);

	if (!CheckWriteAccess(*tagPtr, gqlRequest, errorMessage) || !IsNameAvailable(tagPtr->GetName(), tagPtr->IsSystem(), QByteArray(), gqlRequest, errorMessage)){
		return QJsonObject();
	}

	// A system tag belongs to no tenant, so it must not get the tenant binding of the request.
	istd::TDelPtr<imtbase::IOperationContext> operationContextPtr;
	if (tagPtr->IsSystem()){
		imtbase::COperationContext* systemContextPtr = new imtbase::COperationContext;
		InitOperationContext(gqlRequest, QStringLiteral("Create"), *systemContextPtr);
		systemContextPtr->SetTenantId(QByteArray());
		operationContextPtr.SetPtr(systemContextPtr);
	}
	else if (m_operationContextControllerCompPtr.IsValid()){
		operationContextPtr.SetPtr(m_operationContextControllerCompPtr->CreateOperationContext("Create", tagId, tagPtr));
	}

	const QByteArray newTagId = m_objectCollectionCompPtr->InsertNewObject(
				"Tag",
				tagPtr->GetName(),
				tagPtr->GetDescription(),
				tagPtr,
				tagId,
				nullptr,
				nullptr,
				operationContextPtr.GetPtr());
	if (newTagId.isEmpty()){
		errorMessage = QStringLiteral("Tag '%1' could not be created").arg(tagPtr->GetName());

		return QJsonObject();
	}

	return CreateNotificationResponse(newTagId, true);
}


QJsonObject CTagCollectionControllerComp::UpdateObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QStringLiteral("Tag collection is not configured");

		return QJsonObject();
	}

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	const QByteArray tagId = (inputParamPtr != nullptr) ? inputParamPtr->GetParamArgumentValue("id").toByteArray() : QByteArray();

	imtbase::IObjectCollection::DataPtr dataPtr = GetTagData(tagId);
	imttag::ITag* tagPtr = dynamic_cast<imttag::ITag*>(dataPtr.GetPtr());
	if ((tagPtr == nullptr) || !IsTagVisible(tagId, gqlRequest)){
		errorMessage = QStringLiteral("Tag '%1' does not exist").arg(QString::fromUtf8(tagId));

		return QJsonObject();
	}

	if (!CheckWriteAccess(*tagPtr, gqlRequest, errorMessage)){
		return QJsonObject();
	}

	const bool wasSystem = tagPtr->IsSystem();
	const QString previousName = tagPtr->GetName();

	if (!UpdateObjectFromRequest(gqlRequest, *tagPtr, errorMessage)){
		return QJsonObject();
	}

	tagPtr->SetId(tagId);

	if (tagPtr->IsSystem() != wasSystem){
		errorMessage = QStringLiteral("The system flag of a tag cannot be changed");

		return QJsonObject();
	}

	if ((tagPtr->GetName() != previousName) && !IsNameAvailable(tagPtr->GetName(), wasSystem, tagId, gqlRequest, errorMessage)){
		return QJsonObject();
	}

	istd::TDelPtr<imtbase::IOperationContext> operationContextPtr;
	if (m_operationContextControllerCompPtr.IsValid()){
		operationContextPtr.SetPtr(m_operationContextControllerCompPtr->CreateOperationContext("Update", tagId, tagPtr));
	}

	if (!m_objectCollectionCompPtr->SetObjectData(tagId, *tagPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr.GetPtr())){
		errorMessage = QStringLiteral("Tag '%1' could not be updated").arg(QString::fromUtf8(tagId));

		return QJsonObject();
	}

	m_objectCollectionCompPtr->SetElementName(tagId, tagPtr->GetName());
	m_objectCollectionCompPtr->SetElementDescription(tagId, tagPtr->GetDescription());

	return CreateNotificationResponse(tagId, false);
}


QJsonObject CTagCollectionControllerComp::GetObjectListFromRequest(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (m_systemTagSeederCompPtr.IsValid()){
		m_systemTagSeederCompPtr->EnsureSystemTags();
	}

	return BaseClass::GetObjectListFromRequest(gqlRequest, errorMessage);
}


bool CTagCollectionControllerComp::OnBeforeRemoveElements(const QByteArrayList& elementIds, const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	for (const QByteArray& tagId : elementIds){
		imtbase::IObjectCollection::DataPtr dataPtr = GetTagData(tagId);
		const imttag::ITag* tagPtr = dynamic_cast<const imttag::ITag*>(dataPtr.GetPtr());
		if ((tagPtr == nullptr) || !IsTagVisible(tagId, gqlRequest)){
			errorMessage = QStringLiteral("Tag '%1' does not exist").arg(QString::fromUtf8(tagId));

			return false;
		}

		if (!CheckWriteAccess(*tagPtr, gqlRequest, errorMessage)){
			return false;
		}
	}

	return BaseClass::OnBeforeRemoveElements(elementIds, gqlRequest, errorMessage);
}


bool CTagCollectionControllerComp::OnBeforeSetObjectName(
			const QByteArray& /*objectId*/,
			QString& /*newName*/,
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	errorMessage = QStringLiteral("Tags are renamed with TagUpdate");

	return false;
}


sdl::V1_0::imtbase::CRestoreObjectsPayload CTagCollectionControllerComp::OnRestoreObjects(
			const sdl::V1_0::imtbase::CRestoreObjectsGqlRequest& restoreObjectsRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtbase::CRestoreObjectsPayload response = BaseClass::OnRestoreObjects(restoreObjectsRequest, gqlRequest, errorMessage);
	if (!response.success || !*response.success){
		return response;
	}

	const sdl::V1_0::imtbase::RestoreObjectsRequestArguments arguments = restoreObjectsRequest.GetRequestedArguments();
	const QByteArrayList tagIds = (arguments.input && arguments.input->objectIds) ? arguments.input->objectIds->ToList() : QByteArrayList();

	// A deleted tag is only readable after the restore, so the checks run afterwards and revert on failure.
	for (const QByteArray& tagId : tagIds){
		imtbase::IObjectCollection::DataPtr dataPtr = GetTagData(tagId);
		const imttag::ITag* tagPtr = dynamic_cast<const imttag::ITag*>(dataPtr.GetPtr());

		QString checkError;
		bool isAllowed = (tagPtr != nullptr) && IsTagVisible(tagId, gqlRequest);
		isAllowed = isAllowed && CheckWriteAccess(*tagPtr, gqlRequest, checkError);
		isAllowed = isAllowed && IsNameAvailable(tagPtr->GetName(), tagPtr->IsSystem(), tagId, gqlRequest, checkError);
		if (!isAllowed){
			m_objectCollectionCompPtr->RemoveElements(imtbase::ICollectionInfo::Ids() << tagId);

			errorMessage = checkError.isEmpty() ? QStringLiteral("Tag '%1' cannot be restored").arg(QString::fromUtf8(tagId)) : checkError;
			response.success = false;

			return response;
		}
	}

	return response;
}


sdl::V1_0::imtbase::CRemoveElementSetPayload CTagCollectionControllerComp::OnRemoveElementSet(
			const sdl::V1_0::imtbase::CRemoveElementSetGqlRequest& /*removeElementSetRequest*/,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	errorMessage = QStringLiteral("Tags are removed by ID");

	return sdl::V1_0::imtbase::CRemoveElementSetPayload();
}


sdl::V1_0::imtbase::CRestoreObjectSetPayload CTagCollectionControllerComp::OnRestoreObjectSet(
			const sdl::V1_0::imtbase::CRestoreObjectSetGqlRequest& /*restoreObjectSetRequest*/,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	errorMessage = QStringLiteral("Tags are restored by ID");

	return sdl::V1_0::imtbase::CRestoreObjectSetPayload();
}


// reimplemented (sdl::V1_0::imttag::CTagCollectionControllerCompBase)

bool CTagCollectionControllerComp::CreateRepresentationFromObject(
			const imtbase::IObjectCollectionIterator& objectCollectionIterator,
			const sdl::V1_0::imttag::CTagsListGqlRequest& tagsListRequest,
			sdl::V1_0::imttag::CTagItemData& representationObject,
			QString& errorMessage) const
{
	const QByteArray tagId = objectCollectionIterator.GetObjectId();

	imtbase::IObjectCollection::DataPtr dataPtr;
	const imttag::ITag* tagPtr = nullptr;
	if (objectCollectionIterator.GetObjectData(dataPtr)){
		tagPtr = dynamic_cast<const imttag::ITag*>(dataPtr.GetPtr());
	}

	if (tagPtr == nullptr){
		errorMessage = QStringLiteral("Unable to create representation of tag '%1'").arg(QString::fromUtf8(tagId));

		return false;
	}

	const sdl::V1_0::imttag::TagsListRequestInfo requestInfo = tagsListRequest.GetRequestInfo();

	if (requestInfo.items.isIdRequested){
		representationObject.id = tagId;
	}

	if (requestInfo.items.isTypeIdRequested){
		representationObject.typeId = objectCollectionIterator.GetObjectTypeId();
	}

	if (requestInfo.items.isNameRequested){
		representationObject.name = tagPtr->GetName();
	}

	if (requestInfo.items.isColorRequested){
		representationObject.color = tagPtr->GetColor();
	}

	if (requestInfo.items.isDescriptionRequested){
		representationObject.description = tagPtr->GetDescription();
	}

	if (requestInfo.items.isIsSystemRequested){
		representationObject.isSystem = tagPtr->IsSystem();
	}

	if (requestInfo.items.isUsageCountRequested && m_assignmentManagerCompPtr.IsValid()){
		representationObject.usageCount = m_assignmentManagerCompPtr->GetUsageCounts(QByteArrayList() << tagId).value(tagId);
	}

	if (requestInfo.items.isAddedRequested){
		const QDateTime added = objectCollectionIterator.GetElementInfo("Added").toDateTime().toUTC();
		representationObject.added = added.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
	}

	if (requestInfo.items.isLastModifiedRequested){
		const QDateTime lastModified = objectCollectionIterator.GetElementInfo("LastModified").toDateTime().toUTC();
		representationObject.lastModified = lastModified.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
	}

	return true;
}


bool CTagCollectionControllerComp::CreateRepresentationFromObject(
			const istd::IChangeable& data,
			const sdl::V1_0::imttag::CTagItemGqlRequest& /*tagItemRequest*/,
			sdl::V1_0::imttag::CTagData& representationPayload,
			QString& errorMessage) const
{
	const imttag::ITag* tagPtr = dynamic_cast<const imttag::ITag*>(&data);
	if (tagPtr == nullptr){
		errorMessage = QStringLiteral("Unable to create tag representation. Error: Object is invalid");

		return false;
	}

	representationPayload.id = tagPtr->GetId();
	representationPayload.name = tagPtr->GetName();
	representationPayload.color = tagPtr->GetColor();
	representationPayload.description = tagPtr->GetDescription();
	representationPayload.isSystem = tagPtr->IsSystem();

	return true;
}


istd::IChangeableUniquePtr CTagCollectionControllerComp::CreateObjectFromRepresentation(
			const sdl::V1_0::imttag::CTagData& tagDataRepresentation,
			QByteArray& newObjectId,
			QString& errorMessage) const
{
	if (!m_tagFactCompPtr.IsValid()){
		errorMessage = QStringLiteral("Tag factory is not configured");

		return nullptr;
	}

	imttag::ITagUniquePtr tagPtr = m_tagFactCompPtr.CreateInstance();
	if (!tagPtr.IsValid()){
		errorMessage = QStringLiteral("Tag instance could not be created");

		return nullptr;
	}

	if (!FillTagFromRepresentation(tagDataRepresentation, *tagPtr, errorMessage)){
		return nullptr;
	}

	if (tagDataRepresentation.id){
		newObjectId = *tagDataRepresentation.id;
	}

	return tagPtr;
}


bool CTagCollectionControllerComp::UpdateObjectFromRepresentationRequest(
			const ::imtgql::CGqlRequest& /*rawGqlRequest*/,
			const sdl::V1_0::imttag::CTagUpdateGqlRequest& tagUpdateRequest,
			istd::IChangeable& object,
			QString& errorMessage) const
{
	const sdl::V1_0::imttag::TagUpdateRequestArguments arguments = tagUpdateRequest.GetRequestedArguments();
	if (!arguments.input || !arguments.input->item){
		errorMessage = QStringLiteral("Tag data is missing");

		return false;
	}

	imttag::ITag* tagPtr = dynamic_cast<imttag::ITag*>(&object);
	if (tagPtr == nullptr){
		errorMessage = QStringLiteral("Unable to update tag. Error: Object is invalid");

		return false;
	}

	return FillTagFromRepresentation(*arguments.input->item, *tagPtr, errorMessage);
}


} // namespace imttaggql


