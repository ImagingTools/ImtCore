// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CUserGroupCollectionControllerComp.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Groups.h>


// ACF includes
#include <iqt/iqt.h>

// ImtCore includes
#include <imtauth/CUserGroupInfo.h>
#include <imtauth/IUserInfoProvider.h>


namespace imtauthgql
{


// protected methods

bool CUserGroupCollectionControllerComp::FillObjectFromRepresentation(
			const sdl::V1_0::imtauth::CGroupData& groupDataRepresentation,
			istd::IChangeable& object,
			QByteArray& newObjectId,
			QString& errorMessage) const
{
	auto userGroupInfoPtr = dynamic_cast<imtauth::CIdentifiableUserGroupInfo*>(&object);
	if (userGroupInfoPtr == nullptr){
		errorMessage = QString("Unable to cast user group instance to identifable object. Error: Invalid object");
		SendErrorMessage(0, errorMessage, "CUserGroupCollectionControllerComp");
		return false;
	}

	userGroupInfoPtr->SetId(newObjectId);

	QByteArray productId;
	if (groupDataRepresentation.productId){
		productId = *groupDataRepresentation.productId;
	}

	QString name;
	if (groupDataRepresentation.name){
		name = *groupDataRepresentation.name;
	}
	if (name.isEmpty()){
		errorMessage = QString("Group name cannot be empty");
		return false;
	}

	imtbase::ICollectionInfo::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds();
	for (imtbase::ICollectionInfo::Id& collectionId : collectionIds){
		imtbase::IObjectCollection::DataPtr groupDataPtr;
		if (m_objectCollectionCompPtr->GetObjectData(collectionId, groupDataPtr)){
			auto currentUserGroupInfoPtr = dynamic_cast<imtauth::IUserGroupInfo*>(groupDataPtr.GetPtr());
			if (currentUserGroupInfoPtr != nullptr){
				if (collectionId != newObjectId){
					QString currentUserGroupName = currentUserGroupInfoPtr->GetName();
					if (currentUserGroupName == name){
						errorMessage = QT_TR_NOOP(QString("Group Name '%1' already exists").arg(currentUserGroupName));
						return false;
					}
				}
			}
		}
	}

	userGroupInfoPtr->SetName(name);

	if (groupDataRepresentation.description){
		userGroupInfoPtr->SetDescription(*groupDataRepresentation.description);
	}

	QByteArrayList userIds;
	if (groupDataRepresentation.users){
		userIds = groupDataRepresentation.users->ToList();
	}
	userIds.removeAll("");
	userGroupInfoPtr->SetUsers(userIds);

	QByteArrayList roleIds;
	if (groupDataRepresentation.roles){
		roleIds = groupDataRepresentation.roles->ToList();
	}
	roleIds.removeAll("");

	if (!roleIds.isEmpty()){
		userGroupInfoPtr->SetRoles(productId, roleIds);
	}
	else{
		userGroupInfoPtr->RemoveProduct(productId);
	}

	if (groupDataRepresentation.parentGroups){
		QByteArrayList groupIds = groupDataRepresentation.parentGroups->ToList();
		for (const QByteArray& parentGroupId : groupIds){
			if (!parentGroupId.isEmpty()){
				userGroupInfoPtr->AddParentGroup(parentGroupId);
			}
		}
	}

	return true;
}


// reimplemented (sdl::V1_0::imtbase::CImtCollectionGqlHandlerCompBase)

sdl::V1_0::imtbase::CVisualStatus CUserGroupCollectionControllerComp::OnGetObjectVisualStatus(
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

	QString translation = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), QString(QT_TR_NOOP("Groups")).toUtf8(), languageId, "CRoleCollectionControllerComp");
	response.text = translation + QByteArrayLiteral(" / ") + *response.text;

	return response;
}


