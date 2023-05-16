#include <imtauthgql/CUserGroupControllerComp.h>


// ACF includes
#include <istd/TChangeDelegator.h>

// ImtCore includes
#include <imtauth/CUserGroupInfo.h>


namespace imtauthgql
{


imtbase::CTreeItemModel* CUserGroupControllerComp::GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModel = new imtbase::CTreeItemModel();

	QByteArray userGroupId = GetObjectIdFromInputParams(*gqlRequest.GetParams());
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(userGroupId, dataPtr)){
		imtauth::CIdentifiableUserGroupInfo* userGroupInfoPtr = dynamic_cast<imtauth::CIdentifiableUserGroupInfo*>(dataPtr.GetPtr());
		if (userGroupInfoPtr != nullptr){
			QByteArray groupUuid = userGroupInfoPtr->GetObjectUuid();
			QString name = userGroupInfoPtr->GetName();
			QString description = userGroupInfoPtr->GetDescription();

			dataModel->SetData("Id", userGroupId);
			dataModel->SetData("Name", name);
			dataModel->SetData("Description", description);
			dataModel->SetData("Users", "");

			imtauth::IUserGroupInfo::UserIds userGroupIds = userGroupInfoPtr->GetUsers();
			if (!userGroupIds.empty()){
				QByteArray users = userGroupIds.join(';');
				dataModel->SetData("Users", users);
			}

			imtauth::IUserGroupInfo::RoleIds roleIds = userGroupInfoPtr->GetRoles();
			if (!roleIds.empty()){
				QByteArray roles = roleIds.join(';');
				dataModel->SetData("Roles", roles);
			}

			imtauth::IUserGroupInfo::GroupIds groupIds = userGroupInfoPtr->GetParentGroups();
			if (!groupIds.isEmpty()){
				QByteArray groups = groupIds.join(';');
				dataModel->SetData("ParentGroups", groups);
			}
		}
	}

	rootModelPtr->SetExternTreeModel("data", dataModel);

	return rootModelPtr.PopPtr();
}


istd::IChangeable* CUserGroupControllerComp::CreateObject(
		const QList<imtgql::CGqlObject>& inputParams,
		QByteArray &objectId,
		QString &name,
		QString &description,
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

		imtbase::ICollectionInfo::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds();
		for (imtbase::ICollectionInfo::Id collectionId : collectionIds){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_objectCollectionCompPtr->GetObjectData(collectionId, dataPtr)){
				imtauth::IUserGroupInfo* currentUserGroupInfoPtr = dynamic_cast<imtauth::IUserGroupInfo*>(dataPtr.GetPtr());
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
			QString description = itemModel.GetData("Description").toString();

			userGroupInfoPtr->SetDescription(description);
		}

		if (itemModel.ContainsKey("Users")){
			QByteArray users = itemModel.GetData("Users").toByteArray();
			if (!users.isEmpty()){
				QByteArrayList userIds = users.split(';');
				userGroupInfoPtr->SetUsers(imtauth::IUserGroupInfo::UserIds(userIds.begin(), userIds.end()));
			}
		}

		if (itemModel.ContainsKey("Roles")){
			QByteArray roles = itemModel.GetData("Roles").toByteArray();
			if (!roles.isEmpty()){
				QByteArrayList roleIds = roles.split(';');
				userGroupInfoPtr->SetRoles(imtauth::IUserGroupInfo::RoleIds(roleIds.begin(), roleIds.end()));
			}
		}

		if (itemModel.ContainsKey("ParentGroups")){
			QByteArray groups = itemModel.GetData("ParentGroups").toByteArray();
			if (!groups.isEmpty()){
				QByteArrayList groupIds = groups.split(';');
				for (const QByteArray& parentGroupId : groupIds){
					userGroupInfoPtr->AddParentGroup(parentGroupId);
				}
			}
		}

		return userGroupInfoPtr.PopPtr();
	}

	errorMessage = QObject::tr("Can not create group: %1").arg(QString(objectId));

	return nullptr;
}


} // namespace imtauthgql


