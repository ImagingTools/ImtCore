// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CRoleCollectionControllerComp.h>

// ACF includes
#include <iqt/iqt.h>
#include <iprm/CTextParam.h>

// ImtCore includes
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/ImtCollection.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Roles.h>
#include <imtbase/CComplexCollectionFilterHelper.h>
#include <imtauth/CRole.h>


namespace imtauthgql
{


// protected methods

bool CRoleCollectionControllerComp::FillObjectFromRepresentation(
			const sdl::V1_0::imtauth::CRoleData& roleDataRepresentation,
			istd::IChangeable& object,
			QByteArray& objectId,
			QString& errorMessage) const
{
	imtauth::CIdentifiableRoleInfo* roleInfoPtr = dynamic_cast<imtauth::CIdentifiableRoleInfo*>(&object);
	if (roleInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object. Error: Object is invalid");
		SendErrorMessage(0, errorMessage, "CRoleCollectionControllerComp");

		return false;
	}

	QByteArray roleId;
	if (roleDataRepresentation.roleId){
		roleId = *roleDataRepresentation.roleId;
	}

	if (roleId.isEmpty()){
		errorMessage = QString("Role-ID can't be empty!");
		return false;
	}

	roleInfoPtr->SetRoleId(roleId);

	QByteArray productId;
	if (roleDataRepresentation.productId){
		productId = *roleDataRepresentation.productId;
	}

	imtbase::ICollectionInfo::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds();
	for (const imtbase::ICollectionInfo::Id& collectionId : collectionIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_objectCollectionCompPtr->GetObjectData(collectionId, dataPtr)){
			imtauth::IRole* currentRoleInfoPtr = dynamic_cast<imtauth::IRole*>(dataPtr.GetPtr());
			if (currentRoleInfoPtr != nullptr){
				if (collectionId != objectId){
					QByteArray currentRoleId = currentRoleInfoPtr->GetRoleId();
					QByteArray currentProductId = currentRoleInfoPtr->GetProductId();

					if (currentRoleId == roleId && currentProductId == productId){
						errorMessage = QString("Role with ID: '%1' already exists").arg(currentRoleId);
						return false;
					}
				}
			}
		}
	}

	roleInfoPtr->SetProductId(productId);

	QString roleName;
	if (roleDataRepresentation.name){
		roleName = *roleDataRepresentation.name;
	}
	roleInfoPtr->SetRoleName(roleName);

	QString roleDescription;
	if (roleDataRepresentation.description){
		roleDescription = *roleDataRepresentation.description;
	}
	roleInfoPtr->SetRoleDescription(roleDescription);

	QByteArrayList parentRoles;
	if (roleDataRepresentation.parentRoles){
		parentRoles = roleDataRepresentation.parentRoles->ToList();
	}
	if (!parentRoles.isEmpty()){
		parentRoles.removeAll("");

		for (const QByteArray& parentRoleId : parentRoles){
			if (parentRoleId == objectId || !roleInfoPtr->IncludeRole(parentRoleId)){
				errorMessage = QT_TR_NOOP(QString("Unable include role '%1' to the role '%2'. Check the dependencies between them.")
											.arg(parentRoleId, roleId));

				return false;
			}
		}
	}

	QByteArray permissions;
	if (roleDataRepresentation.permissions){
		permissions = *roleDataRepresentation.permissions;
	}
	QByteArrayList permissionIds = permissions.split(';');
	permissionIds.removeAll("");
	roleInfoPtr->SetLocalPermissions(permissionIds);

	bool isGuest = bool(roleDataRepresentation.isGuest && *roleDataRepresentation.isGuest);
	roleInfoPtr->SetGuest(isGuest);

	bool isDefault = bool(roleDataRepresentation.isDefault && *roleDataRepresentation.isDefault);
	roleInfoPtr->SetDefault(isDefault);

