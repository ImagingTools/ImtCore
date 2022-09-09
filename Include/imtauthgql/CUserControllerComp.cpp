#include <imtauthgql/CUserControllerComp.h>

// ImtCore includes
#include <imtauth/CUserInfo.h>
#include <imtauth/IRoleInfoProvider.h>
#include <imtlic/IFeatureInfo.h>


namespace imtauthgql
{


imtbase::CTreeItemModel* CUserControllerComp::GetObject(
		const QList<imtgql::CGqlObject>& inputParams,
		const imtgql::CGqlObject& gqlObject,
		QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();

		return nullptr;
	}

	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* dataModel = new imtbase::CTreeItemModel();

	dataModel->SetData("UserId", "");
	dataModel->SetData("Username", "");
	dataModel->SetData("Name", "");
	dataModel->SetData("Password", "");
	dataModel->SetData("Email", "");

	imtbase::CTreeItemModel* permissionsModel = dataModel->AddTreeModel("Permissions");
	imtbase::CTreeItemModel* prohibitionsModel = dataModel->AddTreeModel("Prohibitions");
	imtbase::CTreeItemModel* rolesModel = dataModel->AddTreeModel("Roles");

	if (m_rolesCollectionCompPtr.IsValid()){
		imtbase::ICollectionInfo::Ids rolesIds = m_rolesCollectionCompPtr->GetElementIds();

		for (const imtbase::ICollectionInfo::Id& roleId : rolesIds){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_rolesCollectionCompPtr->GetObjectData(roleId, dataPtr)){
				const imtauth::IRole* roleInfoPtr = dynamic_cast<const imtauth::IRole*>(dataPtr.GetPtr());

				QByteArray id = roleInfoPtr->GetRoleId();
				QString name = roleInfoPtr->GetRoleName();

				int index = rolesModel->InsertNewItem();

				rolesModel->SetData("Id", id, index);
				rolesModel->SetData("Name", name, index);
				rolesModel->SetData("State", Qt::CheckState::Unchecked, index);
			}
		}
	}

	QByteArray userId = GetObjectIdFromInputParams(inputParams);

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(userId, dataPtr)){
		const imtauth::IUserInfo* userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(dataPtr.GetPtr());

		if (userInfoPtr == nullptr){
			errorMessage = QT_TR_NOOP("Unable to get an user info");
			return nullptr;
		}

		QByteArray id = userInfoPtr->GetId();
		QString username = userInfoPtr->GetUsername();
		QString name = userInfoPtr->GetName();
		QByteArray passwordHash = userInfoPtr->GetPasswordHash();
		QString mail = userInfoPtr->GetMail();

		dataModel->SetData("UserId", id);
		dataModel->SetData("Username", username);
		dataModel->SetData("Name", name);
		dataModel->SetData("Password", passwordHash);
		dataModel->SetData("Email", mail);

		const imtlic::IFeatureInfoProvider* featuresPtr = userInfoPtr->GetPermissionProvider();

		imtauth::IUserInfo::FeatureIds permissions = userInfoPtr->GetLocalPermissions();
		imtauth::IUserInfo::FeatureIds prohibitions = userInfoPtr->GetProhibitions();

		if (featuresPtr != nullptr){
			for (const QByteArray& id : permissions){
				const imtlic::IFeatureInfo* permissionPtr = featuresPtr->GetFeatureInfo(id);

				if (permissionPtr != nullptr){
					int index = permissionsModel->InsertNewItem();
					permissionsModel->SetData("PermissionId", permissionPtr->GetFeatureId(), index);
					permissionsModel->SetData("PermissionName", permissionPtr->GetFeatureName(), index);
				}
			}

			for (const QByteArray& id : prohibitions){
				const imtlic::IFeatureInfo* prohibitionPtr = featuresPtr->GetFeatureInfo(id);
				if (prohibitionPtr != nullptr){
					int index = prohibitionsModel->InsertNewItem();
					prohibitionsModel->SetData("ProhibitionId", prohibitionPtr->GetFeatureId(), index);
					prohibitionsModel->SetData("ProhibitionName", prohibitionPtr->GetFeatureName(), index);
				}
			}
		}

		imtauth::IUserInfo::RoleIds roles = userInfoPtr->GetRoles();

		for (const QByteArray& id : roles){
			for (int i = 0; i < rolesModel->GetItemsCount(); i++){
				QByteArray roleId = rolesModel->GetData("Id", i).toByteArray();

				if (id == roleId){
					rolesModel->SetData("State", Qt::CheckState::Checked, i);
				}
			}
		}
	}

	rootModel->SetExternTreeModel("data", dataModel);

	return rootModel;
}


