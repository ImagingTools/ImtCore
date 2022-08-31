#include <imtauthgql/CRoleControllerComp.h>


// ImtCore includes
#include <imtauth/CRole.h>
#include <imtlic/CFeatureInfo.h>


namespace imtauthgql
{


imtbase::CTreeItemModel *CRoleControllerComp::GetObject(const QList<imtgql::CGqlObject> &inputParams, const imtgql::CGqlObject &gqlObject, QString &errorMessage) const
{
	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();
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

		QByteArray roleId = GetObjectIdFromInputParams(inputParams);

		QByteArrayList featureIds;
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_objectCollectionCompPtr->GetObjectData(roleId, dataPtr)){
			const imtauth::IRole* roleInfoPtr = dynamic_cast<const imtauth::IRole*>(dataPtr.GetPtr());

			if (roleInfoPtr == nullptr){
				errorMessage = QT_TR_NOOP("Unable to get a role info");
				return nullptr;
			}

			QByteArray roleId = roleInfoPtr->GetRoleId();
			QString roleName = roleInfoPtr->GetRoleName();

            itemModel->SetData("RoleId", roleId);
            itemModel->SetData("RoleName", roleName);

			imtbase::CTreeItemModel* permissionsModel = new imtbase::CTreeItemModel();
			imtbase::CTreeItemModel* prohibitionsModel = new imtbase::CTreeItemModel();
			const imtlic::IFeatureInfoProvider* featuresPtr = roleInfoPtr->GetPermissionProvider();
			imtauth::IRole::FeatureIds permissions = roleInfoPtr->GetPermissions();
			imtauth::IRole::FeatureIds prohibitions = roleInfoPtr->GetProhibitions();
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
		}
		dataModel->SetExternTreeModel("item", itemModel);
	}
	rootModel->SetExternTreeModel("data", dataModel);

	return rootModel;
}


istd::IChangeable *CRoleControllerComp::CreateObject(const QList<imtgql::CGqlObject> &inputParams, QByteArray &objectId, QString &name, QString &description, QString &errorMessage) const
{
	if (!m_roleFactCompPtr.IsValid()){
		errorMessage = QObject::tr("Can not create role: %1").arg(QString(objectId));
		return nullptr;
	}

	QByteArray itemData = inputParams.at(0).GetFieldArgumentValue("Item").toByteArray();
	if (!itemData.isEmpty()){
		imtauth::CRole *roleInfoPtr = new imtauth::CRole();
		if (roleInfoPtr == nullptr){
			errorMessage = QT_TR_NOOP("Unable to get a role!");
			return nullptr;
		}

		imtbase::CTreeItemModel itemModel;
		itemModel.Parse(itemData);

        if (itemModel.ContainsKey("RoleId")){
            QByteArray roleId = itemModel.GetData("RoleId").toByteArray();
			if (roleId.isEmpty()){
				if (objectId.isEmpty()){
					errorMessage = QT_TR_NOOP("Role id can't be empty!");
					return nullptr;
				}
				else{
					roleInfoPtr->SetRoleId(objectId);
				}
			}
			else{
				roleInfoPtr->SetRoleId(roleId);
			}
		}

        if (itemModel.ContainsKey("RoleName")){
            name = itemModel.GetData("RoleName").toString();
			roleInfoPtr->SetRoleName(name);
		}

		if (itemModel.ContainsKey("Permissions")){
			imtbase::CTreeItemModel* permissionsModel = nullptr;
			permissionsModel = itemModel.GetTreeItemModel("Permissions");
			imtauth::IRole::FeatureIds *permissions = nullptr;
			for(int index = 0; index < permissionsModel->GetItemsCount(); index++){
				QByteArray featureId;
                if (permissionsModel->ContainsKey("PermissionId")){
                    featureId = permissionsModel->GetData("PermissionId", index).toByteArray();
					if (!featureId.isEmpty()){
						permissions->insert(featureId);
					}
				}
			}
			roleInfoPtr->SetLocalPermissions(*permissions);
		}
		if (itemModel.ContainsKey("Prohibitions")){
			imtbase::CTreeItemModel* prohibitionsModel = nullptr;
			prohibitionsModel = itemModel.GetTreeItemModel("Prohibitions");
			imtauth::IRole::FeatureIds *prohibitions = nullptr;
			for(int index = 0; index < prohibitionsModel->GetItemsCount(); index++){
				QByteArray featureId;
                if (prohibitionsModel->ContainsKey("ProhibitionId")){
                    featureId = prohibitionsModel->GetData("ProhibitionId", index).toByteArray();
					if (!featureId.isEmpty()){
						prohibitions->insert(featureId);
					}
				}
			}
			roleInfoPtr->SetProhibitions(*prohibitions);
		}

		return roleInfoPtr;
	}

	errorMessage = QObject::tr("Can not create roke: %1").arg(QString(objectId));

	return nullptr;
}


} // namespace imtauth
