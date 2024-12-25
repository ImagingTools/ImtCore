#include <imtauthgql/CUserGroupCollectionControllerComp.h>


// ImtCore includes
#include <imtauth/CUserGroupInfo.h>


namespace imtauthgql
{


// protected methods

// reimplemented (sdl::imtauth::Groups::V1_0::CGroupCollectionControllerCompBase)

bool CUserGroupCollectionControllerComp::CreateRepresentationFromObject(
			const imtbase::IObjectCollectionIterator& objectCollectionIterator,
			const sdl::imtauth::Groups::V1_0::CGroupsListGqlRequest& groupsListRequest,
			sdl::imtauth::Groups::CGroupItem::V1_0& representationObject,
			QString& errorMessage) const
{
	QByteArray objectId = objectCollectionIterator.GetObjectId();
	QByteArray productId;
	if (groupsListRequest.GetRequestedArguments().input.ProductId){
		productId = *groupsListRequest.GetRequestedArguments().input.ProductId;
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

	sdl::imtauth::Groups::V1_0::GroupsListRequestInfo requestInfo = groupsListRequest.GetRequestInfo();

	if (requestInfo.items.isIdRequested){
		representationObject.Id = QByteArray(objectId);
	}

	if (requestInfo.items.isNameRequested){
		representationObject.Name = QString(userGroupInfoPtr->GetName());
	}

	if (requestInfo.items.isRolesRequested){
		representationObject.Roles = QByteArray(userGroupInfoPtr->GetRoles(productId).join(';'));
	}

	if (requestInfo.items.isParentGroupsRequested){
		representationObject.ParentGroups = QByteArray(userGroupInfoPtr->GetParentGroups().join(';'));
	}

	if (requestInfo.items.isDescriptionRequested){
		representationObject.Description = QString(userGroupInfoPtr->GetDescription());
	}

	if (requestInfo.items.isAddedRequested){
		QDateTime addedTime = objectCollectionIterator.GetElementInfo("Added").toDateTime();
		addedTime.setTimeSpec(Qt::UTC);

		QString added = addedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.Added = QString(added);
	}

	if (requestInfo.items.isLastModifiedRequested){
		QDateTime lastModifiedTime = objectCollectionIterator.GetElementInfo("LastModified").toDateTime();
		lastModifiedTime.setTimeSpec(Qt::UTC);

		QString lastModified = lastModifiedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.LastModified = QString(lastModified);
	}

	return true;
}


istd::IChangeable* CUserGroupCollectionControllerComp::CreateObjectFromRepresentation(
			const sdl::imtauth::Groups::CGroupData::V1_0& groupDataRepresentation,
			QByteArray& newObjectId,
			QString& name,
			QString& description,
			QString& errorMessage) const
{
	if (!m_userGroupInfoFactCompPtr.IsValid()){
		errorMessage = QString("Unable to create object from representation. Error: Attribute 'm_userGroupInfoFactCompPtr' was not set");
		SendErrorMessage(0, errorMessage, "CUserGroupCollectionControllerComp");

		return nullptr;
	}

	istd::TDelPtr<imtauth::IUserGroupInfo> userGroupInstancePtr = m_userGroupInfoFactCompPtr.CreateInstance();
	if (!userGroupInstancePtr.IsValid()){
		errorMessage = QString("Unable to create group instance. Error: Invalid object");
		SendErrorMessage(0, errorMessage, "CUserGroupCollectionControllerComp");

		return nullptr;
	}

	imtauth::CIdentifiableUserGroupInfo* userGroupInfoPtr = dynamic_cast<imtauth::CIdentifiableUserGroupInfo*>(userGroupInstancePtr.GetPtr());
	if (userGroupInfoPtr == nullptr){
		errorMessage = QString("Unable to cast user group instance to identifable object. Error: Invalid object");
		SendErrorMessage(0, errorMessage, "CUserGroupCollectionControllerComp");

		return nullptr;
	}

	if (groupDataRepresentation.Id){
		newObjectId = *groupDataRepresentation.Id;
	}
	if (newObjectId.isEmpty()){
		newObjectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}
	userGroupInfoPtr->SetObjectUuid(newObjectId);

	QByteArray productId;
	if (groupDataRepresentation.ProductId){
		productId = *groupDataRepresentation.ProductId;
	}

	if (groupDataRepresentation.Name){
		name = *groupDataRepresentation.Name;
	}
	if (name.isEmpty()){
		errorMessage = QString("Group name cannot be empty");
		return nullptr;
	}

	imtbase::ICollectionInfo::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds();
	for (imtbase::ICollectionInfo::Id& collectionId : collectionIds){
		imtbase::IObjectCollection::DataPtr groupDataPtr;
		if (m_objectCollectionCompPtr->GetObjectData(collectionId, groupDataPtr)){
			imtauth::IUserGroupInfo* currentUserGroupInfoPtr = dynamic_cast<imtauth::IUserGroupInfo*>(groupDataPtr.GetPtr());
			if (currentUserGroupInfoPtr != nullptr){
				if (collectionId != newObjectId){
					QString currentUserGroupName = currentUserGroupInfoPtr->GetName();
					if (currentUserGroupName == name){
						errorMessage = QT_TR_NOOP(QString("Group name '%1' already exists").arg(currentUserGroupName));
						return nullptr;
					}
				}
			}
		}
	}

	userGroupInfoPtr->SetName(name);

	if (groupDataRepresentation.Description){
		description = *groupDataRepresentation.Description;
	}
	userGroupInfoPtr->SetDescription(description);

	QByteArrayList userIds;
	if (groupDataRepresentation.Users){
		userIds = groupDataRepresentation.Users->split(';');
	}
	userIds.removeAll("");
	userGroupInfoPtr->SetUsers(userIds);

	QByteArrayList roleIds;
	if (groupDataRepresentation.Roles){
		roleIds = groupDataRepresentation.Roles->split(';');
	}
	roleIds.removeAll("");

	if (!roleIds.isEmpty()){
		userGroupInfoPtr->SetRoles(productId, roleIds);
	}
	else{
		userGroupInfoPtr->RemoveProduct(productId);
	}

	if (groupDataRepresentation.ParentGroups){
		QByteArrayList groupIds = groupDataRepresentation.ParentGroups->split(';');
		for (const QByteArray& parentGroupId : groupIds){
			if (!parentGroupId.isEmpty()){
				userGroupInfoPtr->AddParentGroup(parentGroupId);
			}
		}
	}

	return userGroupInstancePtr.PopPtr();
}


bool CUserGroupCollectionControllerComp::CreateRepresentationFromObject(
			const istd::IChangeable& data,
			const sdl::imtauth::Groups::V1_0::CGroupItemGqlRequest& groupItemRequest,
			sdl::imtauth::Groups::CGroupDataPayload::V1_0& representationPayload,
			QString& errorMessage) const
{
	const imtauth::CIdentifiableUserGroupInfo* userGroupInfoPtr = dynamic_cast<const imtauth::CIdentifiableUserGroupInfo*>(&data);
	if (userGroupInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object. Error: Object is invalid");
		SendErrorMessage(0, errorMessage, "CUserGroupCollectionControllerComp");

		return false;
	}

	sdl::imtauth::Groups::V1_0::GroupItemRequestArguments arguments = groupItemRequest.GetRequestedArguments();
	sdl::imtauth::Groups::CGroupData::V1_0 groupData;

	QByteArray productId;
	if (arguments.input.ProductId){
		productId = *arguments.input.ProductId;
	}

	groupData.Id = QByteArray(userGroupInfoPtr->GetObjectUuid());
	groupData.Name = QString(userGroupInfoPtr->GetName());
	groupData.Description = QString(userGroupInfoPtr->GetDescription());

	imtauth::IUserGroupInfo::UserIds userIds = userGroupInfoPtr->GetUsers();
	std::sort(userIds.begin(), userIds.end());
	groupData.Users = QByteArray(userIds.join(';'));

	imtauth::IUserGroupInfo::RoleIds roleIds = userGroupInfoPtr->GetRoles(productId);
	std::sort(roleIds.begin(), roleIds.end());
	groupData.Roles = QByteArray(roleIds.join(';'));

	imtauth::IUserGroupInfo::GroupIds groupIds = userGroupInfoPtr->GetParentGroups();
	std::sort(groupIds.begin(), groupIds.end());
	groupData.ParentGroups = QByteArray(groupIds.join(';'));

	representationPayload.GroupData = std::make_optional<sdl::imtauth::Groups::CGroupData::V1_0>(groupData);

	return true;
}


imtbase::CTreeItemModel* CUserGroupCollectionControllerComp::GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& /*errorMessage*/) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return nullptr;
	}

	const imtgql::CGqlObject& params = gqlRequest.GetParams();

	QByteArray productId = params.GetFieldArgumentValue("ProductId").toByteArray();

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	QByteArray userId = GetObjectIdFromInputParams(params);

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(userId, dataPtr)){
		imtbase::CTreeItemModel* dataModel = rootModelPtr->AddTreeModel("data");
		const imtauth::IUserGroupInfo* userGroupInfoPtr = dynamic_cast<const imtauth::IUserGroupInfo*>(dataPtr.GetPtr());
		if (userGroupInfoPtr != nullptr){
			int index = dataModel->InsertNewItem();
			dataModel->SetData("Name", "Parent Groups", index);
			imtbase::CTreeItemModel* children = dataModel->AddTreeModel("Children", index);

			if (m_userGroupInfoProviderCompPtr.IsValid()){
				imtauth::IUserGroupInfo::GroupIds groupIds = userGroupInfoPtr->GetParentGroups();
				if (groupIds.isEmpty()){
					children->SetData("Value", "No parent groups");
				}
				else{
					for (const QByteArray& groupId : groupIds){
						istd::TDelPtr<const imtauth::IUserGroupInfo> parentGroupInfoPtr = m_userGroupInfoProviderCompPtr->GetUserGroup(groupId);
						if (parentGroupInfoPtr.IsValid()){
							QString groupName = parentGroupInfoPtr->GetName();

							int childrenIndex = children->InsertNewItem();
							children->SetData("Value", groupName, childrenIndex);
						}
					}
				}
			}

			index = dataModel->InsertNewItem();
			dataModel->SetData("Name", "Users", index);
			children = dataModel->AddTreeModel("Children", index);

			if (m_userInfoProviderCompPtr.IsValid()){
				imtauth::IUserGroupInfo::UserIds userIds = userGroupInfoPtr->GetUsers();
				if (userIds.isEmpty()){
					children->SetData("Value", "No users");
				}
				else{
					for (const QByteArray& groupUserId : userIds){
						istd::TDelPtr<const imtauth::IUserInfo> userInfoPtr = m_userInfoProviderCompPtr->GetUser(groupUserId);
						if (userInfoPtr.IsValid()){
							QString userName = userInfoPtr->GetName();

							int childrenIndex = children->InsertNewItem();
							children->SetData("Value", userName, childrenIndex);
						}
					}
				}
			}

			index = dataModel->InsertNewItem();
			dataModel->SetData("Name", "Roles", index);
			children = dataModel->AddTreeModel("Children", index);

			if (m_roleInfoProviderCompPtr.IsValid()){
				imtauth::IUserGroupInfo::RoleIds roleIds = userGroupInfoPtr->GetRoles(productId);
				if (roleIds.isEmpty()){
					children->SetData("Value", "No roles");
				}
				else{
					for (const QByteArray& roleId : roleIds){
						istd::TDelPtr<const imtauth::IRole> roleInfoPtr = m_roleInfoProviderCompPtr->GetRole(roleId);
						if (roleInfoPtr.IsValid()){
							QString roleName = roleInfoPtr->GetRoleName();

							int childrenIndex = children->InsertNewItem();
							children->SetData("Value", roleName, childrenIndex);
						}
					}
				}
			}
		}
	}

	return rootModelPtr.PopPtr();
}

bool CUserGroupCollectionControllerComp::UpdateObjectFromRepresentationRequest(
			const imtgql::CGqlRequest& /*rawGqlRequest*/,
			const sdl::imtauth::Groups::V1_0::CGroupUpdateGqlRequest& /*groupUpdateRequest*/,
			istd::IChangeable& /*object*/,
			QString& /*errorMessage*/) const
{
	return false;
}


} // namespace imtauth