istd::IChangeable* CUserControllerComp::CreateObject(
		const QList<imtgql::CGqlObject>& inputParams,
		QByteArray &objectId,
		QString &name,
		QString &description,
		QString& errorMessage) const
{
	if (!m_userInfoFactCompPtr.IsValid()){
		errorMessage = QObject::tr("Can not create User: %1").arg(QString(objectId));
		return nullptr;
	}

	QByteArray itemData = inputParams.at(0).GetFieldArgumentValue("Item").toByteArray();
	if (!itemData.isEmpty()){
		istd::TDelPtr<imtauth::CUserInfo> userInfoPtr = new imtauth::CUserInfo();
		if (userInfoPtr == nullptr){
			errorMessage = QT_TR_NOOP("Unable to get an account info!");
			return nullptr;
		}

		imtbase::CTreeItemModel itemModel;
		itemModel.Parse(itemData);

		if (itemModel.ContainsKey("Username")){
			QString username = itemModel.GetData("Username").toString();

			if (username.isEmpty()){
				errorMessage = QT_TR_NOOP("Username can't be empty!");

				return nullptr;
			}

			userInfoPtr->SetUsername(username);
		}

		if (itemModel.ContainsKey("Name")){
			name = itemModel.GetData("Name").toString();
			userInfoPtr->SetName(name);
		}

		if (itemModel.ContainsKey("Password")){
			QByteArray passwordHash = itemModel.GetData("Password").toByteArray();
			userInfoPtr->SetPasswordHash(passwordHash);
		}

		if (itemModel.ContainsKey("Email")){
			QString mail = itemModel.GetData("Email").toString();
			userInfoPtr->SetMail(mail);
		}

		if (itemModel.ContainsKey("Permissions")){
			imtbase::CTreeItemModel* permissionsModel = itemModel.GetTreeItemModel("Permissions");
			imtauth::IUserInfo::FeatureIds permissions;

			for(int index = 0; index < permissionsModel->GetItemsCount(); index++){
				QByteArray featureId;
				if (permissionsModel->ContainsKey("PermissionId")){
					featureId = permissionsModel->GetData("PermissionId", index).toByteArray();
					if (!featureId.isEmpty()){
						permissions << featureId;
					}
				}
			}

			userInfoPtr->SetLocalPermissions(permissions);
		}

		if (itemModel.ContainsKey("Prohibitions")){
			imtbase::CTreeItemModel* prohibitionsModel = itemModel.GetTreeItemModel("Prohibitions");
			imtauth::IUserInfo::FeatureIds prohibitions;

			for(int index = 0; index < prohibitionsModel->GetItemsCount(); index++){
				QByteArray featureId;
				if (prohibitionsModel->ContainsKey("ProhibitionId")){
					featureId = prohibitionsModel->GetData("ProhibitionId", index).toByteArray();
					if (!featureId.isEmpty()){
						prohibitions << featureId;
					}
				}
			}

			userInfoPtr->SetProhibitions(prohibitions);
		}

		if (itemModel.ContainsKey("Roles")){
			imtbase::CTreeItemModel* rolesModel = itemModel.GetTreeItemModel("Roles");
			imtauth::IUserInfo::RoleIds roles;

			for(int index = 0; index < rolesModel->GetItemsCount(); index++){
				if (rolesModel->ContainsKey("Id", index)){
					QByteArray roleId = rolesModel->GetData("Id", index).toByteArray();

					if (rolesModel->ContainsKey("State", index)){
						int state = rolesModel->GetData("State", index).toInt();

						if (state == Qt::CheckState::Checked){
							roles << roleId;
						}
					}
				}
			}

			userInfoPtr->SetRoles(roles);
		}

		return userInfoPtr.PopPtr();
	}

	errorMessage = QObject::tr("Can not create user: %1").arg(QString(objectId));

	return nullptr;
}


} // namespace imtauthgql