	if (isGuest || isDefault){
		if (m_gqlRequestProviderCompPtr.IsValid()){
			const imtgql::IGqlRequest* gqlRequestPtr = m_gqlRequestProviderCompPtr->GetGqlRequest();
			if (gqlRequestPtr != nullptr){
				const imtgql::IGqlContext* gqlContextPtr = gqlRequestPtr->GetRequestContext();
				if (gqlContextPtr != nullptr){
					if (!gqlContextPtr->GetUserInfo()->IsAdmin()){
						errorMessage = QString("Only the admin can change the default/guest role");
						return false;
					}
				}
			}
		}
	}

	return true;
}


// reimplemented (sdl::V1_0::imtbase::CImtCollectionGqlHandlerCompBase)

sdl::V1_0::imtbase::CVisualStatus CRoleCollectionControllerComp::OnGetObjectVisualStatus(
			const sdl::V1_0::imtbase::CGetObjectVisualStatusGqlRequest& getObjectVisualStatusRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtbase::CVisualStatus response = BaseClass::OnGetObjectVisualStatus(getObjectVisualStatusRequest, gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return sdl::V1_0::imtbase::CVisualStatus();
	}

	if (!response.text.has_value()){
		Q_ASSERT(false);
		return response;
	}

	if (response.text->isEmpty()){
		response.text = "<no name>";
	}

	QByteArray languageId;
	const imtgql::IGqlContext* gqlContextPtr = getObjectVisualStatusRequest.GetRequestContext();
	if (gqlContextPtr != nullptr){
		languageId = gqlContextPtr->GetLanguageId();
	}

	QString translation = iqt::GetTranslation(
				m_translationManagerCompPtr.GetPtr(),
				QString(QT_TR_NOOP("Roles")).toUtf8(),
				languageId,
				"CRoleCollectionControllerComp");
	response.text = translation + QByteArrayLiteral(" / ") + *response.text;
	return response;
}


sdl::V1_0::imtbase::CGetElementMetaInfoPayload CRoleCollectionControllerComp::OnGetElementMetaInfo(
			const sdl::V1_0::imtbase::CGetElementMetaInfoGqlRequest& getElementMetaInfoRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtbase::CGetElementMetaInfoPayload response;
	sdl::V1_0::imtbase::GetElementMetaInfoRequestArguments arguments = getElementMetaInfoRequest.GetRequestedArguments();
	QByteArray objectId;
	if (arguments.input->elementId){
		objectId = *arguments.input->elementId;
	}

	QByteArray productId = gqlRequest.GetHeader("productId");

	sdl::V1_0::imtbase::CElementMetaInfo elementMetaInfo;
	imtsdl::TElementList<sdl::V1_0::imtbase::CParameter> infoParams;

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		const imtauth::IRole* roleInfoPtr = dynamic_cast<const imtauth::IRole*>(dataPtr.GetPtr());
		if (roleInfoPtr == nullptr){
			errorMessage = QT_TR_NOOP("Unable to get a role info");
			return response;
		}

		sdl::V1_0::imtbase::CParameter parentRolesParameter;
		parentRolesParameter.id = QByteArrayLiteral("ParentRoles");
		parentRolesParameter.typeId = parentRolesParameter.id;
		parentRolesParameter.name = QStringLiteral("Parent Roles");

		QByteArrayList parentRolesIds = roleInfoPtr->GetIncludedRoles();

		if (parentRolesIds.isEmpty()){
			parentRolesParameter.data = QStringLiteral("No parent roles");
		}
		else{
			QString parentRolesData;
			for (const QByteArray& parentRoleId : parentRolesIds){
				imtbase::IObjectCollection::DataPtr parentDataPtr;
				if (m_objectCollectionCompPtr->GetObjectData(parentRoleId, parentDataPtr)){
					const imtauth::IRole* parentRoleInfoPtr = dynamic_cast<const imtauth::IRole*>(parentDataPtr.GetPtr());
					if (parentRoleInfoPtr != nullptr){
						QString parentRoleName = parentRoleInfoPtr->GetRoleName();
						parentRolesData += parentRoleName + "\n";
					}
				}
			}
			parentRolesParameter.data = parentRolesData;
		}

		infoParams << parentRolesParameter;

		sdl::V1_0::imtbase::CParameter permissionsParameter;
		permissionsParameter.id = QByteArrayLiteral("Permissions");
		permissionsParameter.typeId = parentRolesParameter.id;
		permissionsParameter.name = QStringLiteral("Permissions");

		imtauth::IRole::FeatureIds permissionsIds = roleInfoPtr->GetPermissions();
		if (permissionsIds.isEmpty()){
			permissionsParameter.data = QStringLiteral("No permissions");
		}
		else{
			QString permissionsData;
			for (const QByteArray& permissionId : permissionsIds){
				permissionsData += permissionId + "\n";
			}

			permissionsParameter.data = permissionsData;
		}

		infoParams << permissionsParameter;
	}

	elementMetaInfo.infoParams = infoParams;
	response.elementMetaInfo = elementMetaInfo;

	return response;
}


