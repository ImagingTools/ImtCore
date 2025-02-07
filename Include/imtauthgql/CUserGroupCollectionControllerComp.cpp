#include <imtauthgql/CUserGroupCollectionControllerComp.h>


// ACF includes
#include <iqt/iqt.h>

// ImtCore includes
#include <imtauth/CUserGroupInfo.h>


namespace imtauthgql
{


// protected methods

bool CUserGroupCollectionControllerComp::FillObjectFromRepresentation(
			const sdl::imtauth::Groups::CGroupData::V1_0& groupDataRepresentation,
			istd::IChangeable& object,
			QByteArray& newObjectId,
			QString& errorMessage) const
{
	imtauth::CIdentifiableUserGroupInfo* userGroupInfoPtr = dynamic_cast<imtauth::CIdentifiableUserGroupInfo*>(&object);
	if (userGroupInfoPtr == nullptr){
		errorMessage = QString("Unable to cast user group instance to identifable object. Error: Invalid object");
		SendErrorMessage(0, errorMessage, "CUserGroupCollectionControllerComp");
		return false;
	}

	QByteArray productId;
	if (groupDataRepresentation.ProductId){
		productId = *groupDataRepresentation.ProductId;
	}

	QString name;
	if (groupDataRepresentation.Name){
		name = *groupDataRepresentation.Name;
	}
	if (name.isEmpty()){
		errorMessage = QString("Group Name cannot be empty");
		return false;
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
						errorMessage = QT_TR_NOOP(QString("Group Name '%1' already exists").arg(currentUserGroupName));
						return false;
					}
				}
			}
		}
	}

	userGroupInfoPtr->SetName(name);

	if (groupDataRepresentation.Description){
		userGroupInfoPtr->SetDescription(*groupDataRepresentation.Description);
	}

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

	return true;
}


// reimplemented (sdl::imtbase::ImtCollection::CGraphQlHandlerCompBase)

sdl::imtbase::ImtCollection::CVisualStatus CUserGroupCollectionControllerComp::OnGetObjectVisualStatus(
			const sdl::imtbase::ImtCollection::CGetObjectVisualStatusGqlRequest& getObjectVisualStatusRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtbase::ImtCollection::CVisualStatus response = BaseClass::OnGetObjectVisualStatus(getObjectVisualStatusRequest, gqlRequest, errorMessage);

	QByteArray languageId;
	const imtgql::IGqlContext* gqlContextPtr = getObjectVisualStatusRequest.GetRequestContext();
	if (gqlContextPtr != nullptr){
		languageId = gqlContextPtr->GetLanguageId();
	}

	if (response.Version_1_0->Text->isEmpty()){
		response.Version_1_0->Text = "<no name>";
	}

	QString translation = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), QString(QT_TR_NOOP("Groups")).toUtf8(), languageId, "CRoleCollectionControllerComp");
	response.Version_1_0->Text = translation + QByteArrayLiteral(" / ") + *response.Version_1_0->Text;

	return response;
}


// reimplemented (sdl::imtauth::Groups::CGroupCollectionControllerCompBase)

bool CUserGroupCollectionControllerComp::CreateRepresentationFromObject(
			const imtbase::IObjectCollectionIterator& objectCollectionIterator,
			const sdl::imtauth::Groups::CGroupsListGqlRequest& groupsListRequest,
			sdl::imtauth::Groups::CGroupItem::V1_0& representationObject,
			QString& errorMessage) const
{
	QByteArray objectId = objectCollectionIterator.GetObjectId();
	QByteArray productId;
	if (groupsListRequest.GetRequestedArguments().input.Version_1_0->ProductId){
		productId = *groupsListRequest.GetRequestedArguments().input.Version_1_0->ProductId;
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

	sdl::imtauth::Groups::GroupsListRequestInfo requestInfo = groupsListRequest.GetRequestInfo();

	if (requestInfo.items.isIdRequested){
		representationObject.Id = QByteArray(objectId);
	}

	if (requestInfo.items.isTypeIdRequested){
		representationObject.TypeId = QByteArray(m_objectCollectionCompPtr->GetObjectTypeId(objectId));
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

	if (!FillObjectFromRepresentation(groupDataRepresentation, *userGroupInfoPtr, newObjectId, errorMessage)){
		return nullptr;
	}

	return userGroupInstancePtr.PopPtr();
}


bool CUserGroupCollectionControllerComp::CreateRepresentationFromObject(
			const istd::IChangeable& data,
			const sdl::imtauth::Groups::CGroupItemGqlRequest& groupItemRequest,
			sdl::imtauth::Groups::CGroupDataPayload::V1_0& representationPayload,
			QString& errorMessage) const
{
	const imtauth::CIdentifiableUserGroupInfo* userGroupInfoPtr = dynamic_cast<const imtauth::CIdentifiableUserGroupInfo*>(&data);
	if (userGroupInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object. Error: Object is invalid");
		SendErrorMessage(0, errorMessage, "CUserGroupCollectionControllerComp");

		return false;
	}

	sdl::imtauth::Groups::GroupItemRequestArguments arguments = groupItemRequest.GetRequestedArguments();
	sdl::imtauth::Groups::CGroupData::V1_0 groupData;

	QByteArray productId;
	if (arguments.input.Version_1_0->ProductId){
		productId = *arguments.input.Version_1_0->ProductId;
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
			const imtgql::CGqlRequest& rawGqlRequest,
			const sdl::imtauth::Groups::CGroupUpdateGqlRequest& groupUpdateRequest,
			istd::IChangeable& object,
			QString& errorMessage) const
{
	sdl::imtauth::Groups::CGroupData representation;

	if (groupUpdateRequest.GetRequestedArguments().input.Version_1_0->Item){
		representation.Version_1_0 = groupUpdateRequest.GetRequestedArguments().input.Version_1_0->Item;
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

	return FillObjectFromRepresentation(*representation.Version_1_0, object, objectId, errorMessage);
}


} // namespace imtauth


