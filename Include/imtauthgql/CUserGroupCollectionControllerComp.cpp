#include <imtauthgql/CUserGroupCollectionControllerComp.h>


// ACF includes
#include <iqt/iqt.h>

// ImtCore includes
#include <imtauth/CUserGroupInfo.h>
#include <imtauth/IUserInfoProvider.h>


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

	if (groupDataRepresentation.description){
		userGroupInfoPtr->SetDescription(*groupDataRepresentation.description);
	}

	QByteArrayList userIds;
	if (groupDataRepresentation.users){
		userIds = groupDataRepresentation.users->split(';');
	}
	userIds.removeAll("");
	userGroupInfoPtr->SetUsers(userIds);

	QByteArrayList roleIds;
	if (groupDataRepresentation.roles){
		roleIds = groupDataRepresentation.roles->split(';');
	}
	roleIds.removeAll("");

	if (!roleIds.isEmpty()){
		userGroupInfoPtr->SetRoles(productId, roleIds);
	}
	else{
		userGroupInfoPtr->RemoveProduct(productId);
	}

	if (groupDataRepresentation.parentGroups){
		QByteArrayList groupIds = groupDataRepresentation.parentGroups->split(';');
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
	if (!errorMessage.isEmpty()){
		return sdl::imtbase::ImtCollection::CVisualStatus();
	}
	
	if (!response.Version_1_0.has_value()){
		Q_ASSERT(false);
		return response;
	}
	
	if (!response.Version_1_0->text.has_value()){
		Q_ASSERT(false);
		return response;
	}
	
	if (response.Version_1_0->text->isEmpty()){
		response.Version_1_0->text = "<no name>";
	}
	
	QByteArray languageId;
	const imtgql::IGqlContext* gqlContextPtr = getObjectVisualStatusRequest.GetRequestContext();
	if (gqlContextPtr != nullptr){
		languageId = gqlContextPtr->GetLanguageId();
	}

	QString translation = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), QString(QT_TR_NOOP("Groups")).toUtf8(), languageId, "CRoleCollectionControllerComp");
	response.Version_1_0->text = translation + QByteArrayLiteral(" / ") + *response.Version_1_0->text;

	return response;
}


// reimplemented (sdl::imtauth::Groups::CGroupCollectionControllerCompBase)

bool CUserGroupCollectionControllerComp::CreateRepresentationFromObject(
			const imtbase::IObjectCollectionIterator& objectCollectionIterator,
			const sdl::imtauth::Groups::CGroupsListGqlRequest& groupsListRequest,
			sdl::imtauth::Groups::CGroupItemData::V1_0& representationObject,
			QString& errorMessage) const
{
	QByteArray objectId = objectCollectionIterator.GetObjectId();
	QByteArray productId;
	if (groupsListRequest.GetRequestedArguments().input.Version_1_0->productId){
		productId = *groupsListRequest.GetRequestedArguments().input.Version_1_0->productId;
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
		representationObject.id = QByteArray(objectId);
	}

	if (requestInfo.items.isTypeIdRequested){
		representationObject.typeId = QByteArray(m_objectCollectionCompPtr->GetObjectTypeId(objectId));
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
		addedTime.setTimeSpec(Qt::UTC);

		QString added = addedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.added = QString(added);
	}

	if (requestInfo.items.isLastModifiedRequested){
		QDateTime lastModifiedTime = objectCollectionIterator.GetElementInfo("LastModified").toDateTime();
		lastModifiedTime.setTimeSpec(Qt::UTC);

		QString lastModified = lastModifiedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.lastModified = QString(lastModified);
	}

	return true;
}


istd::IChangeableUniquePtr CUserGroupCollectionControllerComp::CreateObjectFromRepresentation(
			const sdl::imtauth::Groups::CGroupData::V1_0& groupDataRepresentation,
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

	imtauth::CIdentifiableUserGroupInfo* userGroupInfoPtr = dynamic_cast<imtauth::CIdentifiableUserGroupInfo*>(userGroupInstancePtr.GetPtr());
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

	istd::IChangeableUniquePtr retVal;
	retVal.MoveCastedPtr<imtauth::IUserGroupInfo>(userGroupInstancePtr);

	return retVal;
}


