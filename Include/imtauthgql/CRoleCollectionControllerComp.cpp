#include <imtauthgql/CRoleCollectionControllerComp.h>


// ACF includes
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imtlic/CFeatureInfo.h>
#include <imtauth/CRole.h>


namespace imtauthgql
{


// protected methods

// reimplemented (imtguigql::CObjectCollectionControllerCompBase)

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

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = new imtbase::CTreeItemModel();

	QByteArray roleId = GetObjectIdFromInputParams(*gqlRequest.GetParams());

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(roleId, dataPtr)){
		const imtauth::IRole* roleInfoPtr = dynamic_cast<const imtauth::IRole*>(dataPtr.GetPtr());

		if (roleInfoPtr == nullptr){
			errorMessage = QT_TR_NOOP("Unable to get a role info");
			return nullptr;
		}

		QByteArray roleId = roleInfoPtr->GetRoleId();
		QString roleName = roleInfoPtr->GetRoleName();
		QByteArray roleProductId = roleInfoPtr->GetProductId();

		int index = dataModelPtr->InsertNewItem();
		dataModelPtr->SetData("Name", "Included Roles", index);
		imtbase::CTreeItemModel* children = dataModelPtr->AddTreeModel("Children", index);

		QByteArrayList parentRolesIds = roleInfoPtr->GetIncludedRoles();

		for (const QByteArray& parentRoleId : parentRolesIds){
			QByteArray objectId = parentRoleId + *m_separatorObjectIdAttrPtr + roleProductId;

			imtbase::IObjectCollection::DataPtr parentDataPtr;
			if (m_objectCollectionCompPtr->GetObjectData(objectId, parentDataPtr)){
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

		imtauth::IRole::FeatureIds permissionsIds = roleInfoPtr->GetLocalPermissions();

		for (const QByteArray& permissionId : permissionsIds){
			int childrenIndex = children->InsertNewItem();

			children->SetData("Value", permissionId, childrenIndex);
		}
	}

	rootModelPtr->SetExternTreeModel("data", dataModelPtr);

	return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CRoleCollectionControllerComp::ListObjects(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid() || !m_productProviderCompPtr.IsValid()){
		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");
	Q_ASSERT(dataModelPtr != nullptr);

	imtbase::CTreeItemModel* itemsModelPtr = m_productProviderCompPtr->CreateResponse(gqlRequest, errorMessage);
	if (itemsModelPtr != nullptr){
		for (int i = 0; i < itemsModelPtr->GetItemsCount(); i++){
			QByteArray productId = itemsModelPtr->GetData("Id", i).toByteArray();
			imtbase::CTreeItemModel* rolesModelPtr = itemsModelPtr->AddTreeModel("Roles", i);
			Q_ASSERT(rolesModelPtr != nullptr);

			imtbase::ICollectionInfo::Ids rolesIds = m_objectCollectionCompPtr->GetElementIds();
			for (const imtbase::ICollectionInfo::Id& roleObjectId : rolesIds){
				imtbase::IObjectCollection::DataPtr dataPtr;
				if (m_objectCollectionCompPtr->GetObjectData(roleObjectId, dataPtr)){
					const imtauth::IRole* roleInfoPtr = dynamic_cast<const imtauth::IRole*>(dataPtr.GetPtr());
					if (roleInfoPtr != nullptr){
						QByteArray roleId = roleInfoPtr->GetRoleId();
						QByteArray roleProductId = roleInfoPtr->GetProductId();

						if (productId == roleProductId){
							int roleIndex = rolesModelPtr->InsertNewItem();

							QByteArray objectId = roleInfoPtr->GetRoleId() + *m_separatorObjectIdAttrPtr + roleProductId;

							rolesModelPtr->SetData("Id", objectId, roleIndex);
							rolesModelPtr->SetData("Name", roleInfoPtr->GetRoleName(), roleIndex);
						}
					}
				}
			}
		}
	}

	dataModelPtr->SetExternTreeModel("items", itemsModelPtr);

	return rootModelPtr.PopPtr();
}


} // namespace imtauth