// reimplemented (sdl::V1_0::imtauth::CRoleCollectionControllerCompBase)

bool CRoleCollectionControllerComp::CreateRepresentationFromObject(
			const imtbase::IObjectCollectionIterator& objectCollectionIterator,
			const sdl::V1_0::imtauth::CRolesListGqlRequest& rolesListRequest,
			sdl::V1_0::imtauth::CRoleItemData& representationObject,
			QString& errorMessage) const
{
	QByteArray objectId = objectCollectionIterator.GetObjectId();

	const imtauth::IRole* roleInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr roleDataPtr;
	if (objectCollectionIterator.GetObjectData(roleDataPtr)){
		roleInfoPtr = dynamic_cast<const imtauth::IRole*>(roleDataPtr.GetPtr());
	}

	if (roleInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object '%1'").arg(objectId);
		SendErrorMessage(0, errorMessage, "CRoleCollectionControllerComp");

		return false;
	}

	sdl::V1_0::imtauth::RolesListRequestInfo requestInfo = rolesListRequest.GetRequestInfo();

	if (requestInfo.items.isIdRequested){
		representationObject.id = QByteArray(objectId);
	}

	if (requestInfo.items.isRoleNameRequested){
		representationObject.roleName = QString(roleInfoPtr->GetRoleName());
	}

	if (requestInfo.items.isRoleIdRequested){
		representationObject.roleId = QByteArray(roleInfoPtr->GetRoleId());
	}

	if (requestInfo.items.isRoleDescriptionRequested){
		representationObject.roleDescription = QString(roleInfoPtr->GetRoleDescription());
	}

	if (requestInfo.items.isParentRolesRequested){
		representationObject.parentRoles = QByteArray(roleInfoPtr->GetIncludedRoles().join(';'));
	}

	if (requestInfo.items.isProductIdRequested){
		representationObject.productId = QByteArray(roleInfoPtr->GetProductId());
	}

	if (requestInfo.items.isTypeIdRequested){
		representationObject.typeId = objectCollectionIterator.GetObjectTypeId();
	}

	if (requestInfo.items.isAddedRequested){
		QDateTime addedTime = objectCollectionIterator.GetElementInfo("Added").toDateTime().toUTC();

		QString added = addedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.added = QString(added);
	}

	if (requestInfo.items.isLastModifiedRequested){
		QDateTime lastModifiedTime = objectCollectionIterator.GetElementInfo("LastModified").toDateTime().toUTC();

		QString lastModified = lastModifiedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.lastModified = QString(lastModified);
	}

	return true;
}


istd::IChangeableUniquePtr CRoleCollectionControllerComp::CreateObjectFromRepresentation(
			const sdl::V1_0::imtauth::CRoleData& roleDataRepresentation,
			QByteArray& newObjectId,
			QString& errorMessage) const
{
	if (!m_roleInfoFactCompPtr.IsValid()){
		errorMessage = QString("Unable to create object from representation. Error: Attribute 'm_roleInfoFactCompPtr' was not set");
		SendErrorMessage(0, errorMessage, "CRoleCollectionControllerComp");

		return nullptr;
	}

	imtauth::IRoleUniquePtr roleInstancePtr = m_roleInfoFactCompPtr.CreateInstance();
	if (!roleInstancePtr.IsValid()){
		errorMessage = QString("Unable to create role instance. Error: Invalid object");
		SendErrorMessage(0, errorMessage, "CRoleCollectionControllerComp");

		return nullptr;
	}

	imtauth::CIdentifiableRoleInfo* roleInfoPtr = dynamic_cast<imtauth::CIdentifiableRoleInfo*>(roleInstancePtr.GetPtr());
	if (roleInfoPtr == nullptr){
		errorMessage = QString("Unable to cast role instance to identifable object. Error: Invalid object");
		SendErrorMessage(0, errorMessage, "CRoleCollectionControllerComp");

		return nullptr;
	}

	QByteArray id;
	if (roleDataRepresentation.id){
		id = *roleDataRepresentation.id;
	}
	if (id.isEmpty()){
		id = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}

	roleInfoPtr->SetObjectUuid(id);
	newObjectId = id;

	if (!FillObjectFromRepresentation(roleDataRepresentation, *roleInfoPtr, newObjectId, errorMessage)){
		return nullptr;
	}

	return roleInstancePtr;
}


