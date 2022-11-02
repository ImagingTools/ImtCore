#include <imtlic/CCollectionProductInfoProviderComp.h>


namespace imtlic
{


QByteArray CCollectionProductInfoProviderComp::GetModelId() const
{
	return *m_modelIdAttrPtr;
}


imtbase::CTreeItemModel *CCollectionProductInfoProviderComp::GetTreeItemModel(const QList<imtgql::CGqlObject> &params, const QByteArrayList &fields, const imtgql::IGqlContext *gqlContext)
{
	if (!m_productCollectionCompPtr.IsValid()){
		return nullptr;
	}

	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();

	imtbase::ICollectionInfo::Ids productsIds = m_productCollectionCompPtr->GetElementIds();

	for (const imtbase::ICollectionInfo::Id& productId : productsIds){
		imtbase::IObjectCollection::DataPtr productDataPtr;
		if (m_productCollectionCompPtr->GetObjectData(productId, productDataPtr)){
			const imtlic::IProductLicensingInfo* productPtr = dynamic_cast<const imtlic::IProductLicensingInfo*>(productDataPtr.GetPtr());
			if (productPtr != nullptr){
				QString productName = productPtr->GetName();

				int index = rootModel->InsertNewItem();

				rootModel->SetData("Id", productId, index);
				rootModel->SetData("Name", productName, index);

				imtbase::CTreeItemModel* permissionsModel = rootModel->AddTreeModel("Permissions", index);

				const imtbase::ICollectionInfo& licenseList = productPtr->GetLicenseList();
				const imtbase::IObjectCollectionInfo::Ids licenseCollectionIds = licenseList.GetElementIds();

				for (const QByteArray& licenseId : licenseCollectionIds){
					const imtlic::ILicenseInfo* licenseInfoPtr = productPtr->GetLicenseInfo(licenseId);
					if (licenseInfoPtr != nullptr){
						imtlic::ILicenseInfo::FeatureInfos featureInfos = licenseInfoPtr->GetFeatureInfos();

						for (const imtlic::ILicenseInfo::FeatureInfo& featureInfo : featureInfos){
							int permissionIndex = permissionsModel->InsertNewItem();

							permissionsModel->SetData("Id", featureInfo.id, permissionIndex);
							permissionsModel->SetData("Name", featureInfo.name, permissionIndex);
						}
					}
				}
			}
		}
	}

	return rootModel;
}


} // namespace imtlic


