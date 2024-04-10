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
		errorMessage = QString("Internal error").toUtf8();

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModel = new imtbase::CTreeItemModel();

	const QList<imtgql::CGqlObject> paramsPtr = gqlRequest.GetParams();

	bool isJsonSerialized = false;
	QByteArray userGroupId;
	QByteArray productId;
	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParam("input");
	if (inputParamPtr != nullptr){
		productId = inputParamPtr->GetFieldArgumentValue("ProductId").toByteArray();
		userGroupId = inputParamPtr->GetFieldArgumentValue("Id").toByteArray();
		isJsonSerialized = inputParamPtr->GetFieldArgumentValue("IsJsonSerialized").toBool();
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(userGroupId, dataPtr)){
		imtauth::IUserGroupInfo* userGroupInfoPtr = dynamic_cast<imtauth::IUserGroupInfo*>(dataPtr.GetPtr());
		if (userGroupInfoPtr != nullptr){
			if (isJsonSerialized){
				QByteArray userJson;
				{
					iser::CJsonMemWriteArchive archive(userJson);
					if (!userGroupInfoPtr->Serialize(archive)){
						return nullptr;
					}
				}

				if (!dataModel->CreateFromJson(userJson)){
					return nullptr;
				}
			}
			else{
				QByteArray groupUuid = userGroupId;
				QString name = userGroupInfoPtr->GetName();
				QString description = userGroupInfoPtr->GetDescription();

				dataModel->SetData("Id", userGroupId);
				dataModel->SetData("Name", name);
				dataModel->SetData("Description", description);

				imtauth::IUserGroupInfo::UserIds userGroupIds = userGroupInfoPtr->GetUsers();
				std::sort(userGroupIds.begin(), userGroupIds.end());

				dataModel->SetData("Users", userGroupIds.join(';'));

				imtauth::IUserGroupInfo::RoleIds roleIds = userGroupInfoPtr->GetRoles(productId);
				std::sort(roleIds.begin(), roleIds.end());

				dataModel->SetData("Roles", roleIds.join(';'));

				imtauth::IUserGroupInfo::GroupIds groupIds = userGroupInfoPtr->GetParentGroups();
				std::sort(groupIds.begin(), groupIds.end());

				dataModel->SetData("ParentGroups", groupIds.join(';'));
			}
		}
	}

	rootModelPtr->SetExternTreeModel("data", dataModel);

	return rootModelPtr.PopPtr();
}


istd::IChangeable* CUserGroupControllerComp::CreateObject(
			const QList<imtgql::CGqlObject>& inputParams,
			QByteArray& objectId,
			QString& name,
			QString& /*description*/,
			QString& errorMessage) const
{

	if (!m_userGroupInfoFactCompPtr.IsValid() || !m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT(false);
		return nullptr;
	}

	objectId = GetObjectIdFromInputParams(inputParams);
	if (objectId.isEmpty()){
		objectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}

	QByteArray productId;
	if (!inputParams.empty()){
		productId = inputParams.at(0).GetFieldArgumentValue("ProductId").toByteArray();
	}

	QByteArray itemData = inputParams.at(0).GetFieldArgumentValue("Item").toByteArray();
	if (!itemData.isEmpty()){
		istd::TDelPtr<imtauth::CIdentifiableUserGroupInfo> userGroupInfoPtr = new imtauth::CIdentifiableUserGroupInfo();
		if (userGroupInfoPtr == nullptr){
			errorMessage = QT_TR_NOOP("Unable to get an group info!");
			return nullptr;
		}

		imtbase::CTreeItemModel itemModel;
		itemModel.CreateFromJson(itemData);

		userGroupInfoPtr->SetObjectUuid(objectId);

		if (itemModel.ContainsKey("Name")){
			name = itemModel.GetData("Name").toString();
		}

		if (name.isEmpty()){
			errorMessage = QT_TR_NOOP("Group Name cannot be empty");

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
							errorMessage = QT_TR_NOOP("Group name already exists");
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

	errorMessage = QString("Can not create group: %1").arg(QString(objectId));

	return nullptr;
}


imtbase::CTreeItemModel* CUserGroupControllerComp::DeleteObject(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	const imtgql::CGqlObject* gqlObjectPtr = gqlRequest.GetParam("input");
	if (gqlObjectPtr == nullptr){
		SendErrorMessage(0, QString("Input params from GraphQL request is invalid"));

		return nullptr;
	}

	QByteArray userGroupId = gqlObjectPtr->GetFieldArgumentValue("Id").toByteArray();

	const imtauth::IUserGroupInfo* userGroupInfoPtr = nullptr;

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(userGroupId, dataPtr)){
		userGroupInfoPtr = dynamic_cast<const imtauth::IUserGroupInfo*>(dataPtr.GetPtr());
	}

	if (userGroupInfoPtr == nullptr){
		SendErrorMessage(0, QString("Unable to get group with ID: '%1'").arg(userGroupId));

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