bool CRoleCollectionControllerComp::CreateRepresentationFromObject(
			const istd::IChangeable& data,
			const sdl::V1_0::imtauth::CRoleItemGqlRequest& /* roleItemRequest */,
			sdl::V1_0::imtauth::CRoleData& representationPayload,
			QString& errorMessage) const
{
	const imtauth::CIdentifiableRoleInfo* roleInfoPtr = dynamic_cast<const imtauth::CIdentifiableRoleInfo*>(&data);
	if (roleInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object. Error: Object is invalid");
		SendErrorMessage(0, errorMessage, "CRoleCollectionControllerComp");

		return false;
	}

	QByteArray id = roleInfoPtr->GetObjectUuid();
	representationPayload.id = QByteArray(id);

	QByteArray roleId = roleInfoPtr->GetRoleId();
	representationPayload.roleId = QByteArray(roleId);

	QByteArray productId = roleInfoPtr->GetProductId();
	representationPayload.productId = QByteArray(productId);

	QString name = roleInfoPtr->GetRoleName();
	representationPayload.name = QString(name);

	QString description = roleInfoPtr->GetRoleDescription();
	representationPayload.description = QString(description);

	QByteArrayList parentsRolesIds = roleInfoPtr->GetIncludedRoles();
	std::sort(parentsRolesIds.begin(), parentsRolesIds.end());
	representationPayload.parentRoles.Emplace().FromList(parentsRolesIds);

	imtauth::IRole::FeatureIds permissions = roleInfoPtr->GetLocalPermissions();
	permissions.removeAll("");
	std::sort(permissions.begin(), permissions.end());
	representationPayload.permissions = QByteArray(permissions.join(';'));

	representationPayload.isDefault = bool(roleInfoPtr->IsDefault());
	representationPayload.isGuest = bool(roleInfoPtr->IsGuest());

	return true;
}


void CRoleCollectionControllerComp::SetAdditionalFilters(
			const imtgql::CGqlRequest& gqlRequest,
			imtbase::CComplexCollectionFilter& complexFilter) const
{
	QByteArray productId = GetInputArgumentFromRequest(gqlRequest, "productId").toByteArray();
	if (productId.isEmpty()){
		SendErrorMessage(0, "Unable to set product-ID to collection filter", "CRoleCollectionControllerComp");
		return;
	}

	imtbase::IComplexCollectionFilter::FieldFilter fieldFilter;
	fieldFilter.fieldId = "ProductId";
	fieldFilter.filterValue = productId;

	complexFilter.AddFieldFilter(fieldFilter);
}


