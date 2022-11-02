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


imtbase::CTreeItemModel* CRoleCollectionControllerComp::GetMetaInfo(
		const QList<imtgql::CGqlObject> &inputParams,
		const imtgql::CGqlObject &gqlObject,
		QString &errorMessage) const
{
	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* dataModel = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* metaInfoModel = new imtbase::CTreeItemModel();

	QByteArray roleId = GetObjectIdFromInputParams(inputParams);

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(roleId, dataPtr)){
		const imtauth::IRole* roleInfoPtr = dynamic_cast<const imtauth::IRole*>(dataPtr.GetPtr());

		if (roleInfoPtr == nullptr){
			errorMessage = QT_TR_NOOP("Unable to get a role info");
			return nullptr;
		}

		QByteArray roleId = roleInfoPtr->GetRoleId();
		QString roleName = roleInfoPtr->GetRoleName();

		int index = metaInfoModel->InsertNewItem();
		metaInfoModel->SetData("Name", "Included Roles", index);
		imtbase::CTreeItemModel* children = metaInfoModel->AddTreeModel("Children", index);

		QByteArrayList parentRolesIds = roleInfoPtr->GetIncludedRoles();

		for (const QByteArray& parentRoleId : parentRolesIds){
			int childrenIndex = children->InsertNewItem();

			children->SetData("Value", parentRoleId, childrenIndex);
		}

		index = metaInfoModel->InsertNewItem();
		metaInfoModel->SetData("Name", "Permissions", index);
		children = metaInfoModel->AddTreeModel("Children", index);

		imtauth::IRole::FeatureIds permissions = roleInfoPtr->GetLocalPermissions();

		for (const QByteArray& id : permissions){
			int childrenIndex = children->InsertNewItem();

			children->SetData("Value", id, childrenIndex);
		}

		dataModel->SetExternTreeModel("metaInfo", metaInfoModel);
	}
	rootModel->SetExternTreeModel("data", dataModel);

	return rootModel;
}


imtbase::CTreeItemModel* CRoleCollectionControllerComp::ListObjects(
			const QList<imtgql::CGqlObject>& inputParams,
			const imtgql::CGqlObject& gqlObject,
			QString& errorMessage) const
{
	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* dataModel = nullptr;

	if (!m_objectCollectionCompPtr.IsValid() || !m_productProviderCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();
	}

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsItemModel = rootModel->AddTreeModel("errors");
		errorsItemModel->SetData("message", errorMessage);
	}
	else{
		dataModel = new imtbase::CTreeItemModel();
		imtbase::CTreeItemModel* itemsModel = m_productProviderCompPtr->GetTreeItemModel(inputParams, QByteArrayList());
		if (itemsModel != nullptr){
			for (int i = 0; i < itemsModel->GetItemsCount(); i++){
				QByteArray productId = itemsModel->GetData("Id", i).toByteArray();
				imtbase::CTreeItemModel* rolesModel = itemsModel->AddTreeModel("Roles", i);

				imtbase::ICollectionInfo::Ids rolesIds = m_objectCollectionCompPtr->GetElementIds();

				for (const imtbase::ICollectionInfo::Id& roleObjectId : rolesIds){
					imtbase::IObjectCollection::DataPtr dataPtr;
					if (m_objectCollectionCompPtr->GetObjectData(roleObjectId, dataPtr)){
						const imtauth::IRole* roleInfoPtr = dynamic_cast<const imtauth::IRole*>(dataPtr.GetPtr());

						QByteArray roleId = roleInfoPtr->GetRoleId();
						QByteArray roleProductId = roleInfoPtr->GetProductId();

						if (productId == roleProductId){
							int roleIndex = rolesModel->InsertNewItem();

							QByteArray objectId = roleInfoPtr->GetRoleId() + *m_separatorObjectIdAttrPtr + roleProductId;

							rolesModel->SetData("Id", objectId, roleIndex);
							rolesModel->SetData("Name", roleInfoPtr->GetRoleName(), roleIndex);
						}
					}
				}
			}
		}
//		imtbase::ICollectionInfo::Ids productsIds = m_productCollectionCompPtr->GetElementIds();

//		for (const imtbase::ICollectionInfo::Id& productId : productsIds){
//			QString productName = m_productCollectionCompPtr->GetElementInfo(productId, imtbase::ICollectionInfo::EIT_NAME).toString();

//			int index = itemsModel->InsertNewItem();
//			itemsModel->SetData("Id", productId, index);
//			itemsModel->SetData("Name", productName, index);

//			imtbase::CTreeItemModel* rolesModel = itemsModel->AddTreeModel("Roles", index);

//			imtbase::ICollectionInfo::Ids rolesIds = m_objectCollectionCompPtr->GetElementIds();

//			for (const imtbase::ICollectionInfo::Id& roleObjectId : rolesIds){
//				imtbase::IObjectCollection::DataPtr dataPtr;
//				if (m_objectCollectionCompPtr->GetObjectData(roleObjectId, dataPtr)){
//					const imtauth::IRole* roleInfoPtr = dynamic_cast<const imtauth::IRole*>(dataPtr.GetPtr());

//					QByteArray roleId = roleInfoPtr->GetRoleId();
//					QByteArray roleProductId = roleInfoPtr->GetProductId();

//					if (productId == roleProductId){
//						int roleIndex = rolesModel->InsertNewItem();

//						QByteArray objectId = roleInfoPtr->GetRoleId() + *m_separatorObjectIdAttrPtr + roleProductId;

//						rolesModel->SetData("Id", objectId, roleIndex);
//						rolesModel->SetData("Name", roleInfoPtr->GetRoleName(), roleIndex);
//					}
//				}
//			}
//		}

		dataModel->SetExternTreeModel("items", itemsModel);
	}

	rootModel->SetExternTreeModel("data", dataModel);

	return rootModel;
}


} // namespace imtauth


