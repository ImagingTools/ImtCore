#include <imtauthgql/CUserGroupCollectionControllerComp.h>


// ImtCore includes
#include <imtauth/CUserGroupInfo.h>


namespace imtauthgql
{


// protected methods

// reimplemented (sdl::imtauth::Groups::CGroupCollectionControllerCompBase)

bool CUserGroupCollectionControllerComp::CreateRepresentationFromObject(
			const imtbase::IObjectCollectionIterator& objectCollectionIterator,
			const sdl::imtauth::Groups::CGroupsListGqlRequest& groupsListRequest,
			sdl::imtauth::Groups::CGroupItem& representationObject,
			QString& errorMessage) const
{
	QByteArray objectId = objectCollectionIterator.GetObjectId();
	QByteArray productId = groupsListRequest.GetRequestedArguments().input.GetProductId();

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

	sdl::imtauth::Groups::GroupsListRequestInfo requestInfo = groupsListRequest.GetRequestInfo();

	if (requestInfo.items.isIdRequested){
		representationObject.SetId(objectId);
	}

	if (requestInfo.items.isNameRequested){
		representationObject.SetName(userGroupInfoPtr->GetName());
	}

	if (requestInfo.items.isRolesRequested){
		representationObject.SetRoles(userGroupInfoPtr->GetRoles(productId).join(';'));
	}

	if (requestInfo.items.isParentGroupsRequested){
		representationObject.SetParentGroups(userGroupInfoPtr->GetParentGroups().join(';'));
	}

	if (requestInfo.items.isDescriptionRequested){
		representationObject.SetDescription(userGroupInfoPtr->GetDescription());
	}

	if (requestInfo.items.isAddedRequested){
		QDateTime addedTime = objectCollectionIterator.GetElementInfo("Added").toDateTime();
		addedTime.setTimeSpec(Qt::UTC);

		QString added = addedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.SetAdded(added);
	}

	if (requestInfo.items.isLastModifiedRequested){
		QDateTime lastModifiedTime = objectCollectionIterator.GetElementInfo("LastModified").toDateTime();
		lastModifiedTime.setTimeSpec(Qt::UTC);

		QString lastModified = lastModifiedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.SetLastModified(lastModified);
	}

	return true;
}


istd::IChangeable* CUserGroupCollectionControllerComp::CreateObjectFromRepresentation(
			const sdl::imtauth::Groups::CGroupData& groupDataRepresentation,
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

	newObjectId = groupDataRepresentation.GetId();
	if (newObjectId.isEmpty()){
		newObjectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}
	userGroupInfoPtr->SetObjectUuid(newObjectId);

	QByteArray productId = groupDataRepresentation.GetProductId();

	name = groupDataRepresentation.GetName();
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

	description = groupDataRepresentation.GetDescription();
	userGroupInfoPtr->SetDescription(description);

	QByteArrayList userIds = groupDataRepresentation.GetUsers().split(';');
	userIds.removeAll("");
	userGroupInfoPtr->SetUsers(userIds);

	QByteArrayList roleIds = groupDataRepresentation.GetRoles().split(';');
	roleIds.removeAll("");

	if (!roleIds.isEmpty()){
		userGroupInfoPtr->SetRoles(productId, roleIds);
	}
	else{
		userGroupInfoPtr->RemoveProduct(productId);
	}

	QByteArrayList groupIds = groupDataRepresentation.GetParentGroups().split(';');
	for (const QByteArray& parentGroupId : groupIds){
		if (!parentGroupId.isEmpty()){
			userGroupInfoPtr->AddParentGroup(parentGroupId);
		}
	}

	return userGroupInstancePtr.PopPtr();
}


bool CUserGroupCollectionControllerComp::CreateRepresentationFromObject(
			const istd::IChangeable& data,
			const sdl::imtauth::Groups::CGroupItemGqlRequest& groupItemRequest,
			sdl::imtauth::Groups::CGroupDataPayload& representationPayload,
			QString& errorMessage) const
{
	const imtauth::CIdentifiableUserGroupInfo* userGroupInfoPtr = dynamic_cast<const imtauth::CIdentifiableUserGroupInfo*>(&data);
	if (userGroupInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object. Error: Object is invalid");
		SendErrorMessage(0, errorMessage, "CUserGroupCollectionControllerComp");

		return false;
	}

	sdl::imtauth::Groups::GroupItemRequestArguments arguments = groupItemRequest.GetRequestedArguments();
	sdl::imtauth::Groups::CGroupData groupData;

	QByteArray productId = arguments.input.GetProductId();

	groupData.SetId(userGroupInfoPtr->GetObjectUuid());
	groupData.SetName(userGroupInfoPtr->GetName());
	groupData.SetDescription(userGroupInfoPtr->GetDescription());

	imtauth::IUserGroupInfo::UserIds userIds = userGroupInfoPtr->GetUsers();
	std::sort(userIds.begin(), userIds.end());
	groupData.SetUsers(userIds.join(';'));

	imtauth::IUserGroupInfo::RoleIds roleIds = userGroupInfoPtr->GetRoles(productId);
	std::sort(roleIds.begin(), roleIds.end());
	groupData.SetRoles(roleIds.join(';'));

	imtauth::IUserGroupInfo::GroupIds groupIds = userGroupInfoPtr->GetParentGroups();
	std::sort(groupIds.begin(), groupIds.end());
	groupData.SetParentGroups(groupIds.join(';'));

	representationPayload.SetGroupData(groupData);

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
				for (const QByteArray& groupId : groupIds){
					const imtauth::IUserGroupInfo* parentGroupInfoPtr = m_userGroupInfoProviderCompPtr->GetUserGroup(groupId);
					if (parentGroupInfoPtr != nullptr){
						QString groupName = parentGroupInfoPtr->GetName();

						int childrenIndex = children->InsertNewItem();
						children->SetData("Value", groupName, childrenIndex);
					}
				}
			}

			index = dataModel->InsertNewItem();
			dataModel->SetData("Name", "Users", index);
			children = dataModel->AddTreeModel("Children", index);

			if (m_userInfoProviderCompPtr.IsValid()){
				imtauth::IUserGroupInfo::UserIds userIds = userGroupInfoPtr->GetUsers();
				for (const QByteArray& groupUserId : userIds){
					const imtauth::IUserInfo* userInfoPtr = m_userInfoProviderCompPtr->GetUser(groupUserId);
					if (userInfoPtr != nullptr){
						QString userName = userInfoPtr->GetName();

						int childrenIndex = children->InsertNewItem();
						children->SetData("Value", userName, childrenIndex);
					}
				}
			}

			index = dataModel->InsertNewItem();
			dataModel->SetData("Name", "Roles", index);
			children = dataModel->AddTreeModel("Children", index);

			if (m_roleInfoProviderCompPtr.IsValid()){
				imtauth::IUserGroupInfo::RoleIds roleIds = userGroupInfoPtr->GetRoles(productId);
				for (const QByteArray& roleId : roleIds){
					const imtauth::IRole* roleInfoPtr = m_roleInfoProviderCompPtr->GetRole(roleId);
					if (roleInfoPtr != nullptr){
						QString roleName = roleInfoPtr->GetRoleName();

						int childrenIndex = children->InsertNewItem();
						children->SetData("Value", roleName, childrenIndex);
					}
				}
			}
		}
	}

	return rootModelPtr.PopPtr();
}


} // namespace imtauth