sdl::V1_0::imtbase::CGetElementMetaInfoPayload CUserGroupCollectionControllerComp::OnGetElementMetaInfo(
			const sdl::V1_0::imtbase::CGetElementMetaInfoGqlRequest& getElementMetaInfoRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::imtbase::CGetElementMetaInfoPayload response;
	sdl::V1_0::imtbase::GetElementMetaInfoRequestArguments arguments = getElementMetaInfoRequest.GetRequestedArguments();
	QByteArray objectId;
	if (arguments.input->elementId){
		objectId = *arguments.input->elementId;
	}

	QByteArray productId = gqlRequest.GetHeader("productId");

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		auto userGroupInfoPtr = dynamic_cast<const imtauth::IUserGroupInfo*>(dataPtr.GetPtr());
		if (userGroupInfoPtr != nullptr){
			sdl::V1_0::imtbase::CElementMetaInfo elementMetaInfo;
		
			imtsdl::TElementList<sdl::V1_0::imtbase::CParameter> infoParams;

			sdl::V1_0::imtbase::CParameter parentGroupsParameter;
			parentGroupsParameter.id = QByteArrayLiteral("ParentGroups");
			parentGroupsParameter.typeId = parentGroupsParameter.id;
			parentGroupsParameter.name = QStringLiteral("Parent Groups");

			if (m_userGroupInfoProviderCompPtr.IsValid()){
				imtauth::IUserGroupInfo::GroupIds groupIds = userGroupInfoPtr->GetParentGroups();
				if (groupIds.isEmpty()){
					parentGroupsParameter.data = QStringLiteral("No parent groups");
				}
				else{
					QString parentGroupsData;
					for (const QByteArray& groupId : groupIds){
						imtauth::IUserGroupInfoSharedPtr parentGroupInfoPtr = m_userGroupInfoProviderCompPtr->GetUserGroup(groupId);
						if (parentGroupInfoPtr.IsValid()){
							QString groupName = parentGroupInfoPtr->GetName();
							parentGroupsData += groupName + "\n";
						}
					}
					parentGroupsParameter.data = parentGroupsData;
				}
			}

			infoParams << parentGroupsParameter;

			sdl::V1_0::imtbase::CParameter usersParameter;
			usersParameter.id = QByteArrayLiteral("Users");
			usersParameter.typeId = usersParameter.id;
			usersParameter.name = QStringLiteral("Users");

			if (m_userInfoProviderCompPtr.IsValid()){
				imtauth::IUserGroupInfo::UserIds userIds = userGroupInfoPtr->GetUsers();
				if (userIds.isEmpty()){
					usersParameter.data = QStringLiteral("No users");
				}
				else{
					QString usersData;
					for (const QByteArray& groupUserId : userIds){
						imtauth::IUserInfoUniquePtr userInfoPtr = m_userInfoProviderCompPtr->GetUser(groupUserId);
						if (userInfoPtr.IsValid()){
							QString userName = userInfoPtr->GetName();
							usersData += userName + "\n";
						}
					}
					usersParameter.data = usersData;
				}
			}

			infoParams << usersParameter;

			sdl::V1_0::imtbase::CParameter rolesParameter;
			rolesParameter.id = QByteArrayLiteral("Roles");
			rolesParameter.typeId = rolesParameter.id;
			rolesParameter.name = QStringLiteral("Roles");

			if (m_roleInfoProviderCompPtr.IsValid()){
				imtauth::IUserGroupInfo::RoleIds roleIds = userGroupInfoPtr->GetRoles(productId);
				if (roleIds.isEmpty()){
					rolesParameter.data = QStringLiteral("No roles");
				}
				else{
					QString rolesData;
					for (const QByteArray& roleId : roleIds){
						imtauth::IRoleUniquePtr roleInfoPtr = m_roleInfoProviderCompPtr->GetRole(roleId);
						if (roleInfoPtr.IsValid()){
							QString roleName = roleInfoPtr->GetRoleName();

							rolesData += roleName + "\n";
						}
					}
					rolesParameter.data = rolesData;
				}
			}

			infoParams << rolesParameter;
			elementMetaInfo.infoParams = infoParams;

			response.elementMetaInfo = elementMetaInfo;
		}
	}

	return response;
}

// reimplemented (sdl::V1_0::imtauth::CGroupCollectionControllerCompBase)

