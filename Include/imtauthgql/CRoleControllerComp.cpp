#include <imtauthgql/CRoleControllerComp.h>


// ImtCore includes
#include <imtauth/CRole.h>
#include <imtlic/CFeatureInfo.h>


namespace imtauthgql
{


imtbase::CTreeItemModel *CRoleControllerComp::GetObject(
		const QList<imtgql::CGqlObject> &inputParams,
		const imtgql::CGqlObject &gqlObject,
		QString &errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid() || !m_productCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();

		return nullptr;
	}

	imtbase::CTreeItemModel* dataModel = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* permissionsModel = dataModel->AddTreeModel("Permissions");
	imtbase::CTreeItemModel* parentsModel = dataModel->AddTreeModel("Parents");

	QByteArray objectId = GetObjectIdFromInputParams(inputParams);
	QByteArray productId = inputParams.at(0).GetFieldArgumentValue("ProductId").toByteArray();

	QByteArray roleId;
	if (m_separatorObjectIdAttrPtr.IsValid()){
		QStringList data = QString(objectId).split(*m_separatorObjectIdAttrPtr);
		roleId = qPrintable(data[0]);
	}

	dataModel->SetData("Id", roleId);
	dataModel->SetData("ProductId", productId);
	dataModel->SetData("Name", "");
	dataModel->SetData("Description", "");

	imtbase::IObjectCollection::DataPtr productDataPtr;
	if (m_productCollectionCompPtr->GetObjectData(productId, productDataPtr)){
		const imtlic::IProductLicensingInfo* productPtr = dynamic_cast<const imtlic::IProductLicensingInfo*>(productDataPtr.GetPtr());

		const imtbase::ICollectionInfo& licenseList = productPtr->GetLicenseList();
		const imtbase::IObjectCollectionInfo::Ids licenseCollectionIds = licenseList.GetElementIds();

		for (const QByteArray& licenseId : licenseCollectionIds){
			const imtlic::ILicenseInfo* licenseInfoPtr = productPtr->GetLicenseInfo(licenseId);
			if (licenseInfoPtr == nullptr){
				continue;
			}

			imtlic::ILicenseInfo::FeatureInfos featureInfos = licenseInfoPtr->GetFeatureInfos();
			for (int i = 0; i < featureInfos.size(); i++){
				QByteArray featureId = featureInfos[i].id;
				QString featureName = featureInfos[i].name;

				int index = permissionsModel->InsertNewItem();
				permissionsModel->SetData("Id", featureId, index);
				permissionsModel->SetData("Name", featureName, index);
				permissionsModel->SetData("State", Qt::CheckState::Unchecked, index);
			}
		}
	}

	imtbase::IObjectCollection::DataPtr dataPtr;

	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		const imtauth::IRole* roleInfoPtr = dynamic_cast<const imtauth::IRole*>(dataPtr.GetPtr());

		if (roleInfoPtr == nullptr){
			errorMessage = QT_TR_NOOP("Unable to get a role info");
			return nullptr;
		}

		QByteArray roleId = roleInfoPtr->GetRoleId();
		QString roleName = roleInfoPtr->GetRoleName();

		dataModel->SetData("Name", roleName);

		QString description = m_objectCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();
		dataModel->SetData("Description", description);

		const QByteArrayList parentsRolesIds  = roleInfoPtr->GetIncludedRoles();

		for (const QByteArray& parentRoleId : parentsRolesIds){
			QByteArray objectRoleParentId = parentRoleId + *m_separatorObjectIdAttrPtr + productId;
			imtbase::IObjectCollection::DataPtr parentRoleDataPtr;
			if (m_objectCollectionCompPtr->GetObjectData(objectRoleParentId, parentRoleDataPtr)){
				const imtauth::IRole* role = dynamic_cast<const imtauth::IRole*>(parentRoleDataPtr.GetPtr());
				if (role != nullptr){
					QString parentRoleName = role->GetRoleName();

					int index = parentsModel->InsertNewItem();
					parentsModel->SetData("Id", parentRoleId, index);
					parentsModel->SetData("Name", parentRoleName, index);
				}
			}
		}

