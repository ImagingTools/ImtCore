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
    imtbase::CTreeItemModel* rootModel = BaseClass::GetObject(inputParams, gqlObject, errorMessage);
	imtbase::CTreeItemModel* dataModel = nullptr;
	imtbase::CTreeItemModel* itemModel = nullptr;

	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();
	}

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsItemModel = rootModel->AddTreeModel("errors");
		errorsItemModel->SetData("message", errorMessage);
	}
	else{
		dataModel = new imtbase::CTreeItemModel();
		itemModel = new imtbase::CTreeItemModel();

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

            itemModel->SetData("UserId", id);
			itemModel->SetData("Username", username);
			itemModel->SetData("Name", name);
            itemModel->SetData("PasswordHash", passwordHash);
			itemModel->SetData("Mail", mail);

            imtbase::CTreeItemModel* permissionsModel = new imtbase::CTreeItemModel();
            imtbase::CTreeItemModel* prohibitionsModel = new imtbase::CTreeItemModel();
            imtbase::CTreeItemModel* rolesModel = new imtbase::CTreeItemModel();

            const imtlic::IFeatureInfoProvider* featuresPtr = userInfoPtr->GetPermissionProvider();
            imtauth::IUserInfo::FeatureIds permissions = userInfoPtr->GetPermissions();
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
                itemModel->SetExternTreeModel("Permissions", permissionsModel);
                for (const QByteArray& id : prohibitions){
                    const imtlic::IFeatureInfo* prohibitionPtr = featuresPtr->GetFeatureInfo(id);
                    if (prohibitionPtr != nullptr){
                        int index = prohibitionsModel->InsertNewItem();
                        prohibitionsModel->SetData("ProhibitionId", prohibitionPtr->GetFeatureId(), index);
                        prohibitionsModel->SetData("ProhibitionName", prohibitionPtr->GetFeatureName(), index);
                    }
                }
                itemModel->SetExternTreeModel("Prohibitions", prohibitionsModel);
            }
            const imtauth::IRoleInfoProvider* rolesPtr = userInfoPtr->GetRoleProvider();
            imtauth::IUserInfo::RoleIds roles = userInfoPtr->GetRoles();
            if (rolesPtr != nullptr){
                for (const QByteArray& id : roles){
                    const imtauth::IRole* rolePtr = rolesPtr->GetRole(id);
                    if (rolePtr != nullptr){
                        int index = rolesModel->InsertNewItem();
                        rolesModel->SetData("RoleId", rolePtr->GetRoleId(), index);
                        rolesModel->SetData("RoleName", rolePtr->GetRoleName(), index);
                    }
                }
                itemModel->SetExternTreeModel("Roles", rolesModel);
            }
		}
		dataModel->SetExternTreeModel("item", itemModel);
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
		imtauth::CUserInfo *userInfoPtr = new imtauth::CUserInfo();
		if (userInfoPtr == nullptr){
			errorMessage = QT_TR_NOOP("Unable to get an account info!");
			return nullptr;
		}

		imtbase::CTreeItemModel itemModel;
		itemModel.Parse(itemData);

        if (itemModel.ContainsKey("UserId")){
            QByteArray userId = itemModel.GetData("UserId").toByteArray();
			if (!userId.isEmpty()){
				userInfoPtr->SetId(userId);
			}
			else{
				if (objectId.isEmpty()){
					errorMessage = QT_TR_NOOP("User Id can't be empty!");
					return nullptr;
				}
				else{
					userInfoPtr->SetId(objectId);
				}
			}
		}

		if (itemModel.ContainsKey("Username")){
			QString username = itemModel.GetData("Username").toString();
			userInfoPtr->SetUsername(username);
		}

		if (itemModel.ContainsKey("Name")){
			name = itemModel.GetData("Username").toString();
			userInfoPtr->SetUsername(name);
		}

		if (itemModel.ContainsKey("PasswordHash")){
			QByteArray passwordHash = itemModel.GetData("PasswordHash").toByteArray();
			userInfoPtr->SetPasswordHash(passwordHash);
		}

		if (itemModel.ContainsKey("Mail")){
			QString mail = itemModel.GetData("Mail").toString();
			userInfoPtr->SetMail(mail);
		}

        if (itemModel.ContainsKey("Permissions")){
            imtbase::CTreeItemModel* permissionsModel = nullptr;
            permissionsModel = itemModel.GetTreeItemModel("Permissions");
            imtauth::IUserInfo::FeatureIds *permissions = nullptr;
            for(int index = 0; index < permissionsModel->GetItemsCount(); index++){
                QByteArray featureId;
                if (permissionsModel->ContainsKey("PermissionId")){
                    featureId = permissionsModel->GetData("PermissionId", index).toByteArray();
                    if (!featureId.isEmpty()){
                        permissions->insert(featureId);
                    }
                }
            }
            userInfoPtr->SetLocalPermissions(*permissions);
        }
        if (itemModel.ContainsKey("Prohibitions")){
            imtbase::CTreeItemModel* prohibitionsModel = nullptr;
            prohibitionsModel = itemModel.GetTreeItemModel("Prohibitions");
            imtauth::IUserInfo::FeatureIds *prohibitions = nullptr;
            for(int index = 0; index < prohibitionsModel->GetItemsCount(); index++){
                QByteArray featureId;
                if (prohibitionsModel->ContainsKey("ProhibitionId")){
                    featureId = prohibitionsModel->GetData("ProhibitionId", index).toByteArray();
                    if (!featureId.isEmpty()){
                        prohibitions->insert(featureId);
                    }
                }
            }
            userInfoPtr->SetProhibitions(*prohibitions);
        }
        if (itemModel.ContainsKey("Roles")){
            imtbase::CTreeItemModel* rolesModel = nullptr;
            rolesModel = itemModel.GetTreeItemModel("Roles");
            imtauth::IUserInfo::RoleIds *roles = nullptr;
            for(int index = 0; index < rolesModel->GetItemsCount(); index++){
                QByteArray roleId;
                if (rolesModel->ContainsKey("RoleId")){
                    roleId = rolesModel->GetData("RoleId", index).toByteArray();
                    if (!roleId.isEmpty()){
                        roles->insert(roleId);
                    }
                }
            }
            userInfoPtr->SetRoles(*roles);
        }
		return userInfoPtr;
	}

	errorMessage = QObject::tr("Can not create user: %1").arg(QString(objectId));

	return nullptr;
}


} // namespace imtauthgql