bool CUserGroupCollectionControllerComp::CreateRepresentationFromObject(
			const imtbase::IObjectCollectionIterator& objectCollectionIterator,
			const sdl::V1_0::imtauth::CGroupsListGqlRequest& groupsListRequest,
			sdl::V1_0::imtauth::CGroupItemData& representationObject,
			QString& errorMessage) const
{
	QByteArray objectId = objectCollectionIterator.GetObjectId();
	QByteArray productId;
	if (groupsListRequest.GetRequestedArguments().input->productId){
		productId = *groupsListRequest.GetRequestedArguments().input->productId;
	}

	const imtauth::IUserGroupInfo* userGroupInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr userDataPtr;
	if (objectCollectionIterator.GetObjectData(userDataPtr)){
		userGroupInfoPtr = dynamic_cast<const imtauth::IUserGroupInfo*>(userDataPtr.GetPtr());
	}

	if (userGroupInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object '%1'").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "CUserGroupCollectionControllerComp");

		return false;
	}

	sdl::V1_0::imtauth::GroupsListRequestInfo requestInfo = groupsListRequest.GetRequestInfo();

	if (requestInfo.items.isIdRequested){
		representationObject.id = QByteArray(objectId);
	}

	if (requestInfo.items.isTypeIdRequested){
		representationObject.typeId = objectCollectionIterator.GetObjectTypeId();
	}

	if (requestInfo.items.isNameRequested){
		representationObject.name = QString(userGroupInfoPtr->GetName());
	}

	if (requestInfo.items.isRolesRequested){
		representationObject.roles = QByteArray(userGroupInfoPtr->GetRoles(productId).join(';'));
	}

	if (requestInfo.items.isParentGroupsRequested){
		representationObject.parentGroups = QByteArray(userGroupInfoPtr->GetParentGroups().join(';'));
	}

	if (requestInfo.items.isDescriptionRequested){
		representationObject.description = QString(userGroupInfoPtr->GetDescription());
	}

	if (requestInfo.items.isAddedRequested){
		QDateTime addedTime = objectCollectionIterator.GetElementInfo("Added").toDateTime();
		addedTime.setTimeZone(QTimeZone::utc());

		QString added = addedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.added = QString(added);
	}

	if (requestInfo.items.isLastModifiedRequested){
		QDateTime lastModifiedTime = objectCollectionIterator.GetElementInfo("LastModified").toDateTime();
		lastModifiedTime.setTimeZone(QTimeZone::utc());

		QString lastModified = lastModifiedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.lastModified = QString(lastModified);
	}

	return true;
}


istd::IChangeableUniquePtr CUserGroupCollectionControllerComp::CreateObjectFromRepresentation(
			const sdl::V1_0::imtauth::CGroupData& groupDataRepresentation,
			QByteArray& newObjectId,
			QString& errorMessage) const
{
	if (!m_userGroupInfoFactCompPtr.IsValid()){
		errorMessage = QString("Unable to create object from representation. Error: Attribute 'm_userGroupInfoFactCompPtr' was not set");
		SendErrorMessage(0, errorMessage, "CUserGroupCollectionControllerComp");

		return nullptr;
	}

	imtauth::IUserGroupInfoUniquePtr userGroupInstancePtr = m_userGroupInfoFactCompPtr.CreateInstance();
	if (!userGroupInstancePtr.IsValid()){
		errorMessage = QString("Unable to create group instance. Error: Invalid object");
		SendErrorMessage(0, errorMessage, "CUserGroupCollectionControllerComp");

		return nullptr;
	}

	auto userGroupInfoPtr = dynamic_cast<imtauth::CIdentifiableUserGroupInfo*>(userGroupInstancePtr.GetPtr());
	if (userGroupInfoPtr == nullptr){
		errorMessage = QString("Unable to cast user group instance to identifable object. Error: Invalid object");
		SendErrorMessage(0, errorMessage, "CUserGroupCollectionControllerComp");

		return nullptr;
	}

	if (groupDataRepresentation.id){
		newObjectId = *groupDataRepresentation.id;
	}
	if (newObjectId.isEmpty()){
		newObjectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}
	userGroupInfoPtr->SetObjectUuid(newObjectId);

	if (!FillObjectFromRepresentation(groupDataRepresentation, *userGroupInfoPtr, newObjectId, errorMessage)){
		return nullptr;
	}

	return userGroupInstancePtr;
}