bool CRoleCollectionControllerComp::CheckPermissions(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	QByteArray commandId = gqlRequest.GetCommandId();
	QByteArrayList availableRoleIds;
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr != nullptr){
		QByteArray productId = gqlContextPtr->GetProductId();
		const imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
		if (userInfoPtr != nullptr){
			availableRoleIds = userInfoPtr->GetRoles(productId);
		}
	}

	// Add delegated roles if the user is in a delegated tenant
	if (gqlContextPtr != nullptr && m_delegatedAccessCompPtr.IsValid() && m_membershipManagerCompPtr.IsValid()){
		QByteArray userId = gqlContextPtr->GetUserId();
		QByteArray currentTenantId = gqlContextPtr->GetTenantId();
		if (!userId.isEmpty() && !currentTenantId.isEmpty()){
			const imtauth::ITenantMembershipManager::MembershipIds membershipIds =
				m_membershipManagerCompPtr->GetMembershipsByUser(userId);
			for (const QByteArray& membershipId : membershipIds){
				imtauth::ITenantMembershipUniquePtr homeMembershipPtr = m_membershipManagerCompPtr->GetMembership(membershipId);
				if (!homeMembershipPtr.IsValid() || !homeMembershipPtr->IsActive()){
					continue;
				}
				QByteArray homeTenantId = homeMembershipPtr->GetTenantId();
				if (homeTenantId.isEmpty() || homeTenantId == currentTenantId){
					continue;
				}
				if (m_delegatedAccessCompPtr->IsDelegatedAccess(userId, homeTenantId, currentTenantId)){
					QByteArrayList delegatedRoles = m_delegatedAccessCompPtr->GetDelegatedRoles(homeTenantId, currentTenantId);
					for (const QByteArray& roleId : delegatedRoles){
						if (!availableRoleIds.contains(roleId)){
							availableRoleIds.append(roleId);
						}
					}
				}
			}
		}
	}

	QByteArray roleId;
	if (commandId == sdl::V1_0::imtauth::CRoleItemGqlRequest::GetCommandId()){
		sdl::V1_0::imtauth::CRoleItemGqlRequest roleItemGqlRequest(gqlRequest, false);
		if (roleItemGqlRequest.IsValid()){
			auto arguments = roleItemGqlRequest.GetRequestedArguments();
			if (arguments.input.HasValue()){
				if (arguments.input->id.HasValue()){
					roleId = *arguments.input->id;
				}
			}
		}
	}
	else if (commandId == sdl::V1_0::imtbase::CGetObjectTypeIdGqlRequest::GetCommandId()){
		roleId = ExtractObjectIdFromGetObjectTypeIdGqlRequest(gqlRequest);
	}
	else if (commandId == sdl::V1_0::imtbase::CGetObjectDataGqlRequest::GetCommandId()){
		roleId = ExtractObjectIdFromGetObjectDataGqlRequest(gqlRequest);
	}

	if (availableRoleIds.contains(roleId)){
		return true;
	}

	return BaseClass::CheckPermissions(gqlRequest, errorMessage);
}


bool CRoleCollectionControllerComp::UpdateObjectFromRepresentationRequest(
			const imtgql::CGqlRequest& /*rawGqlRequest*/,
			const sdl::V1_0::imtauth::CRoleUpdateGqlRequest& roleUpdateRequest,
			istd::IChangeable& object,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::RoleUpdateRequestArguments arguments = roleUpdateRequest.GetRequestedArguments();
	if (!arguments.input->item.has_value()){
		Q_ASSERT(false);
		return false;
	}

	sdl::V1_0::imtauth::CRoleData roleDataRepresentation = *roleUpdateRequest.GetRequestedArguments().input->item;
	imtauth::CIdentifiableRoleInfo* roleInfoPtr = dynamic_cast<imtauth::CIdentifiableRoleInfo*>(&object);
	if (roleInfoPtr == nullptr){
		errorMessage = QString("Unable to cast role instance to identifable object. Error: Invalid object");
		SendErrorMessage(0, errorMessage, "CRoleCollectionControllerComp");

		return false;
	}

	QByteArrayList includedRole = roleInfoPtr->GetIncludedRoles();
	for (const QByteArray& roleId : includedRole){
		roleInfoPtr->ExcludeRole(roleId);
	}

	QByteArray objectId = roleInfoPtr->GetObjectUuid();
	return FillObjectFromRepresentation(roleDataRepresentation, object, objectId, errorMessage);
}


// reimplemented (icomp::CComponentBase)

void CRoleCollectionControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_idParamRepresentationControllerPtr.SetPtr(new imtserverapp::CIdParamRepresentationController("ParentListFilter"));
	RegisterFilterToSelectionParams(m_idParam, *m_idParamRepresentationControllerPtr);
}


} // namespace imtauth