		imtauth::IRole::FeatureIds permissions = roleInfoPtr->GetLocalPermissions();

		for (const QByteArray& permissionId : permissions){
			for (int i = 0; i < permissionsModel->GetItemsCount(); i++){
				QByteArray id = permissionsModel->GetData("Id", i).toByteArray();

				if (permissionId == id){
					permissionsModel->SetData("State", Qt::CheckState::Checked, i);
				}
			}
		}
	}

	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();
	rootModel->SetExternTreeModel("data", dataModel);

	return rootModel;
}


istd::IChangeable *CRoleControllerComp::CreateObject(
		const QList<imtgql::CGqlObject> &inputParams,
		QByteArray &objectId, QString &name,
		QString &description,
		QString &errorMessage) const
{
	if (!m_roleFactCompPtr.IsValid()){
		errorMessage = QObject::tr("Can not create role: %1").arg(QString(objectId));
		return nullptr;
	}

	QByteArray itemData = inputParams.at(0).GetFieldArgumentValue("Item").toByteArray();
	if (!itemData.isEmpty()){
		istd::TDelPtr<imtauth::IRole> roleInfoPtr = m_roleFactCompPtr.CreateInstance();

		if (roleInfoPtr == nullptr){
			errorMessage = QT_TR_NOOP("Unable to get a role!");
			return nullptr;
		}

		imtbase::CTreeItemModel itemModel;
		itemModel.Parse(itemData);

		QByteArray roleId;
		if (itemModel.ContainsKey("Id")){
			roleId = itemModel.GetData("Id").toByteArray();
			if (roleId.isEmpty()){

				errorMessage = QT_TR_NOOP("Role-ID can't be empty!");
				return nullptr;
			}

			roleInfoPtr->SetRoleId(roleId);
		}

		if (itemModel.ContainsKey("Name")){
			name = itemModel.GetData("Name").toString();
			roleInfoPtr->SetRoleName(name);
		}

		QByteArray productId;
		if (itemModel.ContainsKey("ProductId")){
			productId = itemModel.GetData("ProductId").toByteArray();

			if (productId.isEmpty()){
				errorMessage = QT_TR_NOOP("Product-ID can't be empty!");
				return nullptr;
			}

			roleInfoPtr->SetProductId(productId);
		}

		if (m_separatorObjectIdAttrPtr.IsValid()){
			objectId = roleId + *m_separatorObjectIdAttrPtr + productId;
		}

		if (itemModel.ContainsKey("Parents")){
			imtbase::CTreeItemModel* parentsModel = itemModel.GetTreeItemModel("Parents");

			for(int index = 0; index < parentsModel->GetItemsCount(); index++){
				QByteArray parentRoleId = parentsModel->GetData("Id", index).toByteArray();
				QString parentRoleIdStr = parentRoleId;
				parentRoleId = parentRoleIdStr.split(*m_separatorObjectIdAttrPtr)[0].toUtf8();

				if (!roleInfoPtr->IncludeRole(parentRoleId)){
					return nullptr;
				}
			}
		}

		if (itemModel.ContainsKey("Permissions")){
			imtbase::CTreeItemModel* permissionsModel = itemModel.GetTreeItemModel("Permissions");

			imtauth::IRole::FeatureIds permissions;
			imtauth::IRole::FeatureIds prohibitions;

			for(int index = 0; index < permissionsModel->GetItemsCount(); index++){
				QByteArray featureId = permissionsModel->GetData("Id", index).toByteArray();

				int state = permissionsModel->GetData("State", index).toInt();

				if (state == Qt::Checked){
					permissions << featureId;
				}
//				else if (state == Qt::Unchecked){
//					prohibitions << featureId;
//				}
			}

			roleInfoPtr->SetLocalPermissions(permissions);
			roleInfoPtr->SetProhibitions(prohibitions);
		}

		return roleInfoPtr.PopPtr();
	}

	return nullptr;
}


} // namespace imtauthgql