bool CUserGroupCollectionControllerComp::CreateRepresentationFromObject(
			const istd::IChangeable& data,
			const sdl::V1_0::imtauth::CGroupItemGqlRequest& groupItemRequest,
			sdl::V1_0::imtauth::CGroupData& representationPayload,
			QString& errorMessage) const
{
	auto userGroupInfoPtr = dynamic_cast<const imtauth::CIdentifiableUserGroupInfo*>(&data);
	if (userGroupInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object. Error: Object is invalid");
		SendErrorMessage(0, errorMessage, "CUserGroupCollectionControllerComp");

		return false;
	}

	sdl::V1_0::imtauth::GroupItemRequestArguments arguments = groupItemRequest.GetRequestedArguments();

	QByteArray productId;
	if (arguments.input->productId){
		productId = *arguments.input->productId;
	}

	representationPayload.id = QByteArray(userGroupInfoPtr->GetObjectUuid());
	representationPayload.name = QString(userGroupInfoPtr->GetName());
	representationPayload.description = QString(userGroupInfoPtr->GetDescription());
	representationPayload.productId = productId;

	imtauth::IUserGroupInfo::UserIds userIds = userGroupInfoPtr->GetUsers();
	representationPayload.users.Emplace().FromList(userIds);

	imtauth::IUserGroupInfo::RoleIds roleIds = userGroupInfoPtr->GetRoles(productId);
	representationPayload.roles.Emplace().FromList(roleIds);

	imtauth::IUserGroupInfo::GroupIds groupIds = userGroupInfoPtr->GetParentGroups();
	representationPayload.parentGroups.Emplace().FromList(groupIds);

	return true;
}


bool CUserGroupCollectionControllerComp::UpdateObjectFromRepresentationRequest(
			const imtgql::CGqlRequest& /*rawGqlRequest*/,
			const sdl::V1_0::imtauth::CGroupUpdateGqlRequest& groupUpdateRequest,
			istd::IChangeable& object,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::CGroupData representation;
	
	istd::TNullableValue<sdl::V1_0::imtauth::CGroupDataInput> input = groupUpdateRequest.GetRequestedArguments().input;
	if (input && input->item){
		representation = *input->item;
	}

	imtauth::CIdentifiableUserGroupInfo* userGroupInfoPtr = dynamic_cast<imtauth::CIdentifiableUserGroupInfo*>(&object);
	if (userGroupInfoPtr == nullptr){
		errorMessage = QString("Unable to cast user group instance to identifable object. Error: Invalid object");
		SendErrorMessage(0, errorMessage, "CUserGroupCollectionControllerComp");

		return false;
	}

	QByteArray objectId = userGroupInfoPtr->GetObjectUuid();

	QByteArrayList groupIds = userGroupInfoPtr->GetParentGroups();
	for (const QByteArray& groupId : groupIds){
		userGroupInfoPtr->RemoveParentGroup(groupId);
	}

	QByteArrayList userIds = userGroupInfoPtr->GetUsers();
	for (const QByteArray& userId : userIds){
		userGroupInfoPtr->RemoveUser(userId);
	}

	return FillObjectFromRepresentation(representation, object, objectId, errorMessage);
}


// reimplemented (imtservergql::CPermissibleGqlRequestHandlerComp)

bool CUserGroupCollectionControllerComp::CheckPermissions(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	QByteArrayList availableGroupIds;
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr != nullptr){
		QByteArray productId = gqlContextPtr->GetProductId();
		const imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
		if (userInfoPtr != nullptr){
			availableGroupIds = userInfoPtr->GetGroups();
		}
	}

	QByteArray commandId = gqlRequest.GetCommandId();
	if (commandId == sdl::V1_0::imtauth::CGroupItemGqlRequest::GetCommandId() ||
		commandId == sdl::V1_0::imtbase::CGetObjectDataGqlRequest::GetCommandId() ||
		commandId == sdl::V1_0::imtbase::CGetObjectTypeIdGqlRequest::GetCommandId()){
		return true;
	}

	QByteArray groupId;
	if (commandId == sdl::V1_0::imtauth::CGroupItemGqlRequest::GetCommandId()){
		sdl::V1_0::imtauth::CGroupItemGqlRequest groupItemGqlRequest(gqlRequest, false);
		if (groupItemGqlRequest.IsValid()){
			auto arguments = groupItemGqlRequest.GetRequestedArguments();
			if (arguments.input.HasValue()){
				if (arguments.input->id.HasValue()){
					groupId = *arguments.input->id;
				}
			}
		}
	}
	else if (commandId == sdl::V1_0::imtbase::CGetObjectTypeIdGqlRequest::GetCommandId()){
		groupId = ExtractObjectIdFromGetObjectTypeIdGqlRequest(gqlRequest);
	}
	else if (commandId == sdl::V1_0::imtbase::CGetObjectDataGqlRequest::GetCommandId()){
		groupId = ExtractObjectIdFromGetObjectDataGqlRequest(gqlRequest);
	}

	return BaseClass::CheckPermissions(gqlRequest, errorMessage);
}


} // namespace imtauth


