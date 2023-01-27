#include <imtauthgql/CRoleControllerComp.h>


// ImtCore includes
#include <imtauth/CRole.h>
#include <imtlic/CFeatureInfo.h>
#include <imtlic/CFeaturePackage.h>


namespace imtauthgql
{


imtbase::CTreeItemModel* CRoleControllerComp::GetObject(const imtgql::CGqlRequest& gqlRequest, QString &errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();

		return nullptr;
	}

	imtbase::CTreeItemModel* dataModelPtr = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* permissionsModelPtr = dataModelPtr->AddTreeModel("Permissions");
	imtbase::CTreeItemModel* parentsModelPtr = dataModelPtr->AddTreeModel("Parents");

	const QList<imtgql::CGqlObject>* inputParams = gqlRequest.GetParams();
	QByteArray objectId = GetObjectIdFromInputParams(*inputParams);
	QByteArray productId = inputParams->at(0).GetFieldArgumentValue("ProductId").toByteArray();

	QByteArray roleId;
	if (m_separatorObjectIdAttrPtr.IsValid()){
		QStringList data = QString(objectId).split(*m_separatorObjectIdAttrPtr);
		roleId = qPrintable(data[0]);
	}

	dataModelPtr->SetData("Id", roleId);
	dataModelPtr->SetData("ProductId", productId);
	dataModelPtr->SetData("Name", "");
	dataModelPtr->SetData("Description", "");

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		const imtauth::IRole* roleInfoPtr = dynamic_cast<const imtauth::IRole*>(dataPtr.GetPtr());
		if (roleInfoPtr == nullptr){
			errorMessage = QT_TR_NOOP("Unable to get a role info");
			return nullptr;
		}

		QByteArray roleId = roleInfoPtr->GetRoleId();
		QString roleName = roleInfoPtr->GetRoleName();

		dataModelPtr->SetData("Name", roleName);

		QString description = m_objectCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();
		dataModelPtr->SetData("Description", description);

		const QByteArrayList parentsRolesIds  = roleInfoPtr->GetIncludedRoles();
		for (const QByteArray& parentRoleId : parentsRolesIds){
			QByteArray objectRoleParentId = parentRoleId + *m_separatorObjectIdAttrPtr + productId;
			imtbase::IObjectCollection::DataPtr parentRoleDataPtr;
			if (m_objectCollectionCompPtr->GetObjectData(objectRoleParentId, parentRoleDataPtr)){
				const imtauth::IRole* role = dynamic_cast<const imtauth::IRole*>(parentRoleDataPtr.GetPtr());
				if (role != nullptr){
					QString parentRoleName = role->GetRoleName();

					int index = parentsModelPtr->InsertNewItem();
					parentsModelPtr->SetData("Id", parentRoleId, index);
					parentsModelPtr->SetData("Name", parentRoleName, index);
				}
			}
		}

		imtauth::IRole::FeatureIds permissions = roleInfoPtr->GetLocalPermissions();
		for (const QByteArray& permissionId : permissions){
			int index = permissionsModelPtr->InsertNewItem();
			permissionsModelPtr->SetData("Id", permissionId, index);
		}
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	rootModelPtr->SetExternTreeModel("data", dataModelPtr);

	return rootModelPtr.PopPtr();
}


istd::IChangeable *CRoleControllerComp::CreateObject(
		const QList<imtgql::CGqlObject>& inputParams,
		QByteArray& objectId,
		QString& name,
		QString& description,
		QString& errorMessage) const
{
	if (!m_roleFactCompPtr.IsValid() || !m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT(false);
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
		itemModel.CreateFromJson(itemData);

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

		if (itemModel.ContainsKey("Description")){
			description = itemModel.GetData("Description").toString();
			roleInfoPtr->SetRoleDescription(description);
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

		imtbase::ICollectionInfo::Ids elementIds = m_objectCollectionCompPtr->GetElementIds();
		if (elementIds.contains(objectId)){
			errorMessage = QT_TR_NOOP("Role with this ID already exists");

			return nullptr;
		}

		if (itemModel.ContainsKey("Parents")){
			imtbase::CTreeItemModel* parentsModelPtr = itemModel.GetTreeItemModel("Parents");

			Q_ASSERT(parentsModelPtr != nullptr);

			for(int index = 0; index < parentsModelPtr->GetItemsCount(); index++){
				QByteArray parentRoleId = parentsModelPtr->GetData("Id", index).toByteArray();
				QString parentRoleIdStr = parentRoleId;
				parentRoleId = parentRoleIdStr.split(*m_separatorObjectIdAttrPtr)[0].toUtf8();

				if (!roleInfoPtr->IncludeRole(parentRoleId)){
					return nullptr;
				}
			}
		}

		if (itemModel.ContainsKey("Permissions")){
			imtbase::CTreeItemModel* permissionsModelPtr = itemModel.GetTreeItemModel("Permissions");
			if (permissionsModelPtr != nullptr){
				imtauth::IRole::FeatureIds permissions;

				for(int index = 0; index < permissionsModelPtr->GetItemsCount(); index++){
					QByteArray featureId;
					if (permissionsModelPtr->ContainsKey("Id", index)){
						featureId = permissionsModelPtr->GetData("Id", index).toByteArray();
					}

					if (!featureId.isEmpty()){
						permissions << featureId;
					}
				}

				roleInfoPtr->SetLocalPermissions(permissions);
			}
		}

		return roleInfoPtr.PopPtr();
	}

	return nullptr;
}


} // namespace imtauthgql
