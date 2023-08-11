#include <imtauthgql/CUserGroupCollectionControllerComp.h>


// ImtCore includes
#include <imtauth/CUserGroupInfo.h>


namespace imtauthgql
{


// protected methods

// reimplemented (imtguigql::CObjectCollectionControllerCompBase)


imtbase::CTreeItemModel* CUserGroupCollectionControllerComp::GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return nullptr;
	}

	const QList<imtgql::CGqlObject> paramsPtr = gqlRequest.GetParams();

	QByteArray productId;
	if (!paramsPtr.empty()){
		productId = paramsPtr.at(0).GetFieldArgumentValue("ProductId").toByteArray();
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	QByteArray userId = GetObjectIdFromInputParams(paramsPtr);

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


bool CUserGroupCollectionControllerComp::SetupGqlItem(
		const imtgql::CGqlRequest& gqlRequest,
		imtbase::CTreeItemModel& model,
		int itemIndex,
		const imtbase::IObjectCollectionIterator* objectCollectionIterator,
		QString& /*errorMessage*/) const
{
	const QList<imtgql::CGqlObject> paramsPtr = gqlRequest.GetParams();

	QByteArray productId;
	if (!paramsPtr.empty()){
		productId = paramsPtr.at(0).GetFieldArgumentValue("ProductId").toByteArray();
	}

	bool retVal = true;

	QByteArrayList informationIds = GetInformationIds(gqlRequest, "items");

	if (!informationIds.isEmpty() && m_objectCollectionCompPtr.IsValid()){
		imtauth::IUserGroupInfo* userGroupInfoPtr = nullptr;
		imtbase::IObjectCollection::DataPtr groupDataPtr;
		if (objectCollectionIterator->GetObjectData(groupDataPtr)){
			userGroupInfoPtr = dynamic_cast<imtauth::IUserGroupInfo*>(groupDataPtr.GetPtr());
		}

		if (userGroupInfoPtr != nullptr){
			QByteArray collectionId = objectCollectionIterator->GetObjectId();
			bool ok = true;
			for (QByteArray informationId : informationIds){

				QVariant elementInformation;
				if(informationId == "TypeId"){
					elementInformation = m_objectCollectionCompPtr->GetObjectTypeId(collectionId);
				}
				else if(informationId == "Id"){
					elementInformation = collectionId;
				}
				else if(informationId == "Name"){
					elementInformation = userGroupInfoPtr->GetName();
				}
				else if(informationId == "Description"){
					elementInformation = userGroupInfoPtr->GetDescription();
				}
				else if(informationId == "Roles"){
					elementInformation = userGroupInfoPtr->GetRoles(productId).join(';');
				}
				else if(informationId == "ParentGroups"){
					elementInformation = userGroupInfoPtr->GetParentGroups().join(';');
				}

				if (elementInformation.isNull()){
					elementInformation = "";
				}

				if (ok){
					retVal = retVal && model.SetData(informationId, elementInformation, itemIndex);
				}
			}
		}

		return true;
	}

	return false;
}


} // namespace imtauth


