#include <imtauthgql/CUserGroupControllerComp.h>


// ACF includes
#include <istd/TChangeDelegator.h>
#include <iser/CJsonMemWriteArchive.h>

// ImtCore includes
#include <imtauth/CUserGroupInfo.h>


namespace imtauthgql
{


imtbase::CTreeItemModel* CUserGroupControllerComp::GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT(false);

		return nullptr;
	}

	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParam("input");
	if (inputParamPtr == nullptr){
		errorMessage = QString("Unable to get a group. Error: GraphQL input params is invalid.").toUtf8();
		SendErrorMessage(0, errorMessage, "CUserGroupControllerComp");

		return nullptr;
	}

	QByteArray userGroupId = inputParamPtr->GetFieldArgumentValue("Id").toByteArray();;
	QByteArray productId = inputParamPtr->GetFieldArgumentValue("ProductId").toByteArray();;

	imtauth::IUserGroupInfo* userGroupInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(userGroupId, dataPtr)){
		userGroupInfoPtr = dynamic_cast<imtauth::IUserGroupInfo*>(dataPtr.GetPtr());
	}

	if (userGroupInfoPtr == nullptr){
		errorMessage = QString("Unable to get group with ID: '%1'. The group does not exist.").arg(userGroupId);
		SendErrorMessage(0, errorMessage, "CRoleControllerComp");

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

	dataModelPtr->SetData("Id", userGroupId);
	dataModelPtr->SetData("Name", userGroupInfoPtr->GetName());
	dataModelPtr->SetData("Description", userGroupInfoPtr->GetDescription());

	imtauth::IUserGroupInfo::UserIds userGroupIds = userGroupInfoPtr->GetUsers();
	std::sort(userGroupIds.begin(), userGroupIds.end());
	dataModelPtr->SetData("Users", userGroupIds.join(';'));

	imtauth::IUserGroupInfo::RoleIds roleIds = userGroupInfoPtr->GetRoles(productId);
	std::sort(roleIds.begin(), roleIds.end());
	dataModelPtr->SetData("Roles", roleIds.join(';'));

	imtauth::IUserGroupInfo::GroupIds groupIds = userGroupInfoPtr->GetParentGroups();
	std::sort(groupIds.begin(), groupIds.end());
	dataModelPtr->SetData("ParentGroups", groupIds.join(';'));

	return rootModelPtr.PopPtr();
}


