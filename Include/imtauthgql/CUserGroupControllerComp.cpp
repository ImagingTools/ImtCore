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
		errorMessage = QObject::tr("Internal error").toUtf8();

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

	dataModel->SetData("Id", "");
	dataModel->SetData("Name", "");
	dataModel->SetData("Description", "");
	dataModel->SetData("Users", "");

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
				dataModel->SetData("Users", "");

				imtauth::IUserGroupInfo::UserIds userGroupIds = userGroupInfoPtr->GetUsers();
				if (!userGroupIds.empty()){
					QByteArray users = userGroupIds.join(';');
					dataModel->SetData("Users", users);
				}

				imtauth::IUserGroupInfo::RoleIds roleIds = userGroupInfoPtr->GetRoles(productId);
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
				for (const QByteArray& productId : oldUserGroupInfoPtr->GetProducts()){
					userGroupInfoPtr->SetRoles(productId, oldUserGroupInfoPtr->GetRoles(productId));
				}
			}
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
			QString groupDescription = itemModel.GetData("Description").toString();

			userGroupInfoPtr->SetDescription(groupDescription);
		}

		if (itemModel.ContainsKey("Users")){
			QByteArray users = itemModel.GetData("Users").toByteArray();
			if (!users.isEmpty()){
				QByteArrayList userIds = users.split(';');

				userGroupInfoPtr->SetUsers(userIds);
			}
		}

		if (itemModel.ContainsKey("Roles")){
			QByteArray roles = itemModel.GetData("Roles").toByteArray();
			if (!roles.isEmpty()){
				QByteArrayList roleIds = roles.split(';');

				userGroupInfoPtr->SetRoles(productId, roleIds);
			}
			else{
				userGroupInfoPtr->RemoveProduct(productId);
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