bool CUserGroupCollectionControllerComp::CreateRepresentationFromObject(
			const istd::IChangeable& data,
			const sdl::imtauth::Groups::CGroupItemGqlRequest& groupItemRequest,
			sdl::imtauth::Groups::CGroupData::V1_0& representationPayload,
			QString& errorMessage) const
{
	const imtauth::CIdentifiableUserGroupInfo* userGroupInfoPtr = dynamic_cast<const imtauth::CIdentifiableUserGroupInfo*>(&data);
	if (userGroupInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object. Error: Object is invalid");
		SendErrorMessage(0, errorMessage, "CUserGroupCollectionControllerComp");

		return false;
	}

	sdl::imtauth::Groups::GroupItemRequestArguments arguments = groupItemRequest.GetRequestedArguments();

	QByteArray productId;
	if (arguments.input.Version_1_0->productId){
		productId = *arguments.input.Version_1_0->productId;
	}

	representationPayload.id = QByteArray(userGroupInfoPtr->GetObjectUuid());
	representationPayload.name = QString(userGroupInfoPtr->GetName());
	representationPayload.description = QString(userGroupInfoPtr->GetDescription());

	imtauth::IUserGroupInfo::UserIds userIds = userGroupInfoPtr->GetUsers();
	std::sort(userIds.begin(), userIds.end());
	representationPayload.users = QByteArray(userIds.join(';'));

	imtauth::IUserGroupInfo::RoleIds roleIds = userGroupInfoPtr->GetRoles(productId);
	std::sort(roleIds.begin(), roleIds.end());
	representationPayload.roles = QByteArray(roleIds.join(';'));

	imtauth::IUserGroupInfo::GroupIds groupIds = userGroupInfoPtr->GetParentGroups();
	std::sort(groupIds.begin(), groupIds.end());
	representationPayload.parentGroups = QByteArray(groupIds.join(';'));

	return true;
}


imtbase::CTreeItemModel* CUserGroupCollectionControllerComp::GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& /*errorMessage*/) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return nullptr;
	}

	const imtgql::CGqlParamObject& params = gqlRequest.GetParams();

	QByteArray productId = params.GetParamArgumentValue("productId").toByteArray();

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	QByteArray userId = GetObjectIdFromInputParams(params);

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(userId, dataPtr)){
		imtbase::CTreeItemModel* dataModel = rootModelPtr->AddTreeModel("data");
		const imtauth::IUserGroupInfo* userGroupInfoPtr = dynamic_cast<const imtauth::IUserGroupInfo*>(dataPtr.GetPtr());
		if (userGroupInfoPtr != nullptr){
			int index = dataModel->InsertNewItem();
			dataModel->SetData("name", "Parent Groups", index);
			imtbase::CTreeItemModel* children = dataModel->AddTreeModel("children", index);

			if (m_userGroupInfoProviderCompPtr.IsValid()){
				imtauth::IUserGroupInfo::GroupIds groupIds = userGroupInfoPtr->GetParentGroups();
				if (groupIds.isEmpty()){
					children->SetData("value", "No parent groups");
				}
				else{
					for (const QByteArray& groupId : groupIds){
						imtauth::IUserGroupInfoSharedPtr parentGroupInfoPtr = m_userGroupInfoProviderCompPtr->GetUserGroup(groupId);
						if (parentGroupInfoPtr.IsValid()){
							QString groupName = parentGroupInfoPtr->GetName();

							int childrenIndex = children->InsertNewItem();
							children->SetData("value", groupName, childrenIndex);
						}
					}
				}
			}

			index = dataModel->InsertNewItem();
			dataModel->SetData("name", "Users", index);
			children = dataModel->AddTreeModel("children", index);

			if (m_userInfoProviderCompPtr.IsValid()){
				imtauth::IUserGroupInfo::UserIds userIds = userGroupInfoPtr->GetUsers();
				if (userIds.isEmpty()){
					children->SetData("value", "No users");
				}
				else{
					for (const QByteArray& groupUserId : userIds){
						istd::TDelPtr<const imtauth::IUserInfo> userInfoPtr = m_userInfoProviderCompPtr->GetUser(groupUserId);
						if (userInfoPtr.IsValid()){
							QString userName = userInfoPtr->GetName();

							int childrenIndex = children->InsertNewItem();
							children->SetData("value", userName, childrenIndex);
						}
					}
				}
			}

			index = dataModel->InsertNewItem();
			dataModel->SetData("name", "Roles", index);
			children = dataModel->AddTreeModel("children", index);

			if (m_roleInfoProviderCompPtr.IsValid()){
				imtauth::IUserGroupInfo::RoleIds roleIds = userGroupInfoPtr->GetRoles(productId);
				if (roleIds.isEmpty()){
					children->SetData("value", "No roles");
				}
				else{
					for (const QByteArray& roleId : roleIds){
						istd::TDelPtr<const imtauth::IRole> roleInfoPtr = m_roleInfoProviderCompPtr->GetRole(roleId);
						if (roleInfoPtr.IsValid()){
							QString roleName = roleInfoPtr->GetRoleName();

							int childrenIndex = children->InsertNewItem();
							children->SetData("value", roleName, childrenIndex);
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
			const sdl::imtauth::Groups::CGroupUpdateGqlRequest& groupUpdateRequest,
			istd::IChangeable& object,
			QString& errorMessage) const
{
	sdl::imtauth::Groups::CGroupData representation;
	
	if (!groupUpdateRequest.GetRequestedArguments().input.Version_1_0.has_value()){
		I_CRITICAL();
		return false;
	}

	if (groupUpdateRequest.GetRequestedArguments().input.Version_1_0->item){
		representation.Version_1_0 = groupUpdateRequest.GetRequestedArguments().input.Version_1_0->item;
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


