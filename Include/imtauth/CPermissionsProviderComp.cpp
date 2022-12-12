#include <imtauth/CPermissionsProviderComp.h>

//ImtCore includes
#include <imtbase/ICollectionInfo.h>
#include <imtlic/IFeatureInfo.h>


namespace imtauth
{


QByteArray CPermissionsProviderComp::GetModelId() const
{
	return *m_modelIdAttrPtr;
}


imtbase::CTreeItemModel *CPermissionsProviderComp::GetRepresentation(const QList<imtgql::CGqlObject> &params, const QByteArrayList &fields, const imtgql::IGqlContext *gqlContext)
{
	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();

	if (m_featurePackageCompPtr.IsValid()){
		QByteArrayList featureCollectionIds = m_featurePackageCompPtr->GetFeatureList().GetElementIds().toList();

		for (const QByteArray& featureCollectionId : featureCollectionIds){
			const imtlic::IFeatureInfo* featureInfoPtr = m_featurePackageCompPtr->GetFeatureInfo(featureCollectionId);
			if (featureInfoPtr != nullptr){
				int index = rootModel->InsertNewItem();

				QByteArray featureId = featureInfoPtr->GetFeatureId();
				QString featureName = featureInfoPtr->GetFeatureName();

				rootModel->SetData("Id", featureId, index);
				rootModel->SetData("Name", featureName, index);

				const imtlic::FeatureInfoList& subFeatures = featureInfoPtr->GetSubFeatures();
				if (subFeatures.GetCount() > 0){
					imtbase::CTreeItemModel* childModel = rootModel->AddTreeModel("ChildModel", index);
					InsertSubFeaturesToModelFromData(subFeatures, *childModel);
				}
			}
		}
	}

	return rootModel;
}


void CPermissionsProviderComp::InsertSubFeaturesToModelFromData(const imtlic::FeatureInfoList &subFeaturesList, imtbase::CTreeItemModel &subFeaturesModel) const
{
	for (int i = 0; i < subFeaturesList.GetCount(); ++i){
		const imtlic::IFeatureInfo* featureInfoPtr = subFeaturesList.GetAt(i);
		if (featureInfoPtr != nullptr){
			QByteArray featureId = featureInfoPtr->GetFeatureId();
			QString featureName = featureInfoPtr->GetFeatureName();

			int index = subFeaturesModel.InsertNewItem();

			subFeaturesModel.SetData("Id", featureId, index);
			subFeaturesModel.SetData("Name", featureName, index);

			const imtlic::FeatureInfoList& subFeatures = featureInfoPtr->GetSubFeatures();
			if (subFeatures.GetCount() > 0){
				imtbase::CTreeItemModel* childModel = subFeaturesModel.AddTreeModel("ChildModel", index);
				InsertSubFeaturesToModelFromData(subFeatures, *childModel);
			}
		}
	}
}


} // namespace imtauth