istd::IChangeable* CUserGroupControllerComp::CreateObject(
		const imtgql::CGqlRequest& gqlRequest,
		QByteArray& objectId,
		QString& name,
		QString& /*description*/,
		QString& errorMessage) const
{

	if (!m_userGroupInfoFactCompPtr.IsValid() || !m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT(false);
		return nullptr;
	}

	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParam("input");
	if (inputParamPtr == nullptr){
		errorMessage = QString("Unable to create a group object. Error: GraphQL input params is invalid.").toUtf8();
		SendErrorMessage(0, errorMessage, "CUserGroupControllerComp");

		return nullptr;
	}

	objectId = inputParamPtr->GetFieldArgumentValue("Id").toByteArray();
	if (objectId.isEmpty()){
		objectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}

	QByteArray productId = inputParamPtr->GetFieldArgumentValue("ProductId").toByteArray();;

	istd::TDelPtr<imtauth::CIdentifiableUserGroupInfo> userGroupInfoPtr;
	userGroupInfoPtr.SetCastedOrRemove(m_userGroupInfoFactCompPtr.CreateInstance());
	if (!userGroupInfoPtr.IsValid()){
		errorMessage = QString("Unable to create a group instance.").toUtf8();
		SendErrorMessage(0, errorMessage, "CUserGroupControllerComp");

		return nullptr;
	}

	QByteArray itemData = inputParamPtr->GetFieldArgumentValue("Item").toByteArray();
	imtbase::CTreeItemModel itemModel;
	if (!itemModel.CreateFromJson(itemData)){
		errorMessage = QString("Unable to create a role object. Error: Failed to create a tree model from json '%1'").arg(itemData);
		SendErrorMessage(0, errorMessage, "CRoleControllerComp");

		return nullptr;
	}

	userGroupInfoPtr->SetObjectUuid(objectId);

	if (itemModel.ContainsKey("Name")){
		name = itemModel.GetData("Name").toString();
	}

	if (name.isEmpty()){
		errorMessage = QString("Group name cannot be empty");

		return nullptr;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		imtauth::IUserGroupInfo* oldUserGroupInfoPtr = dynamic_cast<imtauth::IUserGroupInfo*>(dataPtr.GetPtr());
		if (oldUserGroupInfoPtr != nullptr){
			for (const QByteArray& oldUserGroupProductId : oldUserGroupInfoPtr->GetProducts()){
				userGroupInfoPtr->SetRoles(oldUserGroupProductId, oldUserGroupInfoPtr->GetRoles(oldUserGroupProductId));
			}
		}
	}

	imtbase::ICollectionInfo::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds();
	for (imtbase::ICollectionInfo::Id collectionId : collectionIds){
		imtbase::IObjectCollection::DataPtr groupDataPtr;
		if (m_objectCollectionCompPtr->GetObjectData(collectionId, groupDataPtr)){
			imtauth::IUserGroupInfo* currentUserGroupInfoPtr = dynamic_cast<imtauth::IUserGroupInfo*>(groupDataPtr.GetPtr());
			if (currentUserGroupInfoPtr != nullptr){
				if (collectionId != objectId){
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

	userGroupInfoPtr->SetId(objectId);

	if (itemModel.ContainsKey("Description")){
		QString groupDescription = itemModel.GetData("Description").toString();

		userGroupInfoPtr->SetDescription(groupDescription);
	}

	if (itemModel.ContainsKey("Users")){
		QByteArray users = itemModel.GetData("Users").toByteArray();
		QByteArrayList userIds = users.split(';');
		userIds.removeAll("");

		userGroupInfoPtr->SetUsers(userIds);
	}

	if (itemModel.ContainsKey("Roles")){
		QByteArray roles = itemModel.GetData("Roles").toByteArray();
		if (!roles.isEmpty()){
			QByteArrayList roleIds = roles.split(';');
			roleIds.removeAll("");

			userGroupInfoPtr->SetRoles(productId, roleIds);
		}
		else{
			userGroupInfoPtr->RemoveProduct(productId);
		}
	}

	if (itemModel.ContainsKey("ParentGroups")){
		QByteArray groups = itemModel.GetData("ParentGroups").toByteArray();
		QByteArrayList groupIds = groups.split(';');
		for (const QByteArray& parentGroupId : groupIds){
			if (!parentGroupId.isEmpty()){
				userGroupInfoPtr->AddParentGroup(parentGroupId);
			}
		}
	}

	return userGroupInfoPtr.PopPtr();
}


imtbase::CTreeItemModel* CUserGroupControllerComp::DeleteObject(
		const imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	const imtgql::CGqlObject* gqlObjectPtr = gqlRequest.GetParam("input");
	if (gqlObjectPtr == nullptr){
		errorMessage = QString("Unable to delete group. Error: GraphQL input params is invalid.").toUtf8();
		SendErrorMessage(0, errorMessage, "CUserGroupControllerComp");

		return nullptr;
	}

	QByteArray userGroupId = gqlObjectPtr->GetFieldArgumentValue("Id").toByteArray();

	const imtauth::IUserGroupInfo* userGroupInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(userGroupId, dataPtr)){
		userGroupInfoPtr = dynamic_cast<const imtauth::IUserGroupInfo*>(dataPtr.GetPtr());
	}

	if (userGroupInfoPtr == nullptr){
		errorMessage = QString("The object with ID: '%1' cannot be deleted");
		SendErrorMessage(0, errorMessage, "CUserGroupControllerComp");

		return nullptr;
	}

	imtbase::CTreeItemModel* resultModelPtr = BaseClass::DeleteObject(gqlRequest, errorMessage);
	if (errorMessage.isEmpty()){
		if (m_userCollectionCompPtr.IsValid()){
			QByteArrayList userIds = userGroupInfoPtr->GetUsers();
			for (const QByteArray& userId : userIds){
				imtbase::IObjectCollection::DataPtr userDataPtr;
				if (m_userCollectionCompPtr->GetObjectData(userId, userDataPtr)){
					imtauth::IUserInfo* userInfoPtr = dynamic_cast<imtauth::IUserInfo*>(userDataPtr.GetPtr());
					if (userInfoPtr != nullptr){
						userInfoPtr->RemoveFromGroup(userGroupId);

						if (!m_userCollectionCompPtr->SetObjectData(userId, *userInfoPtr)){
							SendWarningMessage(0, QString("Unable to update an user"));
						}
					}
				}
			}
		}

		imtbase::ICollectionInfo::Ids elementIds = m_objectCollectionCompPtr->GetElementIds();
		for (const imtbase::ICollectionInfo::Id& elementId: elementIds){
			imtbase::IObjectCollection::DataPtr groupDataPtr;
			if (m_objectCollectionCompPtr->GetObjectData(elementId, groupDataPtr)){
				imtauth::IUserGroupInfo* userGroupInfoPtr = dynamic_cast<imtauth::IUserGroupInfo*>(groupDataPtr.GetPtr());
				if (userGroupInfoPtr != nullptr){
					QByteArrayList parentGroupIds = userGroupInfoPtr->GetParentGroups();
					if (parentGroupIds.contains(userGroupId)){
						userGroupInfoPtr->RemoveParentGroup(userGroupId);

						if (!m_objectCollectionCompPtr->SetObjectData(elementId, *userGroupInfoPtr)){
							SendWarningMessage(0, QString("Unable to update a group"));
						}
					}
				}
			}
		}
	}

	return resultModelPtr;
}


} // namespace imtauthgql


