#include <imtauthgql/CRoleMetaInfoControllerProxyComp.h>


// ImtCore includes
#include <imtlic/IFeatureInfo.h>


namespace imtauthgql
{


// protected methods

QString CRoleMetaInfoControllerProxyComp::GetPermissionName(const QByteArray& permissionId) const
{
	if (!m_productInfoCompPtr.IsValid()){
		return QString();
	}

	imtbase::IObjectCollection* featureCollectionPtr = m_productInfoCompPtr->GetFeatures();
	if (featureCollectionPtr == nullptr){
		return QString();
	}

	imtbase::ICollectionInfo::Ids elementIds = featureCollectionPtr->GetElementIds();
	for (const imtbase::ICollectionInfo::Id& elementId : elementIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (featureCollectionPtr->GetObjectData(elementId, dataPtr)){
			const imtlic::IFeatureInfo* featureInfoPtr = dynamic_cast<const imtlic::IFeatureInfo*>(dataPtr.GetPtr());
			if (featureInfoPtr != nullptr){
				for (imtbase::ICollectionInfo::Id& subFeatureId : featureInfoPtr->GetSubFeatureIds()){
					if (permissionId == subFeatureId){
						const imtlic::IFeatureInfo* subFeatureInfoPtr = featureInfoPtr->GetSubFeature(subFeatureId);
						if (subFeatureInfoPtr != nullptr){
							return subFeatureInfoPtr->GetFeatureName();
						}
					}
				}
			}
		}
	}

	return QString();
}


// reimplemented (imtservergql::CGqlRepresentationDataControllerComp)

imtbase::CTreeItemModel* CRoleMetaInfoControllerProxyComp::CreateInternalResponse(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	istd::TDelPtr<imtbase::CTreeItemModel> responseModelPtr = BaseClass::CreateInternalResponse(gqlRequest, errorMessage);
	if (responseModelPtr.IsValid()){
		if (responseModelPtr->ContainsKey("data")){
			imtbase::CTreeItemModel* dataModelPtr = responseModelPtr->GetTreeItemModel("data");
			if (dataModelPtr != nullptr){
				for (int i = 0; i < dataModelPtr->GetItemsCount(); i++){
					QByteArray id = dataModelPtr->GetData("id", i).toByteArray();
					if (id == QByteArray("permissions")){
						imtbase::CTreeItemModel* childrenModelPtr = dataModelPtr->GetTreeItemModel("children", i);
						if (childrenModelPtr != nullptr){
							for (int j = 0; j < childrenModelPtr->GetItemsCount(); j++){
								QByteArray permissionId = childrenModelPtr->GetData("value", j).toByteArray();

								QString permissionName = GetPermissionName(permissionId);
								if (!permissionName.isEmpty()){
									childrenModelPtr->SetData("value", permissionName, j);
								}
							}
						}

						break;
					}
				}
			}
		}
	}

	return responseModelPtr.PopPtr();
}


} // namespace imtauthgql


