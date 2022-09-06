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
		idoc::MetaInfoPtr metaInfo = m_objectCollectionCompPtr->GetElementMetaInfo(objectId);;
		if (metaInfo.IsValid()){
			return metaInfo->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_CREATION_TIME);
		}
	}
	else if (informationId == QByteArray("LastModified")){
		idoc::MetaInfoPtr metaInfo = m_objectCollectionCompPtr->GetElementMetaInfo(objectId);
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
	imtbase::CTreeItemModel* children = nullptr;


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
		metaInfoModel->SetData("Name", "Permissions", index);
		children = metaInfoModel->AddTreeModel("Children", index);

		imtauth::IRole::FeatureIds permissions = roleInfoPtr->GetPermissions();
		const imtlic::IFeatureInfoProvider* featuresPtr = roleInfoPtr->GetPermissionProvider();

		for (const QByteArray& id : permissions){
			const imtlic::IFeatureInfo* permissionPtr = featuresPtr->GetFeatureInfo(id);
			if (permissionPtr != nullptr){
				int childrenIndex = children->InsertNewItem();
				QString permissionName = permissionPtr->GetFeatureName();
				children->SetData("Value", permissionName, childrenIndex);
			}
		}

		index = metaInfoModel->InsertNewItem();
		metaInfoModel->SetData("Name", "Prohibitions", index);
		children = metaInfoModel->AddTreeModel("Children", index);

		imtauth::IRole::FeatureIds prohibitions = roleInfoPtr->GetProhibitions();

		for (const QByteArray& id : prohibitions){
			const imtlic::IFeatureInfo* prohibitionsPtr = featuresPtr->GetFeatureInfo(id);
			if (prohibitionsPtr != nullptr){
				int childrenIndex = children->InsertNewItem();
				QString prohibitionName = prohibitionsPtr->GetFeatureName();
				children->SetData("Value", prohibitionName, childrenIndex);
			}
		}

		dataModel->SetExternTreeModel("metaInfo", metaInfoModel);
	}
	rootModel->SetExternTreeModel("data", dataModel);

	return rootModel;
}


} // namespace imtauth


