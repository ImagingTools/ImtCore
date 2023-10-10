#include <imtauthgql/CRoleCollectionControllerComp.h>


// ACF includes
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imtlic/CFeatureInfo.h>
#include <imtauth/CRole.h>


namespace imtauthgql
{


// protected methods

// reimplemented (imtgql::CObjectCollectionControllerCompBase)

QVariant CRoleCollectionControllerComp::GetObjectInformation(const QByteArray &informationId, const QByteArray &objectId) const
{
	if (informationId == QByteArray("Added")){
		idoc::MetaInfoPtr metaInfo = m_objectCollectionCompPtr->GetDataMetaInfo(objectId);;
		if (metaInfo.IsValid()){
			return metaInfo->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_CREATION_TIME);
		}
	}
	else if (informationId == QByteArray("LastModified")){
		idoc::MetaInfoPtr metaInfo = m_objectCollectionCompPtr->GetDataMetaInfo(objectId);
		if (metaInfo.IsValid()){
			return metaInfo->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME);
		}
	}

	return QVariant();
}


imtbase::CTreeItemModel* CRoleCollectionControllerComp::GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return nullptr;
	}

	const QList<imtgql::CGqlObject> paramsPtr = gqlRequest.GetParams();

	QByteArray productId;
	if (!paramsPtr.isEmpty()){
		productId = paramsPtr.at(0).GetFieldArgumentValue("ProductId").toByteArray();
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = new imtbase::CTreeItemModel();

	QByteArray roleObjectId = GetObjectIdFromInputParams(paramsPtr);

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(roleObjectId, dataPtr)){
		const imtauth::IRole* roleInfoPtr = dynamic_cast<const imtauth::IRole*>(dataPtr.GetPtr());

		if (roleInfoPtr == nullptr){
			errorMessage = QT_TR_NOOP("Unable to get a role info");
			return nullptr;
		}

		QByteArray roleId = roleInfoPtr->GetRoleId();
		QString roleName = roleInfoPtr->GetRoleName();
		QByteArray roleProductId = roleInfoPtr->GetProductId();

		int index = dataModelPtr->InsertNewItem();
		dataModelPtr->SetData("Name", "Parent Roles", index);
		imtbase::CTreeItemModel* children = dataModelPtr->AddTreeModel("Children", index);

		QByteArrayList parentRolesIds = roleInfoPtr->GetIncludedRoles();

		for (const QByteArray& parentRoleId : parentRolesIds){
			imtbase::IObjectCollection::DataPtr parentDataPtr;
			if (m_objectCollectionCompPtr->GetObjectData(parentRoleId, parentDataPtr)){
				const imtauth::IRole* parentRoleInfoPtr = dynamic_cast<const imtauth::IRole*>(parentDataPtr.GetPtr());
				if (parentRoleInfoPtr != nullptr){
					QString parentRoleName = parentRoleInfoPtr->GetRoleName();

					int childrenIndex = children->InsertNewItem();

					children->SetData("Value", parentRoleName, childrenIndex);
				}
			}
		}

		index = dataModelPtr->InsertNewItem();

		dataModelPtr->SetData("Name", "Permissions", index);
		children = dataModelPtr->AddTreeModel("Children", index);

		imtauth::IRole::FeatureIds permissionsIds = roleInfoPtr->GetPermissions();

		for (const QByteArray& permissionId : permissionsIds){
			int childrenIndex = children->InsertNewItem();

			children->SetData("Value", permissionId, childrenIndex);
		}
	}

	rootModelPtr->SetExternTreeModel("data", dataModelPtr);

	return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CRoleCollectionControllerComp::ListObjects(const imtgql::CGqlRequest& gqlRequest, QString& /*errorMessage*/) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");
	Q_ASSERT(dataModelPtr != nullptr);

	QByteArray productId;
	if (m_productIdAttrPtr.IsValid()){
		productId = *m_productIdAttrPtr;
	}

	const imtgql::CGqlObject* inputParams = gqlRequest.GetParam("input");
	if (inputParams != nullptr){
		if (productId.isEmpty()){
			productId = inputParams->GetFieldArgumentValue("ProductId").toByteArray();
		}
	}

	imtbase::CTreeItemModel* itemsModelPtr = dataModelPtr->AddTreeModel("items");

	itemsModelPtr->SetData("Id", productId);
	itemsModelPtr->SetData("Name", productId);
	imtbase::CTreeItemModel* rolesModelPtr = itemsModelPtr->AddTreeModel("Roles");

	imtbase::ICollectionInfo::Ids rolesIds = m_objectCollectionCompPtr->GetElementIds();
	for (const imtbase::ICollectionInfo::Id& roleId : rolesIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_objectCollectionCompPtr->GetObjectData(roleId, dataPtr)){
			const imtauth::IRole* roleInfoPtr = dynamic_cast<const imtauth::IRole*>(dataPtr.GetPtr());
			if (roleInfoPtr != nullptr){
				QByteArray roleProductId = roleInfoPtr->GetProductId();
				if (productId == roleProductId){
					int roleIndex = rolesModelPtr->InsertNewItem();

					rolesModelPtr->SetData("Id", roleId, roleIndex);
					rolesModelPtr->SetData("RoleId", roleInfoPtr->GetRoleId(), roleIndex);
					rolesModelPtr->SetData("ProductId", productId, roleIndex);
					rolesModelPtr->SetData("Name", roleInfoPtr->GetRoleName(), roleIndex);
					rolesModelPtr->SetData("Description", roleInfoPtr->GetRoleDescription(), roleIndex);
					rolesModelPtr->SetData("ParentRoles", roleInfoPtr->GetIncludedRoles().join(';'), roleIndex);
				}
			}
		}
	}

	return rootModelPtr.PopPtr();
}


} // namespace imtauth


