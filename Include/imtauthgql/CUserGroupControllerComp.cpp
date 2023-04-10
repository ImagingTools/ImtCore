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

	dataModel->SetData("Id", "");
	dataModel->SetData("Name", "");

	QByteArray userGroupId = GetObjectIdFromInputParams(*gqlRequest.GetParams());
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(userGroupId, dataPtr)){
		imtauth::CIdentifiableUserGroupInfo* userGroupInfoPtr = dynamic_cast<imtauth::CIdentifiableUserGroupInfo*>(dataPtr.GetPtr());
		if (userGroupInfoPtr != nullptr){
			QByteArray groupUuid = userGroupInfoPtr->GetObjectUuid();
			QString name = userGroupInfoPtr->GetName();
			QString description = userGroupInfoPtr->GetDescription();

			dataModel->SetData("Id", groupUuid);
			dataModel->SetData("Name", name);
			dataModel->SetData("Description", description);
			dataModel->SetData("Users", "");

			imtauth::IUserGroupInfo::UserIds userGroupIds = userGroupInfoPtr->GetUsers();
			if (!userGroupIds.empty()){
				QByteArray users = userGroupIds.values().join(';');
				dataModel->SetData("Users", users);
			}

			imtauth::IUserGroupInfo::RoleIds roleIds = userGroupInfoPtr->GetRoles();
			if (!roleIds.empty()){
				QByteArray roles = roleIds.join(';');
				dataModel->SetData("Roles", roles);
			}

			if (!userGroupInfoPtr->GetSubGroups().IsEmpty()){
				InsertSubGroupsToRepresentationFromModel(*userGroupInfoPtr, *dataModel->AddTreeModel("ChildModel"));
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

	QByteArray itemData = inputParams.at(0).GetFieldArgumentValue("Item").toByteArray();
	if (!itemData.isEmpty()){
		istd::TDelPtr<imtauth::CIdentifiableUserGroupInfo> userGroupInfoPtr = new imtauth::CIdentifiableUserGroupInfo();
		if (userGroupInfoPtr == nullptr){
			errorMessage = QT_TR_NOOP("Unable to get an group info!");
			return nullptr;
		}

		imtbase::CTreeItemModel itemModel;
		itemModel.CreateFromJson(itemData);

		if (itemModel.ContainsKey("Id")){
			QByteArray id = itemModel.GetData("Id").toByteArray();
			if (!id.isEmpty()){
				objectId = id;
			}
		}

		if (objectId.isEmpty()){
			objectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
		}

		userGroupInfoPtr->SetObjectUuid(objectId);

		userGroupInfoPtr->SetId(objectId);

		if (itemModel.ContainsKey("Name")){
			name = itemModel.GetData("Name").toString();
			userGroupInfoPtr->SetName(name);
		}

		if (name.isEmpty()){
			errorMessage = QT_TR_NOOP("Group Name cannot be empty");

			return nullptr;
		}

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

		if (itemModel.ContainsKey("ChildModel")){
			imtbase::CTreeItemModel* childrenModel = itemModel.GetTreeItemModel("ChildModel");
			if (childrenModel != nullptr){
				InsertSubGroupsToModelFromRepresentation(*userGroupInfoPtr.GetPtr(), *childrenModel);
			}
		}

		return userGroupInfoPtr.PopPtr();
	}

	errorMessage = QObject::tr("Can not create group: %1").arg(QString(objectId));

	return nullptr;
}


bool CUserGroupControllerComp::InsertSubGroupsToModelFromRepresentation(imtauth::IUserGroupInfo& parentGroupInfoPtr, const imtbase::CTreeItemModel& representationModel) const
{
	for (int i = 0; i < representationModel.GetItemsCount(); i++){
		QByteArray groupUuid = representationModel.GetData("Id", i).toByteArray();
		QString groupName = representationModel.GetData("Name", i).toString();
		QString groupDescription = representationModel.GetData("Description", i).toString();

		istd::TChangeDelegator<imtauth::CIdentifiableUserGroupInfo>* groupInfoPtr = new istd::TChangeDelegator<imtauth::CIdentifiableUserGroupInfo>;
		groupInfoPtr->SetSlavePtr(&parentGroupInfoPtr);

		groupInfoPtr->SetObjectUuid(groupUuid);
		groupInfoPtr->SetName(groupName);
		groupInfoPtr->SetDescription(groupDescription);

		parentGroupInfoPtr.InsertSubGroup(groupInfoPtr);

		if (representationModel.ContainsKey("ChildModel", i)){
			imtbase::CTreeItemModel *subModelPtr = representationModel.GetTreeItemModel("ChildModel", i);
			if (subModelPtr != nullptr){
				bool result = InsertSubGroupsToModelFromRepresentation(*groupInfoPtr, *subModelPtr);
				if (!result){
					return false;
				}
			}
		}
	}

	return true;
}


void CUserGroupControllerComp::InsertSubGroupsToRepresentationFromModel(const imtauth::IUserGroupInfo& groupInfoPtr, imtbase::CTreeItemModel& representationModel) const
{
	const imtauth::UserGroupInfoList& subGroups = groupInfoPtr.GetSubGroups();
	for (int i = 0; i < subGroups.GetCount(); i++){
		const imtauth::CIdentifiableUserGroupInfo* subGroupPtr = dynamic_cast<const imtauth::CIdentifiableUserGroupInfo*>(subGroups.GetAt(i));
		if (subGroupPtr != nullptr){
			QByteArray groupUuid = subGroupPtr->GetObjectUuid();
			QString groupName = subGroupPtr->GetName();
			QString groupDescription = subGroupPtr->GetDescription();
			QByteArray roleIds = subGroupPtr->GetRoles().join(';');

			int index = representationModel.InsertNewItem();

			representationModel.SetData("Id", groupUuid, index);
			representationModel.SetData("Name", groupName, index);
			representationModel.SetData("Description", groupDescription, index);
			representationModel.SetData("Roles", roleIds, index);

			if (!subGroupPtr->GetSubGroups().IsEmpty()){
				InsertSubGroupsToRepresentationFromModel(*subGroupPtr, *representationModel.AddTreeModel("ChildModel", index));
			}
		}
	}
}


} // namespace imtauthgql


